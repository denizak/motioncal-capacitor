'use strict';

var core = require('@capacitor/core');

const MotionCalibration = core.registerPlugin('MotionCalibration', {
    web: () => Promise.resolve().then(function () { return web; }).then((m) => new m.MotionCalibrationWeb()),
});

class MotionCalibrationWeb extends core.WebPlugin {
    async updateBValue(_options) {
        throw this.unimplemented('Not implemented on web.');
    }
    async getBValue() {
        throw this.unimplemented('Not implemented on web.');
    }
    async isSendCalAvailable() {
        throw this.unimplemented('Not implemented on web.');
    }
    async readDataFromFile(_options) {
        throw this.unimplemented('Not implemented on web.');
    }
    async setResultFilename(_options) {
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

var web = /*#__PURE__*/Object.freeze({
    __proto__: null,
    MotionCalibrationWeb: MotionCalibrationWeb
});

exports.MotionCalibration = MotionCalibration;
//# sourceMappingURL=plugin.cjs.js.map
