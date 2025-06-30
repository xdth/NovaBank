# NovaBank API - Current Endpoints

## Base URL
```
http://localhost:8080
```

## Authentication
All protected endpoints require the `Authorization` header:
```
Authorization: Bearer YOUR_TOKEN_HERE
```

## Endpoints

### 🏥 General

#### Health Check
```http
GET /health
```
**Response:**
```json
{
  "status": "healthy",
  "service": "NovaBank API",
  "version": "0.1.0"
}
```

#### API Info
```http
GET /api/v1
```
**Response:**
```json
{
  "message": "Welcome to NovaBank API v1",
  "endpoints": {
    "auth": "/api/v1/auth/*",
    "users": "/api/v1/users/*",
    "accounts": "/api/v1/accounts/*",
    "transactions": "/api/v1/transactions/*",
    "admin": "/api/v1/admin/*"
  }
}
```

#### Database Test
```http
GET /api/v1/test/db
```
**Response:**
```json
{
  "database_connected": true,
  "user_count": 1,
  "message": "Database is working"
}
```

### 🔐 Authentication

#### Login
```http
POST /api/v1/auth/login
Content-Type: application/json

{
  "username": "admin",
  "pin": "0000"
}
```
**Response:**
```json
{
  "token": "022758d75fa9b900845c71ef540af42f",
  "user": {
    "id": 1,
    "username": "admin",
    "userType": "admin"
  }
}
```

#### Logout
```http
POST /api/v1/auth/logout
Authorization: Bearer YOUR_TOKEN
```
**Response:**
```json
{
  "message": "Logged out successfully"
}
```

#### Get Current User
```http
GET /api/v1/auth/me
Authorization: Bearer YOUR_TOKEN
```
**Response:**
```json
{
  "id": 1,
  "username": "admin",
  "userType": "admin",
  "createdAt": "2025-06-28 21:51:49"
}
```

### 👥 User Management

#### Get All Users (Admin Only)
```http
GET /api/v1/users
Authorization: Bearer YOUR_TOKEN
```
**Response:**
```json
{
  "users": [
    {
      "id": 1,
      "username": "admin",
      "userType": "admin",
      "createdAt": "2025-06-28 21:51:49"
    }
  ]
}
```

#### Get User by ID
```http
GET /api/v1/users/:id
Authorization: Bearer YOUR_TOKEN
```
**Note:** Users can only view their own profile unless they're admin

**Response:**
```json
{
  "id": 1,
  "username": "admin",
  "userType": "admin",
  "createdAt": "2025-06-28 21:51:49",
  "updatedAt": "2025-06-28 21:51:49"
}
```

#### Create User (Admin Only)
```http
POST /api/v1/users
Authorization: Bearer YOUR_TOKEN
Content-Type: application/json

{
  "username": "newuser",
  "pin": "1234",
  "userType": "standard"  // optional, defaults to "standard"
}
```
**Response:**
```json
{
  "id": 2,
  "username": "newuser",
  "userType": "standard",
  "createdAt": "2025-06-28 22:00:00"
}
```

#### Update User
```http
PATCH /api/v1/users/:id
Authorization: Bearer YOUR_TOKEN
Content-Type: application/json

{
  "pin": "5678"  // optional
}
```
**Note:** 
- Users can only update their own profile unless they're admin
- Only admins can update userType

**Response:**
```json
{
  "message": "User updated successfully"
}
```

#### Delete User (Admin Only)
```http
DELETE /api/v1/users/:id
Authorization: Bearer YOUR_TOKEN
```
**Note:** Users cannot delete their own account

**Response:**
```json
{
  "message": "User deleted successfully"
}
```

### 💳 Account Management

#### Get All Accounts
```http
GET /api/v1/accounts
Authorization: Bearer YOUR_TOKEN
```
**Note:** Regular users see only their accounts, admins see all accounts

**Response:**
```json
{
  "accounts": [
    {
      "id": 1,
      "userId": 1,
      "accountNumber": "ACC12345678",
      "accountType": "checking",
      "balance": 1000.0,
      "formattedBalance": "$1000.00",
      "createdAt": "2025-06-28 22:00:00",
      "updatedAt": "2025-06-28 22:00:00"
    }
  ],
  "totalBalance": 1000.0
}
```

#### Get Account by ID
```http
GET /api/v1/accounts/:id
Authorization: Bearer YOUR_TOKEN
```
**Note:** Users can only view their own accounts unless they're admin

**Response:**
```json
{
  "id": 1,
  "userId": 1,
  "accountNumber": "ACC12345678",
  "accountType": "checking",
  "balance": 1000.0,
  "formattedBalance": "$1000.00",
  "createdAt": "2025-06-28 22:00:00",
  "updatedAt": "2025-06-28 22:00:00"
}
```

#### Create Account
```http
POST /api/v1/accounts
Authorization: Bearer YOUR_TOKEN
Content-Type: application/json

{
  "accountType": "checking",  // or "savings"
  "userId": 2,  // optional, admin only
  "initialBalance": 100.00  // optional, admin only
}
```
**Response:**
```json
{
  "id": 2,
  "userId": 1,
  "accountNumber": "ACC87654321",
  "accountType": "checking",
  "balance": 0.0,
  "formattedBalance": "$0.00",
  "createdAt": "2025-06-28 22:00:00",
  "updatedAt": "2025-06-28 22:00:00",
  "message": "Account created successfully"
}
```

#### Transfer Money
```http
POST /api/v1/accounts/:id/transfer
Authorization: Bearer YOUR_TOKEN
Content-Type: application/json

{
  "amount": 100.50,
  "toAccountNumber": "ACC87654321",
  "description": "Rent payment"  // optional
}
```
**Note:** 
- Savings accounts must maintain $25 minimum balance
- Amount must be positive with max 2 decimal places

**Response:**
```json
{
  "message": "Transfer completed successfully",
  "transferDetails": {
    "from": {
      "accountNumber": "ACC12345678",
      "newBalance": 899.50
    },
    "to": {
      "accountNumber": "ACC87654321",
      "newBalance": 100.50
    },
    "amount": 100.50,
    "description": "Rent payment",
    "timestamp": "2025-06-28 22:00:00"
  }
}
```

**Error Response (Insufficient Funds):**
```json
{
  "error": "Insufficient funds",
  "currentBalance": 50.0,
  "requestedAmount": 100.0,
  "minimumBalance": 25.0  // for savings accounts
}
```

### 💸 Transactions

#### Get Transaction History
```http
GET /api/v1/transactions
Authorization: Bearer YOUR_TOKEN
```
**Query Parameters:**
- `accountId` - Filter by specific account
- `type` - Filter by transaction type (deposit, withdrawal, transfer)
- `startDate` - Start date (YYYY-MM-DD)
- `endDate` - End date (YYYY-MM-DD)  
- `limit` - Number of results (default: 100)
- `offset` - Pagination offset (default: 0)

**Response:**
```json
{
  "transactions": [
    {
      "id": 1,
      "type": "deposit",
      "amount": 500.0,
      "formattedAmount": "$500.00",
      "description": "Salary deposit",
      "status": "completed",
      "createdAt": "2025-06-29 12:00:00",
      "toAccount": {
        "id": 1,
        "accountNumber": "ACC12345678",
        "accountType": "checking"
      },
      "direction": "Credit",
      "displayAmount": "+$500.00"
    }
  ],
  "count": 1,
  "total": 10,
  "limit": 100,
  "offset": 0
}
```

#### Get Transaction by ID
```http
GET /api/v1/transactions/:id
Authorization: Bearer YOUR_TOKEN
```
**Response:** Same format as single transaction above

#### Deposit Money
```http
POST /api/v1/transactions/deposit
Authorization: Bearer YOUR_TOKEN
Content-Type: application/json

{
  "accountNumber": "ACC12345678",
  "amount": 100.00,
  "description": "Cash deposit"  // optional
}
```
**Response:**
```json
{
  "message": "Deposit successful",
  "transactionDetails": {
    "accountNumber": "ACC12345678",
    "amount": 100.0,
    "newBalance": 1100.0,
    "formattedBalance": "$1100.00",
    "description": "Cash deposit"
  }
}
```

#### Withdraw Money
```http
POST /api/v1/transactions/withdraw
Authorization: Bearer YOUR_TOKEN
Content-Type: application/json

{
  "accountNumber": "ACC12345678",
  "amount": 50.00,
  "description": "ATM withdrawal"  // optional
}
```
**Response:**
```json
{
  "message": "Withdrawal successful",
  "transactionDetails": {
    "accountNumber": "ACC12345678",
    "amount": 50.0,
    "newBalance": 1050.0,
    "formattedBalance": "$1050.00",
    "description": "ATM withdrawal"
  }
}
```

#### Transfer Money
```http
POST /api/v1/transactions/transfer
Authorization: Bearer YOUR_TOKEN
Content-Type: application/json

{
  "fromAccountNumber": "ACC12345678",
  "toAccountNumber": "ACC87654321",
  "amount": 100.00,
  "description": "Payment"  // optional
}
```
**Response:**
```json
{
  "message": "Transfer successful",
  "transferDetails": {
    "from": {
      "accountNumber": "ACC12345678",
      "newBalance": 950.0
    },
    "to": {
      "accountNumber": "ACC87654321",
      "newBalance": 100.0
    },
    "amount": 100.0,
    "description": "Payment"
  }
}
```

**Error Response (Insufficient Funds):**
```json
{
  "error": "Insufficient funds",
  "currentBalance": 50.0,
  "requestedAmount": 100.0,
  "minimumBalance": 25.0  // for savings accounts
}
```

### 🔧 Admin Management (Admin Only)

#### Get All Users with Balances
```http
GET /api/v1/admin/users
Authorization: Bearer YOUR_TOKEN
```
**Note:** Admin only endpoint that shows detailed user information with account balances

**Response:**
```json
{
  "users": [
    {
      "id": 1,
      "username": "admin",
      "userType": "admin",
      "createdAt": "2025-06-28 21:51:49",
      "accounts": [
        {
          "id": 1,
          "accountNumber": "ACC12345678",
          "accountType": "checking",
          "balance": 1000.0,
          "formattedBalance": "$1000.00"
        }
      ],
      "accountCount": 1,
      "totalBalance": 1000.0,
      "formattedTotalBalance": "$1000.00"
    }
  ],
  "systemTotalBalance": 1000.0,
  "formattedSystemTotal": "$1000.00",
  "userCount": 1
}
```

#### Admin Deposit
```http
POST /api/v1/admin/deposit
Authorization: Bearer YOUR_TOKEN
Content-Type: application/json

{
  "accountNumber": "ACC12345678",
  "amount": 100.00,
  "description": "Admin deposit"  // optional
}
```
**Response:**
```json
{
  "message": "Admin deposit successful",
  "transactionDetails": {
    "accountNumber": "ACC12345678",
    "accountType": "checking",
    "username": "john_doe",
    "amount": 100.0,
    "newBalance": 1100.0,
    "formattedBalance": "$1100.00",
    "description": "Admin deposit",
    "adminUser": "admin"
  }
}
```

#### Admin Withdraw
```http
POST /api/v1/admin/withdraw
Authorization: Bearer YOUR_TOKEN
Content-Type: application/json

{
  "accountNumber": "ACC12345678",
  "amount": 50.00,
  "description": "Admin withdrawal"  // optional
}
```
**Response:**
```json
{
  "message": "Admin withdrawal successful",
  "transactionDetails": {
    "accountNumber": "ACC12345678",
    "accountType": "checking",
    "username": "john_doe",
    "amount": 50.0,
    "newBalance": 1050.0,
    "formattedBalance": "$1050.00",
    "description": "Admin withdrawal",
    "adminUser": "admin"
  }
}
```

#### Admin Transfer
```http
POST /api/v1/admin/transfer
Authorization: Bearer YOUR_TOKEN
Content-Type: application/json

{
  "fromAccountNumber": "ACC12345678",
  "toAccountNumber": "ACC87654321",
  "amount": 100.00,
  "description": "Admin transfer"  // optional
}
```
**Response:**
```json
{
  "message": "Admin transfer successful",
  "transferDetails": {
    "from": {
      "accountNumber": "ACC12345678",
      "username": "john_doe",
      "newBalance": 950.0
    },
    "to": {
      "accountNumber": "ACC87654321",
      "username": "jane_doe",
      "newBalance": 100.0
    },
    "amount": 100.0,
    "description": "Admin transfer",
    "adminUser": "admin"
  }
}
```

All errors follow this format:
```json
{
  "error": "Error message",
  "status": 400
}
```

Common status codes:
- `400` - Bad Request
- `401` - Authentication required
- `403` - Access denied / Admin required
- `404` - Not found
- `409` - Conflict (e.g., username already exists)
- `500` - Server error

## Default Credentials

**Admin User:**
- Username: `admin`
- PIN: `0000`

## Business Rules

### Account Types
- **Checking**: No minimum balance requirement
- **Savings**: Must maintain $25 minimum balance

### Transfers
- Amount must be positive
- Maximum 2 decimal places
- Source account must have sufficient funds
- Both accounts must exist

## Session Notes

- Sessions timeout after 30 minutes of inactivity
- Each login generates a new session token
- Tokens should be included in the Authorization header as: `Bearer YOUR_TOKEN`
