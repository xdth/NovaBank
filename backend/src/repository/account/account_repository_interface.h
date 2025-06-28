#pragma once

#include <memory>
#include <vector>
#include <optional>
#include "domain/account/account.h"

class IAccountRepository {
public:
    virtual ~IAccountRepository() = default;
    
    // Create a new account
    virtual std::optional<Account> create(const Account& account) = 0;
    
    // Find account by ID
    virtual std::optional<Account> findById(int id) = 0;
    
    // Find account by account number
    virtual std::optional<Account> findByAccountNumber(const std::string& accountNumber) = 0;
    
    // Find all accounts for a user
    virtual std::vector<Account> findByUserId(int userId) = 0;
    
    // Get all accounts (admin only)
    virtual std::vector<Account> findAll() = 0;
    
    // Update account (mainly for balance updates)
    virtual bool update(const Account& account) = 0;
    
    // Delete account
    virtual bool deleteById(int id) = 0;
    
    // Check if account number exists
    virtual bool existsByAccountNumber(const std::string& accountNumber) = 0;
    
    // Get total balance for a user across all accounts
    virtual double getTotalBalanceForUser(int userId) = 0;
};
