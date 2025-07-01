import React, { useState } from 'react';
import { Box, Toolbar, useTheme, useMediaQuery } from '@mui/material';
import { Outlet } from 'react-router-dom';
import { AppHeader } from './AppHeader';
import { AppDrawer } from './AppDrawer';

interface AppLayoutProps {
  userName?: string;
  userType?: 'admin' | 'user';
  onLogout?: () => void;
}

const drawerWidth = 280;

export const AppLayout: React.FC<AppLayoutProps> = ({
  userName,
  userType,
  onLogout,
}) => {
  const theme = useTheme();
  const isMobile = useMediaQuery(theme.breakpoints.down('md'));
  const [drawerOpen, setDrawerOpen] = useState(!isMobile);

  const handleDrawerToggle = () => {
    setDrawerOpen(!drawerOpen);
  };

  const handleDrawerClose = () => {
    setDrawerOpen(false);
  };

  return (
    <Box sx={{ display: 'flex', minHeight: '100vh', bgcolor: 'background.default' }}>
      <AppHeader
        onMenuToggle={handleDrawerToggle}
        userName={userName}
        userType={userType}
        onLogout={onLogout}
      />
      
      <AppDrawer
        open={drawerOpen}
        onClose={handleDrawerClose}
        userRole={userType}
        variant={isMobile ? 'temporary' : 'persistent'}
        drawerWidth={drawerWidth}
      />
      
      <Box
        component="main"
        sx={{
          flexGrow: 1,
          ml: isMobile ? 0 : drawerOpen ? `${drawerWidth}px` : 0,
          transition: theme.transitions.create(['margin'], {
            easing: theme.transitions.easing.sharp,
            duration: theme.transitions.duration.leavingScreen,
          }),
          ...(drawerOpen && {
            transition: theme.transitions.create(['margin'], {
              easing: theme.transitions.easing.easeOut,
              duration: theme.transitions.duration.enteringScreen,
            }),
          }),
        }}
      >
        <Toolbar />
        <Box
          sx={{
            p: { xs: 2, sm: 3, md: 4 },
            maxWidth: 1440,
            margin: '0 auto',
            width: '100%',
          }}
        >
          <Outlet />
        </Box>
      </Box>
    </Box>
  );
};
