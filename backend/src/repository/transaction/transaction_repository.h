#pragma once

#include "repository/transaction/transaction_repository_interface.h"
#include "db/db.h"
#include <memory>

class TransactionRepository : public ITransactionRepository {
public:
    explicit TransactionRepository(std::shared_ptr<Database> db);
    
    // ITransactionRepository implementation
    std::optional<Transaction> create(const Transaction& transaction) override;
    std::optional<Transaction> findById(int id) override;
    std::vector<Transaction> findByAccountId(int accountId) override;
    std::vector<Transaction> findByUserId(int userId) override;
    std::vector<Transaction> findAll() override;
    std::vector<Transaction> findWithFilters(
        std::optional<int> accountId,
        std::optional<TransactionType> type,
        std::optional<std::string> startDate,
        std::optional<std::string> endDate,
        int limit = 100,
        int offset = 0
    ) override;
    bool updateStatus(int id, TransactionStatus status) override;
    int getTransactionCount(std::optional<int> accountId = std::nullopt) override;
    
private:
    std::shared_ptr<Database> db_;
    
    // Helper method to create Transaction from query result
    Transaction transactionFromStatement(sqlite3_stmt* stmt);
};
