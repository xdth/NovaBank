#pragma once

#include "repository/user/user_repository_interface.h"
#include "db/db.h"
#include <memory>

class UserRepository : public IUserRepository {
public:
    explicit UserRepository(std::shared_ptr<Database> db);
    
    // IUserRepository implementation
    std::optional<User> create(const User& user) override;
    std::optional<User> findById(int id) override;
    std::optional<User> findByUsername(const std::string& username) override;
    std::vector<User> findAll() override;
    bool update(const User& user) override;
    bool deleteById(int id) override;
    bool existsByUsername(const std::string& username) override;
    
private:
    std::shared_ptr<Database> db_;
    
    // Helper method to create User from query result
    User userFromStatement(sqlite3_stmt* stmt);
};
