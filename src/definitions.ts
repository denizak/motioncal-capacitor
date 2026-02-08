export interface MotionCalibrationPlugin {
  /**
   * Update the B (magnetic field magnitude) value
   * @param options - Object containing the value to set
   */
  updateBValue(options: { value: number }): Promise<void>;

  /**
   * Get the current B value
   * @returns Object containing the B value
   */
  getBValue(): Promise<{ value: number }>;

  /**
   * Check if send calibration is available
   * @returns Object containing availability status (0 or 1)
   */
  isSendCalAvailable(): Promise<{ available: number }>;

  /**
   * Read calibration data from a file
   * @param options - Object containing the filename
   * @returns Object containing the result code
   */
  readDataFromFile(options: { filename: string }): Promise<{ result: number }>;

  /**
   * Set the filename for writing calibration results
   * @param options - Object containing the filename
   */
  setResultFilename(options: { filename: string }): Promise<void>;

  /**
   * Send the calibration data
   * @returns Object containing the result code
   */
  sendCalibration(): Promise<{ result: number }>;

  /**
   * Get the quality surface gap error metric
   * @returns Object containing the error value
   */
  getQualitySurfaceGapError(): Promise<{ error: number }>;

  /**
   * Get the quality magnitude variance error metric
   * @returns Object containing the error value
   */
  getQualityMagnitudeVarianceError(): Promise<{ error: number }>;

  /**
   * Get the quality wobble error metric
   * @returns Object containing the error value
   */
  getQualityWobbleError(): Promise<{ error: number }>;

  /**
   * Get the quality spherical fit error metric
   * @returns Object containing the error value
   */
  getQualitySphericalFitError(): Promise<{ error: number }>;

  /**
   * Trigger the display callback
   */
  displayCallback(): Promise<void>;

  /**
   * Get the calibration data as a base64 encoded string
   * @returns Object containing the base64 encoded calibration data
   */
  getCalibrationData(): Promise<{ data: string }>;

  /**
   * Get the draw points for visualization
   * @returns Object containing the 2D array of points
   */
  getDrawPoints(): Promise<{ points: number[][] }>;

  /**
   * Reset the raw calibration data
   */
  resetRawData(): Promise<void>;

  /**
   * Get the hard iron offset values [x, y, z]
   * @returns Object containing the offset array
   */
  getHardIronOffset(): Promise<{ offset: number[] }>;

  /**
   * Get the soft iron matrix (3x3)
   * @returns Object containing the 3x3 matrix
   */
  getSoftIronMatrix(): Promise<{ matrix: number[][] }>;

  /**
   * Get the geomagnetic field magnitude
   * @returns Object containing the magnitude value
   */
  getGeomagneticFieldMagnitude(): Promise<{ magnitude: number }>;

  /**
   * Clear the draw points data
   */
  clearDrawPoints(): Promise<void>;
}
