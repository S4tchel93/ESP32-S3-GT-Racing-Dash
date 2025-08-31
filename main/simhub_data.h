#ifndef SIMHUB_DATA_H
#define SIMHUB_DATA_H

/***************************************************************************************************************************
 * INCLUDE SECTION
 ***************************************************************************************************************************/

/***************************************************************************************************************************
 * PUBLIC DEFINITIONS
 ***************************************************************************************************************************/

/***************************************************************************************************************************
 * PUBLIC DATA & TYPES
 ***************************************************************************************************************************/

/**
 * @brief Simhub Data structure. This structure is shared between UI update task
 * and simhub task. It's used to store data coming from SimHub and then using it
 * to process it to update elements in the UI.
 */
typedef struct simhub_data_t
{
    char curr_gear[2];
    char curr_speed[4];
    char rpm_percent[4];
    char rpm_redline_threshold[4] ;
    char current_time[9];
    char last_time[9];
    char best_time[9];
    char delta_time[7];
    char fl_wear[6];
    char fr_wear[6];
    char rl_wear[6];
    char rr_wear[6];
    char fl_tire_temp[6];
    char fr_tire_temp[6];
    char rl_tire_temp[6];
    char rr_tire_temp[6];
    char tc_level[6];
    char tc_active[6];
    char abs_level[6];
    char abs_active[6];
    char bb_level[7];
    char engine_map[6];
    char lap_invalid[9];
    char fuel[6];
    char fl_tyre_pressure[6];
    char fr_tyre_pressure[6];
    char rl_tyre_pressure[6];
    char rr_tyre_pressure[6];
    char fl_brake_temp[6];
    char fr_brake_temp[6];
    char rl_brake_temp[6];
    char rr_brake_temp[6];
    char engine_ignition[2];
    char headlights[2];
    char wipers[6];
    char session_name[15];
    char session_time_left[10];
    char position[6];
    char opponent_count[6];
    char throttle[6];
    char brake[6];
    char curr_lap[6];
}simhub_data_t;

/***************************************************************************************************************************
 * PUBLIC FUNCTION DECLARATIONS
 ***************************************************************************************************************************/

#endif // SIMHUB_DATA_H