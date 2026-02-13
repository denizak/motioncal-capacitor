import { registerPlugin } from '@capacitor/core';
const MotionCalibration = registerPlugin('MotionCalibration', {
    web: () => import('./web').then((m) => new m.MotionCalibrationWeb()),
});
export * from './definitions';
export { MotionCalibration };
//# sourceMappingURL=index.js.map