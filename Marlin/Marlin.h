/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef MARLIN_H
#define MARLIN_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>

#include "MarlinConfig.h"

#ifdef DEBUG_GCODE_PARSER
  #include "gcode.h"
#endif

#include "enum.h"
#include "types.h"
#include "fastio.h"
#include "utility.h"
#include "serial.h"

#if ENABLED(PRINTCOUNTER)
  #include "printcounter.h"
#else
  #include "stopwatch.h"
#endif

void idle(bool no_stepper_sleep = false);		// pass true to keep steppers from disabling on timeout

void manage_inactivity(bool ignore_stepper_queue = false);

#if ENABLED(DUAL_X_CARRIAGE) || ENABLED(DUAL_NOZZLE_DUPLICATION_MODE)
  extern bool extruder_duplication_enabled;
#endif

#if HAS_X2_ENABLE
  #define  enable_X() do{ X_ENABLE_WRITE( X_ENABLE_ON); X2_ENABLE_WRITE( X_ENABLE_ON); }while(0)
  #define disable_X() do{ X_ENABLE_WRITE(!X_ENABLE_ON); X2_ENABLE_WRITE(!X_ENABLE_ON); axis_known_position[X_AXIS] = false; }while(0)
#elif HAS_X_ENABLE
//  #define  enable_X() X_ENABLE_WRITE( X_ENABLE_ON)
  #define  enable_X() do{ bool hasEnable = X_ENABLE_READ == X_ENABLE_ON; X_ENABLE_WRITE( X_ENABLE_ON ); if(!hasEnable) safe_delay(500); }while(0)
  #define disable_X() do{ X_ENABLE_WRITE(!X_ENABLE_ON); axis_known_position[X_AXIS] = false; }while(0)
#else
  #define  enable_X() NOOP
  #define disable_X() NOOP
#endif

#if HAS_Y2_ENABLE
  #define  enable_Y() do{ Y_ENABLE_WRITE( Y_ENABLE_ON); Y2_ENABLE_WRITE(Y_ENABLE_ON); }while(0)
  #define disable_Y() do{ Y_ENABLE_WRITE(!Y_ENABLE_ON); Y2_ENABLE_WRITE(!Y_ENABLE_ON); axis_known_position[Y_AXIS] = false; }while(0)
#elif HAS_Y_ENABLE
//  #define  enable_Y() Y_ENABLE_WRITE( Y_ENABLE_ON)
  #define  enable_Y() do{ bool hasEnable = Y_ENABLE_READ == Y_ENABLE_ON; Y_ENABLE_WRITE( Y_ENABLE_ON ); if(!hasEnable) safe_delay(500); }while(0)
  #define disable_Y() do{ Y_ENABLE_WRITE(!Y_ENABLE_ON); axis_known_position[Y_AXIS] = false; }while(0)
#else
  #define  enable_Y() NOOP
  #define disable_Y() NOOP
#endif

#if HAS_Z2_ENABLE
  #define  enable_Z() do{ Z_ENABLE_WRITE( Z_ENABLE_ON); Z2_ENABLE_WRITE(Z_ENABLE_ON); }while(0)
  #define disable_Z() do{ Z_ENABLE_WRITE(!Z_ENABLE_ON); Z2_ENABLE_WRITE(!Z_ENABLE_ON); axis_known_position[Z_AXIS] = false; }while(0)
#elif HAS_Z_ENABLE
  #define  enable_Z() Z_ENABLE_WRITE( Z_ENABLE_ON)
  #define disable_Z() do{ Z_ENABLE_WRITE(!Z_ENABLE_ON); axis_known_position[Z_AXIS] = false; }while(0)
#else
  #define  enable_Z() NOOP
  #define disable_Z() NOOP
#endif

#if ENABLED(MIXING_EXTRUDER)

  /**
   * Mixing steppers synchronize their enable (and direction) together
   */
  #if MIXING_STEPPERS > 3
    #define  enable_E0() { E0_ENABLE_WRITE( E_ENABLE_ON); E1_ENABLE_WRITE( E_ENABLE_ON); E2_ENABLE_WRITE( E_ENABLE_ON); E3_ENABLE_WRITE( E_ENABLE_ON); }
    #define disable_E0() { E0_ENABLE_WRITE(!E_ENABLE_ON); E1_ENABLE_WRITE(!E_ENABLE_ON); E2_ENABLE_WRITE(!E_ENABLE_ON); E3_ENABLE_WRITE(!E_ENABLE_ON); }
  #elif MIXING_STEPPERS > 2
    #define  enable_E0() { E0_ENABLE_WRITE( E_ENABLE_ON); E1_ENABLE_WRITE( E_ENABLE_ON); E2_ENABLE_WRITE( E_ENABLE_ON); }
    #define disable_E0() { E0_ENABLE_WRITE(!E_ENABLE_ON); E1_ENABLE_WRITE(!E_ENABLE_ON); E2_ENABLE_WRITE(!E_ENABLE_ON); }
  #else
    #define  enable_E0() { E0_ENABLE_WRITE( E_ENABLE_ON); E1_ENABLE_WRITE( E_ENABLE_ON); }
    #define disable_E0() { E0_ENABLE_WRITE(!E_ENABLE_ON); E1_ENABLE_WRITE(!E_ENABLE_ON); }
  #endif
  #define  enable_E1() NOOP
  #define disable_E1() NOOP
  #define  enable_E2() NOOP
  #define disable_E2() NOOP
  #define  enable_E3() NOOP
  #define disable_E3() NOOP
  #define  enable_E4() NOOP
  #define disable_E4() NOOP

#else // !MIXING_EXTRUDER

  #if HAS_E0_ENABLE
    #define  enable_E0() E0_ENABLE_WRITE( E_ENABLE_ON)
    #define disable_E0() E0_ENABLE_WRITE(!E_ENABLE_ON)
  #else
    #define  enable_E0() NOOP
    #define disable_E0() NOOP
  #endif

  #if E_STEPPERS > 1 && HAS_E1_ENABLE
    #define  enable_E1() E1_ENABLE_WRITE( E_ENABLE_ON)
    #define disable_E1() E1_ENABLE_WRITE(!E_ENABLE_ON)
  #else
    #define  enable_E1() NOOP
    #define disable_E1() NOOP
  #endif

  #if E_STEPPERS > 2 && HAS_E2_ENABLE
    #define  enable_E2() E2_ENABLE_WRITE( E_ENABLE_ON)
    #define disable_E2() E2_ENABLE_WRITE(!E_ENABLE_ON)
  #else
    #define  enable_E2() NOOP
    #define disable_E2() NOOP
  #endif

  #if E_STEPPERS > 3 && HAS_E3_ENABLE
    #define  enable_E3() E3_ENABLE_WRITE( E_ENABLE_ON)
    #define disable_E3() E3_ENABLE_WRITE(!E_ENABLE_ON)
  #else
    #define  enable_E3() NOOP
    #define disable_E3() NOOP
  #endif

  #if E_STEPPERS > 4 && HAS_E4_ENABLE
    #define  enable_E4() E4_ENABLE_WRITE( E_ENABLE_ON)
    #define disable_E4() E4_ENABLE_WRITE(!E_ENABLE_ON)
  #else
    #define  enable_E4() NOOP
    #define disable_E4() NOOP
  #endif

#endif // !MIXING_EXTRUDER

#if ENABLED(G38_PROBE_TARGET)
  extern bool G38_move,        // flag to tell the interrupt handler that a G38 command is being run
              G38_endstop_hit; // flag from the interrupt handler to indicate if the endstop went active
#endif

/**
 * The axis order in all axis related arrays is X, Y, Z, E
 */
#define _AXIS(AXIS) AXIS ##_AXIS

void enable_all_steppers();
void disable_e_steppers();
void disable_all_steppers();

void FlushSerialRequestResend();
void ok_to_send();

void kill(const char*);

void quickstop_stepper();

#if ENABLED(FILAMENT_RUNOUT_SENSOR)
  void handle_filament_runout();
#endif

extern uint8_t marlin_debug_flags;
#define DEBUGGING(F) (marlin_debug_flags & (DEBUG_## F))

extern bool Running;
inline bool IsRunning() { return  Running; }
inline bool IsStopped() { return !Running; }

bool enqueue_and_echo_command(const char* cmd, bool say_ok=false); // Add a single command to the end of the buffer. Return false on failure.
void enqueue_and_echo_commands_P(const char * const cmd);          // Set one or more commands to be prioritized over the next Serial/SD command.
void enqueue_and_echo_commands(const char * const cmd);
void clear_command_queue();

extern millis_t previous_cmd_ms;
inline void refresh_cmd_timeout() { previous_cmd_ms = millis(); }

#if ENABLED(FAST_PWM_FAN)
  void setPwmFrequency(uint8_t pin, int val);
#endif

/**
 * Feedrate scaling and conversion
 */
extern int16_t feedrate_percentage;

#define MMM_TO_MMS(MM_M) ((MM_M)/60.0)
#define MMS_TO_MMM(MM_S) ((MM_S)*60.0)
#define MMS_SCALED(MM_S) ((MM_S)*feedrate_percentage*0.01)

extern bool axis_relative_modes[];
extern bool volumetric_enabled;
extern int16_t flow_percentage[EXTRUDERS]; // Extrusion factor for each extruder
extern float filament_size[EXTRUDERS]; // cross-sectional area of filament (in millimeters), typically around 1.75 or 2.85, 0 disables the volumetric calculations for the extruder.
extern float volumetric_multiplier[EXTRUDERS]; // reciprocal of cross-sectional area of filament (in square millimeters), stored this way to reduce computational burden in planner
extern bool axis_known_position[XYZ];
extern bool axis_homed[XYZ];
extern volatile bool wait_for_heatup;

#if HAS_RESUME_CONTINUE
  extern volatile bool wait_for_user;
#endif

extern float current_position[NUM_AXIS];

// Workspace offsets
#if HAS_WORKSPACE_OFFSET
  #if HAS_HOME_OFFSET
    extern float home_offset[XYZ];
  #endif
  #if HAS_POSITION_SHIFT
    extern float position_shift[XYZ];
  #endif
#endif

#if HAS_HOME_OFFSET && HAS_POSITION_SHIFT
  extern float workspace_offset[XYZ];
  #define WORKSPACE_OFFSET(AXIS) workspace_offset[AXIS]
#elif HAS_HOME_OFFSET
  #define WORKSPACE_OFFSET(AXIS) home_offset[AXIS]
#elif HAS_POSITION_SHIFT
  #define WORKSPACE_OFFSET(AXIS) position_shift[AXIS]
#else
  #define WORKSPACE_OFFSET(AXIS) 0
#endif

#define LOGICAL_POSITION(POS, AXIS) ((POS) + WORKSPACE_OFFSET(AXIS))
#define RAW_POSITION(POS, AXIS)     ((POS) - WORKSPACE_OFFSET(AXIS))

#if HAS_POSITION_SHIFT || DISABLED(DELTA)
  #define LOGICAL_X_POSITION(POS)   LOGICAL_POSITION(POS, X_AXIS)
  #define LOGICAL_Y_POSITION(POS)   LOGICAL_POSITION(POS, Y_AXIS)
  #define RAW_X_POSITION(POS)       RAW_POSITION(POS, X_AXIS)
  #define RAW_Y_POSITION(POS)       RAW_POSITION(POS, Y_AXIS)
#else
  #define LOGICAL_X_POSITION(POS)   (POS)
  #define LOGICAL_Y_POSITION(POS)   (POS)
  #define RAW_X_POSITION(POS)       (POS)
  #define RAW_Y_POSITION(POS)       (POS)
#endif

#define LOGICAL_Z_POSITION(POS)     LOGICAL_POSITION(POS, Z_AXIS)
#define RAW_Z_POSITION(POS)         RAW_POSITION(POS, Z_AXIS)
#define RAW_CURRENT_POSITION(A)     RAW_##A##_POSITION(current_position[A##_AXIS])

// Hotend Offsets
#if HOTENDS > 1
  extern float hotend_offset[XYZ][HOTENDS];
#endif

// Software Endstops
extern float soft_endstop_min[XYZ], soft_endstop_max[XYZ];

#if HAS_SOFTWARE_ENDSTOPS
  extern bool soft_endstops_enabled;
  void clamp_to_software_endstops(float target[XYZ]);
#else
  #define soft_endstops_enabled false
  #define clamp_to_software_endstops(x) NOOP
#endif

#if HAS_WORKSPACE_OFFSET || ENABLED(DUAL_X_CARRIAGE)
  void update_software_endstops(const AxisEnum axis);
#endif

#if IS_KINEMATIC
  extern float delta[ABC];
  void inverse_kinematics(const float logical[XYZ]);
#endif

#if ENABLED(DELTA)
  extern float endstop_adj[ABC],
               delta_radius,
               delta_diagonal_rod,
               delta_calibration_radius,
               delta_segments_per_second,
               delta_tower_angle_trim[ABC],
               delta_clip_start_height;
  void recalc_delta_settings(float radius, float diagonal_rod, float tower_angle_trim[ABC]);
#elif IS_SCARA
  void forward_kinematics_SCARA(const float &a, const float &b);
#endif

#if ENABLED(AUTO_BED_LEVELING_BILINEAR)
  extern int bilinear_grid_spacing[2], bilinear_start[2];
  extern float bilinear_grid_factor[2],
               z_values[GRID_MAX_POINTS_X][GRID_MAX_POINTS_Y];
  float bilinear_z_offset(const float logical[XYZ]);
#endif

#if ENABLED(AUTO_BED_LEVELING_UBL)
  typedef struct { double A, B, D; } linear_fit;
  linear_fit* lsf_linear_fit(double x[], double y[], double z[], const int);
#endif

#if HAS_LEVELING
  bool leveling_is_valid();
  bool leveling_is_active();
  void set_bed_leveling_enabled(const bool enable=true);
  void reset_bed_level();
#endif

#if ENABLED(ENABLE_LEVELING_FADE_HEIGHT)
  void set_z_fade_height(const float zfh);
#endif

#if ENABLED(Z_DUAL_ENDSTOPS)
  extern float z_endstop_adj;
#endif

#if HAS_BED_PROBE
  extern float zprobe_zoffset;
  void refresh_zprobe_zoffset(const bool no_babystep=false);
  #define DEPLOY_PROBE() set_probe_deployed(true)
  #define STOW_PROBE() set_probe_deployed(false)
#else
  #define DEPLOY_PROBE()
  #define STOW_PROBE()
#endif

#if ENABLED(HOST_KEEPALIVE_FEATURE)
  extern MarlinBusyState busy_state;
  #define KEEPALIVE_STATE(n) do{ busy_state = n; }while(0)
#else
  #define KEEPALIVE_STATE(n) NOOP
#endif

#if FAN_COUNT > 0
  extern int16_t fanSpeeds[FAN_COUNT];
  #if ENABLED(PROBING_FANS_OFF)
    extern bool fans_paused;
    extern int16_t paused_fanSpeeds[FAN_COUNT];
  #endif
#endif

#if ENABLED(BARICUDA)
  extern uint8_t baricuda_valve_pressure, baricuda_e_to_p_pressure;
#endif

#if ENABLED(FILAMENT_WIDTH_SENSOR)
  extern bool filament_sensor;         // Flag that filament sensor readings should control extrusion
  extern float filament_width_nominal, // Theoretical filament diameter i.e., 3.00 or 1.75
               filament_width_meas;    // Measured filament diameter
  extern uint8_t meas_delay_cm,        // Delay distance
                 measurement_delay[];  // Ring buffer to delay measurement
  extern int8_t filwidth_delay_index[2]; // Ring buffer indexes. Used by planner, temperature, and main code
#endif

#if ENABLED(ADVANCED_PAUSE_FEATURE)
  extern AdvancedPauseMenuResponse advanced_pause_menu_response;
#endif

#if ENABLED(PID_EXTRUSION_SCALING)
  extern int lpq_len;
#endif

#if ENABLED(FWRETRACT)
  extern bool autoretract_enabled;                 // M209 S - Autoretract switch
  extern float retract_length,                     // M207 S - G10 Retract length
               retract_feedrate_mm_s,              // M207 F - G10 Retract feedrate
               retract_zlift,                      // M207 Z - G10 Retract hop size
               retract_recover_length,             // M208 S - G11 Recover length
               retract_recover_feedrate_mm_s,      // M208 F - G11 Recover feedrate
               swap_retract_length,                // M207 W - G10 Swap Retract length
               swap_retract_recover_length,        // M208 W - G11 Swap Recover length
               swap_retract_recover_feedrate_mm_s; // M208 R - G11 Swap Recover feedrate
#endif

// Print job timer
#if ENABLED(PRINTCOUNTER)
  extern PrintCounter print_job_timer;
#else
  extern Stopwatch print_job_timer;
#endif

// Handling multiple extruders pins
extern uint8_t active_extruder;

#if HAS_TEMP_HOTEND || HAS_TEMP_BED
  void print_heaterstates();
#endif

#if ENABLED(MIXING_EXTRUDER)
  extern float mixing_factor[MIXING_STEPPERS];
#endif

void calculate_volumetric_multipliers();

/**
 * Blocking movement and shorthand functions
 */
void do_blocking_move_to(const float &x, const float &y, const float &z, const float &fr_mm_s=0.0);
void do_blocking_move_to_x(const float &x, const float &fr_mm_s=0.0);
void do_blocking_move_to_z(const float &z, const float &fr_mm_s=0.0);
void do_blocking_move_to_xy(const float &x, const float &y, const float &fr_mm_s=0.0);

#define HAS_AXIS_UNHOMED_ERR (                                                     \
         ENABLED(Z_PROBE_ALLEN_KEY)                                                \
      || ENABLED(Z_PROBE_SLED)                                                     \
      || HAS_PROBING_PROCEDURE                                                     \
      || HOTENDS > 1                                                               \
      || ENABLED(NOZZLE_CLEAN_FEATURE)                                             \
      || ENABLED(NOZZLE_PARK_FEATURE)                                              \
      || (ENABLED(ADVANCED_PAUSE_FEATURE) && ENABLED(HOME_BEFORE_FILAMENT_CHANGE)) \
    ) || ENABLED(NO_MOTION_BEFORE_HOMING)

#if HAS_AXIS_UNHOMED_ERR
  bool axis_unhomed_error(const bool x=true, const bool y=true, const bool z=true);
#endif

/**
 * position_is_reachable family of functions
 */

#if IS_KINEMATIC // (DELTA or SCARA)

  #if IS_SCARA
    extern const float L1, L2;
  #endif

  inline bool position_is_reachable_raw_xy(const float &rx, const float &ry) {
    #if ENABLED(DELTA)
      return HYPOT2(rx, ry) <= sq(DELTA_PRINTABLE_RADIUS);
    #elif IS_SCARA
      #if MIDDLE_DEAD_ZONE_R > 0
        const float R2 = HYPOT2(rx - SCARA_OFFSET_X, ry - SCARA_OFFSET_Y);
        return R2 >= sq(float(MIDDLE_DEAD_ZONE_R)) && R2 <= sq(L1 + L2);
      #else
        return HYPOT2(rx - SCARA_OFFSET_X, ry - SCARA_OFFSET_Y) <= sq(L1 + L2);
      #endif
    #else // CARTESIAN
      // To be migrated from MakerArm branch in future
    #endif
  }

  inline bool position_is_reachable_by_probe_raw_xy(const float &rx, const float &ry) {

    // Both the nozzle and the probe must be able to reach the point.
    // This won't work on SCARA since the probe offset rotates with the arm.

    return position_is_reachable_raw_xy(rx, ry)
        && position_is_reachable_raw_xy(rx - X_PROBE_OFFSET_FROM_EXTRUDER, ry - Y_PROBE_OFFSET_FROM_EXTRUDER);
  }

#else // CARTESIAN

  inline bool position_is_reachable_raw_xy(const float &rx, const float &ry) {
      // Add 0.001 margin to deal with float imprecision
      return WITHIN(rx, X_MIN_POS - 0.001, X_MAX_POS + 0.001)
          && WITHIN(ry, Y_MIN_POS - 0.001, Y_MAX_POS + 0.001);
  }

  inline bool position_is_reachable_by_probe_raw_xy(const float &rx, const float &ry) {
      // Add 0.001 margin to deal with float imprecision
      return WITHIN(rx, MIN_PROBE_X - 0.001, MAX_PROBE_X + 0.001)
          && WITHIN(ry, MIN_PROBE_Y - 0.001, MAX_PROBE_Y + 0.001);
  }

#endif // CARTESIAN

FORCE_INLINE bool position_is_reachable_by_probe_xy(const float &lx, const float &ly) {
  return position_is_reachable_by_probe_raw_xy(RAW_X_POSITION(lx), RAW_Y_POSITION(ly));
}

FORCE_INLINE bool position_is_reachable_xy(const float &lx, const float &ly) {
  return position_is_reachable_raw_xy(RAW_X_POSITION(lx), RAW_Y_POSITION(ly));
}

/*************** By LYN *******************/
#if ENABLED(ULTIPANEL)
	#define USER_OPERATE		(LCD_OPERATE)		// operate encoder
#elif ENABLED(DWIN_LCD)
	#define USER_OPERATE		(DWIN_TOUCH || dwin_getVar()->valid)		// touch a screen
													//(DWIN_TOUCH && !DWIN_IS_PAGE(PAGE_SHUTDOWN_HOTEMP))
#else
	#define USER_OPERATE		false
#endif

#if ENABLED(SDSUPPORT)
	#include "cardreader.h"
	#define HAS_READER							true
	#define FILE_READER							card
	#define TOUCH_FILE(index)				FILE_READER.getfilename(index)
	#define TOUCH_WORKDIR						FILE_READER.getWorkDirName()
	#define GET_FILE_NR							FILE_READER.getnrfilenames()
	#define CUR_FILE_IS_DIR					FILE_READER.filenameIsDir
	#define FILE_IS_OPEN						FILE_READER.isFileOpen()
	#define FILE_IS_IDLE						(!HAS_SD_PRINT)
	#define FILE_IS_PRINT						IS_SD_PRINTING
	#define FILE_IS_PAUSE						FILE_READER.isPauseState
	#define FILE_START_PRINT				FILE_READER.startFileprint()
	#define FILE_PAUSE_PRINT				FILE_READER.pauseSDPrint()
	#define FILE_STOP_PRINT					FILE_READER.stopSDPrint()
	#define READER_STATE						IS_SD_INSERTED
	#define READER_CONN							(IS_SD_INSERTED == 1)
	#define READER_VALID						FILE_READER.cardOK
#elif ENABLED(UDISKSUPPORT)
	#include "UDiskReader.h"
	#define HAS_READER							true
	#define FILE_READER							UDisk
	#define TOUCH_FILE(index)				FILE_READER.selectFile(index)
	#define TOUCH_WORKDIR						FILE_READER.selectWorkDir()
	#define GET_FILE_NR							FILE_READER.getNr()
	#define CUR_FILE_IS_DIR					IS_UDISK_DIR
	#define FILE_IS_OPEN						IS_UDISK_FILE_OPEN
	#define FILE_IS_IDLE						IS_UDISK_IDLE
	#define FILE_IS_PRINT						IS_UDISK_PRINT
	#define FILE_IS_PAUSE						IS_UDISK_PAUSE
	#define FILE_START_PRINT				FILE_READER.startPrint()
	#define FILE_PAUSE_PRINT				FILE_READER.pausePrint()
	#define FILE_STOP_PRINT					FILE_READER.stopPrint()
	#define READER_STATE						UDISK_STATE
	#define READER_CONN							IS_UDISK_CONN
	#define READER_VALID						IS_UDISK_OK
#else
	#define HAS_READER							false
#endif

  void customDetect();
  void controlTime();
#ifdef COLOR_LED
  void controlLED();
#endif
#ifdef DWIN_LCD
  void dwin_run();
#endif
#ifdef POWER_MANAGEMENT
  void detectPower();
#endif
#ifdef ACCIDENT_DETECT
  void detectAccident();
#endif
#ifdef FILAMENT_DETECT
  void checkFilament();
  void detectFilament();
#endif
#ifdef CHAMBER_FAN
  void detectAirFan();
#endif

#if HAS_AUTO_FAN
	extern int extruder_auto_fan_speed;
#endif

#ifdef CHAMBER_FAN
	extern int air_fan_speed;
#endif

extern uint32_t	usedTime;

#ifdef QUICK_PAUSE
	extern bool invalidLoop;
	extern float pausePos[XYZE];
	extern float pauseSpeed;
	extern uint32_t pauseByte;
	extern float lastPos[XYZE];
	#ifdef HAS_LEVELING
		extern bool pauseLeveling;
	#endif
#endif

#ifdef WIFI_SUPPORT
	extern char hostName[24];
#endif

#define TOOLS_NUM						(MAX_EXTRUDERS + 3)
#ifdef ACCIDENT_DETECT
	#define TOOLS_INDEX_BED		MAX_EXTRUDERS
	#define TOOLS_INDEX_FAN		(MAX_EXTRUDERS + 1)
	#define TOOLS_INDEX_HOT		(MAX_EXTRUDERS + 2)
	extern bool moduleIsReady;
	extern bool isAccidentToPrinting;
	extern bool isAccident;
	extern char lastFilename[MAXPATHNAMELENGTH];
	extern int lastToolsState[TOOLS_NUM];
#endif

extern bool isUnloadingFilament;
extern bool isFilamentUnloaded;

#if HAS_LEVELING
  extern bool probeDone;
#endif

extern PowerState powerState;

extern bool	isSerialPrinting;

#define FW_STR_LEN	11  // Vx.x.x/x.xc
extern char versionFW[FW_STR_LEN];

void updateStateStrings();

#if HAS_BUZZER
	#define MY_BEEPER
	void MyBeeper(uint8_t num);
	void finishTaskBeeper();
#endif

#ifdef QUICK_PAUSE
	uint32_t getGcodePos();
	void saveLastState();
	bool quickPausePrintJob();
	bool quickReusePrintJob();
	void quickStopPrintJob();
#endif

#if defined(FILAMENT_CHANGE) || defined(FILAMENT_DETECT)
	bool pauseToUnloadFilament();
#endif

#ifdef ACCIDENT_DETECT
	void preheaToolsState();
	void openFileFromAccident();
	void resumePrintFromAccident();
	void accidentToResume();
	void accidentToResume_Home();
	void accidentToCancel();
#endif


#ifdef DEBUG_FREE
extern "C" {
	int freeMemory();
}
#endif

/******************************************/


#endif // MARLIN_H