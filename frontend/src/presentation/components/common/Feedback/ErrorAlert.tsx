import React from 'react';
import {
  Alert,
  AlertTitle,
  IconButton,
  Collapse,
  Box,
  Typography,
  Button,
  useTheme,
} from '@mui/material';
import {
  Close as CloseIcon,
  Error as ErrorIcon,
  Warning as WarningIcon,
  Info as InfoIcon,
  CheckCircle as SuccessIcon,
} from '@mui/icons-material';

interface ErrorAlertProps {
  open: boolean;
  severity?: 'error' | 'warning' | 'info' | 'success';
  title?: string;
  message: string;
  details?: string;
  action?: {
    label: string;
    onClick: () => void;
  };
  onClose?: () => void;
  autoHideDuration?: number;
}

const severityIcons = {
  error: ErrorIcon,
  warning: WarningIcon,
  info: InfoIcon,
  success: SuccessIcon,
};

export const ErrorAlert: React.FC<ErrorAlertProps> = ({
  open,
  severity = 'error',
  title,
  message,
  details,
  action,
  onClose,
  autoHideDuration,
}) => {
  const theme = useTheme();
  const Icon = severityIcons[severity];

  React.useEffect(() => {
    if (open && autoHideDuration && onClose) {
      const timer = setTimeout(() => {
        onClose();
      }, autoHideDuration);
      return () => clearTimeout(timer);
    }
  }, [open, autoHideDuration, onClose]);

  return (
    <Collapse in={open}>
      <Alert
        severity={severity}
        icon={<Icon />}
        action={
          <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
            {action && (
              <Button
                size="small"
                color="inherit"
                onClick={action.onClick}
                sx={{ fontWeight: 600 }}
              >
                {action.label}
              </Button>
            )}
            {onClose && (
              <IconButton
                aria-label="close"
                color="inherit"
                size="small"
                onClick={onClose}
              >
                <CloseIcon fontSize="small" />
              </IconButton>
            )}
          </Box>
        }
        sx={{
          mb: 2,
          borderRadius: 2,
          '.MuiAlert-icon': {
            fontSize: '1.5rem',
          },
        }}
      >
        {title && <AlertTitle sx={{ fontWeight: 600 }}>{title}</AlertTitle>}
        <Typography variant="body2">{message}</Typography>
        {details && (
          <Typography
            variant="caption"
            sx={{
              display: 'block',
              mt: 1,
              color: theme.palette.text.secondary,
              fontFamily: 'monospace',
              fontSize: '0.7rem',
            }}
          >
            {details}
          </Typography>
        )}
      </Alert>
    </Collapse>
  );
};
