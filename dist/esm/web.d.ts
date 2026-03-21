import { WebPlugin } from '@capacitor/core';
import type { MotionCalibrationPlugin } from './definitions';
export declare class MotionCalibrationWeb extends WebPlugin implements MotionCalibrationPlugin {
    isSendCalAvailable(): Promise<{
        available: number;
    }>;
    rawData(_options: {
        data: number[];
    }): Promise<void>;
    sendCalibration(): Promise<{
        result: number;
    }>;
    getQualitySurfaceGapError(): Promise<{
        error: number;
    }>;
    getQualityMagnitudeVarianceError(): Promise<{
        error: number;
    }>;
    getQualityWobbleError(): Promise<{
        error: number;
    }>;
    getQualitySphericalFitError(): Promise<{
        error: number;
    }>;
    displayCallback(): Promise<void>;
    getCalibrationData(): Promise<{
        data: string;
    }>;
    getDrawPoints(): Promise<{
        points: number[][];
    }>;
    resetRawData(): Promise<void>;
    getHardIronOffset(): Promise<{
        offset: number[];
    }>;
    getSoftIronMatrix(): Promise<{
        matrix: number[][];
    }>;
    getGeomagneticFieldMagnitude(): Promise<{
        magnitude: number;
    }>;
    clearDrawPoints(): Promise<void>;
}
