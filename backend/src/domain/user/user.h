#pragma once

#include <string>
#include <chrono>
#include <optional>
#include "domain/user/user_type.h"

class User {
public:
    User() = default;
    
    // Constructor for creating new users
    User(const std::string& username, const std::string& pinHash, UserType userType);
    
    // Constructor for loading from database
    User(int id, const std::string& username, const std::string& pinHash, 
         UserType userType, const std::string& createdAt, const std::string& updatedAt);
    
    // Getters
    int getId() const { return id_; }
    const std::string& getUsername() const { return username_; }
    const std::string& getPinHash() const { return pinHash_; }
    UserType getUserType() const { return userType_; }
    const std::string& getCreatedAt() const { return createdAt_; }
    const std::string& getUpdatedAt() const { return updatedAt_; }
    
    // Setters
    void setId(int id) { id_ = id; }
    void setPinHash(const std::string& pinHash) { pinHash_ = pinHash; }
    void setUserType(UserType userType) { userType_ = userType; }
    
    // Business logic
    bool isAdmin() const { return userType_ == UserType::Admin; }
    bool verifyPin(const std::string& pin) const;
    
    // Validation
    bool isValid() const;
    std::string getValidationError() const;
    
private:
    int id_ = 0;
    std::string username_;
    std::string pinHash_;
    UserType userType_ = UserType::Standard;
    std::string createdAt_;
    std::string updatedAt_;
    
    // Helper to get current timestamp
    static std::string getCurrentTimestamp();
};
