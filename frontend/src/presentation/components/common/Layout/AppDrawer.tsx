import React from 'react';
import {
  Drawer,
  List,
  ListItem,
  ListItemButton,
  ListItemIcon,
  ListItemText,
  Divider,
  Box,
  Typography,
  useTheme,
  useMediaQuery,
  Badge,
  Toolbar,
  IconButton,
} from '@mui/material';
import { ChevronLeft as ChevronLeftIcon } from '@mui/icons-material';
import { useNavigate, useLocation } from 'react-router-dom';
import { NavItem, getNavItemsByRole } from './NavItems';
import { bankingColors } from '@presentation/theme';

interface AppDrawerProps {
  open: boolean;
  onClose: () => void;
  userRole?: 'admin' | 'user';
  variant?: 'permanent' | 'persistent' | 'temporary';
  drawerWidth?: number;
}

export const AppDrawer: React.FC<AppDrawerProps> = ({
  open,
  onClose,
  userRole = 'user',
  variant = 'permanent',
  drawerWidth = 280,
}) => {
  const theme = useTheme();
  const navigate = useNavigate();
  const location = useLocation();
  const isMobile = useMediaQuery(theme.breakpoints.down('md'));
  
  const navItems = getNavItemsByRole(userRole);
  const currentVariant = isMobile ? 'temporary' : variant;

  const handleNavigation = (path: string) => {
    navigate(path);
    if (isMobile) {
      onClose();
    }
  };

  const drawerContent = (
    <>
      <Toolbar
        sx={{
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'space-between',
          px: 2,
        }}
      >
        <Box sx={{ display: 'flex', alignItems: 'center', gap: 2 }}>
          <Box
            sx={{
              width: 40,
              height: 40,
              borderRadius: 2,
              background: bankingColors.primaryGradient,
              display: 'flex',
              alignItems: 'center',
              justifyContent: 'center',
              color: 'white',
              fontWeight: 'bold',
              fontSize: '1.25rem',
            }}
          >
            NB
          </Box>
          <Typography variant="h6" fontWeight="600">
            NovaBank
          </Typography>
        </Box>
        {isMobile && (
          <IconButton onClick={onClose} size="small">
            <ChevronLeftIcon />
          </IconButton>
        )}
      </Toolbar>
      <Divider />
      <Box sx={{ flexGrow: 1, py: 2 }}>
        <List>
          {navItems.map((item) => {
            const isSelected = location.pathname === item.path;
            
            return (
              <React.Fragment key={item.id}>
                <ListItem disablePadding sx={{ px: 2 }}>
                  <ListItemButton
                    onClick={() => handleNavigation(item.path)}
                    selected={isSelected}
                    sx={{
                      borderRadius: 2,
                      mb: 0.5,
                      '&.Mui-selected': {
                        backgroundColor: theme.palette.primary.main,
                        color: 'white',
                        '&:hover': {
                          backgroundColor: theme.palette.primary.dark,
                        },
                        '& .MuiListItemIcon-root': {
                          color: 'white',
                        },
                      },
                      '&:hover': {
                        backgroundColor: theme.palette.action.hover,
                      },
                    }}
                  >
                    <ListItemIcon
                      sx={{
                        minWidth: 40,
                        color: isSelected ? 'white' : theme.palette.text.secondary,
                      }}
                    >
                      <Badge
                        badgeContent={item.badge}
                        color="error"
                        invisible={!item.badge}
                      >
                        <item.icon />
                      </Badge>
                    </ListItemIcon>
                    <ListItemText
                      primary={item.title}
                      primaryTypographyProps={{
                        fontWeight: isSelected ? 600 : 400,
                      }}
                    />
                  </ListItemButton>
                </ListItem>
                {item.dividerAfter && (
                  <Divider sx={{ my: 1, mx: 2 }} />
                )}
              </React.Fragment>
            );
          })}
        </List>
      </Box>
      <Box sx={{ p: 2 }}>
        <Box
          sx={{
            p: 2,
            backgroundColor: theme.palette.grey[100],
            borderRadius: 2,
            textAlign: 'center',
          }}
        >
          <Typography variant="caption" color="text.secondary">
            NovaBank v1.0.0
          </Typography>
        </Box>
      </Box>
    </>
  );

  return (
    <Drawer
      variant={currentVariant}
      open={open}
      onClose={onClose}
      sx={{
        width: drawerWidth,
        flexShrink: 0,
        '& .MuiDrawer-paper': {
          width: drawerWidth,
          boxSizing: 'border-box',
          borderRight: currentVariant === 'permanent' ? 'none' : undefined,
          boxShadow: currentVariant === 'permanent' ? theme.shadows[1] : undefined,
        },
      }}
      ModalProps={{
        keepMounted: true, // Better mobile performance
      }}
    >
      {drawerContent}
    </Drawer>
  );
};
