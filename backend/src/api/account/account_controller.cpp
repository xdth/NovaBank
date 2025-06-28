#include "api/account/account_controller.h"
#include "api/shared/error_response.h"
#include "api/shared/auth_middleware.h"
#include "domain/account/account_utils.h"
#include <crow/json.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

AccountController::AccountController(std::shared_ptr<Database> db) 
    : db_(db),
      accountRepository_(std::make_unique<AccountRepository>(db)),
      userRepository_(std::make_unique<UserRepository>(db)) {}

void AccountController::registerRoutes(crow::App<crow::CORSHandler>& app) {
    // Account routes
    CROW_ROUTE(app, "/api/v1/accounts")
        .methods("GET"_method)
        ([this](const crow::request& req) { return getAccounts(req); });
    
    CROW_ROUTE(app, "/api/v1/accounts/<int>")
        .methods("GET"_method)
        ([this](const crow::request& req, int accountId) { return getAccount(req, accountId); });
    
    CROW_ROUTE(app, "/api/v1/accounts")
        .methods("POST"_method)
        ([this](const crow::request& req) { return createAccount(req); });
    
    CROW_ROUTE(app, "/api/v1/accounts/<int>/transfer")
        .methods("POST"_method)
        ([this](const crow::request& req, int accountId) { return transfer(req, accountId); });
}

crow::response AccountController::getAccounts(const crow::request& req) {
    REQUIRE_AUTH(req)
    
    std::vector<Account> accounts;
    
    // Admin can see all accounts
    if (session->isAdmin) {
        accounts = accountRepository_->findAll();
    } else {
        // Regular users see only their accounts
        accounts = accountRepository_->findByUserId(session->userId);
    }
    
    crow::json::wvalue response;
    response["accounts"] = crow::json::wvalue(crow::json::type::List);
    
    for (size_t i = 0; i < accounts.size(); ++i) {
        response["accounts"][i] = accountToJson(accounts[i]);
    }
    
    // Add total balance
    double totalBalance = 0.0;
    for (const auto& account : accounts) {
        totalBalance += account.getBalance();
    }
    response["totalBalance"] = AccountUtils::roundToTwoDecimals(totalBalance);
    
    return successResponse(response);
}

crow::response AccountController::getAccount(const crow::request& req, int accountId) {
    REQUIRE_AUTH(req)
    
    auto account = accountRepository_->findById(accountId);
    if (!account) {
        return errorResponse(404, "Account not found");
    }
    
    // Check access rights
    if (!session->isAdmin && account->getUserId() != session->userId) {
        return errorResponse(403, "Access denied");
    }
    
    crow::json::wvalue response = accountToJson(*account);
    return successResponse(response);
}

crow::response AccountController::createAccount(const crow::request& req) {
    REQUIRE_AUTH(req)
    
    auto body = crow::json::load(req.body);
    if (!body) {
        return errorResponse(400, "Invalid JSON");
    }
    
    // Determine user ID
    int userId = session->userId;
    if (session->isAdmin && body.has("userId")) {
        userId = body["userId"].i();
    }
    
    // Verify user exists
    auto user = userRepository_->findById(userId);
    if (!user) {
        return errorResponse(404, "User not found");
    }
    
    // Get account type
    std::string accountTypeStr = body.has("accountType") ? std::string(body["accountType"].s()) : "checking";
    AccountType accountType = stringToAccountType(accountTypeStr);
    
    // Initial balance (default 0, admin can set initial balance)
    double initialBalance = 0.0;
    if (session->isAdmin && body.has("initialBalance")) {
        initialBalance = body["initialBalance"].d();
        if (initialBalance < 0) {
            return errorResponse(400, "Initial balance cannot be negative");
        }
    }
    
    // Generate unique account number
    std::string accountNumber;
    do {
        accountNumber = AccountUtils::generateAccountNumber();
    } while (accountRepository_->existsByAccountNumber(accountNumber));
    
    // Create account
    Account newAccount(userId, accountNumber, accountType, initialBalance);
    auto createdAccount = accountRepository_->create(newAccount);
    
    if (!createdAccount) {
        return errorResponse(500, "Failed to create account");
    }
    
    crow::json::wvalue response = accountToJson(*createdAccount);
    response["message"] = "Account created successfully";
    
    return successResponse(response, 201);
}

crow::response AccountController::transfer(const crow::request& req, int accountId) {
    REQUIRE_AUTH(req)
    
    auto body = crow::json::load(req.body);
    if (!body) {
        return errorResponse(400, "Invalid JSON");
    }
    
    double amount;
    std::string toAccountNumber;
    std::string description;
    
    if (!validateTransferRequest(body, amount, toAccountNumber, description)) {
        return errorResponse(400, "Invalid transfer request");
    }
    
    // Get source account
    auto fromAccount = accountRepository_->findById(accountId);
    if (!fromAccount) {
        return errorResponse(404, "Source account not found");
    }
    
    // Check ownership
    if (!session->isAdmin && fromAccount->getUserId() != session->userId) {
        return errorResponse(403, "Access denied");
    }
    
    // Get destination account
    auto toAccount = accountRepository_->findByAccountNumber(toAccountNumber);
    if (!toAccount) {
        return errorResponse(404, "Destination account not found");
    }
    
    // Check if source has sufficient funds
    if (!fromAccount->canWithdraw(amount)) {
        crow::json::wvalue response;
        response["error"] = "Insufficient funds";
        response["currentBalance"] = fromAccount->getBalance();
        response["requestedAmount"] = amount;
        if (fromAccount->getAccountType() == AccountType::Savings) {
            response["minimumBalance"] = 25.0;
        }
        return crow::response(400, response);
    }
    
    // Begin transaction
    if (!db_->beginTransaction()) {
        return errorResponse(500, "Failed to start transaction");
    }
    
    // Perform transfer
    fromAccount->withdraw(amount);
    toAccount->deposit(amount);
    
    // Update both accounts
    bool success = accountRepository_->update(*fromAccount) && 
                   accountRepository_->update(*toAccount);
    
    if (success) {
        db_->commit();
        
        crow::json::wvalue response;
        response["message"] = "Transfer completed successfully";
        response["transferDetails"]["from"]["accountNumber"] = fromAccount->getAccountNumber();
        response["transferDetails"]["from"]["newBalance"] = AccountUtils::roundToTwoDecimals(fromAccount->getBalance());
        response["transferDetails"]["to"]["accountNumber"] = toAccount->getAccountNumber();
        response["transferDetails"]["to"]["newBalance"] = AccountUtils::roundToTwoDecimals(toAccount->getBalance());
        response["transferDetails"]["amount"] = AccountUtils::roundToTwoDecimals(amount);
        response["transferDetails"]["description"] = description;
        // Create timestamp
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream timestamp;
        timestamp << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        
        response["transferDetails"]["timestamp"] = timestamp.str();
        
        return successResponse(response);
    } else {
        db_->rollback();
        return errorResponse(500, "Failed to complete transfer");
    }
}

bool AccountController::validateTransferRequest(const crow::json::rvalue& body, 
                                               double& amount, 
                                               std::string& toAccountNumber, 
                                               std::string& description) {
    if (!body.has("amount") || !body.has("toAccountNumber")) {
        return false;
    }
    
    amount = body["amount"].d();
    toAccountNumber = body["toAccountNumber"].s();
    description = body.has("description") ? std::string(body["description"].s()) : "Transfer";
    
    // Validate amount
    if (!AccountUtils::isValidAmount(amount)) {
        return false;
    }
    
    // Round amount to 2 decimal places
    amount = AccountUtils::roundToTwoDecimals(amount);
    
    return true;
}

crow::json::wvalue AccountController::accountToJson(const Account& account) {
    crow::json::wvalue json;
    json["id"] = account.getId();
    json["userId"] = account.getUserId();
    json["accountNumber"] = account.getAccountNumber();
    json["accountType"] = accountTypeToString(account.getAccountType());
    json["balance"] = AccountUtils::roundToTwoDecimals(account.getBalance());
    json["formattedBalance"] = AccountUtils::formatCurrency(account.getBalance());
    json["createdAt"] = account.getCreatedAt();
    json["updatedAt"] = account.getUpdatedAt();
    
    return json;
}
