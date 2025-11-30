#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "irrigation.h"

// Windows (MinGW) does NOT provide strcasestr, so we define it manually.
char *strcasestr_custom(const char *haystack, const char *needle) {
    if (!haystack || !needle) return NULL;

    size_t needle_len = strlen(needle);
    if (needle_len == 0) return (char *)haystack;

    while (*haystack) {
        if (strncasecmp(haystack, needle, needle_len) == 0)
            return (char *)haystack;
        haystack++;
    }
    return NULL;
}
// code ends

/* ----- Initialization ----- */
void farm_init(Farm *f) {
    f->plotCount = 0;
    f->events = NULL;
}

/* ----- Plot management ----- */
int add_plot(Farm *f, const char *crop, float area, float moisture, float threshold, int lastIrrigationDay) {
    if (f->plotCount >= MAX_PLOTS) return -1;
    Plot *p = &f->plots[f->plotCount];
    p->id = f->plotCount + 1;
    strncpy(p->crop, crop, MAX_CROP_NAME-1);
    p->crop[MAX_CROP_NAME-1] = '\0';
    p->area = area;
    p->moisture = moisture;
    p->moistureThreshold = threshold;
    p->lastIrrigationDay = lastIrrigationDay;
    f->plotCount++;
    return p->id;
}

void list_plots(const Farm *f) {
    printf("ID | Crop\t\t| Area(ha) | Moisture(%%) | Threshold(%%) | LastIrrDay\n");
    printf("-----------------------------------------------------------------\n");
    for (int i=0;i<f->plotCount;i++) {
        const Plot *p = &f->plots[i];
        printf("%2d | %-10s | %7.3f  | %10.2f | %11.2f | %10d\n",
               p->id, p->crop, p->area, p->moisture, p->moistureThreshold, p->lastIrrigationDay);
    }
}

Plot* get_plot_by_id(Farm *f, int id) {
    if (id <=0 || id > f->plotCount) return NULL;
    return &f->plots[id-1];
}

/* ----- Sensor input ----- */
void update_moisture(Farm *f, int plotId, float newMoisture) {
    Plot *p = get_plot_by_id(f, plotId);
    if (!p) { printf("Plot not found\n"); return; }
    p->moisture = newMoisture;
}

void apply_rainfall(Farm *f, float rainfall_mm) {
    /* Very simple model: rainfall mm raises moisture by rainfall_mm * 0.1 (arbitrary) */
    float delta = rainfall_mm * 0.1f;
    for (int i=0;i<f->plotCount;i++) {
        f->plots[i].moisture += delta;
        if (f->plots[i].moisture > 100.0f) f->plots[i].moisture = 100.0f;
    }
}

/* ----- Crop coefficient & water calculation ----- */
float crop_coefficient(const char *crop) {
    /* Example coefficients (arbitrary for project) */
    if (strcasestr_custom(crop, "rice")) return 1.2f;
    if (strcasestr_custom(crop, "wheat")) return 0.6f;
    if (strcasestr_custom(crop, "maize") || strcasestr_custom(crop, "corn")) return 0.7f;
    if (strcasestr_custom(crop, "sugar")) return 1.1f;
    /* default */
    return 0.8f;
}

float compute_water_liters(const Plot *p) {
    /* Simple model:
       water (liters) = area(ha) * 10000 (m2/ha) * rootDepth(m) * availableWaterFraction * cropCoeff * (threshold - moisture)/100
       To keep simple, use:
       liters = area * 10000 * 0.1 * coeff * ((threshold - moisture)/100)
       where 0.1 m root zone and soil holds 100 mm = 100 liters/m2 for 0-1m. This is arbitrary but consistent.
    */
    float coeff = crop_coefficient(p->crop);
    float deficitPercent = p->moistureThreshold - p->moisture;
    if (deficitPercent <= 0) return 0.0f;
    float liters = p->area * 10000.0f * 0.1f * coeff * (deficitPercent/100.0f);
    /* safety */
    if (liters < 0) liters = 0;
    return liters;
}

/* ----- Event list (linked list) ----- */
void add_event(Farm *f, int day, int plotId, float liters) {
    EventNode *node = (EventNode*) malloc(sizeof(EventNode));
    node->day = day;
    node->plotId = plotId;
    node->waterLiters = liters;
    node->next = NULL;

    if (!f->events) {
        f->events = node;
    } else {
        /* append to end */
        EventNode *cur = f->events;
        while (cur->next) cur = cur->next;
        cur->next = node;
    }
}

void print_events(Farm *f) {
    if (!f->events) {
        printf("No scheduled irrigation events.\n");
        return;
    }
    printf("Scheduled Events:\nDay | PlotID | Liters\n");
    printf("------------------------\n");
    EventNode *cur = f->events;
    while (cur) {
        printf("%3d | %6d | %7.2f L\n", cur->day, cur->plotId, cur->waterLiters);
        cur = cur->next;
    }
}

void free_events(Farm *f) {
    EventNode *cur = f->events;
    while (cur) {
        EventNode *tmp = cur->next;
        free(cur);
        cur = tmp;
    }
    f->events = NULL;
}

/* ----- Scheduling ----- */
void generate_daily_plan(Farm *f, int day) {
    printf("Generating daily plan for day %d\n", day);
    free_events(f); /* clear previous events for this example */

    for (int i=0;i<f->plotCount;i++) {
        Plot *p = &f->plots[i];
        float req = compute_water_liters(p);
        if (req > 0.0f) {
            add_event(f, day, p->id, req);
        }
    }
    print_events(f);
}

void generate_weekly_plan(Farm *f, int startDay) {
    printf("Generating weekly plan starting day %d\n", startDay);
    free_events(f);

    /* Naive weekly approach: schedule each needy plot on earliest day - distribute equally */
    int day = startDay;
    for (int i=0;i<f->plotCount;i++) {
        Plot *p = &f->plots[i];
        float req = compute_water_liters(p);
        if (req > 0.0f) {
            add_event(f, day, p->id, req);
            day++;
            if (day >= startDay + 7) day = startDay; /* wrap */
        }
    }
    print_events(f);
}

/* ----- Utility sort for priority ----- */
int compare_priority(const void *a, const void *b) {
    const Plot *pa = (const Plot *)a;
    const Plot *pb = (const Plot *)b;
    if (pa->moisture < pb->moisture) return -1;
    if (pa->moisture > pb->moisture) return 1;
    return 0;
}

void sort_plots_by_priority(Plot arr[], int n) {
    qsort(arr, n, sizeof(Plot), compare_priority);
}

 /* ---------------------------------------------------------
   What-if Simulation:
   Distribute limited water based on priority
----------------------------------------------------------*/
void what_if_simulation(Farm *f, float availableLiters) {
    printf("What-if: available water = %.2f L\n", availableLiters);
    if (availableLiters <= 0.0f) {
        printf("No water available.\n");
        return;
    }

    /* Create a temporary array copy to sort by priority */
    Plot *copy = (Plot*) malloc(sizeof(Plot) * f->plotCount);
    for (int i=0;i<f->plotCount;i++) copy[i] = f->plots[i];

     //Sort by moisture (Low moisture = Higher priority)

    sort_plots_by_priority(copy, f->plotCount);

    printf("Priority order (low moisture first):\n");
    printf("PlotID | Crop | Moisture | Required L\n");
    printf("------------------------------------\n");
    for (int i=0;i<f->plotCount;i++) {
        Plot *p = &copy[i];
        float req = compute_water_liters(p);
        printf("%6d | %-6s | %7.2f | %10.2f\n", p->id, p->crop, p->moisture, req);
    }

    printf("\nAllocating water by priority:\n");
    for (int i=0;i<f->plotCount;i++) {
        if (availableLiters <= 0) break;
        Plot *p = &copy[i];
        float req = compute_water_liters(p);
        if (req <= 0) continue;
        if (availableLiters >= req) {
            printf("Irrigate Plot %d fully (%.2f L)\n", p->id, req);
            availableLiters -= req;
        } else {
            printf("Partially irrigate Plot %d with %.2f L (needs %.2f L)\n", p->id, availableLiters, req);
            availableLiters = 0;
        }
    }
    if (availableLiters > 0) {
        printf("Unused water remaining: %.2f L\n", availableLiters);
    } else {
        printf("All water allocated.\n");
    }

    free(copy);
}
