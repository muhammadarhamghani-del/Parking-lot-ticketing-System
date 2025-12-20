#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <ctime>
#include <cmath>

using namespace std;

// --- SIMPLE SETTINGS (easy words) ---
const int MAX_SLOTS = 10;
const string DATA_FILE = "parking_data.txt"; // simple file name

// --- STRUCT (like your students example) ---
struct Ticket {
    int id;             // slot number (1..MAX_SLOTS)
    string plate;       // plate number or "EMPTY"
    int type;           // 1 = Bike, 2 = LTV, 3 = HTV
    time_t entryTime;   // time when parked (as timestamp)
    bool occupied;      // true if vehicle is here
};

// --- FUNCTION PROTOTYPES (easy names) ---
void populate(Ticket lot[], int n);        // set default and load from file
void loadFromFile(Ticket lot[], int n);    // read saved data
void saveToFile(Ticket lot[], int n);      // save data to file
void display(Ticket lot[], int n);         // show parking map
void AddRecord(Ticket lot[], int n);       // park a vehicle (add)
int DeleteRecord(Ticket lot[], int n);     // remove vehicle by plate
int search(Ticket lot[], int n);           // search by plate
int updateRecord(Ticket lot[], int n);     // change plate or type

// --- MAIN ---
int main() {
    int n = MAX_SLOTS;
    Ticket lot[MAX_SLOTS];

    // prepare array and try to load previous data
    populate(lot, n);

    int choice;
    do {
        cout << "\n---- SIMPLE PARKING MENU ----\n";
        display(lot, n);
        cout << "\n1) Park vehicle (Add)\n";
        cout << "2) Exit vehicle (Remove)\n";
        cout << "3) Search vehicle\n";
        cout << "4) Update vehicle info\n";
        cout << "5) Save & Exit\n";
        cout << "Choose: ";
        cin >> choice;

        if (choice == 1) {
            AddRecord(lot, n);
        } else if (choice == 2) {
            int res = DeleteRecord(lot, n);
            if (res == 0) cout << "Removed and saved.\n";
        } else if (choice == 3) {
            search(lot, n);
        } else if (choice == 4) {
            updateRecord(lot, n);
        } else if (choice == 5) {
            saveToFile(lot, n);
            cout << "Saved to file. Bye!\n";
        } else {
            cout << "Invalid choice. Try again.\n";
        }

    } while (choice != 5);

    return 0;
}

// --- HELPERS / IMPLEMENTATION ---

// Set defaults and load from file if exists
void populate(Ticket lot[], int n) {
    for (int i = 0; i < n; i++) {
        lot[i].id = i + 1;
        lot[i].plate = "EMPTY";
        lot[i].type = 0;
        lot[i].entryTime = 0;
        lot[i].occupied = false;
    }
    loadFromFile(lot, n);
}

// Load saved parked vehicles (simple file format)
void loadFromFile(Ticket lot[], int n) {
    ifstream fin(DATA_FILE);
    if (!fin) return; // no file yet

    int id, type;
    string plate;
    long long t;
    bool occ;

    while (fin >> id >> plate >> type >> t >> occ) {
        int idx = id - 1;
        if (idx >= 0 && idx < n) {
            lot[idx].plate = plate;
            lot[idx].type = type;
            lot[idx].entryTime = (time_t)t;
            lot[idx].occupied = occ;
        }
    }
    fin.close();
}

// Save current state (only occupied slots stored)
void saveToFile(Ticket lot[], int n) {
    ofstream fout(DATA_FILE);
    // simple lines: id plate type entryTime occupied
    for (int i = 0; i < n; i++) {
        if (lot[i].occupied) {
            fout << lot[i].id << " "
                 << lot[i].plate << " "
                 << lot[i].type << " "
                 << (long long)lot[i].entryTime << " "
                 << lot[i].occupied << "\n";
        }
    }
    fout.close();
}

// Show parking map in easy words
void display(Ticket lot[], int n) {
    cout << "\n--- Parking Status ---\n";
    for (int i = 0; i < n; i++) {
        if (lot[i].occupied) {
            string tname = (lot[i].type == 1 ? "BIKE" : (lot[i].type == 2 ? "LTV" : "HTV"));
            // show short info and entry time (human)
            cout << "[" << lot[i].id << ":" << tname << ":" << lot[i].plate << "] ";
        } else {
            cout << "[ Slot " << lot[i].id << " ] ";
        }
        if ((i + 1) % 2 == 0) cout << endl;
    }
    cout << endl;
}

// Park a vehicle - find first empty slot
void AddRecord(Ticket lot[], int n) {
    int found = -1;
    for (int i = 0; i < n; i++) {
        if (!lot[i].occupied) { found = i; break; }
    }
    if (found == -1) {
        cout << "Parking full. No space.\n";
        return;
    }

    cout << "Parking at Slot " << lot[found].id << "\n";
    cout << "Enter Plate: ";
    cin >> lot[found].plate;

    cout << "Type (1=Bike, 2=LTV, 3=HTV): ";
    cin >> lot[found].type;
    if (lot[found].type < 1 || lot[found].type > 3) {
        lot[found].type = 2; // default
        cout << "Invalid type. Set to LTV.\n";
    }

    lot[found].entryTime = time(0);
    lot[found].occupied = true;

    saveToFile(lot, n); // quick save
    cout << "Parked. Ticket created.\n";
}

// Remove vehicle by plate, calculate bill
int DeleteRecord(Ticket lot[], int n) {
    string plate;
    cout << "Enter Plate to remove: ";
    cin >> plate;

    for (int i = 0; i < n; i++) {
        if (lot[i].occupied && lot[i].plate == plate) {
            // calculate hours (round up to full hour, min 1)
            double hoursD = difftime(time(0), lot[i].entryTime) / 3600.0;
            int hours = max(1, (int)ceil(hoursD));

            // simple price table (easy words)
            const double PRICE_BIKE = 20.0;
            const double PRICE_LTV  = 50.0;
            const double PRICE_HTV  = 100.0;

            double rate = (lot[i].type == 1 ? PRICE_BIKE : (lot[i].type == 2 ? PRICE_LTV : PRICE_HTV));
            double total = hours * rate;

            cout << fixed << setprecision(2);
            cout << "\n--- BILL ---\n";
            cout << "Plate : " << lot[i].plate << "\n";
            cout << "Time  : " << hours << " hour(s)\n";
            cout << "Total : " << total << " Rs\n";
            cout << "------------\n";

            // clear slot
            lot[i].occupied = false;
            lot[i].plate = "EMPTY";
            lot[i].type = 0;
            lot[i].entryTime = 0;

            saveToFile(lot, n);
            return 0;
        }
    }
    cout << "Vehicle not found.\n";
    return -1;
}

// Search by plate (show info)
int search(Ticket lot[], int n) {
    string plate;
    cout << "Enter Plate to search: ";
    cin >> plate;

    for (int i = 0; i < n; i++) {
        if (lot[i].occupied && lot[i].plate == plate) {
            cout << "Found at Slot " << lot[i].id << "\n";
            string tname = (lot[i].type == 1 ? "BIKE" : (lot[i].type == 2 ? "LTV" : "HTV"));
            cout << "Type : " << tname << "\n";
            cout << "Entry: " << ctime(&lot[i].entryTime); // human time
            return 0;
        }
    }
    cout << "Not found.\n";
    return -1;
}

// Update plate or type for a parked vehicle
int updateRecord(Ticket lot[], int n) {
    string plate;
    cout << "Enter Plate to update: ";
    cin >> plate;

    for (int i = 0; i < n; i++) {
        if (lot[i].occupied && lot[i].plate == plate) {
            cout << "Slot " << lot[i].id << " selected.\n";
            cout << "Enter new Plate (or same): ";
            cin >> lot[i].plate;
            cout << "Enter new Type (1=Bike,2=LTV,3=HTV): ";
            cin >> lot[i].type;
            if (lot[i].type < 1 || lot[i].type > 3) lot[i].type = 2;
            saveToFile(lot, n);
            cout << "Updated and saved.\n";
            return 0;
        }
    }
    cout << "Vehicle not found to update.\n";
    return -1;
}
