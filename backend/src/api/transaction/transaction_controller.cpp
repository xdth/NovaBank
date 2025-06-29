#include "api/transaction/transaction_controller.h"
#include "api/shared/error_response.h"
#include "api/shared/auth_middleware.h"
#include "domain/account/account_utils.h"
#include "domain/transaction/transaction_utils.h"
#include <crow/json.h>
#include <iostream>

TransactionController::TransactionController(std::shared_ptr<Database> db) 
    : db_(db),
      transactionRepository_(std::make_unique<TransactionRepository>(db)),
      accountRepository_(std::make_unique<AccountRepository>(db)) {}

void TransactionController::registerRoutes(crow::App<crow::CORSHandler>& app) {
    // Transaction routes
    CROW_ROUTE(app, "/api/v1/transactions")
        .methods("GET"_method)
        ([this](const crow::request& req) { return getTransactions(req); });
    
    CROW_ROUTE(app, "/api/v1/transactions/<int>")
        .methods("GET"_method)
        ([this](const crow::request& req, int id) { return getTransaction(req, id); });
    
    CROW_ROUTE(app, "/api/v1/transactions/deposit")
        .methods("POST"_method)
        ([this](const crow::request& req) { return deposit(req); });
    
    CROW_ROUTE(app, "/api/v1/transactions/withdraw")
        .methods("POST"_method)
        ([this](const crow::request& req) { return withdraw(req); });
    
    CROW_ROUTE(app, "/api/v1/transactions/transfer")
        .methods("POST"_method)
        ([this](const crow::request& req) { return transfer(req); });
}

crow::response TransactionController::getTransactions(const crow::request& req) {
    REQUIRE_AUTH(req)
    
    // Parse query parameters
    auto accountIdStr = req.url_params.get("accountId");
    auto typeStr = req.url_params.get("type");
    auto startDate = req.url_params.get("startDate");
    auto endDate = req.url_params.get("endDate");
    auto limitStr = req.url_params.get("limit");
    auto offsetStr = req.url_params.get("offset");
    
    std::optional<int> accountId;
    if (accountIdStr) {
        accountId = std::stoi(accountIdStr);
        
        // Verify user owns this account (unless admin)
        if (!session->isAdmin) {
            auto account = accountRepository_->findById(accountId.value());
            if (!account || account->getUserId() != session->userId) {
                return errorResponse(403, "Access denied");
            }
        }
    }
    
    std::optional<TransactionType> type;
    if (typeStr) {
        type = stringToTransactionType(typeStr);
    }
    
    int limit = limitStr ? std::stoi(limitStr) : 100;
    int offset = offsetStr ? std::stoi(offsetStr) : 0;
    
    // Get transactions
    std::vector<Transaction> transactions;
    
    if (session->isAdmin && !accountId.has_value()) {
        // Admin can see all transactions if no account specified
        transactions = transactionRepository_->findWithFilters(
            std::nullopt, type, 
            startDate ? std::optional<std::string>(startDate) : std::nullopt,
            endDate ? std::optional<std::string>(endDate) : std::nullopt,
            limit, offset
        );
    } else if (accountId.has_value()) {
        // Get transactions for specific account
        transactions = transactionRepository_->findWithFilters(
            accountId, type,
            startDate ? std::optional<std::string>(startDate) : std::nullopt,
            endDate ? std::optional<std::string>(endDate) : std::nullopt,
            limit, offset
        );
    } else {
        // Get all transactions for user
        transactions = transactionRepository_->findByUserId(session->userId);
    }
    
    crow::json::wvalue response;
    response["transactions"] = crow::json::wvalue(crow::json::type::List);
    
    for (size_t i = 0; i < transactions.size(); ++i) {
        response["transactions"][i] = transactionToJson(transactions[i], session->userId);
    }
    
    response["count"] = static_cast<int>(transactions.size());
    response["total"] = transactionRepository_->getTransactionCount(accountId);
    response["limit"] = limit;
    response["offset"] = offset;
    
    return successResponse(response);
}

crow::response TransactionController::getTransaction(const crow::request& req, int id) {
    REQUIRE_AUTH(req)
    
    auto transaction = transactionRepository_->findById(id);
    if (!transaction) {
        return errorResponse(404, "Transaction not found");
    }
    
    // Check access rights
    if (!session->isAdmin) {
        bool hasAccess = false;
        
        // Check if user owns the from account
        if (transaction->getFromAccountId().has_value()) {
            auto fromAccount = accountRepository_->findById(transaction->getFromAccountId().value());
            if (fromAccount && fromAccount->getUserId() == session->userId) {
                hasAccess = true;
            }
        }
        
        // Check if user owns the to account
        if (!hasAccess && transaction->getToAccountId().has_value()) {
            auto toAccount = accountRepository_->findById(transaction->getToAccountId().value());
            if (toAccount && toAccount->getUserId() == session->userId) {
                hasAccess = true;
            }
        }
        
        if (!hasAccess) {
            return errorResponse(403, "Access denied");
        }
    }
    
    crow::json::wvalue response = transactionToJson(*transaction, session->userId);
    return successResponse(response);
}

crow::response TransactionController::deposit(const crow::request& req) {
    REQUIRE_AUTH(req)
    
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
        std::string(body["description"].s()) : "Deposit";
    
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
    
    // Check ownership (unless admin)
    if (!session->isAdmin && account->getUserId() != session->userId) {
        return errorResponse(403, "Access denied");
    }
    
    // Process deposit
    if (!processDeposit(account->getId(), amount, description)) {
        return errorResponse(500, "Failed to process deposit");
    }
    
    // Get updated account balance
    account = accountRepository_->findById(account->getId());
    
    crow::json::wvalue response;
    response["message"] = "Deposit successful";
    response["transactionDetails"]["accountNumber"] = account->getAccountNumber();
    response["transactionDetails"]["amount"] = amount;
    response["transactionDetails"]["newBalance"] = AccountUtils::roundToTwoDecimals(account->getBalance());
    response["transactionDetails"]["formattedBalance"] = AccountUtils::formatCurrency(account->getBalance());
    response["transactionDetails"]["description"] = description;
    
    return successResponse(response);
}

crow::response TransactionController::withdraw(const crow::request& req) {
    REQUIRE_AUTH(req)
    
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
        std::string(body["description"].s()) : "Withdrawal";
    
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
    
    // Check ownership (unless admin)
    if (!session->isAdmin && account->getUserId() != session->userId) {
        return errorResponse(403, "Access denied");
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
    if (!processWithdrawal(account->getId(), amount, description)) {
        return errorResponse(500, "Failed to process withdrawal");
    }
    
    // Get updated account balance
    account = accountRepository_->findById(account->getId());
    
    crow::json::wvalue response;
    response["message"] = "Withdrawal successful";
    response["transactionDetails"]["accountNumber"] = account->getAccountNumber();
    response["transactionDetails"]["amount"] = amount;
    response["transactionDetails"]["newBalance"] = AccountUtils::roundToTwoDecimals(account->getBalance());
    response["transactionDetails"]["formattedBalance"] = AccountUtils::formatCurrency(account->getBalance());
    response["transactionDetails"]["description"] = description;
    
    return successResponse(response);
}

crow::response TransactionController::transfer(const crow::request& req) {
    REQUIRE_AUTH(req)
    
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
        std::string(body["description"].s()) : "Transfer";
    
    // Validate amount
    if (!AccountUtils::isValidAmount(amount)) {
        return errorResponse(400, "Invalid amount");
    }
    amount = AccountUtils::roundToTwoDecimals(amount);
    
    // Get source account
    auto fromAccount = accountRepository_->findByAccountNumber(fromAccountNumber);
    if (!fromAccount) {
        return errorResponse(404, "Source account not found");
    }
    
    // Check ownership (unless admin)
    if (!session->isAdmin && fromAccount->getUserId() != session->userId) {
        return errorResponse(403, "Access denied");
    }
    
    // Get destination account
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
    if (!processTransfer(fromAccount->getId(), toAccount->getId(), amount, description)) {
        return errorResponse(500, "Failed to process transfer");
    }
    
    // Get updated account balances
    fromAccount = accountRepository_->findById(fromAccount->getId());
    toAccount = accountRepository_->findById(toAccount->getId());
    
    crow::json::wvalue response;
    response["message"] = "Transfer successful";
    response["transferDetails"]["from"]["accountNumber"] = fromAccount->getAccountNumber();
    response["transferDetails"]["from"]["newBalance"] = AccountUtils::roundToTwoDecimals(fromAccount->getBalance());
    response["transferDetails"]["to"]["accountNumber"] = toAccount->getAccountNumber();
    response["transferDetails"]["to"]["newBalance"] = AccountUtils::roundToTwoDecimals(toAccount->getBalance());
    response["transferDetails"]["amount"] = amount;
    response["transferDetails"]["description"] = description;
    
    return successResponse(response);
}

crow::json::wvalue TransactionController::transactionToJson(const Transaction& transaction, 
                                                           std::optional<int> currentUserId) {
    crow::json::wvalue json;
    json["id"] = transaction.getId();
    json["type"] = transactionTypeToString(transaction.getTransactionType());
    json["amount"] = AccountUtils::roundToTwoDecimals(transaction.getAmount());
    json["formattedAmount"] = AccountUtils::formatCurrency(transaction.getAmount());
    json["description"] = transaction.getDescription();
    json["status"] = transactionStatusToString(transaction.getStatus());
    json["createdAt"] = transaction.getCreatedAt();
    
    // Add account details
    if (transaction.getFromAccountId().has_value()) {
        auto fromAccount = accountRepository_->findById(transaction.getFromAccountId().value());
        if (fromAccount) {
            json["fromAccount"]["id"] = fromAccount->getId();
            json["fromAccount"]["accountNumber"] = fromAccount->getAccountNumber();
            json["fromAccount"]["accountType"] = accountTypeToString(fromAccount->getAccountType());
        }
    }
    
    if (transaction.getToAccountId().has_value()) {
        auto toAccount = accountRepository_->findById(transaction.getToAccountId().value());
        if (toAccount) {
            json["toAccount"]["id"] = toAccount->getId();
            json["toAccount"]["accountNumber"] = toAccount->getAccountNumber();
            json["toAccount"]["accountType"] = accountTypeToString(toAccount->getAccountType());
        }
    }
    
    // Add direction and sign for current user's perspective
    if (currentUserId.has_value()) {
        // Find which account belongs to the current user
        int userAccountId = 0;
        if (transaction.getFromAccountId().has_value()) {
            auto account = accountRepository_->findById(transaction.getFromAccountId().value());
            if (account && account->getUserId() == currentUserId.value()) {
                userAccountId = account->getId();
            }
        }
        if (userAccountId == 0 && transaction.getToAccountId().has_value()) {
            auto account = accountRepository_->findById(transaction.getToAccountId().value());
            if (account && account->getUserId() == currentUserId.value()) {
                userAccountId = account->getId();
            }
        }
        
        if (userAccountId > 0) {
            json["direction"] = TransactionUtils::getTransactionDirection(transaction, userAccountId);
            std::string sign = TransactionUtils::getAmountSign(transaction, userAccountId);
            json["displayAmount"] = sign + AccountUtils::formatCurrency(transaction.getAmount());
        }
    }
    
    return json;
}

bool TransactionController::processDeposit(int accountId, double amount, const std::string& description) {
    // Begin transaction
    if (!db_->beginTransaction()) {
        return false;
    }
    
    // Get account
    auto account = accountRepository_->findById(accountId);
    if (!account) {
        db_->rollback();
        return false;
    }
    
    // Update account balance
    account->deposit(amount);
    if (!accountRepository_->update(*account)) {
        db_->rollback();
        return false;
    }
    
    // Create transaction record
    Transaction transaction(std::nullopt, accountId, amount, 
                          TransactionType::Deposit, description);
    
    if (!transactionRepository_->create(transaction)) {
        db_->rollback();
        return false;
    }
    
    db_->commit();
    return true;
}

bool TransactionController::processWithdrawal(int accountId, double amount, const std::string& description) {
    // Begin transaction
    if (!db_->beginTransaction()) {
        return false;
    }
    
    // Get account
    auto account = accountRepository_->findById(accountId);
    if (!account || !account->canWithdraw(amount)) {
        db_->rollback();
        return false;
    }
    
    // Update account balance
    account->withdraw(amount);
    if (!accountRepository_->update(*account)) {
        db_->rollback();
        return false;
    }
    
    // Create transaction record
    Transaction transaction(accountId, std::nullopt, amount, 
                          TransactionType::Withdrawal, description);
    
    if (!transactionRepository_->create(transaction)) {
        db_->rollback();
        return false;
    }
    
    db_->commit();
    return true;
}

bool TransactionController::processTransfer(int fromAccountId, int toAccountId, 
                                          double amount, const std::string& description) {
    // Begin transaction
    if (!db_->beginTransaction()) {
        return false;
    }
    
    // Get both accounts
    auto fromAccount = accountRepository_->findById(fromAccountId);
    auto toAccount = accountRepository_->findById(toAccountId);
    
    if (!fromAccount || !toAccount || !fromAccount->canWithdraw(amount)) {
        db_->rollback();
        return false;
    }
    
    // Update account balances
    fromAccount->withdraw(amount);
    toAccount->deposit(amount);
    
    if (!accountRepository_->update(*fromAccount) || !accountRepository_->update(*toAccount)) {
        db_->rollback();
        return false;
    }
    
    // Create transaction record
    Transaction transaction(fromAccountId, toAccountId, amount, 
                          TransactionType::Transfer, description);
    
    if (!transactionRepository_->create(transaction)) {
        db_->rollback();
        return false;
    }
    
    db_->commit();
    return true;
}
