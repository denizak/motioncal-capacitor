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
  
  s.pod_target_xcconfig = {
    'HEADER_SEARCH_PATHS' => '"${PODS_TARGET_SRCROOT}/common"',
    'SWIFT_OBJC_BRIDGING_HEADER' => '${PODS_TARGET_SRCROOT}/ios/Sources/MotionCalibrationPlugin/MotionCalibration-Bridging-Header.h'
  }
end
