import { Components, Theme } from '@mui/material/styles';

export const components: Components<Theme> = {
  MuiCssBaseline: {
    styleOverrides: {
      body: {
        scrollbarColor: '#9E9E9E #F5F5F5',
        '&::-webkit-scrollbar, & *::-webkit-scrollbar': {
          width: '8px',
          height: '8px',
        },
        '&::-webkit-scrollbar-thumb, & *::-webkit-scrollbar-thumb': {
          borderRadius: 8,
          backgroundColor: '#9E9E9E',
          border: '2px solid #F5F5F5',
        },
        '&::-webkit-scrollbar-track, & *::-webkit-scrollbar-track': {
          borderRadius: 8,
          backgroundColor: '#F5F5F5',
        },
      },
    },
  },
  MuiButton: {
    styleOverrides: {
      root: {
        borderRadius: 8,
        textTransform: 'none',
        fontWeight: 500,
        padding: '8px 16px',
        transition: 'all 0.3s ease',
      },
      contained: {
        boxShadow: '0 2px 4px rgba(0,0,0,0.1)',
        '&:hover': {
          boxShadow: '0 4px 8px rgba(0,0,0,0.15)',
        },
      },
      sizeSmall: {
        padding: '6px 12px',
      },
      sizeLarge: {
        padding: '12px 24px',
        fontSize: '1rem',
      },
    },
  },
  MuiCard: {
    styleOverrides: {
      root: {
        borderRadius: 12,
        boxShadow: '0 2px 8px rgba(0,0,0,0.08)',
        transition: 'box-shadow 0.3s ease',
        '&:hover': {
          boxShadow: '0 4px 16px rgba(0,0,0,0.12)',
        },
      },
    },
  },
  MuiPaper: {
    styleOverrides: {
      root: {
        borderRadius: 12,
        boxShadow: '0 2px 8px rgba(0,0,0,0.08)',
      },
      elevation1: {
        boxShadow: '0 2px 4px rgba(0,0,0,0.08)',
      },
      elevation2: {
        boxShadow: '0 4px 8px rgba(0,0,0,0.10)',
      },
      elevation3: {
        boxShadow: '0 6px 12px rgba(0,0,0,0.12)',
      },
    },
  },
  MuiTextField: {
    defaultProps: {
      variant: 'outlined',
    },
    styleOverrides: {
      root: {
        '& .MuiOutlinedInput-root': {
          borderRadius: 8,
          backgroundColor: '#FFFFFF',
          transition: 'all 0.3s ease',
          '&:hover': {
            backgroundColor: '#FAFAFA',
          },
          '&.Mui-focused': {
            backgroundColor: '#FFFFFF',
          },
        },
      },
    },
  },
  MuiChip: {
    styleOverrides: {
      root: {
        borderRadius: 16,
        fontWeight: 500,
        transition: 'all 0.3s ease',
      },
      sizeSmall: {
        fontSize: '0.75rem',
      },
    },
  },
  MuiDialog: {
    styleOverrides: {
      paper: {
        borderRadius: 16,
        padding: 8,
      },
    },
  },
  MuiDrawer: {
    styleOverrides: {
      paper: {
        borderRight: 'none',
        boxShadow: '2px 0 8px rgba(0,0,0,0.08)',
      },
    },
  },
  MuiAppBar: {
    styleOverrides: {
      root: {
        boxShadow: '0 2px 4px rgba(0,0,0,0.08)',
        borderBottom: '1px solid rgba(0,0,0,0.08)',
      },
    },
  },
  MuiTableCell: {
    styleOverrides: {
      root: {
        borderBottom: '1px solid rgba(0,0,0,0.08)',
      },
      head: {
        fontWeight: 600,
        backgroundColor: '#F5F7FA',
      },
    },
  },
  MuiAlert: {
    styleOverrides: {
      root: {
        borderRadius: 8,
      },
    },
  },
  MuiLinearProgress: {
    styleOverrides: {
      root: {
        borderRadius: 4,
        height: 6,
      },
    },
  },
  MuiCircularProgress: {
    styleOverrides: {
      root: {
        color: 'inherit',
      },
    },
  },
  MuiSkeleton: {
    styleOverrides: {
      root: {
        borderRadius: 8,
      },
    },
  },
};
