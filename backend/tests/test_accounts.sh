#!/bin/bash

# Test script for NovaBank account endpoints

BASE_URL="http://localhost:8080/api/v1"

echo "🏦 Testing NovaBank Account Endpoints"
echo "===================================="

# First, login as admin
echo -e "\n1️⃣ Logging in as admin..."
LOGIN_RESPONSE=$(curl -s -X POST $BASE_URL/auth/login \
  -H "Content-Type: application/json" \
  -d '{"username": "admin", "pin": "0000"}')

TOKEN=$(echo $LOGIN_RESPONSE | grep -o '"token":"[^"]*' | grep -o '[^"]*$')
echo "✅ Admin token: ${TOKEN:0:10}..."

# Test 2: Create a checking account for admin
echo -e "\n2️⃣ Creating checking account for admin..."
curl -s -X POST $BASE_URL/accounts \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{"accountType": "checking", "initialBalance": 1000.00}' | jq '.'

# Test 3: Create a savings account for admin
echo -e "\n3️⃣ Creating savings account for admin..."
curl -s -X POST $BASE_URL/accounts \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{"accountType": "savings", "initialBalance": 5000.00}' | jq '.'

# Test 4: Get all accounts for admin
echo -e "\n4️⃣ Getting all accounts for admin..."
ACCOUNTS_RESPONSE=$(curl -s -X GET $BASE_URL/accounts \
  -H "Authorization: Bearer $TOKEN")
echo $ACCOUNTS_RESPONSE | jq '.'

# Extract first account ID and number for testing
ACCOUNT_ID=$(echo $ACCOUNTS_RESPONSE | grep -o '"id":[0-9]*' | head -1 | grep -o '[0-9]*')
ACCOUNT_NUMBER=$(echo $ACCOUNTS_RESPONSE | grep -o '"accountNumber":"[^"]*' | head -1 | sed 's/"accountNumber":"//')

echo "Using Account ID: $ACCOUNT_ID, Number: $ACCOUNT_NUMBER"

# Test 5: Get specific account
echo -e "\n5️⃣ Getting specific account details..."
curl -s -X GET $BASE_URL/accounts/$ACCOUNT_ID \
  -H "Authorization: Bearer $TOKEN" | jq '.'

# Test 6: Create a standard user
echo -e "\n6️⃣ Creating standard user..."
curl -s -X POST $BASE_URL/users \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{"username": "john_doe", "pin": "1234", "userType": "standard"}' | jq '.'

# Test 7: Login as standard user
echo -e "\n7️⃣ Logging in as standard user..."
USER_LOGIN=$(curl -s -X POST $BASE_URL/auth/login \
  -H "Content-Type: application/json" \
  -d '{"username": "john_doe", "pin": "1234"}')

USER_TOKEN=$(echo $USER_LOGIN | grep -o '"token":"[^"]*' | grep -o '[^"]*$')
echo "✅ User token: ${USER_TOKEN:0:10}..."

# Test 8: Create account for standard user
echo -e "\n8️⃣ Creating checking account for standard user..."
USER_ACCOUNT=$(curl -s -X POST $BASE_URL/accounts \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $USER_TOKEN" \
  -d '{"accountType": "checking"}')
echo $USER_ACCOUNT | jq '.'

USER_ACCOUNT_NUMBER=$(echo $USER_ACCOUNT | grep -o '"accountNumber":"[^"]*' | sed 's/"accountNumber":"//')

# Test 9: Admin deposits money into user's account
echo -e "\n9️⃣ Admin creating account with initial balance for user..."
curl -s -X POST $BASE_URL/accounts \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d "{\"userId\": 2, \"accountType\": \"savings\", \"initialBalance\": 500.00}" | jq '.'

# Test 10: Transfer money between accounts
echo -e "\n🔟 Testing transfer from admin to user..."
TRANSFER_RESPONSE=$(curl -s -X POST $BASE_URL/accounts/$ACCOUNT_ID/transfer \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d "{\"amount\": 250.50, \"toAccountNumber\": \"$USER_ACCOUNT_NUMBER\", \"description\": \"Test transfer\"}")
echo $TRANSFER_RESPONSE | jq '.'

# Test 11: Check balances after transfer
echo -e "\n1️⃣1️⃣ Checking admin accounts after transfer..."
curl -s -X GET $BASE_URL/accounts \
  -H "Authorization: Bearer $TOKEN" | jq '.'

echo -e "\n1️⃣2️⃣ Checking user accounts after transfer..."
curl -s -X GET $BASE_URL/accounts \
  -H "Authorization: Bearer $USER_TOKEN" | jq '.'

# Test 12: Test insufficient funds
echo -e "\n1️⃣3️⃣ Testing transfer with insufficient funds..."
curl -s -X POST $BASE_URL/accounts/$ACCOUNT_ID/transfer \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{"amount": 10000.00, "toAccountNumber": "'$USER_ACCOUNT_NUMBER'"}' | jq '.'

echo -e "\n✨ Account tests complete!"
