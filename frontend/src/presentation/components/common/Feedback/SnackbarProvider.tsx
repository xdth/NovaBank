import React, { ReactNode } from 'react';
import { SnackbarProvider as NotistackProvider, MaterialDesignContent } from 'notistack';
import { styled, useTheme } from '@mui/material/styles';
import {
  CheckCircle as SuccessIcon,
  Error as ErrorIcon,
  Info as InfoIcon,
  Warning as WarningIcon,
} from '@mui/icons-material';

interface SnackbarProviderProps {
  children: ReactNode;
}

const StyledMaterialDesignContent = styled(MaterialDesignContent)(({ theme }) => ({
  '&.notistack-MuiContent': {
    borderRadius: theme.shape.borderRadius * 1.5,
    boxShadow: theme.shadows[4],
    padding: theme.spacing(1.5, 2),
    minWidth: 300,
    fontSize: '0.875rem',
    fontWeight: 500,
  },
  '&.notistack-MuiContent-success': {
    backgroundColor: theme.palette.success.main,
    color: theme.palette.success.contrastText,
    '& .notistack-MuiContent-icon': {
      color: theme.palette.success.contrastText,
    },
  },
  '&.notistack-MuiContent-error': {
    backgroundColor: theme.palette.error.main,
    color: theme.palette.error.contrastText,
    '& .notistack-MuiContent-icon': {
      color: theme.palette.error.contrastText,
    },
  },
  '&.notistack-MuiContent-warning': {
    backgroundColor: theme.palette.warning.main,
    color: theme.palette.warning.contrastText,
    '& .notistack-MuiContent-icon': {
      color: theme.palette.warning.contrastText,
    },
  },
  '&.notistack-MuiContent-info': {
    backgroundColor: theme.palette.info.main,
    color: theme.palette.info.contrastText,
    '& .notistack-MuiContent-icon': {
      color: theme.palette.info.contrastText,
    },
  },
  '& .notistack-MuiContent-icon': {
    marginRight: theme.spacing(1.5),
    fontSize: '1.25rem',
  },
  '& .notistack-MuiContent-message': {
    display: 'flex',
    alignItems: 'center',
  },
}));

export const SnackbarProvider: React.FC<SnackbarProviderProps> = ({ children }) => {
  const theme = useTheme();

  return (
    <NotistackProvider
      maxSnack={3}
      autoHideDuration={4000}
      anchorOrigin={{
        vertical: 'top',
        horizontal: 'right',
      }}
      preventDuplicate
      iconVariant={{
        success: <SuccessIcon />,
        error: <ErrorIcon />,
        warning: <WarningIcon />,
        info: <InfoIcon />,
      }}
      Components={{
        success: StyledMaterialDesignContent,
        error: StyledMaterialDesignContent,
        warning: StyledMaterialDesignContent,
        info: StyledMaterialDesignContent,
        default: StyledMaterialDesignContent,
      }}
      TransitionProps={{
        direction: 'left',
      }}
      style={{
        marginTop: theme.spacing(8), // Below AppBar
      }}
    >
      {children}
    </NotistackProvider>
  );
};
