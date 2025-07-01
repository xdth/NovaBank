import React from 'react';
import {
  Backdrop,
  CircularProgress,
  Box,
  Typography,
  useTheme,
  alpha,
} from '@mui/material';

interface LoadingBackdropProps {
  open: boolean;
  message?: string;
}

export const LoadingBackdrop: React.FC<LoadingBackdropProps> = ({
  open,
  message = 'Loading...',
}) => {
  const theme = useTheme();

  return (
    <Backdrop
      sx={{
        color: '#fff',
        zIndex: theme.zIndex.drawer + 1,
        backgroundColor: alpha(theme.palette.common.black, 0.7),
      }}
      open={open}
    >
      <Box
        sx={{
          display: 'flex',
          flexDirection: 'column',
          alignItems: 'center',
          gap: 2,
        }}
      >
        <CircularProgress
          size={48}
          thickness={4}
          sx={{
            color: theme.palette.primary.light,
          }}
        />
        <Typography variant="subtitle1" sx={{ mt: 1 }}>
          {message}
        </Typography>
      </Box>
    </Backdrop>
  );
};
