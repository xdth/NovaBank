#pragma once

#include <crow/json.h>
#include "domain/user/user.h"

class UserSerializer {
public:
    static crow::json::wvalue toJson(const User& user, bool includePrivate = false) {
        crow::json::wvalue json;
        json["id"] = user.getId();
        json["username"] = user.getUsername();
        json["userType"] = userTypeToString(user.getUserType());
        json["createdAt"] = user.getCreatedAt();
        json["updatedAt"] = user.getUpdatedAt();
        
        // Don't include PIN hash in response
        return json;
    }
    
    static crow::json::wvalue toJsonList(const std::vector<User>& users) {
        crow::json::wvalue json;
        json = crow::json::wvalue(crow::json::type::List);
        
        for (size_t i = 0; i < users.size(); ++i) {
            json[i] = toJson(users[i]);
        }
        
        return json;
    }
};
