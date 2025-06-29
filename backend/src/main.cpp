#include <crow.h>
#include <crow/middlewares/cors.h>
#include <iostream>
#include <memory>

// Include controllers
#include "api/user/user_controller.h"
#include "api/account/account_controller.h"
#include "api/transaction/transaction_controller.h"

int main() {
    // Initialize Crow app with CORS middleware
    crow::App<crow::CORSHandler> app;
    
    // Configure CORS
    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors
        .global()
        .headers("Content-Type", "Authorization")
        .methods("GET"_method, "POST"_method, "PUT"_method, "PATCH"_method, "DELETE"_method)
        .origin("http://localhost:5173");  // React dev server
    
    // Health check endpoint
    CROW_ROUTE(app, "/health")
    .methods("GET"_method)
    ([](const crow::request&) {
        crow::json::wvalue response;
        response["status"] = "healthy";
        response["service"] = "NovaBank API";
        response["version"] = "0.1.0";
        return crow::response(200, response);
    });
    
    // API version endpoint
    CROW_ROUTE(app, "/api/v1")
    .methods("GET"_method)
    ([](const crow::request&) {
        crow::json::wvalue response;
        response["message"] = "Welcome to NovaBank API v1";
        response["endpoints"]["auth"] = "/api/v1/auth/*";
        response["endpoints"]["users"] = "/api/v1/users/*";
        response["endpoints"]["accounts"] = "/api/v1/accounts/*";
        response["endpoints"]["transactions"] = "/api/v1/transactions/*";
        response["endpoints"]["admin"] = "/api/v1/admin/*";
        return crow::response(200, response);
    });
    
    // Initialize database connection
    std::shared_ptr<Database> db;
    try {
        db = std::make_shared<Database>("novabank.db");
        std::cout << "✅ Database initialized successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "❌ Failed to initialize database: " << e.what() << std::endl;
        return 1;
    }
    
    // Test endpoint to verify database is working
    CROW_ROUTE(app, "/api/v1/test/db")
    .methods("GET"_method)
    ([db](const crow::request&) {
        crow::json::wvalue response;
        bool dbWorking = false;
        int userCount = 0;
        
        // Test database with a simple query
        db->query("SELECT COUNT(*) FROM users", [&dbWorking, &userCount](sqlite3_stmt* stmt) {
            dbWorking = true;
            userCount = sqlite3_column_int(stmt, 0);
        });
        
        response["database_connected"] = dbWorking;
        response["user_count"] = userCount;
        response["message"] = dbWorking ? "Database is working" : "Database connection failed";
        
        return crow::response(dbWorking ? 200 : 500, response);
    });
    
    // Register controllers
    UserController userController(db);
    userController.registerRoutes(app);
    
    AccountController accountController(db);
    accountController.registerRoutes(app);
    
    TransactionController transactionController(db);
    transactionController.registerRoutes(app);
    
    // Start server
    app.port(8080)
       .multithreaded()
       .run();
    
    return 0;
}
