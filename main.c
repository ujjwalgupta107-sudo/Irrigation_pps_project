#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "irrigation.h"
// used void function instead of main.
void print_menu() {
    printf("\n--- Precision Irrigation Scheduler ---\n"); // From here we will get input of all the details required in the project.
    printf("1. Add plot\n");
    printf("2. List plots\n");
    printf("3. Update plot moisture\n");
    printf("4. Apply rainfall (simulated)\n");
    printf("5. Generate daily plan (today)\n");
    printf("6. Generate weekly plan (7 days)\n");
    printf("7. What-if simulation (limited water)\n");
    printf("8. Show scheduled events\n");
    printf("9. Exit\n");
    printf("Choose option: "); // These are all the info that we need.
}

int main() {
    Farm farm;
    farm_init(&farm);

    /* Preload some sample plots (optional) */
    add_plot(&farm, "Wheat", 0.5f, 30.0f, 50.0f, 0);
    add_plot(&farm, "Rice", 0.3f, 70.0f, 75.0f, 0);
    add_plot(&farm, "Maize", 0.2f, 25.0f, 45.0f, 0);

    int choice; //Need choice input from the user
    while (1) {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            while (getchar()!='\n'); /* flush */
            printf("Invalid input\n");
            continue;
        }
        if (choice == 1) {   //new code added
            char crop[MAX_CROP_NAME];
            float area, moisture, threshold;
            int lastDay;
            printf("Enter crop name: ");
            scanf("%s", crop);
            printf("Enter area (ha): ");
            scanf("%f", &area);
            printf("Enter current moisture (0-100): ");
            scanf("%f", &moisture);
            printf("Enter moisture threshold (0-100): ");
            scanf("%f", &threshold);
            printf("Enter last irrigation day (int): ");
            scanf("%d", &lastDay);
            int id = add_plot(&farm, crop, area, moisture, threshold, lastDay);
            if (id < 0) printf("Failed to add plot (max reached)\n");
            else printf("Plot added with ID %d\n", id);
        } else if (choice == 2) {
            list_plots(&farm);
        } else if (choice == 3) {
            int id; float m;
            printf("Enter plot ID: "); scanf("%d", &id);
            printf("Enter new moisture value: "); scanf("%f", &m);
            update_moisture(&farm, id, m);
        } else if (choice == 4) {
            float r; printf("Enter rainfall (mm): "); scanf("%f", &r);
            apply_rainfall(&farm, r);
        } else if (choice == 5) {
            int day; printf("Enter day number (int): "); scanf("%d", &day);
            generate_daily_plan(&farm, day);
        } else if (choice == 6) {
            int day; printf("Enter start day number (int): "); scanf("%d", &day);
            generate_weekly_plan(&farm, day);
        } else if (choice == 7) {
            float liters; printf("Enter available liters: "); scanf("%f", &liters);
            what_if_simulation(&farm, liters);
        } else if (choice == 8) {
            print_events(&farm);
        } else if (choice == 9) {
            break;
        } else {
            printf("Unknown option\n");
        }
    }

    free_events(&farm);//changes in this code 
    printf("Exiting. Goodbye.\n");
    return 0;
}
