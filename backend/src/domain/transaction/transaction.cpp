#include "domain/transaction/transaction.h"
#include <ctime>
#include <iomanip>
#include <sstream>

Transaction::Transaction(std::optional<int> fromAccountId, std::optional<int> toAccountId,
                        double amount, TransactionType type, const std::string& description,
                        TransactionStatus status)
    : fromAccountId_(fromAccountId), toAccountId_(toAccountId), amount_(amount),
      transactionType_(type), description_(description), status_(status) {
    createdAt_ = getCurrentTimestamp();
}

Transaction::Transaction(int id, std::optional<int> fromAccountId, std::optional<int> toAccountId,
                        double amount, TransactionType type, const std::string& description,
                        TransactionStatus status, const std::string& createdAt)
    : id_(id), fromAccountId_(fromAccountId), toAccountId_(toAccountId), amount_(amount),
      transactionType_(type), description_(description), status_(status), createdAt_(createdAt) {
}

bool Transaction::isValid() const {
    // Amount must be positive
    if (amount_ <= 0) {
        return false;
    }
    
    // Validate based on transaction type
    switch (transactionType_) {
        case TransactionType::Deposit:
            // Deposit: no from_account, must have to_account
            if (fromAccountId_.has_value() || !toAccountId_.has_value()) {
                return false;
            }
            break;
            
        case TransactionType::Withdrawal:
            // Withdrawal: must have from_account, no to_account
            if (!fromAccountId_.has_value() || toAccountId_.has_value()) {
                return false;
            }
            break;
            
        case TransactionType::Transfer:
            // Transfer: must have both accounts
            if (!fromAccountId_.has_value() || !toAccountId_.has_value()) {
                return false;
            }
            // Cannot transfer to same account
            if (fromAccountId_.value() == toAccountId_.value()) {
                return false;
            }
            break;
    }
    
    return true;
}

std::string Transaction::getValidationError() const {
    if (amount_ <= 0) {
        return "Amount must be positive";
    }
    
    switch (transactionType_) {
        case TransactionType::Deposit:
            if (fromAccountId_.has_value()) {
                return "Deposit cannot have a source account";
            }
            if (!toAccountId_.has_value()) {
                return "Deposit must have a destination account";
            }
            break;
            
        case TransactionType::Withdrawal:
            if (!fromAccountId_.has_value()) {
                return "Withdrawal must have a source account";
            }
            if (toAccountId_.has_value()) {
                return "Withdrawal cannot have a destination account";
            }
            break;
            
        case TransactionType::Transfer:
            if (!fromAccountId_.has_value()) {
                return "Transfer must have a source account";
            }
            if (!toAccountId_.has_value()) {
                return "Transfer must have a destination account";
            }
            if (fromAccountId_.value() == toAccountId_.value()) {
                return "Cannot transfer to the same account";
            }
            break;
    }
    
    return "";
}

std::string Transaction::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
