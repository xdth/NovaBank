#include "db/db.h"
#include <iostream>
#include <fstream>
#include <sstream>

Database::Database(const std::string& dbPath) : db_(nullptr) {
    int rc = sqlite3_open(dbPath.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_close(db_);
        db_ = nullptr;
        throw std::runtime_error("Failed to open database");
    }
    
    // Enable foreign keys
    execute("PRAGMA foreign_keys = ON");
    
    // Initialize schema
    if (!initializeSchema()) {
        throw std::runtime_error("Failed to initialize database schema");
    }
}

Database::~Database() {
    if (db_) {
        sqlite3_close(db_);
    }
}

bool Database::execute(const std::string& sql) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::string error = errMsg ? errMsg : "Unknown error";
        sqlite3_free(errMsg);
        std::cerr << "SQL error: " << error << std::endl;
        return false;
    }
    
    return true;
}

bool Database::query(const std::string& sql, 
                    std::function<void(sqlite3_stmt*)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        callback(stmt);
    }
    
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

std::shared_ptr<sqlite3_stmt> Database::prepare(const std::string& sql) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db_) << std::endl;
        return nullptr;
    }
    
    return std::shared_ptr<sqlite3_stmt>(stmt, sqlite3_finalize);
}

bool Database::beginTransaction() {
    return execute("BEGIN TRANSACTION");
}

bool Database::commit() {
    return execute("COMMIT");
}

bool Database::rollback() {
    return execute("ROLLBACK");
}

std::string Database::getLastError() const {
    return sqlite3_errmsg(db_);
}

int64_t Database::getLastInsertId() const {
    return sqlite3_last_insert_rowid(db_);
}

bool Database::initializeSchema() {
    // Check if tables exist
    bool tablesExist = false;
    query("SELECT name FROM sqlite_master WHERE type='table' AND name='users'",
          [&tablesExist](sqlite3_stmt*) { tablesExist = true; });
    
    if (tablesExist) {
        std::cout << "Database schema already initialized" << std::endl;
        return true;
    }
    
    std::cout << "Initializing database schema..." << std::endl;
    
    // Try to read migrations.sql file
    std::ifstream migrationFile("migrations.sql");
    if (!migrationFile.is_open()) {
        // If file not found, use embedded schema
        const char* schema = R"(
            -- Users table
            CREATE TABLE IF NOT EXISTS users (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                username TEXT UNIQUE NOT NULL,
                pin_hash TEXT NOT NULL,
                user_type TEXT NOT NULL CHECK(user_type IN ('standard', 'admin')),
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            );

            -- Accounts table
            CREATE TABLE IF NOT EXISTS accounts (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                user_id INTEGER NOT NULL,
                account_number TEXT UNIQUE NOT NULL,
                account_type TEXT NOT NULL CHECK(account_type IN ('checking', 'savings')),
                balance REAL NOT NULL DEFAULT 0.0,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
            );

            -- Transactions table
            CREATE TABLE IF NOT EXISTS transactions (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                from_account_id INTEGER,
                to_account_id INTEGER,
                amount REAL NOT NULL,
                transaction_type TEXT NOT NULL CHECK(transaction_type IN ('deposit', 'withdrawal', 'transfer')),
                description TEXT,
                status TEXT NOT NULL DEFAULT 'completed' CHECK(status IN ('pending', 'completed', 'failed')),
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (from_account_id) REFERENCES accounts(id),
                FOREIGN KEY (to_account_id) REFERENCES accounts(id),
                CHECK (
                    (transaction_type = 'deposit' AND from_account_id IS NULL AND to_account_id IS NOT NULL) OR
                    (transaction_type = 'withdrawal' AND from_account_id IS NOT NULL AND to_account_id IS NULL) OR
                    (transaction_type = 'transfer' AND from_account_id IS NOT NULL AND to_account_id IS NOT NULL)
                )
            );

            -- Create indexes
            CREATE INDEX idx_accounts_user_id ON accounts(user_id);
            CREATE INDEX idx_transactions_from_account ON transactions(from_account_id);
            CREATE INDEX idx_transactions_to_account ON transactions(to_account_id);
            CREATE INDEX idx_transactions_created_at ON transactions(created_at);

            -- Insert default admin user (PIN: 0000)
            INSERT INTO users (username, pin_hash, user_type) 
            VALUES ('admin', '9af15b336e6a9619928537df30b2e6a2376569fcf9d7e773eccede65606529a0', 'admin');
        )";
        
        return execute(schema);
    }
    
    // Read and execute migrations from file
    std::stringstream buffer;
    buffer << migrationFile.rdbuf();
    return execute(buffer.str());
}
