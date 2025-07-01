import React from 'react';
import { BrowserRouter, Routes, Route } from 'react-router-dom';
import { ThemeContextProvider } from '@presentation/contexts/ThemeContext';
import { ThemeProvider } from '@presentation/theme/ThemeProvider';
import { SnackbarProvider } from '@presentation/components/common/Feedback';
import { AppLayout } from '@presentation/components/common/Layout';
import { Dashboard } from '@presentation/pages/Dashboard';

function App() {
  // TODO: Replace with actual auth context values
  const userName = 'John Doe';
  const userType: 'admin' | 'user' = 'admin';
  
  const handleLogout = () => {
    // TODO: Implement logout logic
    console.log('Logout clicked');
  };

  return (
    <ThemeContextProvider>
      <ThemeProvider>
        <SnackbarProvider>
          <BrowserRouter>
            <Routes>
              <Route
                path="/"
                element={
                  <AppLayout
                    userName={userName}
                    userType={userType}
                    onLogout={handleLogout}
                  />
                }
              >
                <Route index element={<Dashboard />} />
                <Route path="accounts" element={<Dashboard />} />
                <Route path="transfer" element={<Dashboard />} />
                <Route path="transactions" element={<Dashboard />} />
                <Route path="admin" element={<Dashboard />} />
                <Route path="profile" element={<Dashboard />} />
                <Route path="settings" element={<Dashboard />} />
              </Route>
            </Routes>
          </BrowserRouter>
        </SnackbarProvider>
      </ThemeProvider>
    </ThemeContextProvider>
  );
}

export default App;
