#pragma once

#include <crow.h>
#include <crow/middlewares/cors.h>
#include <memory>
#include "repository/account/account_repository.h"
#include "repository/user/user_repository.h"
#include "db/db.h"

class AccountController {
public:
    AccountController(std::shared_ptr<Database> db);
    
    void registerRoutes(crow::App<crow::CORSHandler>& app);
    
private:
    std::shared_ptr<Database> db_;
    std::unique_ptr<AccountRepository> accountRepository_;
    std::unique_ptr<UserRepository> userRepository_;
    
    // Account endpoints
    crow::response getAccounts(const crow::request& req);
    crow::response getAccount(const crow::request& req, int accountId);
    crow::response createAccount(const crow::request& req);
    crow::response transfer(const crow::request& req, int accountId);
    
    // Helper methods
    bool validateTransferRequest(const crow::json::rvalue& body, double& amount, std::string& toAccountNumber, std::string& description);
    crow::json::wvalue accountToJson(const Account& account);
};
