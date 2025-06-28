#pragma once

#include <crow/json.h>
#include "domain/account/account.h"
#include "domain/account/account_utils.h"

class AccountSerializer {
public:
    static crow::json::wvalue toJson(const Account& account) {
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
    
    static crow::json::wvalue toJsonList(const std::vector<Account>& accounts) {
        crow::json::wvalue json;
        json = crow::json::wvalue(crow::json::type::List);
        
        for (size_t i = 0; i < accounts.size(); ++i) {
            json[i] = toJson(accounts[i]);
        }
        
        return json;
    }
};
