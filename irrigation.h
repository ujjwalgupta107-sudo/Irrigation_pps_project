#ifndef IRRIGATION_H
#define IRRIGATION_H

#define MAX_PLOTS 50
#define MAX_CROP_NAME 32

typedef struct Plot {
    int id;
    char crop[MAX_CROP_NAME];   //main code
    float area;                // in hectares
    float moisture;            // current moisture % (0-100)
    float moistureThreshold;   // threshold % below which irrigation required
    int lastIrrigationDay;     // day count
} Plot;

/* Node for dynamic irrigation events (pointer usage) */
typedef struct EventNode {
    int day;
    int plotId;
    float waterLiters;
    struct EventNode *next;
} EventNode;

/* Farm container  begins */
typedef struct Farm {
    Plot plots[MAX_PLOTS];
    int plotCount;
    EventNode *events; /* linked list of irrigation events */
} Farm;

/* Initialization */
void farm_init(Farm *f);

/* Plot management */
int add_plot(Farm *f, const char *crop, float area, float moisture, float threshold, int lastIrrigationDay);
void list_plots(const Farm *f);
Plot* get_plot_by_id(Farm *f, int id);

/* Sensor input */
void update_moisture(Farm *f, int plotId, float newMoisture);
void apply_rainfall(Farm *f, float rainfall_mm); /* simple model */

/* Water requirement & coefficients */
float crop_coefficient(const char *crop); /* returns coefficient */
float compute_water_liters(const Plot *p); /* liters required */

/* Scheduler & plans */
void generate_daily_plan(Farm *f, int day);
void generate_weekly_plan(Farm *f, int startDay);

/* What-if simulation */
void what_if_simulation(Farm *f, float availableLiters);

/* Event list management */
void add_event(Farm *f, int day, int plotId, float liters);
void print_events(Farm *f);
void free_events(Farm *f);

/* Utility */
void sort_plots_by_priority(Plot arr[], int n); /* sorts in-place by moisture ascending */
#endif
