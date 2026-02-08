#ifndef MotionCalibration_Bridging_Header_h
#define MotionCalibration_Bridging_Header_h

#include "motioncalibration.h"
#include "imuread.h"

// Additional declarations for Swift compatibility
extern int read_ipc_file_data(const char *filename);
extern void set_result_filename(const char *filename);
extern void raw_data_reset(void);
extern int send_calibration(void);
extern float quality_surface_gap_error(void);
extern float quality_magnitude_variance_error(void);
extern float quality_wobble_error(void);
extern float quality_spherical_fit_error(void);
extern void display_callback(void);
extern const uint8_t* get_calibration_data(void);
extern float* get_draw_points(void);
extern int get_draw_points_count(void);
extern void clear_draw_points(void);
extern void get_hard_iron_offset(float V[3]);
extern void get_soft_iron_matrix(float invW[3][3]);
extern float get_geomagnetic_field_magnitude(void);
extern short is_send_cal_available(void);

#endif /* MotionCalibration_Bridging_Header_h */
