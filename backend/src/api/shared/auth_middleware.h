#pragma once

#include <crow.h>
#include <crow/middlewares/cors.h>
#include <unordered_map>
#include <string>
#include <chrono>
#include <mutex>
#include <random>
#include <sstream>
#include <iomanip>
#include "api/shared/error_response.h"

struct Session {
    int userId;
    std::string username;
    bool isAdmin;
    std::chrono::steady_clock::time_point lastActivity;
};

class AuthMiddleware {
public:
    static AuthMiddleware& getInstance() {
        static AuthMiddleware instance;
        return instance;
    }
    
    // Generate a new session token
    std::string createSession(int userId, const std::string& username, bool isAdmin) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::string token = generateToken();
        sessions_[token] = Session{
            userId,
            username,
            isAdmin,
            std::chrono::steady_clock::now()
        };
        
        return token;
    }
    
    // Validate and get session
    std::optional<Session> getSession(const std::string& token) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = sessions_.find(token);
        if (it == sessions_.end()) {
            return std::nullopt;
        }
        
        // Check if session expired (30 minutes timeout)
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::minutes>(now - it->second.lastActivity);
        
        if (duration.count() > 30) {
            sessions_.erase(it);
            return std::nullopt;
        }
        
        // Update last activity
        it->second.lastActivity = now;
        return it->second;
    }
    
    // Destroy session
    void destroySession(const std::string& token) {
        std::lock_guard<std::mutex> lock(mutex_);
        sessions_.erase(token);
    }
    
    // Extract token from request
    std::string extractToken(const crow::request& req) {
        auto auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            return "";
        }
        
        // Expected format: "Bearer <token>"
        const std::string bearer_prefix = "Bearer ";
        if (auth_header.substr(0, bearer_prefix.length()) == bearer_prefix) {
            return auth_header.substr(bearer_prefix.length());
        }
        
        return "";
    }
    
private:
    AuthMiddleware() = default;
    std::unordered_map<std::string, Session> sessions_;
    std::mutex mutex_;
    
    std::string generateToken() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);
        
        std::stringstream ss;
        for (int i = 0; i < 32; ++i) {
            ss << std::hex << dis(gen);
        }
        return ss.str();
    }
};

// Helper macro for protected routes
#define REQUIRE_AUTH(req) \
    auto token = AuthMiddleware::getInstance().extractToken(req); \
    auto session = AuthMiddleware::getInstance().getSession(token); \
    if (!session) { \
        return errorResponse(401, "Authentication required"); \
    }

#define REQUIRE_ADMIN(req) \
    REQUIRE_AUTH(req) \
    if (!session->isAdmin) { \
        return errorResponse(403, "Admin access required"); \
    }
    