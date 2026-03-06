# CapacitorMotioncal - Bridging Header Fix

## Quick Summary for Dependency Agent
This fix resolves the iOS build error "Using bridging headers with framework targets is unsupported" by replacing the bridging header configuration with `SWIFT_INCLUDE_PATHS`. The changes are:
- Modified: `CapacitorMotionCal.podspec` (removed SWIFT_OBJC_BRIDGING_HEADER, added SWIFT_INCLUDE_PATHS)
- No changes needed to Swift code (C functions are accessible through header search paths)
- Optional: Delete bridging header file (no longer needed)

**No API changes** - This is purely a build system fix. All functionality remains identical.

## Problem

When building the iOS project with CocoaPods using `use_frameworks!`, the following error occurs:

```
error: Using bridging headers with framework targets is unsupported (in target 'CapacitorMotioncal' from project 'Pods')
```

This happens because bridging headers (`SWIFT_OBJC_BRIDGING_HEADER`) are only supported for app targets, not framework targets. When using `use_frameworks!` in the Podfile, all CocoaPods dependencies become framework targets.

## Solution

Replace the bridging header configuration with `SWIFT_INCLUDE_PATHS` to make C headers accessible to Swift code without using a bridging header.

## Required Changes

### 1. Update `CapacitorMotionCal.podspec`

Remove the bridging header configuration and add `SWIFT_INCLUDE_PATHS`:

```ruby
require 'json'

package = JSON.parse(File.read(File.join(__dir__, 'package.json')))

Pod::Spec.new do |s|
  s.name = 'CapacitorMotioncal'
  s.version = package['version']
  s.summary = package['description']
  s.license = package['license']
  s.homepage = package['repository']['url']
  s.author = package['author']
  s.source = { :git => package['repository']['url'], :tag => s.version.to_s }
  s.source_files = 'ios/Sources/**/*.{swift,h,m,c}', 'common/**/*.{h,c}'
  s.public_header_files = 'common/*.h'
  s.ios.deployment_target = '14.0'
  s.dependency 'Capacitor'
  s.swift_version = '5.1'
  
  # CHANGED: Remove SWIFT_OBJC_BRIDGING_HEADER, use SWIFT_INCLUDE_PATHS instead
  s.pod_target_xcconfig = {
    'HEADER_SEARCH_PATHS' => '"${PODS_TARGET_SRCROOT}/common"',
    'SWIFT_INCLUDE_PATHS' => '$(PODS_TARGET_SRCROOT)/common',
    'DEFINES_MODULE' => 'YES'
  }
  
  s.xcconfig = { 
    'SWIFT_INCLUDE_PATHS' => '$(PODS_TARGET_SRCROOT)/common' 
  }
end
```

**Key Changes:**
- ✅ Removed: `'SWIFT_OBJC_BRIDGING_HEADER' => '${PODS_TARGET_SRCROOT}/ios/Sources/MotionCalibrationPlugin/MotionCalibration-Bridging-Header.h'`
- ✅ Added: `'SWIFT_INCLUDE_PATHS' => '$(PODS_TARGET_SRCROOT)/common'`
- ✅ Added: `'DEFINES_MODULE' => 'YES'`
- ✅ Added: `s.xcconfig` block to ensure Swift can find the headers at both pod target and user target levels

### 2. Remove Bridging Header File (Optional)

The bridging header file at `ios/Sources/MotionCalibrationPlugin/MotionCalibration-Bridging-Header.h` is no longer needed and can be deleted:

```bash
rm ios/Sources/MotionCalibrationPlugin/MotionCalibration-Bridging-Header.h
```

**Note:** The Swift code in `MotionCalibrationPlugin.swift` already calls the C functions directly by name (e.g., `motioncal.B`, `read_ipc_file_data()`, etc.), so the bridging header was only providing the declarations. With `HEADER_SEARCH_PATHS` configured, Swift can find these declarations in the C headers themselves.

### 3. Verify Swift Code

Ensure `MotionCalibrationPlugin.swift` doesn't import any module for the C code:

```swift
import Foundation
import Capacitor
// ✅ No import of bridging header module needed

@objc(MotionCalibrationPlugin)
public class MotionCalibrationPlugin: CAPPlugin, CAPBridgedPlugin {
    // ... rest of the code
}
```

The C functions are now accessible directly through the header search paths configured in the podspec.

## Technical Explanation

### Why This Works

1. **HEADER_SEARCH_PATHS**: Tells the C compiler where to find the `.h` files when compiling C source files
2. **SWIFT_INCLUDE_PATHS**: Tells the Swift compiler where to find the C headers so Swift can see the function declarations
3. **DEFINES_MODULE**: Enables module generation, which is required for framework targets

### Bridging Headers vs. Include Paths

| Aspect | Bridging Header | Include Paths |
|--------|----------------|---------------|
| Target Type | App targets only | Both app and framework targets |
| Use with `use_frameworks!` | ❌ Not supported | ✅ Supported |
| Complexity | Single file | Configured in podspec |
| Maintenance | Manual header list | Automatic from source files |

## Testing the Fix

After making these changes, verify the build works:

```bash
# Navigate to iOS directory
cd ios/App

# Clean and reinstall pods
pod deintegrate
pod install

# Build the project
xcodebuild -workspace App.xcworkspace -scheme App -configuration Debug -destination 'generic/platform=iOS' build
```

Expected output: `** BUILD SUCCEEDED **`

## Files to Commit

- `CapacitorMotionCal.podspec` (modified)
- `ios/Sources/MotionCalibrationPlugin/MotionCalibration-Bridging-Header.h` (deleted, optional)

## Version Update

Consider incrementing the plugin version in `package.json` and `CapacitorMotionCal.podspec` after these changes:

```json
{
  "version": "0.0.3"
}
```

## Related Documentation

- [CocoaPods: Creating a Pod](https://guides.cocoapods.org/making/making-a-cocoapod.html)
- [Swift and C Interoperability](https://developer.apple.com/documentation/swift/imported-c-and-objective-c-apis/importing-objective-c-into-swift)
- [Swift Include Paths](https://developer.apple.com/documentation/xcode/build-settings-reference#Header-Search-Paths)