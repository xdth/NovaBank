#include "api/user/user_controller.h"
#include "api/shared/error_response.h"
#include "api/shared/auth_middleware.h"
#include "domain/user/user_utils.h"
#include <crow/json.h>
#include <crow/middlewares/cors.h>
#include <iostream>

UserController::UserController(std::shared_ptr<Database> db) 
    : userRepository_(std::make_unique<UserRepository>(db)) {}

void UserController::registerRoutes(crow::App<crow::CORSHandler>& app) {
    // Authentication routes
    CROW_ROUTE(app, "/api/v1/auth/login")
        .methods("POST"_method)
        ([this](const crow::request& req) { return login(req); });
    
    CROW_ROUTE(app, "/api/v1/auth/logout")
        .methods("POST"_method)
        ([this](const crow::request& req) { return logout(req); });
    
    CROW_ROUTE(app, "/api/v1/auth/me")
        .methods("GET"_method)
        ([this](const crow::request& req) { return getCurrentUser(req); });
    
    // User management routes
    CROW_ROUTE(app, "/api/v1/users")
        .methods("GET"_method)
        ([this](const crow::request& req) { return getUsers(req); });
    
    CROW_ROUTE(app, "/api/v1/users/<int>")
        .methods("GET"_method)
        ([this](const crow::request& req, int id) { return getUser(req, id); });
    
    CROW_ROUTE(app, "/api/v1/users")
        .methods("POST"_method)
        ([this](const crow::request& req) { return createUser(req); });
    
    CROW_ROUTE(app, "/api/v1/users/<int>")
        .methods("PATCH"_method)
        ([this](const crow::request& req, int id) { return updateUser(req, id); });
    
    CROW_ROUTE(app, "/api/v1/users/<int>")
        .methods("DELETE"_method)
        ([this](const crow::request& req, int id) { return deleteUser(req, id); });
}

crow::response UserController::login(const crow::request& req) {
    auto body = crow::json::load(req.body);
    if (!body) {
        return errorResponse(400, "Invalid JSON");
    }
    
    if (!body.has("username") || !body.has("pin")) {
        return errorResponse(400, "Username and PIN are required");
    }
    
    std::string username = body["username"].s();
    std::string pin = body["pin"].s();
    
    // Find user
    auto user = userRepository_->findByUsername(username);
    if (!user) {
        return errorResponse(401, "Invalid credentials");
    }
    
    // Verify PIN
    std::string hashedPin = UserUtils::hashPin(pin);
    if (user->getPinHash() != hashedPin) {
        return errorResponse(401, "Invalid credentials");
    }
    
    // Create session
    auto& auth = AuthMiddleware::getInstance();
    std::string token = auth.createSession(user->getId(), user->getUsername(), user->isAdmin());
    
    crow::json::wvalue response;
    response["token"] = token;
    response["user"]["id"] = user->getId();
    response["user"]["username"] = user->getUsername();
    response["user"]["userType"] = userTypeToString(user->getUserType());
    
    return successResponse(response);
}

crow::response UserController::logout(const crow::request& req) {
    auto token = AuthMiddleware::getInstance().extractToken(req);
    if (!token.empty()) {
        AuthMiddleware::getInstance().destroySession(token);
    }
    
    crow::json::wvalue response;
    response["message"] = "Logged out successfully";
    return successResponse(response);
}

crow::response UserController::getCurrentUser(const crow::request& req) {
    REQUIRE_AUTH(req)
    
    auto user = userRepository_->findById(session->userId);
    if (!user) {
        return errorResponse(404, "User not found");
    }
    
    crow::json::wvalue response;
    response["id"] = user->getId();
    response["username"] = user->getUsername();
    response["userType"] = userTypeToString(user->getUserType());
    response["createdAt"] = user->getCreatedAt();
    
    return successResponse(response);
}

crow::response UserController::getUsers(const crow::request& req) {
    REQUIRE_ADMIN(req)
    
    auto users = userRepository_->findAll();
    
    crow::json::wvalue response;
    response["users"] = crow::json::wvalue(crow::json::type::List);
    
    for (size_t i = 0; i < users.size(); ++i) {
        response["users"][i]["id"] = users[i].getId();
        response["users"][i]["username"] = users[i].getUsername();
        response["users"][i]["userType"] = userTypeToString(users[i].getUserType());
        response["users"][i]["createdAt"] = users[i].getCreatedAt();
    }
    
    return successResponse(response);
}

crow::response UserController::getUser(const crow::request& req, int id) {
    REQUIRE_AUTH(req)
    
    // Users can only view their own profile unless they're admin
    if (!session->isAdmin && session->userId != id) {
        return errorResponse(403, "Access denied");
    }
    
    auto user = userRepository_->findById(id);
    if (!user) {
        return errorResponse(404, "User not found");
    }
    
    crow::json::wvalue response;
    response["id"] = user->getId();
    response["username"] = user->getUsername();
    response["userType"] = userTypeToString(user->getUserType());
    response["createdAt"] = user->getCreatedAt();
    response["updatedAt"] = user->getUpdatedAt();
    
    return successResponse(response);
}

crow::response UserController::createUser(const crow::request& req) {
    REQUIRE_ADMIN(req)
    
    auto body = crow::json::load(req.body);
    if (!body) {
        return errorResponse(400, "Invalid JSON");
    }
    
    if (!body.has("username") || !body.has("pin")) {
        return errorResponse(400, "Username and PIN are required");
    }
    
    std::string username = body["username"].s();
    std::string pin = body["pin"].s();
    std::string userTypeStr = body.has("userType") ? std::string(body["userType"].s()) : std::string("standard");
    
    // Validate PIN
    if (!UserUtils::isValidPin(pin)) {
        return errorResponse(400, "PIN must be 4-6 digits");
    }
    
    // Check if username already exists
    if (userRepository_->existsByUsername(username)) {
        return errorResponse(409, "Username already exists");
    }
    
    // Create user
    User newUser(username, UserUtils::hashPin(pin), stringToUserType(userTypeStr));
    auto createdUser = userRepository_->create(newUser);
    
    if (!createdUser) {
        return errorResponse(500, "Failed to create user");
    }
    
    crow::json::wvalue response;
    response["id"] = createdUser->getId();
    response["username"] = createdUser->getUsername();
    response["userType"] = userTypeToString(createdUser->getUserType());
    response["createdAt"] = createdUser->getCreatedAt();
    
    return successResponse(response, 201);
}

crow::response UserController::updateUser(const crow::request& req, int id) {
    REQUIRE_AUTH(req)
    
    // Users can only update their own profile unless they're admin
    if (!session->isAdmin && session->userId != id) {
        return errorResponse(403, "Access denied");
    }
    
    auto user = userRepository_->findById(id);
    if (!user) {
        return errorResponse(404, "User not found");
    }
    
    auto body = crow::json::load(req.body);
    if (!body) {
        return errorResponse(400, "Invalid JSON");
    }
    
    // Update PIN if provided
    if (body.has("pin")) {
        std::string pin = body["pin"].s();
        if (!UserUtils::isValidPin(pin)) {
            return errorResponse(400, "PIN must be 4-6 digits");
        }
        user->setPinHash(UserUtils::hashPin(pin));
    }
    
    // Update user type if admin
    if (session->isAdmin && body.has("userType")) {
        user->setUserType(stringToUserType(body["userType"].s()));
    }
    
    if (!userRepository_->update(*user)) {
        return errorResponse(500, "Failed to update user");
    }
    
    crow::json::wvalue response;
    response["message"] = "User updated successfully";
    return successResponse(response);
}

crow::response UserController::deleteUser(const crow::request& req, int id) {
    REQUIRE_ADMIN(req)
    
    // Prevent deleting own account
    if (session->userId == id) {
        return errorResponse(400, "Cannot delete your own account");
    }
    
    if (!userRepository_->deleteById(id)) {
        return errorResponse(404, "User not found");
    }
    
    crow::json::wvalue response;
    response["message"] = "User deleted successfully";
    return successResponse(response);
}
