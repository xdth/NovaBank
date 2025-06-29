#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <sstream>
#include <iomanip>
#include "domain/transaction/transaction.h"

class TransactionUtils {
public:
    // Generate transaction reference/ID
    static std::string generateTransactionReference() {
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        ).count();
        
        std::stringstream ss;
        ss << "TXN" << timestamp;
        return ss.str();
    }
    
    // Format transaction for display
    static std::string formatTransactionType(TransactionType type) {
        switch (type) {
            case TransactionType::Deposit:
                return "Deposit";
            case TransactionType::Withdrawal:
                return "Withdrawal";
            case TransactionType::Transfer:
                return "Transfer";
            default:
                return "Unknown";
        }
    }
    
    // Get transaction sign for amount display
    static std::string getAmountSign(const Transaction& transaction, int accountId) {
        // For deposits to this account, it's positive
        if (transaction.isDeposit() && 
            transaction.getToAccountId().has_value() && 
            transaction.getToAccountId().value() == accountId) {
            return "+";
        }
        
        // For withdrawals from this account, it's negative
        if (transaction.isWithdrawal() && 
            transaction.getFromAccountId().has_value() && 
            transaction.getFromAccountId().value() == accountId) {
            return "-";
        }
        
        // For transfers
        if (transaction.isTransfer()) {
            if (transaction.getFromAccountId().has_value() && 
                transaction.getFromAccountId().value() == accountId) {
                return "-";  // Money going out
            } else if (transaction.getToAccountId().has_value() && 
                       transaction.getToAccountId().value() == accountId) {
                return "+";  // Money coming in
            }
        }
        
        return "";
    }
    
    // Get transaction direction for an account
    static std::string getTransactionDirection(const Transaction& transaction, int accountId) {
        if (transaction.isDeposit()) {
            return "Credit";
        }
        
        if (transaction.isWithdrawal()) {
            return "Debit";
        }
        
        if (transaction.isTransfer()) {
            if (transaction.getFromAccountId().has_value() && 
                transaction.getFromAccountId().value() == accountId) {
                return "Debit";
            } else if (transaction.getToAccountId().has_value() && 
                       transaction.getToAccountId().value() == accountId) {
                return "Credit";
            }
        }
        
        return "Unknown";
    }
    
    // Parse date string for filtering
    static bool isWithinDateRange(const std::string& transactionDate, 
                                  const std::string& startDate, 
                                  const std::string& endDate) {
        return transactionDate >= startDate && transactionDate <= endDate;
    }
};
