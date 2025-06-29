#pragma once

#include <string>

enum class TransactionStatus {
    Pending,
    Completed,
    Failed
};

enum class TransactionType {
    Deposit,
    Withdrawal,
    Transfer
};

inline std::string transactionStatusToString(TransactionStatus status) {
    switch (status) {
        case TransactionStatus::Pending:
            return "pending";
        case TransactionStatus::Completed:
            return "completed";
        case TransactionStatus::Failed:
            return "failed";
        default:
            return "unknown";
    }
}

inline TransactionStatus stringToTransactionStatus(const std::string& str) {
    if (str == "pending") {
        return TransactionStatus::Pending;
    } else if (str == "failed") {
        return TransactionStatus::Failed;
    }
    return TransactionStatus::Completed;
}

inline std::string transactionTypeToString(TransactionType type) {
    switch (type) {
        case TransactionType::Deposit:
            return "deposit";
        case TransactionType::Withdrawal:
            return "withdrawal";
        case TransactionType::Transfer:
            return "transfer";
        default:
            return "unknown";
    }
}

inline TransactionType stringToTransactionType(const std::string& str) {
    if (str == "deposit") {
        return TransactionType::Deposit;
    } else if (str == "withdrawal") {
        return TransactionType::Withdrawal;
    }
    return TransactionType::Transfer;
}
