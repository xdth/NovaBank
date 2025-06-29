#pragma once

#include <sqlite3.h>
#include <string>
#include <memory>
#include <mutex>
#include <vector>
#include <functional>

class Database {
public:
    explicit Database(const std::string& dbPath);
    ~Database();
    
    // Prevent copying
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    
    // Allow moving
    Database(Database&&) = default;
    Database& operator=(Database&&) = default;
    
    // Execute a SQL statement (no results expected)
    bool execute(const std::string& sql);
    
    // Execute a SQL query with callback for each row
    bool query(const std::string& sql, 
               std::function<void(sqlite3_stmt*)> callback);
    
    // Prepare a statement for repeated use
    std::shared_ptr<sqlite3_stmt> prepare(const std::string& sql);
    
    // Transaction management
    bool beginTransaction();
    bool commit();
    bool rollback();
    
    // Get last error message
    std::string getLastError() const;
    
    // Get last insert row ID
    int64_t getLastInsertId() const;
    
    // Direct SQLite handle access (for use within transactions)
    sqlite3* getHandle() const { return db_; }
    
private:
    sqlite3* db_;
    mutable std::mutex mutex_;
    bool inTransaction_ = false;
    
    // Initialize database schema
    bool initializeSchema();
};
