import Foundation
import Capacitor

@objc(MotionCalibrationPlugin)
public class MotionCalibrationPlugin: CAPPlugin, CAPBridgedPlugin {
    public let identifier = "MotionCalibrationPlugin"
    public let jsName = "MotionCalibration"
    public let pluginMethods: [CAPPluginMethod] = [
        CAPPluginMethod(name: "updateBValue", returnType: CAPPluginReturnPromise),
        CAPPluginMethod(name: "getBValue", returnType: CAPPluginReturnPromise),
        CAPPluginMethod(name: "isSendCalAvailable", returnType: CAPPluginReturnPromise),
        CAPPluginMethod(name: "readDataFromFile", returnType: CAPPluginReturnPromise),
        CAPPluginMethod(name: "setResultFilename", returnType: CAPPluginReturnPromise),
        CAPPluginMethod(name: "sendCalibration", returnType: CAPPluginReturnPromise),
        CAPPluginMethod(name: "getQualitySurfaceGapError", returnType: CAPPluginReturnPromise),
        CAPPluginMethod(name: "getQualityMagnitudeVarianceError", returnType: CAPPluginReturnPromise),
        CAPPluginMethod(name: "getQualityWobbleError", returnType: CAPPluginReturnPromise),
        CAPPluginMethod(name: "getQualitySphericalFitError", returnType: CAPPluginReturnPromise),
        CAPPluginMethod(name: "displayCallback", returnType: CAPPluginReturnPromise),
        CAPPluginMethod(name: "getCalibrationData", returnType: CAPPluginReturnPromise),
        CAPPluginMethod(name: "getDrawPoints", returnType: CAPPluginReturnPromise),
        CAPPluginMethod(name: "resetRawData", returnType: CAPPluginReturnPromise),
        CAPPluginMethod(name: "getHardIronOffset", returnType: CAPPluginReturnPromise),
        CAPPluginMethod(name: "getSoftIronMatrix", returnType: CAPPluginReturnPromise),
        CAPPluginMethod(name: "getGeomagneticFieldMagnitude", returnType: CAPPluginReturnPromise),
        CAPPluginMethod(name: "clearDrawPoints", returnType: CAPPluginReturnPromise)
    ]
    
    public override func load() {
        // Initialize the C struct if needed
        motioncal.B = 0.0
    }
    
    @objc func updateBValue(_ call: CAPPluginCall) {
        guard let value = call.getFloat("value") else {
            call.reject("Value is required")
            return
        }
        motioncal.B = value * 2
        call.resolve()
    }
    
    @objc func getBValue(_ call: CAPPluginCall) {
        let bValue = motioncal.B
        call.resolve(["value": bValue])
    }
    
    @objc func isSendCalAvailable(_ call: CAPPluginCall) {
        let available = is_send_cal_available()
        call.resolve(["available": Int(available)])
    }
    
    @objc func readDataFromFile(_ call: CAPPluginCall) {
        guard let filename = call.getString("filename") else {
            call.reject("Filename is required")
            return
        }
        
        let documentsPath = NSSearchPathForDirectoriesInDomains(.documentDirectory, .userDomainMask, true)[0]
        let fullPath = (documentsPath as NSString).appendingPathComponent(filename)
        
        let result = read_ipc_file_data(fullPath)
        call.resolve(["result": Int(result)])
    }
    
    @objc func setResultFilename(_ call: CAPPluginCall) {
        guard let filename = call.getString("filename") else {
            call.reject("Filename is required")
            return
        }
        
        let documentsPath = NSSearchPathForDirectoriesInDomains(.documentDirectory, .userDomainMask, true)[0]
        let fullPath = (documentsPath as NSString).appendingPathComponent(filename)
        
        set_result_filename(fullPath)
        call.resolve()
    }
    
    @objc func sendCalibration(_ call: CAPPluginCall) {
        DispatchQueue.global(qos: .userInitiated).async {
            let result = send_calibration()
            raw_data_reset()
            
            DispatchQueue.main.async {
                call.resolve(["result": Int(result)])
            }
        }
    }
    
    @objc func getQualitySurfaceGapError(_ call: CAPPluginCall) {
        var error = quality_surface_gap_error()
        if error.isNaN {
            error = 100.0
        }
        call.resolve(["error": error])
    }
    
    @objc func getQualityMagnitudeVarianceError(_ call: CAPPluginCall) {
        var error = quality_magnitude_variance_error()
        if error.isNaN {
            error = 100.0
        }
        call.resolve(["error": error])
    }
    
    @objc func getQualityWobbleError(_ call: CAPPluginCall) {
        var error = quality_wobble_error()
        if error.isNaN {
            error = 100.0
        }
        call.resolve(["error": error])
    }
    
    @objc func getQualitySphericalFitError(_ call: CAPPluginCall) {
        var error = quality_spherical_fit_error()
        if error.isNaN {
            error = 100.0
        }
        call.resolve(["error": error])
    }
    
    @objc func displayCallback(_ call: CAPPluginCall) {
        display_callback()
        call.resolve()
    }
    
    @objc func getCalibrationData(_ call: CAPPluginCall) {
        if let dataPtr = get_calibration_data() {
            // Calibration data is 68 bytes (matching Obj-C implementation)
            let data = Data(bytes: dataPtr, count: 68)
            let base64String = data.base64EncodedString()
            call.resolve(["data": base64String])
        } else {
            call.reject("No calibration data available")
        }
    }
    
    @objc func getDrawPoints(_ call: CAPPluginCall) {
        DispatchQueue.global(qos: .userInitiated).async {
            let pointsPtr = get_draw_points()
            let count = get_draw_points_count()
            
            var points: [[Float]] = []
            if let ptr = pointsPtr, count > 0 {
                for i in 0..<Int(count) {
                    let x = ptr[i * 3]
                    let y = ptr[i * 3 + 1]
                    let z = ptr[i * 3 + 2]
                    points.append([x, y, z])
                }
            }
            
            DispatchQueue.main.async {
                call.resolve(["points": points])
            }
        }
    }
    
    @objc func resetRawData(_ call: CAPPluginCall) {
        raw_data_reset()
        call.resolve()
    }
    
    @objc func getHardIronOffset(_ call: CAPPluginCall) {
        var V: [Float] = [0, 0, 0]
        get_hard_iron_offset(&V)
        call.resolve(["offset": V])
    }
    
    @objc func getSoftIronMatrix(_ call: CAPPluginCall) {
        // C expects float[3][3], which Swift imports as pointer to tuple (Float, Float, Float)
        var matrix: ((Float, Float, Float), (Float, Float, Float), (Float, Float, Float)) = (
            (0, 0, 0),
            (0, 0, 0),
            (0, 0, 0)
        )
        withUnsafeMutablePointer(to: &matrix) { ptr in
            ptr.withMemoryRebound(to: (Float, Float, Float).self, capacity: 3) { rowPtr in
                get_soft_iron_matrix(rowPtr)
            }
        }
        // Convert tuple to 2D array for JSON response
        let invW: [[Float]] = [
            [matrix.0.0, matrix.0.1, matrix.0.2],
            [matrix.1.0, matrix.1.1, matrix.1.2],
            [matrix.2.0, matrix.2.1, matrix.2.2]
        ]
        call.resolve(["matrix": invW])
    }
    
    @objc func getGeomagneticFieldMagnitude(_ call: CAPPluginCall) {
        let magnitude = get_geomagnetic_field_magnitude()
        call.resolve(["magnitude": magnitude])
    }
    
    @objc func clearDrawPoints(_ call: CAPPluginCall) {
        clear_draw_points()
        call.resolve()
    }
}
