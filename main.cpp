#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <ctime>
#include <cmath>

using namespace std;

const int MAX_SLOTS = 20;
const string DATA_FILE = "parking_data.txt";

/* ---------- NESTED STRUCT ---------- */
struct TimeInfo {
    time_t entry;
};

struct Ticket {
    int slotID;
    string plate;
    int type;          // 1 Bike, 2 LTV, 3 HTV
    bool occupied;
    TimeInfo time;     // nested structure
};

/* ---------- FUNCTION PROTOTYPES ---------- */
void initialize(Ticket lot[], int n);
void loadFromFile(Ticket lot[], int n);
void saveToFile(Ticket lot[], int n);

void menu();
void inputRecord(Ticket lot[], int n);
void display(Ticket lot[], int n);
int search(Ticket lot[], int n);
void updateRecord(Ticket lot[], int n);
void deleteRecord(Ticket lot[], int n);
void sortByPlate(Ticket lot[], int n);

/* ---------- MAIN ---------- */
int main() {
    Ticket lot[MAX_SLOTS];
    int choice;

    initialize(lot, MAX_SLOTS);

    do {
        menu();
        cin >> choice;

        switch (choice) {
        case 1:
            inputRecord(lot, MAX_SLOTS);
            break;
        case 2:
            deleteRecord(lot, MAX_SLOTS);
            break;
        case 3:
            search(lot, MAX_SLOTS);
            break;
        case 4:
            updateRecord(lot, MAX_SLOTS);
            break;
        case 5:
            sortByPlate(lot, MAX_SLOTS);
            cout << "Sorted by plate.\n";
            break;
        case 6:
            saveToFile(lot, MAX_SLOTS);
            cout << "Saved & Exit.\n";
            break;
        default:
            cout << "Invalid choice!\n";
        }

    } while (choice != 6);

    return 0;
}

/* ---------- FUNCTIONS ---------- */

void menu() {
    cout << "\n--- PARKING SYSTEM MENU ---\n";
    cout << "1. Park Vehicle\n";
    cout << "2. Remove Vehicle\n";
    cout << "3. Search Vehicle\n";
    cout << "4. Update Vehicle\n";
    cout << "5. Sort by Plate\n";
    cout << "6. Save & Exit\n";
    cout << "Enter choice: ";
}

void initialize(Ticket lot[], int n) {
    for (int i = 0; i < n; i++) {
        lot[i].slotID = i + 1;
        lot[i].plate = "EMPTY";
        lot[i].type = 0;
        lot[i].occupied = false;
        lot[i].time.entry = 0;
    }
    loadFromFile(lot, n);
}

void inputRecord(Ticket lot[], int n) {
    for (int i = 0; i < n; i++) {
        if (!lot[i].occupied) {
            cout << "Enter Plate: ";
            cin >> lot[i].plate;

            cout << "Type (1=Bike, 2=LTV, 3=HTV): ";
            cin >> lot[i].type;

            if (lot[i].type < 1 || lot[i].type > 3) {
                cout << "Invalid type. Set to LTV.\n";
                lot[i].type = 2;
            }

            lot[i].occupied = true;
            lot[i].time.entry = time(0);
            saveToFile(lot, n);
            cout << "Vehicle Parked at Slot " << lot[i].slotID << endl;
            return;
        }
    }
    cout << "Parking Full!\n";
}

void display(Ticket lot[], int n) {
    cout << "\n--- PARKING STATUS ---\n";
    for (int i = 0; i < n; i++) {
        if (lot[i].occupied)
            cout << "[" << lot[i].slotID << ":" << lot[i].plate << "] ";
        else
            cout << "[Slot " << lot[i].slotID << "] ";

        if ((i + 1) % 5 == 0) cout << endl;
    }
}

int search(Ticket lot[], int n) {
    string plate;
    cout << "Enter Plate: ";
    cin >> plate;

    for (int i = 0; i < n; i++) {
        if (lot[i].occupied && lot[i].plate == plate) {
            cout << "Found at Slot " << lot[i].slotID << endl;
            cout << "Entry Time: " << ctime(&lot[i].time.entry);
            return 1;
        }
    }
    cout << "Vehicle Not Found.\n";
    return 0;
}

void updateRecord(Ticket lot[], int n) {
    string plate;
    cout << "Enter Plate to Update: ";
    cin >> plate;

    for (int i = 0; i < n; i++) {
        if (lot[i].occupied && lot[i].plate == plate) {
            cout << "New Plate: ";
            cin >> lot[i].plate;
            cout << "New Type (1-3): ";
            cin >> lot[i].type;
            saveToFile(lot, n);
            cout << "Updated Successfully.\n";
            return;
        }
    }
    cout << "Vehicle Not Found.\n";
}

void deleteRecord(Ticket lot[], int n) {
    string plate;
    cout << "Enter Plate to Remove: ";
    cin >> plate;

    for (int i = 0; i < n; i++) {
        if (lot[i].occupied && lot[i].plate == plate) {
            double hours = difftime(time(0), lot[i].time.entry) / 3600;
            int billHours = max(1, (int)ceil(hours));

            int rate = (lot[i].type == 1 ? 20 : lot[i].type == 2 ? 50 : 100);
            cout << "Bill: " << billHours * rate << " Rs\n";

            lot[i].occupied = false;
            lot[i].plate = "EMPTY";
            saveToFile(lot, n);
            return;
        }
    }
    cout << "Vehicle Not Found.\n";
}

/* ---------- BUBBLE SORT ---------- */
void sortByPlate(Ticket lot[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (lot[j].plate > lot[j + 1].plate) {
                swap(lot[j], lot[j + 1]);
            }
        }
    }
}

/* ---------- FILE HANDLING ---------- */
void saveToFile(Ticket lot[], int n) {
    ofstream fout(DATA_FILE);
    for (int i = 0; i < n; i++) {
        if (lot[i].occupied) {
            fout << lot[i].slotID << " "
                 << lot[i].plate << " "
                 << lot[i].type << " "
                 << lot[i].time.entry << " "
                 << lot[i].occupied << endl;
        }
    }
    fout.close();
}

void loadFromFile(Ticket lot[], int n) {
    ifstream fin(DATA_FILE);
    if (!fin) return;

    int id, type;
    string plate;
    time_t t;
    bool occ;

    while (fin >> id >> plate >> type >> t >> occ) {
        int idx = id - 1;
        lot[idx].plate = plate;
        lot[idx].type = type;
        lot[idx].time.entry = t;
        lot[idx].occupied = occ;
    }
    fin.close();
}
