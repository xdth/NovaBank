#pragma once

#include <crow.h>
#include <crow/middlewares/cors.h>
#include <memory>
#include "repository/user/user_repository.h"
#include "db/db.h"

class UserController {
public:
    UserController(std::shared_ptr<Database> db);
    
    void registerRoutes(crow::App<crow::CORSHandler>& app);
    
private:
    std::unique_ptr<UserRepository> userRepository_;
    
    // Auth endpoints
    crow::response login(const crow::request& req);
    crow::response logout(const crow::request& req);
    crow::response getCurrentUser(const crow::request& req);
    
    // User CRUD endpoints
    crow::response getUsers(const crow::request& req);
    crow::response getUser(const crow::request& req, int id);
    crow::response createUser(const crow::request& req);
    crow::response updateUser(const crow::request& req, int id);
    crow::response deleteUser(const crow::request& req, int id);
};
