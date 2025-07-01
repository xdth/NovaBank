import { createTheme, ThemeOptions, PaletteMode } from '@mui/material/styles';
import { getLightPalette, getDarkPalette } from './palette';
import { typography } from './typography';
import { components } from './components';
import { breakpoints } from './breakpoints';

export const getTheme = (mode: PaletteMode) => {
  const palette = mode === 'light' ? getLightPalette() : getDarkPalette();
  
  const themeOptions: ThemeOptions = {
    palette,
    typography,
    components,
    breakpoints,
    shape: {
      borderRadius: 8,
    },
    spacing: 8,
    transitions: {
      duration: {
        shortest: 150,
        shorter: 200,
        short: 250,
        standard: 300,
        complex: 375,
        enteringScreen: 225,
        leavingScreen: 195,
      },
      easing: {
        easeInOut: 'cubic-bezier(0.4, 0, 0.2, 1)',
        easeOut: 'cubic-bezier(0.0, 0, 0.2, 1)',
        easeIn: 'cubic-bezier(0.4, 0, 1, 1)',
        sharp: 'cubic-bezier(0.4, 0, 0.6, 1)',
      },
    },
  };

  return createTheme(themeOptions);
};

// Export theme utilities
export { bankingColors } from './palette';
export { responsiveSpacing } from './breakpoints';
