# Capacitor 8 migration validation

Date: 2026-11-04. Branch: `codex/capacitor-8`. Candidate package: 0.0.6.

## Changes

- Capacitor development dependencies and peer requirement moved from 7 to 8.
- Android fallback SDK levels: min 24, compile/target 36; AGP 8.13.0 and current
  AndroidX test/AppCompat defaults. The consuming app supplies Gradle 8.14.3+.
- iOS deployment target is 15.0; CocoaPods now requires Capacitor ~> 8.0.
- Android ELF load segments have 16 KB alignment, incorporating SensaLink's
  previously separate CMake patch.
- Source lint excludes generated declarations.
- Calibration algorithms (`common/`), native method implementations, JS API,
  and generated `dist/` contents are unchanged.

## Checks performed

- `npm run build`: passed (doc generation, TypeScript, Rollup).
- `npm run eslint`: passed, with the existing unused `_options` warning in the
  unimplemented web stub.
- `npm pack`: passed; package includes native sources, C algorithms, and dist.
- Installed the package in an isolated host at
  `/tmp/motioncal-capacitor8-host`, with Capacitor Core/Android/iOS/CLI 8.6.0.
- Android `:app:assembleDebug`: passed using JDK 21, SDK 36 and Gradle 8.14.3.
  All four plugin ABI libraries compiled and packaged.
- Inspected APK ELF PT_LOAD headers: every segment of `libmotioncalibration.so`
  for arm64-v8a, armeabi-v7a, x86 and x86_64 has alignment 16384.
- iOS CocoaPods sync: passed. Simulator Debug host build passed for arm64 and
  x86_64 with Xcode 27 and signing disabled. Build output is under
  `/tmp/motioncal-capacitor8-derived-data`.
- `git diff --check`: passed.

## Remaining release checks

Builds establish native API/build compatibility, not calibration accuracy or
physical-device behavior. Before release, exercise rawData ingestion, reset,
quality metrics, readiness, finalization, calibration blob export, and repeat
sessions on Android and iOS with representative captured/real IMU samples.
Verify expected calibration values against known data. Swift Package Manager
support is not included; use CocoaPods for iOS.

No package was published and no release tag was created. Initial native
validation used the isolated host because the shared SensaLink checkout was
being reset externally. Subsequent integration in SensaLink commit `4e19de5`
uses this candidate archive and passed full Android/iOS simulator builds and
95 suites / 1,383 tests in a separate worktree. Physical-device release checks
remain outstanding as described above.
