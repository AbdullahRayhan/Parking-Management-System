#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // For timestamp handling

#define MAX_VEHICLES 50
#define CAR_RATE 30
#define BIKE_RATE 15
#define CAR_LIMIT_FINE 1000
#define BIKE_LIMIT_FINE 600
#define HOUR_LIMIT 24

typedef struct {
    char licensePlate[15];
    char ownerName[30];
    int parkingSlot;
    time_t entryTime; // Stores entry timestamp
} Vehicle;

typedef struct {
    int slotNumber;
    int isOccupied;
} ParkingSlot;

typedef struct {
    char licensePlate[15];
    float fineAmount;
} Fine;

Vehicle cars[MAX_VEHICLES];
Vehicle bikes[MAX_VEHICLES];
ParkingSlot slots[MAX_VEHICLES];
Fine fines[MAX_VEHICLES];
int carCount = 0, bikeCount = 0, fineCount = 0;

// Function prototypes
void clearScreen();
void displayMenu();
void addVehicle(Vehicle vehicles[], int *vehicleCount, const char *type);
void removeVehicle(Vehicle vehicles[], int *vehicleCount, const char *type);
float calculateFine(Vehicle vehicle, const char *type);
void assignFine();
void viewFines();
void viewVehicles(Vehicle vehicles[], int vehicleCount, const char *type); // Add this prototype

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void displayMenu() {
    clearScreen();
    printf("\033[1;36m"); // Cyan color
    printf("****************************************\n");
    printf("*    24/7 PUBLIC PARKING MANAGEMENT SYSTEM         *\n");
    printf("****************************************\n");
    printf("* 1. Add Car                            *\n");
    printf("* 2. Add Bike                           *\n");
    printf("* 3. Remove Car                         *\n");
    printf("* 4. Remove Bike                        *\n");
    printf("* 5. View Cars                          *\n");
    printf("* 6. View Bikes                         *\n");
    printf("* 7. Assign Fine                        *\n");
    printf("* 8. View Fines                         *\n");
    printf("* 9. Exit                               *\n");
    printf("****************************************\n");
    printf("\033[0m"); // Reset color
    printf("Choose an option: ");
}

void addVehicle(Vehicle vehicles[], int *vehicleCount, const char *type) {
    if (*vehicleCount >= MAX_VEHICLES) {
        printf("\n\033[1;31mParking full for %s!\033[0m\n", type);
        return;
    }

    Vehicle newVehicle;
    printf("\nEnter license plate: ");
    scanf("%s", newVehicle.licensePlate);
    printf("Enter owner name: ");
    scanf("%s", newVehicle.ownerName);

    for (int i = 0; i < MAX_VEHICLES; i++) {
        if (slots[i].isOccupied == 0) {
            newVehicle.parkingSlot = i;
            newVehicle.entryTime = time(NULL); // Store entry time
            slots[i].isOccupied = 1;
            vehicles[(*vehicleCount)++] = newVehicle;
            printf("\n\033[1;32m%s added to slot %d.\033[0m\n", type, i);
            return;
        }
    }
    printf("\n\033[1;31mNo available slots!\033[0m\n");
}

void removeVehicle(Vehicle vehicles[], int *vehicleCount, const char *type) {
    char licensePlate[15];
    printf("\nEnter license plate of %s to remove: ", type);
    scanf("%s", licensePlate);

    for (int i = 0; i < *vehicleCount; i++) {
        if (strcmp(vehicles[i].licensePlate, licensePlate) == 0) {
            slots[vehicles[i].parkingSlot].isOccupied = 0;
            for (int j = i; j < *vehicleCount - 1; j++) {
                vehicles[j] = vehicles[j + 1];
            }
            (*vehicleCount)--;
            printf("\n\033[1;32m%s removed.\033[0m\n", type);
            return;
        }
    }
    printf("\n\033[1;31m%s not found.\033[0m\n", type);
}

float calculateFine(Vehicle vehicle, const char *type) {
    time_t currentTime = time(NULL);
    double parkedHours = difftime(currentTime, vehicle.entryTime) / 3600.0; // Convert seconds to hours

    float fine = 0;
    if (strcmp(type, "Car") == 0) {
        if (parkedHours > HOUR_LIMIT) {
            fine = CAR_LIMIT_FINE;
        } else {
            fine = parkedHours * CAR_RATE;
        }
    } else if (strcmp(type, "Bike") == 0) {
        if (parkedHours > HOUR_LIMIT) {
            fine = BIKE_LIMIT_FINE;
        } else {
            fine = parkedHours * BIKE_RATE;
        }
    }

    return fine;
}

void assignFine() {
    char licensePlate[15];
    char type[10];
    printf("\nEnter vehicle type (Car/Bike): ");
    scanf("%s", type);
    printf("Enter license plate: ");
    scanf("%s", licensePlate);

    Vehicle *vehicles = (strcmp(type, "Car") == 0) ? cars : bikes;
    int vehicleCount = (strcmp(type, "Car") == 0) ? carCount : bikeCount;

    for (int i = 0; i < vehicleCount; i++) {
        if (strcmp(vehicles[i].licensePlate, licensePlate) == 0) {
            float fineAmount = calculateFine(vehicles[i], type);

            // Add fine to the record
            strcpy(fines[fineCount].licensePlate, licensePlate);
            fines[fineCount++].fineAmount = fineAmount;

            printf("\n\033[1;32mFine for %s: %.2f Taka.\033[0m\n", licensePlate, fineAmount);
            return;
        }
    }
    printf("\n\033[1;31mVehicle not found.\033[0m\n");
}

void viewFines() {
    printf("\n\033[1;34m=== FINES ===\033[0m\n");
    if (fineCount == 0) {
        printf("\n\033[1;33mNo fines assigned.\033[0m\n");
        return;
    }

    for (int i = 0; i < fineCount; i++) {
        printf("License Plate: \033[1;33m%s\033[0m, Fine: \033[1;33m%.2f Taka\033[0m\n",
               fines[i].licensePlate, fines[i].fineAmount);
    }
    printf("\n\033[1;34m==============\033[0m\n");
}

void viewVehicles(Vehicle vehicles[], int vehicleCount, const char *type) {
    printf("\n\033[1;34m=== PARKED %sS ===\033[0m\n", type);
    if (vehicleCount == 0) {
        printf("\n\033[1;33mNo %ss parked.\033[0m\n", type);
        return;
    }

    for (int i = 0; i < vehicleCount; i++) {
        printf("License Plate: \033[1;33m%s\033[0m, Owner: \033[1;33m%s\033[0m, Slot: \033[1;33m%d\033[0m\n",
               vehicles[i].licensePlate, vehicles[i].ownerName, vehicles[i].parkingSlot);
    }
    printf("\n\033[1;34m===================\033[0m\n");
}

int main() {
    int choice;

    for (int i = 0; i < MAX_VEHICLES; i++) {
        slots[i].slotNumber = i;
        slots[i].isOccupied = 0;
    }

    while (1) {
        displayMenu();
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addVehicle(cars, &carCount, "Car");
                break;
            case 2:
                addVehicle(bikes, &bikeCount, "Bike");
                break;
            case 3:
                removeVehicle(cars, &carCount, "Car");
                break;
            case 4:
                removeVehicle(bikes, &bikeCount, "Bike");
                break;
            case 5:
                viewVehicles(cars, carCount, "Car");
                break;
            case 6:
                viewVehicles(bikes, bikeCount, "Bike");
                break;
            case 7:
                assignFine();
                break;
            case 8:
                viewFines();
                break;
            case 9:
                printf("\n\033[1;31mExiting...\033[0m\n");
                exit(0);
                break;
            default:
                printf("\n\033[1;31mInvalid option. Try again.\033[0m\n");
        }

        printf("\nPress Enter to continue...");
        getchar(); // Consume newline
        getchar(); // Wait for user input
    }

    return 0;
}
