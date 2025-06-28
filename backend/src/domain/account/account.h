#pragma once

#include <string>
#include <chrono>
#include "domain/account/account_type.h"

class Account {
public:
    Account() = default;
    
    // Constructor for creating new accounts
    Account(int userId, const std::string& accountNumber, AccountType accountType, double balance = 0.0);
    
    // Constructor for loading from database
    Account(int id, int userId, const std::string& accountNumber, AccountType accountType, 
            double balance, const std::string& createdAt, const std::string& updatedAt);
    
    // Getters
    int getId() const { return id_; }
    int getUserId() const { return userId_; }
    const std::string& getAccountNumber() const { return accountNumber_; }
    AccountType getAccountType() const { return accountType_; }
    double getBalance() const { return balance_; }
    const std::string& getCreatedAt() const { return createdAt_; }
    const std::string& getUpdatedAt() const { return updatedAt_; }
    
    // Setters
    void setId(int id) { id_ = id; }
    void setBalance(double balance) { balance_ = balance; }
    
    // Business logic
    bool canWithdraw(double amount) const;
    bool deposit(double amount);
    bool withdraw(double amount);
    
    // Validation
    bool isValid() const;
    std::string getValidationError() const;
    
private:
    int id_ = 0;
    int userId_ = 0;
    std::string accountNumber_;
    AccountType accountType_ = AccountType::Checking;
    double balance_ = 0.0;
    std::string createdAt_;
    std::string updatedAt_;
    
    // Helper to get current timestamp
    static std::string getCurrentTimestamp();
};
