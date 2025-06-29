#pragma once

#include <memory>
#include <vector>
#include <optional>
#include "domain/transaction/transaction.h"

class ITransactionRepository {
public:
    virtual ~ITransactionRepository() = default;
    
    // Create a new transaction
    virtual std::optional<Transaction> create(const Transaction& transaction) = 0;
    
    // Find transaction by ID
    virtual std::optional<Transaction> findById(int id) = 0;
    
    // Find all transactions for an account
    virtual std::vector<Transaction> findByAccountId(int accountId) = 0;
    
    // Find all transactions for a user (across all their accounts)
    virtual std::vector<Transaction> findByUserId(int userId) = 0;
    
    // Find all transactions (admin only)
    virtual std::vector<Transaction> findAll() = 0;
    
    // Find transactions with filters
    virtual std::vector<Transaction> findWithFilters(
        std::optional<int> accountId,
        std::optional<TransactionType> type,
        std::optional<std::string> startDate,
        std::optional<std::string> endDate,
        int limit = 100,
        int offset = 0
    ) = 0;
    
    // Update transaction status
    virtual bool updateStatus(int id, TransactionStatus status) = 0;
    
    // Get transaction count for pagination
    virtual int getTransactionCount(std::optional<int> accountId = std::nullopt) = 0;
};
