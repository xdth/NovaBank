#pragma once

#include <string>
#include <chrono>
#include <optional>
#include "domain/transaction/transaction_status.h"

class Transaction {
public:
    Transaction() = default;
    
    // Constructor for creating new transactions
    Transaction(std::optional<int> fromAccountId, std::optional<int> toAccountId,
                double amount, TransactionType type, const std::string& description,
                TransactionStatus status = TransactionStatus::Completed);
    
    // Constructor for loading from database
    Transaction(int id, std::optional<int> fromAccountId, std::optional<int> toAccountId,
                double amount, TransactionType type, const std::string& description,
                TransactionStatus status, const std::string& createdAt);
    
    // Getters
    int getId() const { return id_; }
    std::optional<int> getFromAccountId() const { return fromAccountId_; }
    std::optional<int> getToAccountId() const { return toAccountId_; }
    double getAmount() const { return amount_; }
    TransactionType getTransactionType() const { return transactionType_; }
    const std::string& getDescription() const { return description_; }
    TransactionStatus getStatus() const { return status_; }
    const std::string& getCreatedAt() const { return createdAt_; }
    
    // Setters
    void setId(int id) { id_ = id; }
    void setStatus(TransactionStatus status) { status_ = status; }
    void setDescription(const std::string& description) { description_ = description; }
    
    // Business logic
    bool isValid() const;
    std::string getValidationError() const;
    
    // Helper methods
    bool isDeposit() const { return transactionType_ == TransactionType::Deposit; }
    bool isWithdrawal() const { return transactionType_ == TransactionType::Withdrawal; }
    bool isTransfer() const { return transactionType_ == TransactionType::Transfer; }
    
private:
    int id_ = 0;
    std::optional<int> fromAccountId_;
    std::optional<int> toAccountId_;
    double amount_ = 0.0;
    TransactionType transactionType_;
    std::string description_;
    TransactionStatus status_ = TransactionStatus::Completed;
    std::string createdAt_;
    
    // Helper to get current timestamp
    static std::string getCurrentTimestamp();
};
