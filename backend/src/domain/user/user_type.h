#pragma once

#include <string>

enum class UserType {
    Standard,
    Admin
};

inline std::string userTypeToString(UserType type) {
    switch (type) {
        case UserType::Standard:
            return "standard";
        case UserType::Admin:
            return "admin";
        default:
            return "unknown";
    }
}

inline UserType stringToUserType(const std::string& str) {
    if (str == "admin") {
        return UserType::Admin;
    }
    return UserType::Standard;
}
