# Capacitor MotionCal Plugin

A Capacitor plugin for real-time magnetometer calibration using the Freescale/NXP MotionCal algorithm (ported from [PaulStoffregen/MotionCal](https://github.com/PaulStoffregen/MotionCal)). The plugin accepts raw IMU sensor readings and computes hard-iron offset, soft-iron matrix, and geomagnetic field magnitude — the three values needed to correct a magnetometer heading.

## How it works

```
Ionic app (sensor events)
        │
        ▼  rawData({ data: [9 int16 counts] })         — every sensor sample
  MotionCal plugin  ──►  C calibration engine
        │
        ▼  isSendCalAvailable()                        — poll until quality is good
        │
        ▼  sendCalibration()                           — finalise
        │
        ▼  getHardIronOffset() / getSoftIronMatrix()   — apply to compass readings
```

The calibration engine accumulates magnetic field samples in a sphere-fitting buffer. Once the sphere is well-covered (quality metrics below threshold), `isSendCalAvailable()` returns `1` and you can call `sendCalibration()` to lock in the result.

---

## Installation

### From npm (after publish)

```bash
npm install capacitor-motioncal
npx cap sync
```

### From GitHub

```bash
npm install github:denizak/motioncal-capacitor
npx cap sync
```

### Local path install (for testing before publish — see [Testing](#testing-locally-before-publishing))

```bash
npm install ../path/to/Capacitor-MotionCal
npx cap sync
```

---

## Ionic / Angular Integration

### 1. Install and sync

```bash
npm install capacitor-motioncal
npx cap sync
```

### 2. Create a calibration service

Create `src/app/services/motioncal.service.ts`:

```typescript
import { Injectable } from '@angular/core';
import { MotionCalibration } from 'capacitor-motioncal';

export interface CalibrationResult {
  hardIronOffset: number[];   // [x, y, z] in µT
  softIronMatrix: number[][]; // 3×3 correction matrix
  fieldMagnitude: number;     // geomagnetic field strength in µT
  drawPoints: number[][];     // collected sphere points for visualisation
}

@Injectable({ providedIn: 'root' })
export class MotionCalService {

  /** Feed one IMU sample into the calibration engine.
   *
   *  All values must be converted to int16 raw counts using:
   *    Accelerometer:  multiply g       × 8192  (G_PER_COUNT      = 1/8192)
   *    Gyroscope:      multiply deg/s   × 16    (DEG_PER_SEC_PER_COUNT = 1/16)
   *    Magnetometer:   multiply µT      × 10    (UT_PER_COUNT     = 0.1)
   */
  async feedSample(
    ax: number, ay: number, az: number,   // accelerometer in g
    gx: number, gy: number, gz: number,   // gyroscope in deg/s
    mx: number, my: number, mz: number,   // magnetometer in µT
  ): Promise<void> {
    await MotionCalibration.rawData({
      data: [
        Math.round(ax * 8192), Math.round(ay * 8192), Math.round(az * 8192),
        Math.round(gx * 16),   Math.round(gy * 16),   Math.round(gz * 16),
        Math.round(mx * 10),   Math.round(my * 10),   Math.round(mz * 10),
      ],
    });
  }

  /** Returns true when the algorithm has enough data for a good calibration. */
  async isReady(): Promise<boolean> {
    const { available } = await MotionCalibration.isSendCalAvailable();
    return available === 1;
  }

  /** Returns quality error values — lower is better.
   *  isSendCalAvailable() already gates on these, but you can surface them in UI. */
  async getQuality() {
    const [gap, variance, wobble, fit] = await Promise.all([
      MotionCalibration.getQualitySurfaceGapError(),
      MotionCalibration.getQualityMagnitudeVarianceError(),
      MotionCalibration.getQualityWobbleError(),
      MotionCalibration.getQualitySphericalFitError(),
    ]);
    return {
      surfaceGap:        gap.error,
      magnitudeVariance: variance.error,
      wobble:            wobble.error,
      sphericalFit:      fit.error,
    };
  }

  /** Finalise calibration and return all results. Call only after isReady() === true. */
  async finalise(): Promise<CalibrationResult> {
    await MotionCalibration.sendCalibration();

    const [offset, matrix, magnitude, points] = await Promise.all([
      MotionCalibration.getHardIronOffset(),
      MotionCalibration.getSoftIronMatrix(),
      MotionCalibration.getGeomagneticFieldMagnitude(),
      MotionCalibration.getDrawPoints(),
    ]);

    return {
      hardIronOffset: offset.offset,
      softIronMatrix: matrix.matrix,
      fieldMagnitude: magnitude.magnitude,
      drawPoints:     points.points,
    };
  }

  /** Reset the engine to start a new calibration session. */
  async reset(): Promise<void> {
    await MotionCalibration.resetRawData();
    await MotionCalibration.clearDrawPoints();
  }
}
```

### 3. Use the service in a component

```typescript
import { Component, OnDestroy } from '@angular/core';
import { MotionCalService, CalibrationResult } from '../services/motioncal.service';

// Use @capacitor/motion or the Web DeviceMotion API to obtain sensor events.
// The plugin also integrates with @awesome-cordova-plugins/device-motion if needed.

@Component({
  selector: 'app-calibration',
  template: `
    <ion-content>
      <ion-button (click)="start()" [disabled]="running">Start Calibration</ion-button>
      <ion-button (click)="stop()"  [disabled]="!running">Stop</ion-button>

      <div *ngIf="running">
        <p>Quality — gap: {{ quality?.surfaceGap | number:'1.3-3' }}
                    wobble: {{ quality?.wobble | number:'1.3-3' }}</p>
        <p *ngIf="ready">✓ Ready — tap Stop to finalise</p>
      </div>

      <div *ngIf="result">
        <h3>Calibration Complete</h3>
        <p>Hard iron offset: {{ result.hardIronOffset | json }}</p>
        <p>Field magnitude: {{ result.fieldMagnitude | number:'1.2-2' }} µT</p>
      </div>
    </ion-content>
  `,
})
export class CalibrationPage implements OnDestroy {
  running = false;
  ready   = false;
  quality: Awaited<ReturnType<MotionCalService['getQuality']>> | null = null;
  result: CalibrationResult | null = null;

  private intervalId: ReturnType<typeof setInterval> | null = null;

  constructor(private cal: MotionCalService) {}

  async start() {
    await this.cal.reset();
    this.running = true;
    this.result  = null;

    // Subscribe to your sensor source here.
    // Example using window.addEventListener for DeviceMotion + DeviceOrientation:
    window.addEventListener('devicemotion',      this.onMotion);
    window.addEventListener('deviceorientation', this.onOrientation);

    // Poll quality every 500 ms for UI feedback.
    this.intervalId = setInterval(async () => {
      this.quality = await this.cal.getQuality();
      this.ready   = await this.cal.isReady();
    }, 500);
  }

  async stop() {
    this.running = false;
    window.removeEventListener('devicemotion',      this.onMotion);
    window.removeEventListener('deviceorientation', this.onOrientation);
    if (this.intervalId) clearInterval(this.intervalId);

    if (this.ready) {
      this.result = await this.cal.finalise();
    }
  }

  // Replace with actual sensor values from your sensor plugin/service.
  private onMotion = async (e: DeviceMotionEvent) => {
    const a = e.accelerationIncludingGravity;
    const r = e.rotationRate;
    if (!a || !r) return;
    // Magnetometer must come from a separate sensor source (e.g. @capacitor/motion
    // does not expose magnetometer — use a dedicated plugin or native bridge).
    // Here mx/my/mz are placeholders; replace with real values.
    await this.cal.feedSample(
      a.x ?? 0, a.y ?? 0, a.z ?? 0,
      r.alpha ?? 0, r.beta ?? 0, r.gamma ?? 0,
      0, 0, 0, // ← replace with real magnetometer µT values
    );
  };

  private onOrientation = (_e: DeviceOrientationEvent) => { /* optional */ };

  ngOnDestroy() { this.stop(); }
}
```

### 4. Apply calibration to compass readings

```typescript
// After calibration is done, correct raw magnetometer readings before computing heading:
function applyCalibration(
  rawMag: [number, number, number],
  hardIron: number[],
  softIron: number[][],
): [number, number, number] {
  // Step 1: subtract hard-iron offset
  const hx = rawMag[0] - hardIron[0];
  const hy = rawMag[1] - hardIron[1];
  const hz = rawMag[2] - hardIron[2];

  // Step 2: multiply by soft-iron matrix
  return [
    softIron[0][0]*hx + softIron[0][1]*hy + softIron[0][2]*hz,
    softIron[1][0]*hx + softIron[1][1]*hy + softIron[1][2]*hz,
    softIron[2][0]*hx + softIron[2][1]*hy + softIron[2][2]*hz,
  ];
}

function headingDegrees(mx: number, my: number): number {
  return (Math.atan2(my, mx) * 180 / Math.PI + 360) % 360;
}
```

---

## Testing Locally Before Publishing

### Step 1 — Build the plugin

```bash
cd /path/to/Capacitor-MotionCal
npm run build
```

Verify `dist/` was generated with no TypeScript errors.

### Step 2 — Install into the Ionic app via local path

In your Ionic app directory:

```bash
npm install /path/to/Capacitor-MotionCal
npx cap sync
```

This installs the exact local build. You can iterate on the plugin and re-run `npm run build` + `npx cap sync` to pick up changes without publishing.

### Step 3 — Verify the iOS build compiles

Back in the plugin directory:

```bash
npm run verify:ios
```

This runs `pod install` + `xcodebuild` for the iOS target. Fix any Swift/C compile errors before proceeding.

### Step 4 — Verify the Android build compiles

```bash
npm run verify:android
```

Runs `./gradlew clean build test`. Requires the Android SDK in your `$PATH` / `$ANDROID_HOME`.

### Step 5 — Smoke test on a real device

1. Open the Ionic app in Xcode (`npx cap open ios`) or Android Studio (`npx cap open android`).
2. Run on a physical device (simulators have no magnetometer).
3. With the app open, wave/rotate the device in a figure-8 pattern to ensure `rawData` calls are being accepted and `isSendCalAvailable` eventually returns `1`.
4. Call `sendCalibration()` and log the results — `hardIronOffset` should be non-zero and `fieldMagnitude` should be in the range 20–80 µT (typical for Earth's field).

### Step 6 — Pack and inspect what will be published

```bash
npm pack --dry-run
```

This lists all files that will be included in the npm package. Verify:
- `dist/` is present
- `android/src/main/` is present
- `ios/Sources/` is present
- `common/` (C source files) is present
- No test files, node_modules, or local config bleed in

To create the actual tarball for manual inspection:

```bash
npm pack
# creates capacitor-motioncal-x.x.x.tgz
tar -tzf capacitor-motioncal-*.tgz | sort
```

### Step 7 — Publish

```bash
npm login          # one-time
npm publish
```

For a scoped package use `npm publish --access public`.

---

## Full API Reference

| Method | Parameters | Returns | Notes |
|--------|------------|---------|-------|
| `rawData` | `{ data: number[] }` | `Promise<void>` | 9 int16 counts; call every sensor sample |
| `isSendCalAvailable` | — | `Promise<{ available: number }>` | `1` = ready to finalise |
| `sendCalibration` | — | `Promise<{ result: number }>` | Finalises calibration; call after `isSendCalAvailable` |
| `getHardIronOffset` | — | `Promise<{ offset: number[] }>` | [x, y, z] bias in µT |
| `getSoftIronMatrix` | — | `Promise<{ matrix: number[][] }>` | 3×3 scaling/rotation matrix |
| `getGeomagneticFieldMagnitude` | — | `Promise<{ magnitude: number }>` | Earth field strength in µT |
| `getCalibrationData` | — | `Promise<{ data: string }>` | Raw 68-byte calibration packet as base64 |
| `getQualitySurfaceGapError` | — | `Promise<{ error: number }>` | Lower is better |
| `getQualityMagnitudeVarianceError` | — | `Promise<{ error: number }>` | Lower is better |
| `getQualityWobbleError` | — | `Promise<{ error: number }>` | Lower is better |
| `getQualitySphericalFitError` | — | `Promise<{ error: number }>` | Lower is better |
| `displayCallback` | — | `Promise<void>` | Triggers internal visualisation update |
| `getDrawPoints` | — | `Promise<{ points: number[][] }>` | Sphere surface points for 3D display |
| `clearDrawPoints` | — | `Promise<void>` | Clears the visualisation buffer |
| `resetRawData` | — | `Promise<void>` | Resets entire calibration state |

### int16 conversion factors

| Sensor | Unit | Multiply by | Constant |
|--------|------|-------------|----------|
| Accelerometer | g | 8192 | `G_PER_COUNT = 1/8192` |
| Gyroscope | deg/s | 16 | `DEG_PER_SEC_PER_COUNT = 1/16` |
| Magnetometer | µT | 10 | `UT_PER_COUNT = 0.1` |


