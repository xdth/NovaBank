#include "repository/user/user_repository.h"
#include <iostream>

UserRepository::UserRepository(std::shared_ptr<Database> db) : db_(db) {}

std::optional<User> UserRepository::create(const User& user) {
    if (!user.isValid()) {
        std::cerr << "Invalid user: " << user.getValidationError() << std::endl;
        return std::nullopt;
    }
    
    const std::string sql = "INSERT INTO users (username, pin_hash, user_type) VALUES (?, ?, ?)";
    auto stmt = db_->prepare(sql);
    
    if (!stmt) {
        return std::nullopt;
    }
    
    sqlite3_bind_text(stmt.get(), 1, user.getUsername().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt.get(), 2, user.getPinHash().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt.get(), 3, userTypeToString(user.getUserType()).c_str(), -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
        std::cerr << "Failed to create user: " << db_->getLastError() << std::endl;
        return std::nullopt;
    }
    
    // Return the created user with ID
    return findById(db_->getLastInsertId());
}

std::optional<User> UserRepository::findById(int id) {
    const std::string sql = "SELECT id, username, pin_hash, user_type, created_at, updated_at FROM users WHERE id = ?";
    auto stmt = db_->prepare(sql);
    
    if (!stmt) {
        return std::nullopt;
    }
    
    sqlite3_bind_int(stmt.get(), 1, id);
    
    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        return userFromStatement(stmt.get());
    }
    
    return std::nullopt;
}

std::optional<User> UserRepository::findByUsername(const std::string& username) {
    const std::string sql = "SELECT id, username, pin_hash, user_type, created_at, updated_at FROM users WHERE username = ?";
    auto stmt = db_->prepare(sql);
    
    if (!stmt) {
        return std::nullopt;
    }
    
    sqlite3_bind_text(stmt.get(), 1, username.c_str(), -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        return userFromStatement(stmt.get());
    }
    
    return std::nullopt;
}

std::vector<User> UserRepository::findAll() {
    std::vector<User> users;
    const std::string sql = "SELECT id, username, pin_hash, user_type, created_at, updated_at FROM users ORDER BY id";
    
    db_->query(sql, [&users, this](sqlite3_stmt* stmt) {
        users.push_back(userFromStatement(stmt));
    });
    
    return users;
}

bool UserRepository::update(const User& user) {
    if (!user.isValid() || user.getId() <= 0) {
        return false;
    }
    
    const std::string sql = "UPDATE users SET pin_hash = ?, user_type = ? WHERE id = ?";
    auto stmt = db_->prepare(sql);
    
    if (!stmt) {
        return false;
    }
    
    sqlite3_bind_text(stmt.get(), 1, user.getPinHash().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt.get(), 2, userTypeToString(user.getUserType()).c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt.get(), 3, user.getId());
    
    return sqlite3_step(stmt.get()) == SQLITE_DONE;
}

bool UserRepository::deleteById(int id) {
    const std::string sql = "DELETE FROM users WHERE id = ?";
    auto stmt = db_->prepare(sql);
    
    if (!stmt) {
        return false;
    }
    
    sqlite3_bind_int(stmt.get(), 1, id);
    return sqlite3_step(stmt.get()) == SQLITE_DONE;
}

bool UserRepository::existsByUsername(const std::string& username) {
    const std::string sql = "SELECT COUNT(*) FROM users WHERE username = ?";
    auto stmt = db_->prepare(sql);
    
    if (!stmt) {
        return false;
    }
    
    sqlite3_bind_text(stmt.get(), 1, username.c_str(), -1, SQLITE_STATIC);
    
    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        return sqlite3_column_int(stmt.get(), 0) > 0;
    }
    
    return false;
}

User UserRepository::userFromStatement(sqlite3_stmt* stmt) {
    int id = sqlite3_column_int(stmt, 0);
    std::string username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    std::string pinHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    std::string userTypeStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    std::string createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
    std::string updatedAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
    
    UserType userType = stringToUserType(userTypeStr);
    
    return User(id, username, pinHash, userType, createdAt, updatedAt);
}
