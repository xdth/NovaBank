#!/bin/bash

# Test script for NovaBank admin endpoints

BASE_URL="http://localhost:8080/api/v1"

echo "👨‍💼 Testing NovaBank Admin Endpoints"
echo "==================================="

# Login as admin
echo -e "\n1️⃣ Logging in as admin..."
LOGIN_RESPONSE=$(curl -s -X POST $BASE_URL/auth/login \
  -H "Content-Type: application/json" \
  -d '{"username": "admin", "pin": "0000"}')

TOKEN=$(echo $LOGIN_RESPONSE | grep -o '"token":"[^"]*' | grep -o '[^"]*$')
echo "✅ Admin token: ${TOKEN:0:10}..."

# Test 1: Get all users with balances
echo -e "\n2️⃣ Getting all users with balances..."
curl -s -X GET $BASE_URL/admin/users \
  -H "Authorization: Bearer $TOKEN" | jq '.'

# Create test users and accounts if needed
echo -e "\n3️⃣ Creating test user (John)..."
curl -s -X POST $BASE_URL/users \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{"username": "john_smith", "pin": "1111", "userType": "standard"}' | jq '.'

echo -e "\n4️⃣ Creating test user (Alice)..."
curl -s -X POST $BASE_URL/users \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{"username": "alice_jones", "pin": "2222", "userType": "standard"}' | jq '.'

# Login as John to create his account
echo -e "\n5️⃣ Logging in as John..."
JOHN_LOGIN=$(curl -s -X POST $BASE_URL/auth/login \
  -H "Content-Type: application/json" \
  -d '{"username": "john_smith", "pin": "1111"}')

JOHN_TOKEN=$(echo $JOHN_LOGIN | grep -o '"token":"[^"]*' | grep -o '[^"]*$')

# Create accounts for John
echo -e "\n6️⃣ Creating checking account for John..."
JOHN_ACCOUNT=$(curl -s -X POST $BASE_URL/accounts \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $JOHN_TOKEN" \
  -d '{"accountType": "checking"}')
echo $JOHN_ACCOUNT | jq '.'

JOHN_ACCOUNT_NUMBER=$(echo $JOHN_ACCOUNT | grep -o '"accountNumber":"[^"]*' | sed 's/"accountNumber":"//')

# Login as Alice to create her account
echo -e "\n7️⃣ Logging in as Alice..."
ALICE_LOGIN=$(curl -s -X POST $BASE_URL/auth/login \
  -H "Content-Type: application/json" \
  -d '{"username": "alice_jones", "pin": "2222"}')

ALICE_TOKEN=$(echo $ALICE_LOGIN | grep -o '"token":"[^"]*' | grep -o '[^"]*$')

# Create accounts for Alice
echo -e "\n8️⃣ Creating savings account for Alice..."
ALICE_ACCOUNT=$(curl -s -X POST $BASE_URL/accounts \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $ALICE_TOKEN" \
  -d '{"accountType": "savings"}')
echo $ALICE_ACCOUNT | jq '.'

ALICE_ACCOUNT_NUMBER=$(echo $ALICE_ACCOUNT | grep -o '"accountNumber":"[^"]*' | sed 's/"accountNumber":"//')

# Test admin deposit
echo -e "\n9️⃣ Admin depositing $1000 into John's account..."
ADMIN_DEPOSIT=$(curl -s -X POST $BASE_URL/admin/deposit \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d "{\"accountNumber\": \"$JOHN_ACCOUNT_NUMBER\", \"amount\": 1000.00, \"description\": \"Initial funding by admin\"}")
echo $ADMIN_DEPOSIT | jq '.'

# Test admin deposit to Alice
echo -e "\n🔟 Admin depositing $500 into Alice's account..."
curl -s -X POST $BASE_URL/admin/deposit \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d "{\"accountNumber\": \"$ALICE_ACCOUNT_NUMBER\", \"amount\": 500.00, \"description\": \"Welcome bonus\"}" | jq '.'

# Test admin withdrawal
echo -e "\n1️⃣1️⃣ Admin withdrawing $50 from John's account..."
curl -s -X POST $BASE_URL/admin/withdraw \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d "{\"accountNumber\": \"$JOHN_ACCOUNT_NUMBER\", \"amount\": 50.00, \"description\": \"Service fee\"}" | jq '.'

# Test admin transfer
echo -e "\n1️⃣2️⃣ Admin transferring $100 from John to Alice..."
curl -s -X POST $BASE_URL/admin/transfer \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d "{\"fromAccountNumber\": \"$JOHN_ACCOUNT_NUMBER\", \"toAccountNumber\": \"$ALICE_ACCOUNT_NUMBER\", \"amount\": 100.00, \"description\": \"Admin-initiated transfer\"}" | jq '.'

# Get updated user list with balances
echo -e "\n1️⃣3️⃣ Getting updated user list with balances..."
curl -s -X GET $BASE_URL/admin/users \
  -H "Authorization: Bearer $TOKEN" | jq '.'

# Test access control - standard user trying admin endpoint
echo -e "\n1️⃣4️⃣ Testing access control (John trying admin endpoint)..."
curl -s -X GET $BASE_URL/admin/users \
  -H "Authorization: Bearer $JOHN_TOKEN" | jq '.'

# Test admin withdrawal with insufficient funds
echo -e "\n1️⃣5️⃣ Testing admin withdrawal with insufficient funds from Alice's savings..."
curl -s -X POST $BASE_URL/admin/withdraw \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d "{\"accountNumber\": \"$ALICE_ACCOUNT_NUMBER\", \"amount\": 580.00, \"description\": \"Test insufficient funds\"}" | jq '.'

# Get transaction history for John
echo -e "\n1️⃣6️⃣ Getting John's transaction history..."
curl -s -X GET $BASE_URL/transactions \
  -H "Authorization: Bearer $JOHN_TOKEN" | jq '.transactions | length as $count | "Transaction count: \($count)"'

echo -e "\n✨ Admin tests complete!"
