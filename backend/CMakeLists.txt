cmake_minimum_required(VERSION 3.16)
project(NovaBank VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find required packages
find_package(Threads REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(OpenSSL REQUIRED)

# Try to find nlohmann_json, if not found, fetch it
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann_json not found, fetching from GitHub...")
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# Crow setup
include(FetchContent)
FetchContent_Declare(
    crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.0+5
)
FetchContent_MakeAvailable(crow)

# Source files
set(SOURCES
    src/main.cpp
    # Database
    src/db/db.cpp
    
    # Domain models
    src/domain/user/user.cpp
    src/domain/account/account.cpp
    src/domain/transaction/transaction.cpp
    
    # Repositories
    src/repository/user/user_repository.cpp
    src/repository/account/account_repository.cpp
    src/repository/transaction/transaction_repository.cpp
    
    # API Controllers
    src/api/user/user_controller.cpp
    src/api/account/account_controller.cpp
    src/api/transaction/transaction_controller.cpp
    src/api/admin/admin_controller.cpp
    # src/domain/account/account.cpp
    # src/domain/transaction/transaction.cpp
    
    # Repositories
    # src/repository/user/user_repository.cpp
    # src/repository/account/account_repository.cpp
    # src/repository/transaction/transaction_repository.cpp
    
    # API Controllers
    # src/api/user/user_controller.cpp
    # src/api/account/account_controller.cpp
    # src/api/transaction/transaction_controller.cpp
    
    # Utils
    # src/utils/json_utils.cpp
)

# Create executable
add_executable(novabank ${SOURCES})

# Include directories
target_include_directories(novabank PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SQLite3_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(novabank PRIVATE
    Crow::Crow
    ${SQLite3_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
    Threads::Threads
)

# Link nlohmann_json
if(TARGET nlohmann_json::nlohmann_json)
    target_link_libraries(novabank PRIVATE nlohmann_json::nlohmann_json)
else()
    target_link_libraries(novabank PRIVATE nlohmann_json)
endif()

# Copy database migrations to build directory
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/db/migrations.sql
    ${CMAKE_CURRENT_BINARY_DIR}/migrations.sql
    COPYONLY
)
