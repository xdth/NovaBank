import React from 'react';
import {
  Dashboard as DashboardIcon,
  AccountBalance as AccountBalanceIcon,
  SwapHoriz as TransferIcon,
  Receipt as TransactionIcon,
  AdminPanelSettings as AdminIcon,
  Person as PersonIcon,
  Settings as SettingsIcon,
} from '@mui/icons-material';
import { SvgIconComponent } from '@mui/icons-material';

export interface NavItem {
  id: string;
  title: string;
  path: string;
  icon: SvgIconComponent;
  badge?: number;
  requiredRole?: 'admin' | 'user';
  dividerAfter?: boolean;
}

export const navItems: NavItem[] = [
  {
    id: 'dashboard',
    title: 'Dashboard',
    path: '/',
    icon: DashboardIcon,
  },
  {
    id: 'accounts',
    title: 'Accounts',
    path: '/accounts',
    icon: AccountBalanceIcon,
  },
  {
    id: 'transfer',
    title: 'Transfer',
    path: '/transfer',
    icon: TransferIcon,
  },
  {
    id: 'transactions',
    title: 'Transactions',
    path: '/transactions',
    icon: TransactionIcon,
    dividerAfter: true,
  },
  {
    id: 'admin',
    title: 'Admin',
    path: '/admin',
    icon: AdminIcon,
    requiredRole: 'admin',
    dividerAfter: true,
  },
  {
    id: 'profile',
    title: 'Profile',
    path: '/profile',
    icon: PersonIcon,
  },
  {
    id: 'settings',
    title: 'Settings',
    path: '/settings',
    icon: SettingsIcon,
  },
];

export const getNavItemsByRole = (userRole?: 'admin' | 'user'): NavItem[] => {
  return navItems.filter(item => {
    if (!item.requiredRole) return true;
    return userRole === item.requiredRole;
  });
};
