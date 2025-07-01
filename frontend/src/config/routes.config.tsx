import React, { lazy, Suspense } from 'react';
import { RouteObject } from 'react-router-dom';
import { LoadingBackdrop } from '@presentation/components/common/Feedback';

// Lazy load pages
const Dashboard = lazy(() => import('@presentation/pages/Dashboard').then(m => ({ default: m.Dashboard })));
const NotFound = lazy(() => import('@presentation/pages/NotFound').then(m => ({ default: m.NotFound })));

// Loading component for lazy loaded pages
const PageLoader = () => <LoadingBackdrop open={true} message="Loading page..." />;

// Wrap lazy components with Suspense
const withSuspense = (Component: React.LazyExoticComponent<React.FC>) => {
  return (
    <Suspense fallback={<PageLoader />}>
      <Component />
    </Suspense>
  );
};

export const routes: RouteObject[] = [
  {
    path: '/',
    element: withSuspense(Dashboard),
  },
  {
    path: '/accounts',
    element: withSuspense(Dashboard), // Placeholder for now
  },
  {
    path: '/transfer',
    element: withSuspense(Dashboard), // Placeholder for now
  },
  {
    path: '/transactions',
    element: withSuspense(Dashboard), // Placeholder for now
  },
  {
    path: '/admin',
    element: withSuspense(Dashboard), // Placeholder for now
  },
  {
    path: '/profile',
    element: withSuspense(Dashboard), // Placeholder for now
  },
  {
    path: '/settings',
    element: withSuspense(Dashboard), // Placeholder for now
  },
  {
    path: '*',
    element: withSuspense(NotFound),
  },
];
