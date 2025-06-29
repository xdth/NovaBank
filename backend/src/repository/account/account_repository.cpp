#include "repository/account/account_repository.h"
#include <iostream>

AccountRepository::AccountRepository(std::shared_ptr<Database> db) : db_(db) {}

std::optional<Account> AccountRepository::create(const Account& account) {
    if (!account.isValid()) {
        std::cerr << "Invalid account: " << account.getValidationError() << std::endl;
        return std::nullopt;
    }
    
    const std::string sql = "INSERT INTO accounts (user_id, account_number, account_type, balance) VALUES (?, ?, ?, ?)";
    auto stmt = db_->prepare(sql);
    
    if (!stmt) {
        return std::nullopt;
    }
    
    sqlite3_bind_int(stmt.get(), 1, account.getUserId());
    sqlite3_bind_text(stmt.get(), 2, account.getAccountNumber().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt.get(), 3, accountTypeToString(account.getAccountType()).c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt.get(), 4, account.getBalance());
    
    if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
        std::cerr << "Failed to create account: " << db_->getLastError() << std::endl;
        return std::nullopt;
    }
    
    // Return the created account with ID
    return findById(db_->getLastInsertId());
}

std::optional<Account> AccountRepository::findById(int id) {
    const std::string sql = "SELECT id, user_id, account_number, account_type, balance, created_at, updated_at FROM accounts WHERE id = ?";
    auto stmt = db_->prepare(sql);
    
    if (!stmt) {
        return std::nullopt;
    }
    
    sqlite3_bind_int(stmt.get(), 1, id);
    
    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        return accountFromStatement(stmt.get());
    }
    
    return std::nullopt;
}

std::optional<Account> AccountRepository::findByAccountNumber(const std::string& accountNumber) {
    const std::string sql = "SELECT id, user_id, account_number, account_type, balance, created_at, updated_at FROM accounts WHERE account_number = ?";
    auto stmt = db_->prepare(sql);
    
    if (!stmt) {
        return std::nullopt;
    }
    
    sqlite3_bind_text(stmt.get(), 1, accountNumber.c_str(), -1, SQLITE_TRANSIENT);
    
    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        return accountFromStatement(stmt.get());
    }
    
    return std::nullopt;
}

std::vector<Account> AccountRepository::findByUserId(int userId) {
    std::vector<Account> accounts;
    const std::string sql = "SELECT id, user_id, account_number, account_type, balance, created_at, updated_at FROM accounts WHERE user_id = ? ORDER BY created_at";
    
    auto stmt = db_->prepare(sql);
    if (!stmt) {
        return accounts;
    }
    
    sqlite3_bind_int(stmt.get(), 1, userId);
    
    while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        accounts.push_back(accountFromStatement(stmt.get()));
    }
    
    return accounts;
}

std::vector<Account> AccountRepository::findAll() {
    std::vector<Account> accounts;
    const std::string sql = "SELECT id, user_id, account_number, account_type, balance, created_at, updated_at FROM accounts ORDER BY user_id, created_at";
    
    db_->query(sql, [&accounts, this](sqlite3_stmt* stmt) {
        accounts.push_back(accountFromStatement(stmt));
    });
    
    return accounts;
}

bool AccountRepository::update(const Account& account) {
    if (!account.isValid() || account.getId() <= 0) {
        std::cerr << "Invalid account for update" << std::endl;
        return false;
    }
    
    const std::string sql = "UPDATE accounts SET balance = ? WHERE id = ?";
    auto stmt = db_->prepare(sql);
    
    if (!stmt) {
        std::cerr << "Failed to prepare account update statement" << std::endl;
        return false;
    }
    
    sqlite3_bind_double(stmt.get(), 1, account.getBalance());
    sqlite3_bind_int(stmt.get(), 2, account.getId());
    
    int rc = sqlite3_step(stmt.get());
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to execute account update: " << sqlite3_errmsg(sqlite3_db_handle(stmt.get())) << std::endl;
        return false;
    }
    
    return true;
}

bool AccountRepository::deleteById(int id) {
    const std::string sql = "DELETE FROM accounts WHERE id = ?";
    auto stmt = db_->prepare(sql);
    
    if (!stmt) {
        return false;
    }
    
    sqlite3_bind_int(stmt.get(), 1, id);
    return sqlite3_step(stmt.get()) == SQLITE_DONE;
}

bool AccountRepository::existsByAccountNumber(const std::string& accountNumber) {
    const std::string sql = "SELECT COUNT(*) FROM accounts WHERE account_number = ?";
    auto stmt = db_->prepare(sql);
    
    if (!stmt) {
        return false;
    }
    
    sqlite3_bind_text(stmt.get(), 1, accountNumber.c_str(), -1, SQLITE_TRANSIENT);
    
    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        return sqlite3_column_int(stmt.get(), 0) > 0;
    }
    
    return false;
}

double AccountRepository::getTotalBalanceForUser(int userId) {
    const std::string sql = "SELECT COALESCE(SUM(balance), 0.0) FROM accounts WHERE user_id = ?";
    auto stmt = db_->prepare(sql);
    
    if (!stmt) {
        return 0.0;
    }
    
    sqlite3_bind_int(stmt.get(), 1, userId);
    
    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        return sqlite3_column_double(stmt.get(), 0);
    }
    
    return 0.0;
}

Account AccountRepository::accountFromStatement(sqlite3_stmt* stmt) {
    int id = sqlite3_column_int(stmt, 0);
    int userId = sqlite3_column_int(stmt, 1);
    std::string accountNumber = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    std::string accountTypeStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    double balance = sqlite3_column_double(stmt, 4);
    std::string createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
    std::string updatedAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
    
    AccountType accountType = stringToAccountType(accountTypeStr);
    
    return Account(id, userId, accountNumber, accountType, balance, createdAt, updatedAt);
}
