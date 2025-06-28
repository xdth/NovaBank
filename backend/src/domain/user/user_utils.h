#pragma once

#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>

class UserUtils {
public:
    // Hash a PIN using SHA256 (simplified - in production use bcrypt)
    static std::string hashPin(const std::string& pin) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256((unsigned char*)pin.c_str(), pin.length(), hash);
        
        std::stringstream ss;
        for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        return ss.str();
    }
    
    // Verify a PIN against a hash
    static bool verifyPin(const std::string& pin, const std::string& hash) {
        return hashPin(pin) == hash;
    }
    
    // Validate PIN format (4-6 digits)
    static bool isValidPin(const std::string& pin) {
        if (pin.length() < 4 || pin.length() > 6) {
            return false;
        }
        
        for (char c : pin) {
            if (!std::isdigit(c)) {
                return false;
            }
        }
        
        return true;
    }
    
    // Generate a unique account number
    static std::string generateAccountNumber() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(10000000, 99999999);
        
        std::stringstream ss;
        ss << "ACC" << dis(gen);
        return ss.str();
    }
};
