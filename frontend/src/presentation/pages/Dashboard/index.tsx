import React from 'react';
import { Box, Typography, Card, CardContent, IconButton } from '@mui/material';
import Grid from '@mui/material/Grid';
import { TrendingUp } from '@mui/icons-material';
import { useSnackbar } from 'notistack';
import { bankingColors } from '@presentation/theme';

export const Dashboard: React.FC = () => {
  const { enqueueSnackbar } = useSnackbar();

  React.useEffect(() => {
    enqueueSnackbar('Welcome to NovaBank!', { variant: 'success' });
  }, [enqueueSnackbar]);

  return (
    <Box>
      <Typography variant="h4" gutterBottom fontWeight="600">
        Dashboard
      </Typography>
      <Grid container spacing={3}>
        {/* <Grid item xs={12} md={4}> */}
        <Grid size={4}>
          <Card>
            <CardContent>
              <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'flex-start' }}>
                <Box>
                  <Typography variant="subtitle2" color="text.secondary" gutterBottom>
                    Total Balance
                  </Typography>
                  <Typography variant="h3" sx={{ fontFeatureSettings: '"tnum"', fontWeight: 700 }}>
                    $12,345.67
                  </Typography>
                  <Box sx={{ display: 'flex', alignItems: 'center', mt: 1 }}>
                    <TrendingUp sx={{ color: bankingColors.positive, mr: 0.5, fontSize: 20 }} />
                    <Typography variant="body2" sx={{ color: bankingColors.positive }}>
                      +5.2% from last month
                    </Typography>
                  </Box>
                </Box>
                <IconButton size="small" color="primary">
                  <TrendingUp />
                </IconButton>
              </Box>
            </CardContent>
          </Card>
        </Grid>
        
        {/* <Grid item xs={12} md={4}> */}
        <Grid size={4}>
          <Card>
            <CardContent>
              <Typography variant="subtitle2" color="text.secondary" gutterBottom>
                Checking Account
              </Typography>
              <Typography variant="h4" sx={{ fontFeatureSettings: '"tnum"', fontWeight: 600 }}>
                $8,234.56
              </Typography>
            </CardContent>
          </Card>
        </Grid>
        
        {/* <Grid item xs={12} md={4}> */}
        <Grid size={4}>
          <Card>
            <CardContent>
              <Typography variant="subtitle2" color="text.secondary" gutterBottom>
                Savings Account
              </Typography>
              <Typography variant="h4" sx={{ fontFeatureSettings: '"tnum"', fontWeight: 600 }}>
                $4,111.11
              </Typography>
            </CardContent>
          </Card>
        </Grid>
      </Grid>
    </Box>
  );
};
