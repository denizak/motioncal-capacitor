import { WebPlugin } from '@capacitor/core';

import type { MotionCalibrationPlugin } from './definitions';

export class MotionCalibrationWeb extends WebPlugin implements MotionCalibrationPlugin {
  async updateBValue(_options: { value: number }): Promise<void> {
    throw this.unimplemented('Not implemented on web.');
  }

  async getBValue(): Promise<{ value: number }> {
    throw this.unimplemented('Not implemented on web.');
  }

  async isSendCalAvailable(): Promise<{ available: number }> {
    throw this.unimplemented('Not implemented on web.');
  }

  async readDataFromFile(_options: { filename: string }): Promise<{ result: number }> {
    throw this.unimplemented('Not implemented on web.');
  }

  async setResultFilename(_options: { filename: string }): Promise<void> {
    throw this.unimplemented('Not implemented on web.');
  }

  async sendCalibration(): Promise<{ result: number }> {
    throw this.unimplemented('Not implemented on web.');
  }

  async getQualitySurfaceGapError(): Promise<{ error: number }> {
    throw this.unimplemented('Not implemented on web.');
  }

  async getQualityMagnitudeVarianceError(): Promise<{ error: number }> {
    throw this.unimplemented('Not implemented on web.');
  }

  async getQualityWobbleError(): Promise<{ error: number }> {
    throw this.unimplemented('Not implemented on web.');
  }

  async getQualitySphericalFitError(): Promise<{ error: number }> {
    throw this.unimplemented('Not implemented on web.');
  }

  async displayCallback(): Promise<void> {
    throw this.unimplemented('Not implemented on web.');
  }

  async getCalibrationData(): Promise<{ data: string }> {
    throw this.unimplemented('Not implemented on web.');
  }

  async getDrawPoints(): Promise<{ points: number[][] }> {
    throw this.unimplemented('Not implemented on web.');
  }

  async resetRawData(): Promise<void> {
    throw this.unimplemented('Not implemented on web.');
  }

  async getHardIronOffset(): Promise<{ offset: number[] }> {
    throw this.unimplemented('Not implemented on web.');
  }

  async getSoftIronMatrix(): Promise<{ matrix: number[][] }> {
    throw this.unimplemented('Not implemented on web.');
  }

  async getGeomagneticFieldMagnitude(): Promise<{ magnitude: number }> {
    throw this.unimplemented('Not implemented on web.');
  }

  async clearDrawPoints(): Promise<void> {
    throw this.unimplemented('Not implemented on web.');
  }
}
