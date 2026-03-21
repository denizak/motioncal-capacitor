import { WebPlugin } from '@capacitor/core';
export class MotionCalibrationWeb extends WebPlugin {
    async isSendCalAvailable() {
        throw this.unimplemented('Not implemented on web.');
    }
    async rawData(_options) {
        throw this.unimplemented('Not implemented on web.');
    }
    async sendCalibration() {
        throw this.unimplemented('Not implemented on web.');
    }
    async getQualitySurfaceGapError() {
        throw this.unimplemented('Not implemented on web.');
    }
    async getQualityMagnitudeVarianceError() {
        throw this.unimplemented('Not implemented on web.');
    }
    async getQualityWobbleError() {
        throw this.unimplemented('Not implemented on web.');
    }
    async getQualitySphericalFitError() {
        throw this.unimplemented('Not implemented on web.');
    }
    async displayCallback() {
        throw this.unimplemented('Not implemented on web.');
    }
    async getCalibrationData() {
        throw this.unimplemented('Not implemented on web.');
    }
    async getDrawPoints() {
        throw this.unimplemented('Not implemented on web.');
    }
    async resetRawData() {
        throw this.unimplemented('Not implemented on web.');
    }
    async getHardIronOffset() {
        throw this.unimplemented('Not implemented on web.');
    }
    async getSoftIronMatrix() {
        throw this.unimplemented('Not implemented on web.');
    }
    async getGeomagneticFieldMagnitude() {
        throw this.unimplemented('Not implemented on web.');
    }
    async clearDrawPoints() {
        throw this.unimplemented('Not implemented on web.');
    }
}
//# sourceMappingURL=web.js.map