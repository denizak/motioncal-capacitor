export interface MotionCalibrationPlugin {
    /**
     * Check if send calibration is available
     * @returns Object containing availability status (0 or 1)
     */
    isSendCalAvailable(): Promise<{
        available: number;
    }>;
    /**
     * Pass raw sensor data directly to the calibration engine.
     * Replaces the serial port data feed from the upstream MotionCal desktop app.
     * @param options - Object containing an array of 9 int16 values:
     *   [accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z, mag_x, mag_y, mag_z]
     *   Conversion factors (from imuread.h):
     *   - Accel: multiply g values by 8192 (G_PER_COUNT = 1/8192)
     *   - Gyro: multiply deg/s values by 16 (DEG_PER_SEC_PER_COUNT = 1/16)
     *   - Mag: multiply µT values by 10 (UT_PER_COUNT = 0.1)
     */
    rawData(options: {
        data: number[];
    }): Promise<void>;
    /**
     * Send the calibration data
     * @returns Object containing the result code
     */
    sendCalibration(): Promise<{
        result: number;
    }>;
    /**
     * Get the quality surface gap error metric
     * @returns Object containing the error value
     */
    getQualitySurfaceGapError(): Promise<{
        error: number;
    }>;
    /**
     * Get the quality magnitude variance error metric
     * @returns Object containing the error value
     */
    getQualityMagnitudeVarianceError(): Promise<{
        error: number;
    }>;
    /**
     * Get the quality wobble error metric
     * @returns Object containing the error value
     */
    getQualityWobbleError(): Promise<{
        error: number;
    }>;
    /**
     * Get the quality spherical fit error metric
     * @returns Object containing the error value
     */
    getQualitySphericalFitError(): Promise<{
        error: number;
    }>;
    /**
     * Trigger the display callback
     */
    displayCallback(): Promise<void>;
    /**
     * Get the calibration data as a base64 encoded string
     * @returns Object containing the base64 encoded calibration data
     */
    getCalibrationData(): Promise<{
        data: string;
    }>;
    /**
     * Get the draw points for visualization
     * @returns Object containing the 2D array of points
     */
    getDrawPoints(): Promise<{
        points: number[][];
    }>;
    /**
     * Reset the raw calibration data
     */
    resetRawData(): Promise<void>;
    /**
     * Get the hard iron offset values [x, y, z]
     * @returns Object containing the offset array
     */
    getHardIronOffset(): Promise<{
        offset: number[];
    }>;
    /**
     * Get the soft iron matrix (3x3)
     * @returns Object containing the 3x3 matrix
     */
    getSoftIronMatrix(): Promise<{
        matrix: number[][];
    }>;
    /**
     * Get the geomagnetic field magnitude
     * @returns Object containing the magnitude value
     */
    getGeomagneticFieldMagnitude(): Promise<{
        magnitude: number;
    }>;
    /**
     * Clear the draw points data
     */
    clearDrawPoints(): Promise<void>;
}
