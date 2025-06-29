#pragma once

#include <crow.h>
#include <crow/middlewares/cors.h>
#include <memory>
#include "repository/transaction/transaction_repository.h"
#include "repository/account/account_repository.h"
#include "db/db.h"

class TransactionController {
public:
    TransactionController(std::shared_ptr<Database> db);
    
    void registerRoutes(crow::App<crow::CORSHandler>& app);
    
private:
    std::shared_ptr<Database> db_;
    std::unique_ptr<TransactionRepository> transactionRepository_;
    std::unique_ptr<AccountRepository> accountRepository_;
    
    // Transaction endpoints
    crow::response getTransactions(const crow::request& req);
    crow::response getTransaction(const crow::request& req, int id);
    crow::response deposit(const crow::request& req);
    crow::response withdraw(const crow::request& req);
    crow::response transfer(const crow::request& req);
    
    // Helper methods
    crow::json::wvalue transactionToJson(const Transaction& transaction, std::optional<int> currentUserId = std::nullopt);
    bool processDeposit(int accountId, double amount, const std::string& description);
    bool processWithdrawal(int accountId, double amount, const std::string& description);
    bool processTransfer(int fromAccountId, int toAccountId, double amount, const std::string& description);
};
