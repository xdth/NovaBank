import { PaletteOptions } from '@mui/material/styles';

export const getLightPalette = (): PaletteOptions => ({
  mode: 'light',
  primary: {
    main: '#1976D2',
    light: '#42A5F5',
    dark: '#1565C0',
    contrastText: '#FFFFFF',
  },
  secondary: {
    main: '#00897B',
    light: '#4DB6AC',
    dark: '#00695C',
    contrastText: '#FFFFFF',
  },
  error: {
    main: '#D32F2F',
    light: '#EF5350',
    dark: '#C62828',
  },
  warning: {
    main: '#F57C00',
    light: '#FFB74D',
    dark: '#E65100',
  },
  success: {
    main: '#388E3C',
    light: '#66BB6A',
    dark: '#2E7D32',
  },
  info: {
    main: '#0288D1',
    light: '#29B6F6',
    dark: '#01579B',
  },
  background: {
    default: '#F5F7FA',
    paper: '#FFFFFF',
  },
  text: {
    primary: 'rgba(0, 0, 0, 0.87)',
    secondary: 'rgba(0, 0, 0, 0.6)',
  },
});

export const getDarkPalette = (): PaletteOptions => ({
  mode: 'dark',
  primary: {
    main: '#42A5F5',
    light: '#64B5F6',
    dark: '#1976D2',
    contrastText: '#000000',
  },
  secondary: {
    main: '#4DB6AC',
    light: '#80CBC4',
    dark: '#00897B',
    contrastText: '#000000',
  },
  error: {
    main: '#EF5350',
    light: '#E57373',
    dark: '#D32F2F',
  },
  warning: {
    main: '#FFB74D',
    light: '#FFCC80',
    dark: '#F57C00',
  },
  success: {
    main: '#66BB6A',
    light: '#81C784',
    dark: '#388E3C',
  },
  info: {
    main: '#29B6F6',
    light: '#4FC3F7',
    dark: '#0288D1',
  },
  background: {
    default: '#0A1929',
    paper: '#0F2942',
  },
  text: {
    primary: '#FFFFFF',
    secondary: 'rgba(255, 255, 255, 0.7)',
  },
});

// Banking-specific colors remain the same
export const bankingColors = {
  positive: '#4CAF50',
  negative: '#F44336',
  neutral: '#9E9E9E',
  checking: '#2196F3',
  savings: '#00897B',
  deposit: '#4CAF50',
  withdrawal: '#F44336',
  transfer: '#2196F3',
  primaryGradient: 'linear-gradient(135deg, #1976D2 0%, #42A5F5 100%)',
  secondaryGradient: 'linear-gradient(135deg, #00897B 0%, #4DB6AC 100%)',
};
