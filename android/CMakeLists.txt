cmake_minimum_required(VERSION 3.22.1)
project("motioncalibration" C)

# Suppress warnings from upstream C code (Freescale/NXP algorithm, not authored here).
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -w")

add_library(
    motioncalibration
    SHARED

    # JNI bridge (Java ↔ C)
    src/main/jni/jni_bridge.c

    # Algorithm sources (shared with iOS via common/)
    ../common/rawdata.c
    ../common/serialdata.c
    ../common/visualize.c
    ../common/magcal.c
    ../common/mahony.c
    ../common/matrix.c
    ../common/quality.c
)

# Make imuread.h and other common/ headers visible to all C files.
target_include_directories(motioncalibration PRIVATE ../common)

# Android system libraries
find_library(log-lib log)
target_link_libraries(motioncalibration ${log-lib} m)
