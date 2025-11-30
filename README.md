PPS Mini Project – C Programming (Sem 1)

This is our group’s PPS project where we made a farm irrigation scheduler system in C.
The project simulates how farmers decide when to irrigate crops based on:

Soil moisture

Crop type

Moisture threshold

Water requirement

Simple daily/weekly irrigation schedule

We made this completely in C language using VS Code, and the program runs in terminal/console only (no website or UI).

📌 Project Features

✔ Add farm plots with crop details
✔ Update soil moisture (sensor simulation)
✔ Simulate rainfall
✔ Calculate how much water each plot needs
✔ Generate Daily Irrigation Plan
✔ Generate Weekly Irrigation Plan
✔ “What-if” simulation for limited water supply
✔ Store irrigation events using linked list
✔ Uses arrays, structures, pointers, functions, dynamic memory

This project covers almost all syllabus topics of PPS.

📂 Folder Structure
/project-folder
│
├── main.c
├── irrigation.c
├── irrigation.h
└── README.md

⚙️ How to Run the Project (beginner steps)
1. Install compiler

We used MSYS2 → gcc (recommended for Windows + VS Code).

2. Open the project folder in VS Code
3. Compile the project

Open terminal in VS Code and run:

gcc main.c irrigation.c -o irrigation.exe

4. Run the program
./irrigation.exe

🧠 Things We Used From C Syllabus

structures → to store each farm plot

arrays → list of plots

pointers → linked list for irrigation events

dynamic memory (malloc) → for event nodes

functions → water calculation, scheduling, sensor updates

decision making → checking moisture threshold

loops → to process multiple plots

This matches CO1 to CO5 given in the project file.

🧪 Sample Output (Short)
1. Add Plot
2. List Plots
3. Update Moisture
4. Apply Rainfall
5. Generate Daily Plan
6. Generate Weekly Plan
7. What-if Simulation
8. Show Events
9. Exit

👥 Group Members of this project
1. Ujwal
2. Kishan
3. Anshika
4. Ayush

👥 Group Members Contribution (for GitHub commits)

To show equal contribution, each member can:

Edit small parts (comments, variable name changes)

Add simple functions (like print menu, validations)

Update README

Push sample input files

Test and commit bug fixes

Important: Every member must push something to GitHub so commits appear.

🎯 Conclusion

This project helps us understand how C programming can solve real-life farm problems such as irrigation management.
We used all basic concepts of C and made the program modular using multiple .c and .h files.
