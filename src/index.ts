import { registerPlugin } from '@capacitor/core';

import type { MotionCalibrationPlugin } from './definitions';

const MotionCalibration = registerPlugin<MotionCalibrationPlugin>('MotionCalibration', {
  web: () => import('./web').then((m) => new m.MotionCalibrationWeb()),
});

export * from './definitions';
export { MotionCalibration };
