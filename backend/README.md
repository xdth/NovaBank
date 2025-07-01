# 🏦 NovaBank

A modern simulated banking platform built with C++ (backend) and React (frontend). NovaBank provides a complete banking experience with user authentication, account management, and transaction processing.

## 🚀 Features

### ✅ Implemented (Backend)
- **User Management**
  - User registration and authentication with PIN
  - Role-based access control (Admin/Standard users)
  - Session management with 30-minute timeout
  - User profile updates

- **Account Management**
  - Multiple account types (Checking/Savings)
  - Account creation with initial balance (admin only)
  - Balance inquiries
  - Minimum balance enforcement for savings accounts ($25)

- **Transaction Processing**
  - Deposits
  - Withdrawals
  - Transfers between accounts
  - Transaction history with filtering
  - Transaction validation and atomic operations

- **Admin Operations**
  - View all users with account balances
  - System-wide balance tracking
  - Admin deposits to any account
  - Admin withdrawals from any account
  - Admin transfers between any accounts
  - Admin action attribution

- **Security**
  - SHA256 PIN hashing
  - Bearer token authentication
  - Role-based endpoint protection
  - Session timeout

### 📋 Planned
- React frontend
- Docker containerization
- CI/CD pipeline
- API documentation (OpenAPI/Swagger)

## 🛠️ Tech Stack

### Backend
- **Language:** C++17
- **Framework:** Crow (HTTP server)
- **Database:** SQLite
- **Authentication:** Custom session-based with bearer tokens
- **Build System:** CMake

### Frontend (Planned)
- **Framework:** React with TypeScript
- **UI Library:** MUI 7
- **State Management:** Context API
- **HTTP Client:** Axios

## 📁 Project Structure

```
NovaBank/
├── backend/
│   ├── src/
│   │   ├── api/            # REST API controllers
│   │   ├── domain/         # Business domain models
│   │   ├── repository/     # Data access layer
│   │   ├── db/            # Database management
│   │   └── utils/         # Utility functions
│   ├── tests/             # Unit tests
│   ├── CMakeLists.txt     # Build configuration
│   └── Dockerfile         # Container definition
├── frontend/              # React application (planned)
├── docker-compose.yml     # Multi-container setup
└── README.md             # This file
```

## 🏃 Quick Start

### Prerequisites
- C++17 compatible compiler
- CMake 3.16+
- SQLite3
- OpenSSL

### Backend Setup

```bash
# Clone the repository
git clone https://github.com/yourusername/NovaBank.git
cd NovaBank/backend

# Create build directory
mkdir build && cd build

# Build the project
cmake ..
make

# Run the server
./novabank
```

The server will start on `http://localhost:8080`

### Default Credentials
- **Username:** admin
- **PIN:** 0000

## 📡 API Endpoints

### Authentication
- `POST /api/v1/auth/login` - User login
- `POST /api/v1/auth/logout` - User logout
- `GET /api/v1/auth/me` - Get current user

### Users
- `GET /api/v1/users` - List all users (admin)
- `GET /api/v1/users/:id` - Get user details
- `POST /api/v1/users` - Create user (admin)
- `PATCH /api/v1/users/:id` - Update user
- `DELETE /api/v1/users/:id` - Delete user (admin)

### Accounts
- `GET /api/v1/accounts` - List accounts
- `GET /api/v1/accounts/:id` - Get account details
- `POST /api/v1/accounts` - Create account
- `POST /api/v1/accounts/:id/transfer` - Transfer money (deprecated, use /transactions/transfer)

### Transactions
- `GET /api/v1/transactions` - Transaction history
- `GET /api/v1/transactions/:id` - Transaction details
- `POST /api/v1/transactions/deposit` - Deposit money
- `POST /api/v1/transactions/withdraw` - Withdraw money
- `POST /api/v1/transactions/transfer` - Transfer money

### Admin
- `GET /api/v1/admin/users` - List all users with balances (admin)
- `POST /api/v1/admin/deposit` - Admin deposit to any account
- `POST /api/v1/admin/withdraw` - Admin withdraw from any account
- `POST /api/v1/admin/transfer` - Admin transfer between accounts

## 🧪 Testing

### Test Scripts
```bash
# Test authentication
./test_auth.sh

# Test accounts
./test_accounts.sh

# Test transactions
./test_transactions.sh

# Test admin operations
./test_admin.sh
```

### API Testing
Import `NovaBank_Insomnia_Collection.json` into Insomnia for complete API testing.

### Quick Test
Run all tests in sequence:
```bash
./test_auth.sh && ./test_accounts.sh && ./test_transactions.sh && ./test_admin.sh
```

## 📊 Database Schema

### Users Table
- `id` - Primary key
- `username` - Unique username
- `pin_hash` - Hashed PIN
- `user_type` - admin/standard
- `created_at`, `updated_at` - Timestamps

### Accounts Table
- `id` - Primary key
- `user_id` - Foreign key to users
- `account_number` - Unique account number
- `account_type` - checking/savings
- `balance` - Current balance
- `created_at`, `updated_at` - Timestamps

### Transactions Table
- `id` - Primary key
- `from_account_id` - Source account (nullable)
- `to_account_id` - Destination account (nullable)
- `amount` - Transaction amount
- `transaction_type` - deposit/withdrawal/transfer
- `description` - Transaction description
- `status` - pending/completed/failed
- `created_at` - Transaction timestamp

## 🏦 Business Rules

### Account Types
- **Checking Account**: No minimum balance requirement
- **Savings Account**: Must maintain $25 minimum balance

### Transaction Rules
- All amounts must be positive
- Maximum 2 decimal places for currency
- Deposits: Create money in the system (admin or user initiated)
- Withdrawals: Remove money from the system
- Transfers: Move money between accounts (no system balance change)

### User Roles
- **Standard User**: Can manage own accounts and transactions
- **Admin User**: Full system access, can manage all users and accounts

### Session Management
- Sessions expire after 30 minutes of inactivity
- Each login generates a new session token
- Tokens must be included in Authorization header as `Bearer TOKEN`

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 🙏 Acknowledgments

- Crow framework for the excellent C++ HTTP server
- SQLite for the reliable embedded database
- The C++ community for invaluable resources

## 📞 Contact

dth - #Linux - irc.alterland.net

Project Link: [https://github.com/xdth/NovaBank](https://github.com/xdth/NovaBank)
