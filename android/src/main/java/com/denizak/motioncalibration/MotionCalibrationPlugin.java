package com.denizak.motioncalibration;

import android.util.Base64;

import com.getcapacitor.JSArray;
import com.getcapacitor.JSObject;
import com.getcapacitor.Plugin;
import com.getcapacitor.PluginCall;
import com.getcapacitor.PluginMethod;
import com.getcapacitor.annotation.CapacitorPlugin;

import org.json.JSONArray;
import org.json.JSONException;

@CapacitorPlugin(name = "MotionCalibration")
public class MotionCalibrationPlugin extends Plugin {

    // Load the native library
    static {
        System.loadLibrary("motioncalibration");
    }

    // Native method declarations
    private native void updateBValueNative(float bValue);
    private native float getBValueNative();
    private native short isSendCalAvailableNative();
    private native int readDataFromFileNative(String filename);
    private native void setResultFilenameNative(String filename);
    private native int sendCalibrationNative();
    private native float getQualitySurfaceGapErrorNative();
    private native float getQualityMagnitudeVarianceErrorNative();
    private native float getQualityWobbleErrorNative();
    private native float getQualitySphericalFitErrorNative();
    private native void displayCallbackNative();
    private native byte[] getCalibrationDataNative();
    private native float[][] convertDrawPoints();
    private native void resetRawDataNative();
    private native float[] getHardIronOffsetNative();
    private native float[][] getSoftIronMatrixNative();
    private native float getGeomagneticFieldMagnitudeNative();
    private native void clearDrawPointsNative();

    @PluginMethod
    public void updateBValue(PluginCall call) {
        Float value = call.getFloat("value");
        if (value == null) {
            call.reject("Value is required");
            return;
        }
        updateBValueNative(value);
        call.resolve();
    }

    @PluginMethod
    public void getBValue(PluginCall call) {
        float result = getBValueNative();
        JSObject ret = new JSObject();
        ret.put("value", result);
        call.resolve(ret);
    }

    @PluginMethod
    public void isSendCalAvailable(PluginCall call) {
        short isAvailable = isSendCalAvailableNative();
        JSObject ret = new JSObject();
        ret.put("available", (int) isAvailable);
        call.resolve(ret);
    }

    @PluginMethod
    public void readDataFromFile(PluginCall call) {
        String filename = call.getString("filename");
        if (filename == null) {
            call.reject("Filename is required");
            return;
        }
        String fullPath = getContext().getFilesDir().getAbsolutePath() + "/" + filename;
        int result = readDataFromFileNative(fullPath);
        JSObject ret = new JSObject();
        ret.put("result", result);
        call.resolve(ret);
    }

    @PluginMethod
    public void setResultFilename(PluginCall call) {
        String filename = call.getString("filename");
        if (filename == null) {
            call.reject("Filename is required");
            return;
        }
        String fullPath = getContext().getFilesDir().getAbsolutePath() + "/" + filename;
        setResultFilenameNative(fullPath);
        call.resolve();
    }

    @PluginMethod
    public void sendCalibration(PluginCall call) {
        getActivity().runOnUiThread(() -> {
            new Thread(() -> {
                int result = sendCalibrationNative();
                JSObject ret = new JSObject();
                ret.put("result", result);
                call.resolve(ret);
            }).start();
        });
    }

    @PluginMethod
    public void getQualitySurfaceGapError(PluginCall call) {
        float error = getQualitySurfaceGapErrorNative();
        if (Float.isNaN(error)) {
            error = 100.0f;
        }
        JSObject ret = new JSObject();
        ret.put("error", error);
        call.resolve(ret);
    }

    @PluginMethod
    public void getQualityMagnitudeVarianceError(PluginCall call) {
        float error = getQualityMagnitudeVarianceErrorNative();
        if (Float.isNaN(error)) {
            error = 100.0f;
        }
        JSObject ret = new JSObject();
        ret.put("error", error);
        call.resolve(ret);
    }

    @PluginMethod
    public void getQualityWobbleError(PluginCall call) {
        float error = getQualityWobbleErrorNative();
        if (Float.isNaN(error)) {
            error = 100.0f;
        }
        JSObject ret = new JSObject();
        ret.put("error", error);
        call.resolve(ret);
    }

    @PluginMethod
    public void getQualitySphericalFitError(PluginCall call) {
        float error = getQualitySphericalFitErrorNative();
        if (Float.isNaN(error)) {
            error = 100.0f;
        }
        JSObject ret = new JSObject();
        ret.put("error", error);
        call.resolve(ret);
    }

    @PluginMethod
    public void displayCallback(PluginCall call) {
        displayCallbackNative();
        call.resolve();
    }

    @PluginMethod
    public void getCalibrationData(PluginCall call) {
        byte[] calibrationData = getCalibrationDataNative();
        if (calibrationData != null) {
            String base64Data = Base64.encodeToString(calibrationData, Base64.DEFAULT);
            JSObject ret = new JSObject();
            ret.put("data", base64Data);
            call.resolve(ret);
        } else {
            call.reject("No calibration data available");
        }
    }

    @PluginMethod
    public void getDrawPoints(PluginCall call) {
        new Thread(() -> {
            try {
                float[][] points = convertDrawPoints();
                JSArray jsonPoints = new JSArray();
                for (float[] point : points) {
                    JSArray jsonPoint = new JSArray();
                    for (float coord : point) {
                        jsonPoint.put(coord);
                    }
                    jsonPoints.put(jsonPoint);
                }
                JSObject ret = new JSObject();
                ret.put("points", jsonPoints);
                call.resolve(ret);
            } catch (Exception e) {
                call.reject("Failed to get draw points: " + e.getMessage());
            }
        }).start();
    }

    @PluginMethod
    public void resetRawData(PluginCall call) {
        resetRawDataNative();
        call.resolve();
    }

    @PluginMethod
    public void getHardIronOffset(PluginCall call) {
        try {
            float[] offset = getHardIronOffsetNative();
            JSArray jsonOffset = new JSArray();
            for (float val : offset) {
                jsonOffset.put(val);
            }
            JSObject ret = new JSObject();
            ret.put("offset", jsonOffset);
            call.resolve(ret);
        } catch (Exception e) {
            call.reject("Failed to get hard iron offset: " + e.getMessage());
        }
    }

    @PluginMethod
    public void getSoftIronMatrix(PluginCall call) {
        try {
            float[][] matrix = getSoftIronMatrixNative();
            JSArray jsonMatrix = new JSArray();
            for (float[] row : matrix) {
                JSArray jsonRow = new JSArray();
                for (float val : row) {
                    jsonRow.put(val);
                }
                jsonMatrix.put(jsonRow);
            }
            JSObject ret = new JSObject();
            ret.put("matrix", jsonMatrix);
            call.resolve(ret);
        } catch (Exception e) {
            call.reject("Failed to get soft iron matrix: " + e.getMessage());
        }
    }

    @PluginMethod
    public void getGeomagneticFieldMagnitude(PluginCall call) {
        float magnitude = getGeomagneticFieldMagnitudeNative();
        JSObject ret = new JSObject();
        ret.put("magnitude", magnitude);
        call.resolve(ret);
    }

    @PluginMethod
    public void clearDrawPoints(PluginCall call) {
        clearDrawPointsNative();
        call.resolve();
    }
}
