# How to Import NovaBank Collection into Insomnia

## 📥 Import Steps

1. **Save the JSON file**
   - Save `NovaBank_Insomnia_Collection.json` to your computer

2. **Open Insomnia**
   - Launch the Insomnia application

3. **Import the collection**
   - Click on the dropdown arrow next to your workspace name
   - Select **Import/Export** → **Import Data** → **From File**
   - Navigate to and select `NovaBank_Insomnia_Collection.json`
   - Click **Import**

4. **Select the environment**
   - In the top-left corner, you'll see an environment dropdown
   - Select **Local Development** (it's already configured with `http://localhost:8080`)

## 🔑 Using Authentication

1. **First, login:**
   - Go to **Authentication** folder → **Login** request
   - Click **Send**
   - Copy the token from the response

2. **Update the token:**
   - Click on the environment dropdown (top-left)
   - Click **Manage Environments**
   - Select **Local Development**
   - Replace `YOUR_TOKEN_HERE` with your actual token
   - Click **Done**

3. **Now all authenticated requests will work!**

## 📁 Collection Structure

```
NovaBank API/
├── General/
│   ├── Health Check
│   ├── API Info
│   └── Database Test
├── Authentication/
│   ├── Login
│   ├── Logout
│   └── Get Current User
└── Users/
    ├── Get All Users
    ├── Get User by ID
    ├── Create User
    ├── Update User
    └── Delete User
```

## 💡 Tips

- The **admin** credentials are pre-filled in the Login request
- After logging in, update the `token` environment variable
- Requests are organized by functionality
- Each request includes a description of what it does
- Admin-only endpoints are marked in their descriptions

## 🧪 Quick Test

1. Send **Health Check** (no auth required)
2. Send **Login** with admin credentials
3. Copy token and update environment
4. Send **Get Current User** to verify auth is working
5. Try other endpoints!
