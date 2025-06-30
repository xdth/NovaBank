#pragma once

#include <crow.h>
#include <crow/middlewares/cors.h>
#include <memory>
#include "repository/user/user_repository.h"
#include "repository/account/account_repository.h"
#include "repository/transaction/transaction_repository.h"
#include "db/db.h"

class AdminController {
public:
    AdminController(std::shared_ptr<Database> db);
    
    void registerRoutes(crow::App<crow::CORSHandler>& app);
    
private:
    std::shared_ptr<Database> db_;
    std::unique_ptr<UserRepository> userRepository_;
    std::unique_ptr<AccountRepository> accountRepository_;
    std::unique_ptr<TransactionRepository> transactionRepository_;
    
    // Admin endpoints
    crow::response getUsersWithBalances(const crow::request& req);
    crow::response adminDeposit(const crow::request& req);
    crow::response adminWithdraw(const crow::request& req);
    crow::response adminTransfer(const crow::request& req);
    
    // Helper methods
    crow::json::wvalue userWithBalanceToJson(const User& user);
    bool processAdminDeposit(const std::string& accountNumber, double amount, const std::string& description);
    bool processAdminWithdrawal(const std::string& accountNumber, double amount, const std::string& description);
    bool processAdminTransfer(const std::string& fromAccountNumber, const std::string& toAccountNumber, 
                             double amount, const std::string& description);
};
