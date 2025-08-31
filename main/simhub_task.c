/***************************************************************************************************************************
 * INCLUDE SECTION
 ***************************************************************************************************************************/
/*project includes*/
#include "simhub_task.h"
#include "simhub_data.h"

/*System/ESP IDF Includes*/
#include <stdio.h>
#include <unistd.h>
#include "freertos/task.h"
#include "driver/usb_serial_jtag.h"
#include "string.h"

/***************************************************************************************************************************
 * PRIVATE DEFINITIONS
 ***************************************************************************************************************************/

 /**
  * @brief String used as Header or Sync for a full SimHub data packet
  */
#define SIMHUB_SYNC "SH;"

/***************************************************************************************************************************
 * PRIVATE DATA & TYPES
 ***************************************************************************************************************************/

/**
 * @brief structure that aids iterating through all the
 * simhub data elements when parsing the simhub data packet
 */
typedef struct {
    char *dest;      // pointer into struct field
    size_t max_size; // including null terminator
} simhub_field_t;

/**
 * @brief Holds pointers to simhub data elements
 * and their max sizes
 */
static simhub_field_t simhub_fields[] = {
    { NULL, 2 },   // curr_gear
    { NULL, 4 },   // curr_speed
    { NULL, 4 },   // rpm_percent
    { NULL, 4 },   // rpm_redline_threshold
    { NULL, 9 },   // current_time
    { NULL, 9 },   // last_time
    { NULL, 9 },   // best_time
    { NULL, 7 },   // delta_time
    { NULL, 6 },   // fl_wear
    { NULL, 6 },   // fr_wear
    { NULL, 6 },   // rl_wear
    { NULL, 6 },   // rr_wear
    { NULL, 6 },   // fl_tire_temp
    { NULL, 6 },   // fr_tire_temp
    { NULL, 6 },   // rl_tire_temp
    { NULL, 6 },   // rr_tire_temp
    { NULL, 6 },   //tc_level
    { NULL, 6 },   //tc_active
    { NULL, 6 },   //abs_level
    { NULL, 6 },   //abs_active
    { NULL, 7 },   //bb_level
    { NULL, 6 },   //engine_map
    { NULL, 9 },   //lap_invalid
    { NULL, 6 },   //fuel
    { NULL, 6 },   //fl_tyre_pressure
    { NULL, 6 },   //fr_tyre_pressure
    { NULL, 6 },   //rl_tyre_pressure
    { NULL, 6 },   //rr_tyre_pressure
    { NULL, 6 },   //fl_brake_temp
    { NULL, 6 },   //fr_brake_temp
    { NULL, 6 },   //rl_brake_temp
    { NULL, 6 },   //rr_brake_temp
    { NULL, 2 },   //engine_ignition
    { NULL, 2 },   //headlights
    { NULL, 6 },   //wipers
    { NULL, 15},   //session_name
    { NULL, 10},   //session_time_left
    { NULL, 6 },   //position
    { NULL, 6 },   //opponent_count
    { NULL, 6 },   //throttle
    { NULL, 6 },   //brake
    { NULL, 6 },   //curr_lap
};

static QueueHandle_t xQueue;

/***************************************************************************************************************************
 * PRIVATE FUNCTION DECLARATIONS
 ***************************************************************************************************************************/

static bool wait_for_sync(void);
static void bind_simhub_fields(simhub_data_t *data);
static bool parse_simhub_packet(simhub_data_t *out_data);

 /***************************************************************************************************************************
 * PRIVATE FUNCTION DEFINITIONS
 ***************************************************************************************************************************/

/**
 * @brief Reads uart/jtag until SIMHUB_SYNC header is found
 * 
 * @return true if SIMHUB_SYNC header is found
 * @return false after timeout
 */
static bool wait_for_sync(void) {
    const char *sync = SIMHUB_SYNC;
    int matched = 0;

    while (1) {
        char c;
        int len = usb_serial_jtag_read_bytes(&c, 1, 50 / portTICK_PERIOD_MS);
        if (len <= 0) {
            return false; // timeout
        }

        if (c == sync[matched]) {
            matched++;
            if (matched == 3) {
                return true; // full "SH;" matched
            }
        } else {
            matched = (c == sync[0]) ? 1 : 0; // restart match if 'S' seen
        }
    }
}

/**
 * @brief Assigns the simhub data elements to simhub_fields struct to be able to iterate
 * through the elements within parse_simhub_packet()
 * 
 * @param data simhub data pointer
 */
static void bind_simhub_fields(simhub_data_t *data) {
    int i = 0;
    simhub_fields[i++].dest = data->curr_gear;
    simhub_fields[i++].dest = data->curr_speed;
    simhub_fields[i++].dest = data->rpm_percent;
    simhub_fields[i++].dest = data->rpm_redline_threshold;
    simhub_fields[i++].dest = data->current_time;
    simhub_fields[i++].dest = data->last_time;
    simhub_fields[i++].dest = data->best_time;
    simhub_fields[i++].dest = data->delta_time;
    simhub_fields[i++].dest = data->fl_wear;
    simhub_fields[i++].dest = data->fr_wear;
    simhub_fields[i++].dest = data->rl_wear;
    simhub_fields[i++].dest = data->rr_wear;
    simhub_fields[i++].dest = data->fl_tire_temp;
    simhub_fields[i++].dest = data->fr_tire_temp;
    simhub_fields[i++].dest = data->rl_tire_temp;
    simhub_fields[i++].dest = data->rr_tire_temp;
    simhub_fields[i++].dest = data->tc_level;
    simhub_fields[i++].dest = data->tc_active;
    simhub_fields[i++].dest = data->abs_level;
    simhub_fields[i++].dest = data->abs_active;
    simhub_fields[i++].dest = data->bb_level;
    simhub_fields[i++].dest = data->engine_map;
    simhub_fields[i++].dest = data->lap_invalid;
    simhub_fields[i++].dest = data->fuel;
    simhub_fields[i++].dest = data->fl_tyre_pressure;
    simhub_fields[i++].dest = data->fr_tyre_pressure;
    simhub_fields[i++].dest = data->rl_tyre_pressure;
    simhub_fields[i++].dest = data->rr_tyre_pressure;
    simhub_fields[i++].dest = data->fl_brake_temp;
    simhub_fields[i++].dest = data->fr_brake_temp;
    simhub_fields[i++].dest = data->rl_brake_temp;
    simhub_fields[i++].dest = data->rr_brake_temp;
    simhub_fields[i++].dest = data->engine_ignition;
    simhub_fields[i++].dest = data->headlights;
    simhub_fields[i++].dest = data->wipers;
    simhub_fields[i++].dest = data->session_name;
    simhub_fields[i++].dest = data->session_time_left;
    simhub_fields[i++].dest = data->position;
    simhub_fields[i++].dest = data->opponent_count;
    simhub_fields[i++].dest = data->throttle;
    simhub_fields[i++].dest = data->brake;
    simhub_fields[i++].dest = data->curr_lap;
}

/**
 * @brief Function that parses the incoming simhub data and places it as strings
 * on their corresponding simhub_data_t structure elements.
 * 
 * @param simhub_data simhub data structure used to store strings of the incoming data
 * @return true when all fields have been received (full expected packet)
 * @return false when reading uart/jtag returns 0, times out after 20 ms
 */
static bool parse_simhub_packet(simhub_data_t *simhub_data) {
    bind_simhub_fields(simhub_data);

    char field_buf[64];
    size_t field_len = 0;
    int field_idx = 0;
    int total_fields = sizeof(simhub_fields) / sizeof(simhub_fields[0]);

    while (field_idx < total_fields) {
        char c;
        int len = usb_serial_jtag_read_bytes(&c, 1, 20 / portTICK_PERIOD_MS);
        if (len <= 0) return false;

        if (c == ';') {
            field_buf[field_len] = '\0';
            strncpy(simhub_fields[field_idx].dest, field_buf,
                    simhub_fields[field_idx].max_size - 1);
            simhub_fields[field_idx].dest[simhub_fields[field_idx].max_size - 1] = '\0';

            field_idx++;
            field_len = 0;
        } else {
            if (field_len < sizeof(field_buf) - 1) {
                field_buf[field_len++] = c;
            }
        }
    }

    return true;
}

 /***************************************************************************************************************************
 * PUBLIC FUNCTION DEFINITIONS
 ***************************************************************************************************************************/

 /**
  * @brief Task that waits for SimHub Sync/Header and then reads
  * a full simhub data packet.
  * 
  * @details After receiving a full simhub data packet, it sends it to
  * ui_update_task through the xQueue.
  * 
  * @param arg parameters passed from main, none for now
  */
void simhub_task(void *arg) {
    /*initialize queue to send data to ui update task*/
    xQueue = xQueueCreate(5U, sizeof(simhub_data_t));

    while (1) {
        simhub_data_t simhub_data = {0};

        // Step 1: Wait until sync marker appears
        if (!wait_for_sync()) {
            vTaskDelay(10 / portTICK_PERIOD_MS);
            continue;
        }

        // Step 2: Parse fields after sync
        if (parse_simhub_packet(&simhub_data)) {
            xQueueSend(xQueue, &simhub_data, 10 / portTICK_PERIOD_MS);
        }
    }
}

/**
 * @brief Get the simhub data queue object
 * 
 * @return QueueHandle_t pointer
 */
QueueHandle_t* get_simhub_data_queue(void)
{
    return &xQueue;
}