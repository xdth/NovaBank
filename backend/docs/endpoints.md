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

## Error Responses

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

## Session Notes

- Sessions timeout after 30 minutes of inactivity
- Each login generates a new session token
- Tokens should be included in the Authorization header as: `Bearer YOUR_TOKEN`
