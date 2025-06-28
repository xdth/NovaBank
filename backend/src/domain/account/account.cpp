#include "domain/account/account.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cmath>

Account::Account(int userId, const std::string& accountNumber, AccountType accountType, double balance)
    : userId_(userId), accountNumber_(accountNumber), accountType_(accountType), balance_(balance) {
    std::string timestamp = getCurrentTimestamp();
    createdAt_ = timestamp;
    updatedAt_ = timestamp;
}

Account::Account(int id, int userId, const std::string& accountNumber, AccountType accountType, 
                double balance, const std::string& createdAt, const std::string& updatedAt)
    : id_(id), userId_(userId), accountNumber_(accountNumber), accountType_(accountType),
      balance_(balance), createdAt_(createdAt), updatedAt_(updatedAt) {
}

bool Account::canWithdraw(double amount) const {
    if (amount <= 0) {
        return false;
    }
    
    // For savings accounts, maintain minimum balance
    if (accountType_ == AccountType::Savings) {
        const double MIN_SAVINGS_BALANCE = 25.0;
        return (balance_ - amount) >= MIN_SAVINGS_BALANCE;
    }
    
    // For checking accounts, allow up to balance
    return balance_ >= amount;
}

bool Account::deposit(double amount) {
    if (amount <= 0) {
        return false;
    }
    
    balance_ += amount;
    return true;
}

bool Account::withdraw(double amount) {
    if (!canWithdraw(amount)) {
        return false;
    }
    
    balance_ -= amount;
    return true;
}

bool Account::isValid() const {
    // User ID must be positive
    if (userId_ <= 0) {
        return false;
    }
    
    // Account number validation
    if (accountNumber_.empty() || accountNumber_.length() != 11) {
        return false;
    }
    
    // Account number should start with "ACC"
    if (accountNumber_.substr(0, 3) != "ACC") {
        return false;
    }
    
    // Balance cannot be negative
    if (balance_ < 0) {
        return false;
    }
    
    return true;
}

std::string Account::getValidationError() const {
    if (userId_ <= 0) {
        return "User ID must be positive";
    }
    
    if (accountNumber_.empty()) {
        return "Account number cannot be empty";
    }
    
    if (accountNumber_.length() != 11) {
        return "Account number must be 11 characters long";
    }
    
    if (accountNumber_.substr(0, 3) != "ACC") {
        return "Account number must start with 'ACC'";
    }
    
    if (balance_ < 0) {
        return "Balance cannot be negative";
    }
    
    return "";
}

std::string Account::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
