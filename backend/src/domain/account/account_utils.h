#pragma once

#include <string>
#include <random>
#include <sstream>
#include <iomanip>

class AccountUtils {
public:
    // Generate a unique account number (format: ACC12345678)
    static std::string generateAccountNumber() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(10000000, 99999999);
        
        std::stringstream ss;
        ss << "ACC" << dis(gen);
        return ss.str();
    }
    
    // Format currency for display
    static std::string formatCurrency(double amount) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << "$" << amount;
        return ss.str();
    }
    
    // Validate amount (positive, max 2 decimal places)
    static bool isValidAmount(double amount) {
        if (amount <= 0) {
            return false;
        }
        
        // Check if amount has more than 2 decimal places
        double cents = amount * 100;
        return std::abs(cents - std::round(cents)) < 0.001;
    }
    
    // Round to 2 decimal places
    static double roundToTwoDecimals(double amount) {
        return std::round(amount * 100.0) / 100.0;
    }
};
