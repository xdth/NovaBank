#pragma once

#include <crow/json.h>
#include "domain/transaction/transaction.h"
#include "domain/transaction/transaction_utils.h"
#include "domain/account/account_utils.h"

class TransactionSerializer {
public:
    static crow::json::wvalue toJson(const Transaction& transaction) {
        crow::json::wvalue json;
        json["id"] = transaction.getId();
        json["type"] = transactionTypeToString(transaction.getTransactionType());
        json["amount"] = AccountUtils::roundToTwoDecimals(transaction.getAmount());
        json["formattedAmount"] = AccountUtils::formatCurrency(transaction.getAmount());
        json["description"] = transaction.getDescription();
        json["status"] = transactionStatusToString(transaction.getStatus());
        json["createdAt"] = transaction.getCreatedAt();
        
        if (transaction.getFromAccountId().has_value()) {
            json["fromAccountId"] = transaction.getFromAccountId().value();
        }
        
        if (transaction.getToAccountId().has_value()) {
            json["toAccountId"] = transaction.getToAccountId().value();
        }
        
        return json;
    }
    
    static crow::json::wvalue toJsonList(const std::vector<Transaction>& transactions) {
        crow::json::wvalue json;
        json = crow::json::wvalue(crow::json::type::List);
        
        for (size_t i = 0; i < transactions.size(); ++i) {
            json[i] = toJson(transactions[i]);
        }
        
        return json;
    }
};
