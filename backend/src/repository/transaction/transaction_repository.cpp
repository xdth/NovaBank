#include "repository/transaction/transaction_repository.h"
#include <iostream>
#include <sstream>

TransactionRepository::TransactionRepository(std::shared_ptr<Database> db) : db_(db) {}

std::optional<Transaction> TransactionRepository::create(const Transaction& transaction) {
    if (!transaction.isValid()) {
        std::cerr << "Invalid transaction: " << transaction.getValidationError() << std::endl;
        return std::nullopt;
    }
    
    const std::string sql = "INSERT INTO transactions (from_account_id, to_account_id, amount, "
                           "transaction_type, description, status) VALUES (?, ?, ?, ?, ?, ?)";
    auto stmt = db_->prepare(sql);
    
    if (!stmt) {
        std::cerr << "Failed to prepare transaction insert statement" << std::endl;
        return std::nullopt;
    }
    
    // Bind from_account_id (can be NULL)
    if (transaction.getFromAccountId().has_value()) {
        sqlite3_bind_int(stmt.get(), 1, transaction.getFromAccountId().value());
    } else {
        sqlite3_bind_null(stmt.get(), 1);
    }
    
    // Bind to_account_id (can be NULL)
    if (transaction.getToAccountId().has_value()) {
        sqlite3_bind_int(stmt.get(), 2, transaction.getToAccountId().value());
    } else {
        sqlite3_bind_null(stmt.get(), 2);
    }
    
    sqlite3_bind_double(stmt.get(), 3, transaction.getAmount());
    sqlite3_bind_text(stmt.get(), 4, transactionTypeToString(transaction.getTransactionType()).c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt.get(), 5, transaction.getDescription().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt.get(), 6, transactionStatusToString(transaction.getStatus()).c_str(), -1, SQLITE_TRANSIENT);
    
    int rc = sqlite3_step(stmt.get());
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to create transaction: " << db_->getLastError() 
                  << " (rc=" << rc << ")" << std::endl;
        return std::nullopt;
    }
    
    int64_t newId = db_->getLastInsertId();
    std::cout << "Transaction created with ID: " << newId << std::endl;
    
    return findById(newId);
}

std::optional<Transaction> TransactionRepository::findById(int id) {
    const std::string sql = "SELECT id, from_account_id, to_account_id, amount, "
                           "transaction_type, description, status, created_at "
                           "FROM transactions WHERE id = ?";
    auto stmt = db_->prepare(sql);
    
    if (!stmt) {
        return std::nullopt;
    }
    
    sqlite3_bind_int(stmt.get(), 1, id);
    
    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        return transactionFromStatement(stmt.get());
    }
    
    return std::nullopt;
}

std::vector<Transaction> TransactionRepository::findByAccountId(int accountId) {
    std::vector<Transaction> transactions;
    const std::string sql = "SELECT id, from_account_id, to_account_id, amount, "
                           "transaction_type, description, status, created_at "
                           "FROM transactions "
                           "WHERE from_account_id = ? OR to_account_id = ? "
                           "ORDER BY created_at DESC";
    
    auto stmt = db_->prepare(sql);
    if (!stmt) {
        return transactions;
    }
    
    sqlite3_bind_int(stmt.get(), 1, accountId);
    sqlite3_bind_int(stmt.get(), 2, accountId);
    
    while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        transactions.push_back(transactionFromStatement(stmt.get()));
    }
    
    return transactions;
}

std::vector<Transaction> TransactionRepository::findByUserId(int userId) {
    std::vector<Transaction> transactions;
    const std::string sql = "SELECT DISTINCT t.id, t.from_account_id, t.to_account_id, t.amount, "
                           "t.transaction_type, t.description, t.status, t.created_at "
                           "FROM transactions t "
                           "LEFT JOIN accounts a1 ON t.from_account_id = a1.id "
                           "LEFT JOIN accounts a2 ON t.to_account_id = a2.id "
                           "WHERE a1.user_id = ? OR a2.user_id = ? "
                           "ORDER BY t.created_at DESC";
    
    auto stmt = db_->prepare(sql);
    if (!stmt) {
        return transactions;
    }
    
    sqlite3_bind_int(stmt.get(), 1, userId);
    sqlite3_bind_int(stmt.get(), 2, userId);
    
    while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        transactions.push_back(transactionFromStatement(stmt.get()));
    }
    
    return transactions;
}

std::vector<Transaction> TransactionRepository::findAll() {
    std::vector<Transaction> transactions;
    const std::string sql = "SELECT id, from_account_id, to_account_id, amount, "
                           "transaction_type, description, status, created_at "
                           "FROM transactions ORDER BY created_at DESC";
    
    db_->query(sql, [&transactions, this](sqlite3_stmt* stmt) {
        transactions.push_back(transactionFromStatement(stmt));
    });
    
    return transactions;
}

std::vector<Transaction> TransactionRepository::findWithFilters(
    std::optional<int> accountId,
    std::optional<TransactionType> type,
    std::optional<std::string> startDate,
    std::optional<std::string> endDate,
    int limit,
    int offset) {
    
    std::vector<Transaction> transactions;
    std::stringstream sql;
    sql << "SELECT id, from_account_id, to_account_id, amount, "
        << "transaction_type, description, status, created_at "
        << "FROM transactions WHERE 1=1";
    
    // Build dynamic WHERE clause
    if (accountId.has_value()) {
        sql << " AND (from_account_id = " << accountId.value() 
            << " OR to_account_id = " << accountId.value() << ")";
    }
    
    if (type.has_value()) {
        sql << " AND transaction_type = '" << transactionTypeToString(type.value()) << "'";
    }
    
    if (startDate.has_value()) {
        sql << " AND created_at >= '" << startDate.value() << "'";
    }
    
    if (endDate.has_value()) {
        sql << " AND created_at <= '" << endDate.value() << " 23:59:59'";
    }
    
    sql << " ORDER BY created_at DESC";
    sql << " LIMIT " << limit << " OFFSET " << offset;
    
    db_->query(sql.str(), [&transactions, this](sqlite3_stmt* stmt) {
        transactions.push_back(transactionFromStatement(stmt));
    });
    
    return transactions;
}

bool TransactionRepository::updateStatus(int id, TransactionStatus status) {
    const std::string sql = "UPDATE transactions SET status = ? WHERE id = ?";
    auto stmt = db_->prepare(sql);
    
    if (!stmt) {
        return false;
    }
    
    sqlite3_bind_text(stmt.get(), 1, transactionStatusToString(status).c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt.get(), 2, id);
    
    return sqlite3_step(stmt.get()) == SQLITE_DONE;
}

int TransactionRepository::getTransactionCount(std::optional<int> accountId) {
    std::stringstream sql;
    sql << "SELECT COUNT(*) FROM transactions WHERE 1=1";
    
    if (accountId.has_value()) {
        sql << " AND (from_account_id = " << accountId.value() 
            << " OR to_account_id = " << accountId.value() << ")";
    }
    
    auto stmt = db_->prepare(sql.str());
    if (!stmt) {
        return 0;
    }
    
    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        return sqlite3_column_int(stmt.get(), 0);
    }
    
    return 0;
}

Transaction TransactionRepository::transactionFromStatement(sqlite3_stmt* stmt) {
    int id = sqlite3_column_int(stmt, 0);
    
    // Handle nullable from_account_id
    std::optional<int> fromAccountId;
    if (sqlite3_column_type(stmt, 1) != SQLITE_NULL) {
        fromAccountId = sqlite3_column_int(stmt, 1);
    }
    
    // Handle nullable to_account_id
    std::optional<int> toAccountId;
    if (sqlite3_column_type(stmt, 2) != SQLITE_NULL) {
        toAccountId = sqlite3_column_int(stmt, 2);
    }
    
    double amount = sqlite3_column_double(stmt, 3);
    std::string typeStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
    std::string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
    std::string statusStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
    std::string createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
    
    TransactionType type = stringToTransactionType(typeStr);
    TransactionStatus status = stringToTransactionStatus(statusStr);
    
    return Transaction(id, fromAccountId, toAccountId, amount, type, description, status, createdAt);
}
