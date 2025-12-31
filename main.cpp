#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

using namespace std;

const int MAX_SLOTS = 10;
const string DATA_FILE = "parking_data.txt";

struct Ticket {
    int id;
    string plate;
    int type;     // 1 = Bike, 2 = LTV, 3 = HTV
    bool occupied;
};

void populate(Ticket lot[], int n);
void loadFromFile(Ticket lot[], int n);
void saveToFile(Ticket lot[], int n);
void display(Ticket lot[], int n);
void AddRecord(Ticket lot[], int n);
int DeleteRecord(Ticket lot[], int n);
int searchRecord(Ticket lot[], int n);
int updateRecord(Ticket lot[], int n);

int main() {
    int n = MAX_SLOTS;
    Ticket lot[MAX_SLOTS];

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
            searchRecord(lot, n);
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

void populate(Ticket lot[], int n) {
    for (int i = 0; i < n; i++) {
        lot[i].id = i + 1;
        lot[i].plate = "EMPTY";
        lot[i].type = 0;
        lot[i].occupied = false;
    }
    loadFromFile(lot, n);
}

void loadFromFile(Ticket lot[], int n) {
    ifstream fin(DATA_FILE);
    if (!fin) return;

    int id, type;
    string plate;
    int occInt;

    // File format: id plate type occupied(0/1)
    while (fin >> id >> plate >> type >> occInt) {
        int idx = id - 1;
        if (idx >= 0 && idx < n) {
            lot[idx].plate = plate;
            lot[idx].type = type;
            lot[idx].occupied = (occInt != 0);
        }
    }
    fin.close();
}

void saveToFile(Ticket lot[], int n) {
    ofstream fout(DATA_FILE);
    // write only occupied slots (but storing occ too keeps format simple)
    for (int i = 0; i < n; i++) {
        if (lot[i].occupied) {
            fout << lot[i].id << " "
                 << lot[i].plate << " "
                 << lot[i].type << " "
                 << (lot[i].occupied ? 1 : 0) << "\n";
        }
    }
    fout.close();
}

void display(Ticket lot[], int n) {
    cout << "\n--- Parking Status ---\n";
    for (int i = 0; i < n; i++) {
        if (lot[i].occupied) {
            string tname = (lot[i].type == 1 ? "BIKE" : (lot[i].type == 2 ? "LTV" : "HTV"));
            cout << "[" << lot[i].id << ":" << tname << ":" << lot[i].plate << "] ";
        } else {
            cout << "[ Slot " << lot[i].id << " ] ";
        }
        if ((i + 1) % 2 == 0) cout << endl;
    }
    cout << endl;
}

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
        lot[found].type = 2;
        cout << "Invalid type. Set to LTV.\n";
    }

    lot[found].occupied = true;

    saveToFile(lot, n);
    cout << "Parked. Ticket created.\n";
}

int DeleteRecord(Ticket lot[], int n) {
    string plate;
    cout << "Enter Plate to remove: ";
    cin >> plate;

    for (int i = 0; i < n; i++) {
        if (lot[i].occupied && lot[i].plate == plate) {
            // no time or billing — just remove
            lot[i].occupied = false;
            lot[i].plate = "EMPTY";
            lot[i].type = 0;

            saveToFile(lot, n);
            cout << "Vehicle removed (no time tracking).\n";
            return 0;
        }
    }
    cout << "Vehicle not found.\n";
    return -1;
}

int searchRecord(Ticket lot[], int n) {
    string plate;
    cout << "Enter Plate to search: ";
    cin >> plate;

    for (int i = 0; i < n; i++) {
        if (lot[i].occupied && lot[i].plate == plate) {
            cout << "Found at Slot " << lot[i].id << "\n";
            string tname = (lot[i].type == 1 ? "BIKE" : (lot[i].type == 2 ? "LTV" : "HTV"));
            cout << "Type : " << tname << "\n";
            return 0;
        }
    }
    cout << "Not found.\n";
    return -1;
}

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
