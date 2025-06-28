#pragma once

#include <crow.h>
#include <string>

inline crow::response errorResponse(int statusCode, const std::string& message) {
    crow::json::wvalue response;
    response["error"] = message;
    response["status"] = statusCode;
    return crow::response(statusCode, response);
}

inline crow::response successResponse(const crow::json::wvalue& data, int statusCode = 200) {
    std::string jsonStr = data.dump();
    crow::response resp(statusCode);
    resp.set_header("Content-Type", "application/json");
    resp.body = jsonStr;
    return resp;
}
