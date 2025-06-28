#pragma once

#include "repository/account/account_repository_interface.h"
#include "db/db.h"
#include <memory>

class AccountRepository : public IAccountRepository {
public:
    explicit AccountRepository(std::shared_ptr<Database> db);
    
    // IAccountRepository implementation
    std::optional<Account> create(const Account& account) override;
    std::optional<Account> findById(int id) override;
    std::optional<Account> findByAccountNumber(const std::string& accountNumber) override;
    std::vector<Account> findByUserId(int userId) override;
    std::vector<Account> findAll() override;
    bool update(const Account& account) override;
    bool deleteById(int id) override;
    bool existsByAccountNumber(const std::string& accountNumber) override;
    double getTotalBalanceForUser(int userId) override;
    
private:
    std::shared_ptr<Database> db_;
    
    // Helper method to create Account from query result
    Account accountFromStatement(sqlite3_stmt* stmt);
};
