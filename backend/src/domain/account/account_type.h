#pragma once

#include <string>

enum class AccountType {
    Checking,
    Savings
};

inline std::string accountTypeToString(AccountType type) {
    switch (type) {
        case AccountType::Checking:
            return "checking";
        case AccountType::Savings:
            return "savings";
        default:
            return "unknown";
    }
}

inline AccountType stringToAccountType(const std::string& str) {
    if (str == "savings") {
        return AccountType::Savings;
    }
    return AccountType::Checking;
}
