/**
 * jni_bridge.c
 *
 * JNI wrappers exposing the MotionCal C algorithm to the Java plugin.
 * Each function maps 1-to-1 to a `private native` declaration in MotionCalibration.java.
 */

#include <jni.h>
#include <stdint.h>
#include <string.h>
#include "imuread.h"

/* Expand JNI symbol names without repeating the full package path each time. */
#define JNIFUNC(name) \
    Java_com_denizak_motioncalibration_MotionCalibration_##name

/* Size of the calibration output packet (see send_calibration() in rawdata.c). */
#define CAL_DATA_SIZE 68


/* ── isSendCalAvailableNative ─────────────────────────────────────────────── */

JNIEXPORT jshort JNICALL
JNIFUNC(isSendCalAvailableNative)(JNIEnv *env, jobject thiz)
{
    return (jshort) is_send_cal_available();
}


/* ── rawDataNative ─────────────────────────────────────────────────────────
 *  Receives the 9-element int16 array from Java and feeds it to the algorithm.
 *  Array layout: [ax, ay, az, gx, gy, gz, mx, my, mz]
 */

JNIEXPORT void JNICALL
JNIFUNC(rawDataNative)(JNIEnv *env, jobject thiz, jshortArray data)
{
    jshort *elems = (*env)->GetShortArrayElements(env, data, NULL);
    raw_data((const int16_t *) elems);
    (*env)->ReleaseShortArrayElements(env, data, elems, JNI_ABORT);
}


/* ── sendCalibrationNative ─────────────────────────────────────────────────── */

JNIEXPORT jint JNICALL
JNIFUNC(sendCalibrationNative)(JNIEnv *env, jobject thiz)
{
    return (jint) send_calibration();
}


/* ── Quality metrics ──────────────────────────────────────────────────────── */

JNIEXPORT jfloat JNICALL
JNIFUNC(getQualitySurfaceGapErrorNative)(JNIEnv *env, jobject thiz)
{
    return (jfloat) quality_surface_gap_error();
}

JNIEXPORT jfloat JNICALL
JNIFUNC(getQualityMagnitudeVarianceErrorNative)(JNIEnv *env, jobject thiz)
{
    return (jfloat) quality_magnitude_variance_error();
}

JNIEXPORT jfloat JNICALL
JNIFUNC(getQualityWobbleErrorNative)(JNIEnv *env, jobject thiz)
{
    return (jfloat) quality_wobble_error();
}

JNIEXPORT jfloat JNICALL
JNIFUNC(getQualitySphericalFitErrorNative)(JNIEnv *env, jobject thiz)
{
    return (jfloat) quality_spherical_fit_error();
}


/* ── displayCallbackNative ─────────────────────────────────────────────────── */

JNIEXPORT void JNICALL
JNIFUNC(displayCallbackNative)(JNIEnv *env, jobject thiz)
{
    display_callback();
}


/* ── getCalibrationDataNative ──────────────────────────────────────────────── */

JNIEXPORT jbyteArray JNICALL
JNIFUNC(getCalibrationDataNative)(JNIEnv *env, jobject thiz)
{
    const uint8_t *data = get_calibration_data();
    if (data == NULL) return NULL;

    jbyteArray result = (*env)->NewByteArray(env, CAL_DATA_SIZE);
    if (result == NULL) return NULL;

    (*env)->SetByteArrayRegion(env, result, 0, CAL_DATA_SIZE, (const jbyte *) data);
    return result;
}


/* ── convertDrawPoints ─────────────────────────────────────────────────────
 *  Returns float[][] where each inner array is [x, y, z] for one sphere point.
 */

JNIEXPORT jobjectArray JNICALL
JNIFUNC(convertDrawPoints)(JNIEnv *env, jobject thiz)
{
    int    count  = get_draw_points_count();
    float *points = get_draw_points();

    jclass       floatArrayCls = (*env)->FindClass(env, "[F");
    jobjectArray result        = (*env)->NewObjectArray(env, count, floatArrayCls, NULL);

    for (int i = 0; i < count; i++) {
        jfloatArray pt = (*env)->NewFloatArray(env, 3);
        jfloat coords[3] = {
            (jfloat) points[i * 3],
            (jfloat) points[i * 3 + 1],
            (jfloat) points[i * 3 + 2]
        };
        (*env)->SetFloatArrayRegion(env, pt, 0, 3, coords);
        (*env)->SetObjectArrayElement(env, result, i, pt);
        (*env)->DeleteLocalRef(env, pt);
    }
    return result;
}


/* ── resetRawDataNative ────────────────────────────────────────────────────── */

JNIEXPORT void JNICALL
JNIFUNC(resetRawDataNative)(JNIEnv *env, jobject thiz)
{
    raw_data_reset();
}


/* ── getHardIronOffsetNative ───────────────────────────────────────────────── */

JNIEXPORT jfloatArray JNICALL
JNIFUNC(getHardIronOffsetNative)(JNIEnv *env, jobject thiz)
{
    float V[3];
    get_hard_iron_offset(V);

    jfloatArray result = (*env)->NewFloatArray(env, 3);
    (*env)->SetFloatArrayRegion(env, result, 0, 3, (const jfloat *) V);
    return result;
}


/* ── getSoftIronMatrixNative ───────────────────────────────────────────────── */

JNIEXPORT jobjectArray JNICALL
JNIFUNC(getSoftIronMatrixNative)(JNIEnv *env, jobject thiz)
{
    float invW[3][3];
    get_soft_iron_matrix(invW);

    jclass       floatArrayCls = (*env)->FindClass(env, "[F");
    jobjectArray result        = (*env)->NewObjectArray(env, 3, floatArrayCls, NULL);

    for (int i = 0; i < 3; i++) {
        jfloatArray row = (*env)->NewFloatArray(env, 3);
        jfloat vals[3] = {
            (jfloat) invW[i][0],
            (jfloat) invW[i][1],
            (jfloat) invW[i][2]
        };
        (*env)->SetFloatArrayRegion(env, row, 0, 3, vals);
        (*env)->SetObjectArrayElement(env, result, i, row);
        (*env)->DeleteLocalRef(env, row);
    }
    return result;
}


/* ── getGeomagneticFieldMagnitudeNative ────────────────────────────────────── */

JNIEXPORT jfloat JNICALL
JNIFUNC(getGeomagneticFieldMagnitudeNative)(JNIEnv *env, jobject thiz)
{
    return (jfloat) get_geomagnetic_field_magnitude();
}


/* ── clearDrawPointsNative ─────────────────────────────────────────────────── */

JNIEXPORT void JNICALL
JNIFUNC(clearDrawPointsNative)(JNIEnv *env, jobject thiz)
{
    clear_draw_points();
}
