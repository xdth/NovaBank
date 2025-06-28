#include "domain/user/user.h"
#include "domain/user/user_utils.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <regex>

User::User(const std::string& username, const std::string& pinHash, UserType userType)
    : username_(username), pinHash_(pinHash), userType_(userType) {
    std::string timestamp = getCurrentTimestamp();
    createdAt_ = timestamp;
    updatedAt_ = timestamp;
}

User::User(int id, const std::string& username, const std::string& pinHash, 
           UserType userType, const std::string& createdAt, const std::string& updatedAt)
    : id_(id), username_(username), pinHash_(pinHash), userType_(userType),
      createdAt_(createdAt), updatedAt_(updatedAt) {
}

bool User::verifyPin(const std::string& pin) const {
    // Use SHA256 hashing for PIN verification
    return UserUtils::verifyPin(pin, pinHash_);
}

bool User::isValid() const {
    // Username validation
    if (username_.empty() || username_.length() < 3 || username_.length() > 50) {
        return false;
    }
    
    // Username should contain only alphanumeric characters and underscores
    std::regex usernameRegex("^[a-zA-Z0-9_]+$");
    if (!std::regex_match(username_, usernameRegex)) {
        return false;
    }
    
    // PIN hash should not be empty
    if (pinHash_.empty()) {
        return false;
    }
    
    return true;
}

std::string User::getValidationError() const {
    if (username_.empty()) {
        return "Username cannot be empty";
    }
    
    if (username_.length() < 3) {
        return "Username must be at least 3 characters long";
    }
    
    if (username_.length() > 50) {
        return "Username cannot exceed 50 characters";
    }
    
    std::regex usernameRegex("^[a-zA-Z0-9_]+$");
    if (!std::regex_match(username_, usernameRegex)) {
        return "Username can only contain letters, numbers, and underscores";
    }
    
    if (pinHash_.empty()) {
        return "PIN cannot be empty";
    }
    
    return "";
}

std::string User::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
