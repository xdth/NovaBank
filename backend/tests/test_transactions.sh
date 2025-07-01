#!/bin/bash

# Test script for NovaBank transaction endpoints

BASE_URL="http://localhost:8080/api/v1"

echo "💰 Testing NovaBank Transaction Endpoints"
echo "======================================"

# Login as admin
echo -e "\n1️⃣ Logging in as admin..."
LOGIN_RESPONSE=$(curl -s -X POST $BASE_URL/auth/login \
  -H "Content-Type: application/json" \
  -d '{"username": "admin", "pin": "0000"}')

TOKEN=$(echo $LOGIN_RESPONSE | grep -o '"token":"[^"]*' | grep -o '[^"]*$')
echo "✅ Admin token: ${TOKEN:0:10}..."

# Create accounts for admin first
echo -e "\n2️⃣ Creating checking account for admin..."
ADMIN_CHECKING=$(curl -s -X POST $BASE_URL/accounts \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{"accountType": "checking", "initialBalance": 1000.00}')
echo $ADMIN_CHECKING | jq '.'

ACCOUNT_NUMBER=$(echo $ADMIN_CHECKING | grep -o '"accountNumber":"[^"]*' | sed 's/"accountNumber":"//')
ACCOUNT_ID=$(echo $ADMIN_CHECKING | grep -o '"id":[0-9]*' | grep -o '[0-9]*')
echo "Created Account Number: $ACCOUNT_NUMBER (ID: $ACCOUNT_ID)"

# Test deposit
echo -e "\n3️⃣ Testing deposit..."
DEPOSIT_RESPONSE=$(curl -s -X POST $BASE_URL/transactions/deposit \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d "{\"accountNumber\": \"$ACCOUNT_NUMBER\", \"amount\": 500.00, \"description\": \"Salary deposit\"}")
echo $DEPOSIT_RESPONSE | jq '.'

# Test withdrawal
echo -e "\n4️⃣ Testing withdrawal..."
WITHDRAW_RESPONSE=$(curl -s -X POST $BASE_URL/transactions/withdraw \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d "{\"accountNumber\": \"$ACCOUNT_NUMBER\", \"amount\": 50.00, \"description\": \"ATM withdrawal\"}")
echo $WITHDRAW_RESPONSE | jq '.'

# Create a second user and account for transfer test
echo -e "\n5️⃣ Creating second user..."
curl -s -X POST $BASE_URL/users \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{"username": "jane_doe", "pin": "5678", "userType": "standard"}' | jq '.'

# Login as second user
echo -e "\n6️⃣ Logging in as second user..."
USER_LOGIN=$(curl -s -X POST $BASE_URL/auth/login \
  -H "Content-Type: application/json" \
  -d '{"username": "jane_doe", "pin": "5678"}')

USER_TOKEN=$(echo $USER_LOGIN | grep -o '"token":"[^"]*' | grep -o '[^"]*$')

# Create account for second user
echo -e "\n7️⃣ Creating account for second user..."
USER_ACCOUNT=$(curl -s -X POST $BASE_URL/accounts \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $USER_TOKEN" \
  -d '{"accountType": "checking"}')
echo $USER_ACCOUNT | jq '.'

USER_ACCOUNT_NUMBER=$(echo $USER_ACCOUNT | grep -o '"accountNumber":"[^"]*' | sed 's/"accountNumber":"//')
echo "User Account Number: $USER_ACCOUNT_NUMBER"

# Test transfer
echo -e "\n8️⃣ Testing transfer..."
TRANSFER_RESPONSE=$(curl -s -X POST $BASE_URL/transactions/transfer \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d "{\"fromAccountNumber\": \"$ACCOUNT_NUMBER\", \"toAccountNumber\": \"$USER_ACCOUNT_NUMBER\", \"amount\": 100.00, \"description\": \"Payment to Jane\"}")
echo $TRANSFER_RESPONSE | jq '.'

# Get transaction history
echo -e "\n9️⃣ Getting transaction history for admin..."
curl -s -X GET $BASE_URL/transactions \
  -H "Authorization: Bearer $TOKEN" | jq '.'

# Get filtered transactions
echo -e "\n🔟 Getting only deposit transactions..."
curl -s -X GET "$BASE_URL/transactions?type=deposit" \
  -H "Authorization: Bearer $TOKEN" | jq '.'

# Get specific transaction
echo -e "\n1️⃣1️⃣ Getting specific transaction (ID: 1)..."
curl -s -X GET $BASE_URL/transactions/1 \
  -H "Authorization: Bearer $TOKEN" | jq '.'

# Test insufficient funds
echo -e "\n1️⃣2️⃣ Testing withdrawal with insufficient funds..."
curl -s -X POST $BASE_URL/transactions/withdraw \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d "{\"accountNumber\": \"$ACCOUNT_NUMBER\", \"amount\": 10000.00, \"description\": \"Large withdrawal\"}" | jq '.'

# Check final balances
echo -e "\n1️⃣3️⃣ Final account balances for admin..."
curl -s -X GET $BASE_URL/accounts \
  -H "Authorization: Bearer $TOKEN" | jq '.'

echo -e "\n1️⃣4️⃣ Final account balances for jane_doe..."
curl -s -X GET $BASE_URL/accounts \
  -H "Authorization: Bearer $USER_TOKEN" | jq '.'

# Test account-specific transaction history
echo -e "\n1️⃣5️⃣ Getting transactions for specific account..."
curl -s -X GET "$BASE_URL/transactions?accountId=$ACCOUNT_ID" \
  -H "Authorization: Bearer $TOKEN" | jq '.'

echo -e "\n✨ Transaction tests complete!"
