#pragma once

#include <memory>
#include <vector>
#include <optional>
#include "domain/user/user.h"

class IUserRepository {
public:
    virtual ~IUserRepository() = default;
    
    // Create a new user
    virtual std::optional<User> create(const User& user) = 0;
    
    // Find user by ID
    virtual std::optional<User> findById(int id) = 0;
    
    // Find user by username
    virtual std::optional<User> findByUsername(const std::string& username) = 0;
    
    // Get all users
    virtual std::vector<User> findAll() = 0;
    
    // Update user
    virtual bool update(const User& user) = 0;
    
    // Delete user
    virtual bool deleteById(int id) = 0;
    
    // Check if username exists
    virtual bool existsByUsername(const std::string& username) = 0;
};
