# Capacitor MotionCal Plugin

A Capacitor plugin for motion/magnetometer calibration using native C libraries.

## Installation

Install directly from GitHub (capacitor branch):

```bash
npm install github:denizak/Cordova-MotionCal#capacitor
npx cap sync
```

Or clone and install locally:

```bash
git clone -b capacitor https://github.com/denizak/Cordova-MotionCal.git capacitor-motioncal
npm install ./capacitor-motioncal
npx cap sync
```

## API

### updateBValue(options)

Update the B (magnetic field magnitude) value.

```typescript
import { MotionCalibration } from 'capacitor-motioncal';

await MotionCalibration.updateBValue({ value: 45.0 });
```

### getBValue()

Get the current B value.

```typescript
const result = await MotionCalibration.getBValue();
console.log('B value:', result.value);
```

### readDataFromFile(options)

Read calibration data from a file.

```typescript
const result = await MotionCalibration.readDataFromFile({ filename: 'magdata.bin' });
console.log('Read result:', result.result);
```

### sendCalibration()

Send/process the calibration data.

```typescript
const result = await MotionCalibration.sendCalibration();
console.log('Calibration result:', result.result);
```

### Quality Metrics

```typescript
const gapError = await MotionCalibration.getQualitySurfaceGapError();
const varianceError = await MotionCalibration.getQualityMagnitudeVarianceError();
const wobbleError = await MotionCalibration.getQualityWobbleError();
const fitError = await MotionCalibration.getQualitySphericalFitError();
```

### Calibration Results

```typescript
// Get hard iron offset [x, y, z]
const offset = await MotionCalibration.getHardIronOffset();
console.log('Hard iron offset:', offset.offset);

// Get soft iron matrix (3x3)
const matrix = await MotionCalibration.getSoftIronMatrix();
console.log('Soft iron matrix:', matrix.matrix);

// Get geomagnetic field magnitude
const magnitude = await MotionCalibration.getGeomagneticFieldMagnitude();
console.log('Field magnitude:', magnitude.magnitude);
```

### Visualization

```typescript
// Get points for 3D visualization
const points = await MotionCalibration.getDrawPoints();
console.log('Draw points:', points.points);

// Clear points
await MotionCalibration.clearDrawPoints();
```

### Reset

```typescript
await MotionCalibration.resetRawData();
```

## Full API Reference

| Method | Parameters | Returns |
|--------|------------|---------|
| `updateBValue` | `{ value: number }` | `Promise<void>` |
| `getBValue` | - | `Promise<{ value: number }>` |
| `isSendCalAvailable` | - | `Promise<{ available: number }>` |
| `readDataFromFile` | `{ filename: string }` | `Promise<{ result: number }>` |
| `setResultFilename` | `{ filename: string }` | `Promise<void>` |
| `sendCalibration` | - | `Promise<{ result: number }>` |
| `getQualitySurfaceGapError` | - | `Promise<{ error: number }>` |
| `getQualityMagnitudeVarianceError` | - | `Promise<{ error: number }>` |
| `getQualityWobbleError` | - | `Promise<{ error: number }>` |
| `getQualitySphericalFitError` | - | `Promise<{ error: number }>` |
| `displayCallback` | - | `Promise<void>` |
| `getCalibrationData` | - | `Promise<{ data: string }>` |
| `getDrawPoints` | - | `Promise<{ points: number[][] }>` |
| `resetRawData` | - | `Promise<void>` |
| `getHardIronOffset` | - | `Promise<{ offset: number[] }>` |
| `getSoftIronMatrix` | - | `Promise<{ matrix: number[][] }>` |
| `getGeomagneticFieldMagnitude` | - | `Promise<{ magnitude: number }>` |
| `clearDrawPoints` | - | `Promise<void>` |

## Branch Structure

This repository contains both Cordova and Capacitor versions:

- **`main` branch**: Cordova plugin (in `MotionCalCordova/`)
- **`capacitor` branch**: Capacitor plugin (at root level)
