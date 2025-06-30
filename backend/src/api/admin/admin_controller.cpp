#include "api/admin/admin_controller.h"
#include "api/shared/error_response.h"
#include "api/shared/auth_middleware.h"
#include "domain/account/account_utils.h"
#include "domain/transaction/transaction_utils.h"
#include <crow/json.h>
#include <iostream>

AdminController::AdminController(std::shared_ptr<Database> db) 
    : db_(db),
      userRepository_(std::make_unique<UserRepository>(db)),
      accountRepository_(std::make_unique<AccountRepository>(db)),
      transactionRepository_(std::make_unique<TransactionRepository>(db)) {}

void AdminController::registerRoutes(crow::App<crow::CORSHandler>& app) {
    // Admin routes
    CROW_ROUTE(app, "/api/v1/admin/users")
        .methods("GET"_method)
        ([this](const crow::request& req) { return getUsersWithBalances(req); });
    
    CROW_ROUTE(app, "/api/v1/admin/deposit")
        .methods("POST"_method)
        ([this](const crow::request& req) { return adminDeposit(req); });
    
    CROW_ROUTE(app, "/api/v1/admin/withdraw")
        .methods("POST"_method)
        ([this](const crow::request& req) { return adminWithdraw(req); });
    
    CROW_ROUTE(app, "/api/v1/admin/transfer")
        .methods("POST"_method)
        ([this](const crow::request& req) { return adminTransfer(req); });
}

crow::response AdminController::getUsersWithBalances(const crow::request& req) {
    REQUIRE_ADMIN(req)
    
    auto users = userRepository_->findAll();
    
    crow::json::wvalue response;
    response["users"] = crow::json::wvalue(crow::json::type::List);
    
    double systemTotalBalance = 0.0;
    
    for (size_t i = 0; i < users.size(); ++i) {
        crow::json::wvalue userJson = userWithBalanceToJson(users[i]);
        
        // Get user's total balance
        auto accounts = accountRepository_->findByUserId(users[i].getId());
        double userTotal = 0.0;
        for (const auto& account : accounts) {
            userTotal += account.getBalance();
        }
        systemTotalBalance += userTotal;
        
        response["users"][i] = std::move(userJson);
    }
    
    response["systemTotalBalance"] = AccountUtils::roundToTwoDecimals(systemTotalBalance);
    response["formattedSystemTotal"] = AccountUtils::formatCurrency(systemTotalBalance);
    response["userCount"] = static_cast<int>(users.size());
    
    return successResponse(response);
}

crow::response AdminController::adminDeposit(const crow::request& req) {
    REQUIRE_ADMIN(req)
    
    auto body = crow::json::load(req.body);
    if (!body) {
        return errorResponse(400, "Invalid JSON");
    }
    
    if (!body.has("accountNumber") || !body.has("amount")) {
        return errorResponse(400, "Account number and amount are required");
    }
    
    std::string accountNumber = body["accountNumber"].s();
    double amount = body["amount"].d();
    std::string description = body.has("description") ? 
        std::string(body["description"].s()) : "Admin deposit";
    
    // Validate amount
    if (!AccountUtils::isValidAmount(amount)) {
        return errorResponse(400, "Invalid amount");
    }
    amount = AccountUtils::roundToTwoDecimals(amount);
    
    // Get account
    auto account = accountRepository_->findByAccountNumber(accountNumber);
    if (!account) {
        return errorResponse(404, "Account not found");
    }
    
    // Process deposit
    if (!processAdminDeposit(accountNumber, amount, description)) {
        return errorResponse(500, "Failed to process deposit");
    }
    
    // Get updated account
    account = accountRepository_->findByAccountNumber(accountNumber);
    
    // Get user info
    auto user = userRepository_->findById(account->getUserId());
    
    crow::json::wvalue response;
    response["message"] = "Admin deposit successful";
    response["transactionDetails"]["accountNumber"] = account->getAccountNumber();
    response["transactionDetails"]["accountType"] = accountTypeToString(account->getAccountType());
    response["transactionDetails"]["username"] = user->getUsername();
    response["transactionDetails"]["amount"] = amount;
    response["transactionDetails"]["newBalance"] = AccountUtils::roundToTwoDecimals(account->getBalance());
    response["transactionDetails"]["formattedBalance"] = AccountUtils::formatCurrency(account->getBalance());
    response["transactionDetails"]["description"] = description;
    response["transactionDetails"]["adminUser"] = session->username;
    
    return successResponse(response);
}

crow::response AdminController::adminWithdraw(const crow::request& req) {
    REQUIRE_ADMIN(req)
    
    auto body = crow::json::load(req.body);
    if (!body) {
        return errorResponse(400, "Invalid JSON");
    }
    
    if (!body.has("accountNumber") || !body.has("amount")) {
        return errorResponse(400, "Account number and amount are required");
    }
    
    std::string accountNumber = body["accountNumber"].s();
    double amount = body["amount"].d();
    std::string description = body.has("description") ? 
        std::string(body["description"].s()) : "Admin withdrawal";
    
    // Validate amount
    if (!AccountUtils::isValidAmount(amount)) {
        return errorResponse(400, "Invalid amount");
    }
    amount = AccountUtils::roundToTwoDecimals(amount);
    
    // Get account
    auto account = accountRepository_->findByAccountNumber(accountNumber);
    if (!account) {
        return errorResponse(404, "Account not found");
    }
    
    // Check sufficient funds
    if (!account->canWithdraw(amount)) {
        crow::json::wvalue error;
        error["error"] = "Insufficient funds";
        error["currentBalance"] = account->getBalance();
        error["requestedAmount"] = amount;
        if (account->getAccountType() == AccountType::Savings) {
            error["minimumBalance"] = 25.0;
        }
        return crow::response(400, error);
    }
    
    // Process withdrawal
    if (!processAdminWithdrawal(accountNumber, amount, description)) {
        return errorResponse(500, "Failed to process withdrawal");
    }
    
    // Get updated account
    account = accountRepository_->findByAccountNumber(accountNumber);
    
    // Get user info
    auto user = userRepository_->findById(account->getUserId());
    
    crow::json::wvalue response;
    response["message"] = "Admin withdrawal successful";
    response["transactionDetails"]["accountNumber"] = account->getAccountNumber();
    response["transactionDetails"]["accountType"] = accountTypeToString(account->getAccountType());
    response["transactionDetails"]["username"] = user->getUsername();
    response["transactionDetails"]["amount"] = amount;
    response["transactionDetails"]["newBalance"] = AccountUtils::roundToTwoDecimals(account->getBalance());
    response["transactionDetails"]["formattedBalance"] = AccountUtils::formatCurrency(account->getBalance());
    response["transactionDetails"]["description"] = description;
    response["transactionDetails"]["adminUser"] = session->username;
    
    return successResponse(response);
}

crow::response AdminController::adminTransfer(const crow::request& req) {
    REQUIRE_ADMIN(req)
    
    auto body = crow::json::load(req.body);
    if (!body) {
        return errorResponse(400, "Invalid JSON");
    }
    
    if (!body.has("fromAccountNumber") || !body.has("toAccountNumber") || !body.has("amount")) {
        return errorResponse(400, "From account number, to account number, and amount are required");
    }
    
    std::string fromAccountNumber = body["fromAccountNumber"].s();
    std::string toAccountNumber = body["toAccountNumber"].s();
    double amount = body["amount"].d();
    std::string description = body.has("description") ? 
        std::string(body["description"].s()) : "Admin transfer";
    
    // Validate amount
    if (!AccountUtils::isValidAmount(amount)) {
        return errorResponse(400, "Invalid amount");
    }
    amount = AccountUtils::roundToTwoDecimals(amount);
    
    // Get accounts
    auto fromAccount = accountRepository_->findByAccountNumber(fromAccountNumber);
    if (!fromAccount) {
        return errorResponse(404, "Source account not found");
    }
    
    auto toAccount = accountRepository_->findByAccountNumber(toAccountNumber);
    if (!toAccount) {
        return errorResponse(404, "Destination account not found");
    }
    
    // Check not transferring to same account
    if (fromAccount->getId() == toAccount->getId()) {
        return errorResponse(400, "Cannot transfer to the same account");
    }
    
    // Check sufficient funds
    if (!fromAccount->canWithdraw(amount)) {
        crow::json::wvalue error;
        error["error"] = "Insufficient funds";
        error["currentBalance"] = fromAccount->getBalance();
        error["requestedAmount"] = amount;
        if (fromAccount->getAccountType() == AccountType::Savings) {
            error["minimumBalance"] = 25.0;
        }
        return crow::response(400, error);
    }
    
    // Process transfer
    if (!processAdminTransfer(fromAccountNumber, toAccountNumber, amount, description)) {
        return errorResponse(500, "Failed to process transfer");
    }
    
    // Get updated accounts
    fromAccount = accountRepository_->findByAccountNumber(fromAccountNumber);
    toAccount = accountRepository_->findByAccountNumber(toAccountNumber);
    
    // Get user info
    auto fromUser = userRepository_->findById(fromAccount->getUserId());
    auto toUser = userRepository_->findById(toAccount->getUserId());
    
    crow::json::wvalue response;
    response["message"] = "Admin transfer successful";
    response["transferDetails"]["from"]["accountNumber"] = fromAccount->getAccountNumber();
    response["transferDetails"]["from"]["username"] = fromUser->getUsername();
    response["transferDetails"]["from"]["newBalance"] = AccountUtils::roundToTwoDecimals(fromAccount->getBalance());
    response["transferDetails"]["to"]["accountNumber"] = toAccount->getAccountNumber();
    response["transferDetails"]["to"]["username"] = toUser->getUsername();
    response["transferDetails"]["to"]["newBalance"] = AccountUtils::roundToTwoDecimals(toAccount->getBalance());
    response["transferDetails"]["amount"] = amount;
    response["transferDetails"]["description"] = description;
    response["transferDetails"]["adminUser"] = session->username;
    
    return successResponse(response);
}

crow::json::wvalue AdminController::userWithBalanceToJson(const User& user) {
    crow::json::wvalue json;
    json["id"] = user.getId();
    json["username"] = user.getUsername();
    json["userType"] = userTypeToString(user.getUserType());
    json["createdAt"] = user.getCreatedAt();
    
    // Get user's accounts
    auto accounts = accountRepository_->findByUserId(user.getId());
    json["accounts"] = crow::json::wvalue(crow::json::type::List);
    
    double totalBalance = 0.0;
    for (size_t i = 0; i < accounts.size(); ++i) {
        crow::json::wvalue accountJson;
        accountJson["id"] = accounts[i].getId();
        accountJson["accountNumber"] = accounts[i].getAccountNumber();
        accountJson["accountType"] = accountTypeToString(accounts[i].getAccountType());
        accountJson["balance"] = AccountUtils::roundToTwoDecimals(accounts[i].getBalance());
        accountJson["formattedBalance"] = AccountUtils::formatCurrency(accounts[i].getBalance());
        
        json["accounts"][i] = std::move(accountJson);
        totalBalance += accounts[i].getBalance();
    }
    
    json["accountCount"] = static_cast<int>(accounts.size());
    json["totalBalance"] = AccountUtils::roundToTwoDecimals(totalBalance);
    json["formattedTotalBalance"] = AccountUtils::formatCurrency(totalBalance);
    
    return json;
}

bool AdminController::processAdminDeposit(const std::string& accountNumber, double amount, const std::string& description) {
    auto account = accountRepository_->findByAccountNumber(accountNumber);
    if (!account) {
        return false;
    }
    
    // Begin transaction
    if (!db_->beginTransaction()) {
        return false;
    }
    
    try {
        // Update account balance
        account->deposit(amount);
        if (!accountRepository_->update(*account)) {
            db_->rollback();
            return false;
        }
        
        // Create transaction record
        Transaction transaction(std::nullopt, account->getId(), amount, 
                              TransactionType::Deposit, description);
        
        auto createdTransaction = transactionRepository_->create(transaction);
        if (!createdTransaction) {
            db_->rollback();
            return false;
        }
        
        if (!db_->commit()) {
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        db_->rollback();
        return false;
    }
}

bool AdminController::processAdminWithdrawal(const std::string& accountNumber, double amount, const std::string& description) {
    auto account = accountRepository_->findByAccountNumber(accountNumber);
    if (!account || !account->canWithdraw(amount)) {
        return false;
    }
    
    // Begin transaction
    if (!db_->beginTransaction()) {
        return false;
    }
    
    try {
        // Update account balance
        account->withdraw(amount);
        if (!accountRepository_->update(*account)) {
            db_->rollback();
            return false;
        }
        
        // Create transaction record
        Transaction transaction(account->getId(), std::nullopt, amount, 
                              TransactionType::Withdrawal, description);
        
        auto createdTransaction = transactionRepository_->create(transaction);
        if (!createdTransaction) {
            db_->rollback();
            return false;
        }
        
        if (!db_->commit()) {
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        db_->rollback();
        return false;
    }
}

bool AdminController::processAdminTransfer(const std::string& fromAccountNumber, 
                                          const std::string& toAccountNumber, 
                                          double amount, const std::string& description) {
    auto fromAccount = accountRepository_->findByAccountNumber(fromAccountNumber);
    auto toAccount = accountRepository_->findByAccountNumber(toAccountNumber);
    
    if (!fromAccount || !toAccount || !fromAccount->canWithdraw(amount)) {
        return false;
    }
    
    // Begin transaction
    if (!db_->beginTransaction()) {
        return false;
    }
    
    try {
        // Update account balances
        fromAccount->withdraw(amount);
        toAccount->deposit(amount);
        
        if (!accountRepository_->update(*fromAccount) || !accountRepository_->update(*toAccount)) {
            db_->rollback();
            return false;
        }
        
        // Create transaction record
        Transaction transaction(fromAccount->getId(), toAccount->getId(), amount, 
                              TransactionType::Transfer, description);
        
        auto createdTransaction = transactionRepository_->create(transaction);
        if (!createdTransaction) {
            db_->rollback();
            return false;
        }
        
        if (!db_->commit()) {
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        db_->rollback();
        return false;
    }
}
