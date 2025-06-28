#!/bin/bash

# Test script for NovaBank authentication endpoints

BASE_URL="http://localhost:8080/api/v1"

echo "🧪 Testing NovaBank Authentication Endpoints"
echo "=========================================="

# Test 1: Login with admin credentials
echo -e "\n1️⃣ Testing admin login..."
LOGIN_RESPONSE=$(curl -s -X POST $BASE_URL/auth/login \
  -H "Content-Type: application/json" \
  -d '{"username": "admin", "pin": "0000"}')

echo "Response: $LOGIN_RESPONSE"

# Extract token from response
TOKEN=$(echo $LOGIN_RESPONSE | grep -o '"token":"[^"]*' | grep -o '[^"]*$')

if [ -z "$TOKEN" ]; then
    echo "❌ Login failed - no token received"
    exit 1
fi

echo "✅ Login successful! Token: ${TOKEN:0:10}..."

# Test 2: Get current user
echo -e "\n2️⃣ Testing /auth/me endpoint..."
curl -s -X GET $BASE_URL/auth/me \
  -H "Authorization: Bearer $TOKEN" | jq '.' 2>/dev/null || echo "Install jq for pretty JSON"

# Test 3: Get all users (admin only)
echo -e "\n3️⃣ Testing GET /users (admin only)..."
curl -s -X GET $BASE_URL/users \
  -H "Authorization: Bearer $TOKEN" | jq '.' 2>/dev/null || echo "Install jq for pretty JSON"

# Test 4: Create a new user
echo -e "\n4️⃣ Testing create new user..."
CREATE_RESPONSE=$(curl -s -X POST $BASE_URL/users \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{"username": "testuser", "pin": "1234", "userType": "standard"}')

echo "Response: $CREATE_RESPONSE"

# Test 5: Login with new user
echo -e "\n5️⃣ Testing login with new user..."
USER_LOGIN_RESPONSE=$(curl -s -X POST $BASE_URL/auth/login \
  -H "Content-Type: application/json" \
  -d '{"username": "testuser", "pin": "1234"}')

echo "Response: $USER_LOGIN_RESPONSE"

USER_TOKEN=$(echo $USER_LOGIN_RESPONSE | grep -o '"token":"[^"]*' | grep -o '[^"]*$')

# Test 6: Try to access admin endpoint with standard user
echo -e "\n6️⃣ Testing access control (standard user trying admin endpoint)..."
curl -s -X GET $BASE_URL/users \
  -H "Authorization: Bearer $USER_TOKEN"

# Test 7: Logout
echo -e "\n7️⃣ Testing logout..."
curl -s -X POST $BASE_URL/auth/logout \
  -H "Authorization: Bearer $TOKEN"

# Test 8: Try to use token after logout
echo -e "\n8️⃣ Testing token after logout (should fail)..."
curl -s -X GET $BASE_URL/auth/me \
  -H "Authorization: Bearer $TOKEN"

echo -e "\n✨ Authentication tests complete!"
