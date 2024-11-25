#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
using namespace std;

// Data structures
struct MNTEntry {
    string macro_name;
    int MDT_index;
    int PNTAB_index;
    int KPDTAB_index;
    int EVNTAB_index;
};

struct MDTEntry {
    int index;
    string definition;
};

struct PNTABEntry {
    string param_name;
};

struct KPDTABEntry {
    string keyword;
    string default_value;
};

struct EVNTABEntry {
    string variable_name;
};

struct SSTABEntry {
    string section_name;
};

// Global tables
vector<MNTEntry> MNT;
vector<MDTEntry> MDT;
vector<PNTABEntry> PNTAB;
vector<KPDTABEntry> KPDTAB;
vector<EVNTABEntry> EVNTAB;
vector<SSTABEntry> SSTAB;

// Function to process input file and create the tables
void processMacroDefinition(string inputFile) {
    ifstream file(inputFile);
    string line;
    bool inMacro = false;
    int MDT_index = 0;
    
    while (getline(file, line)) {
        if (line.find("MACRO") != string::npos) {
            inMacro = true;
            MNTEntry mntEntry;
            string macroName;
            file >> macroName;
            mntEntry.macro_name = macroName;
            mntEntry.MDT_index = MDT_index;
            MNT.push_back(mntEntry);
        } else if (line.find("MEND") != string::npos) {
            inMacro = false;
        } else if (inMacro) {
            MDTEntry mdtEntry;
            mdtEntry.index = MDT_index++;
            mdtEntry.definition = line;
            MDT.push_back(mdtEntry);
        }
    }
}

// Function to detect errors (like undefined macros)
void detectErrors() {
    for (const auto& mntEntry : MNT) {
        bool found = false;
        for (const auto& mdtEntry : MDT) {
            if (mdtEntry.definition.find(mntEntry.macro_name) != string::npos) {
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "Error: Undefined macro " << mntEntry.macro_name << endl;
        }
    }
}

// Function to display tables
void displayTables() {
    cout << "\nMNT (Macro Name Table):\n";
    for (const auto& entry : MNT) {
        cout << "Macro: " << entry.macro_name << ", MDT Index: " << entry.MDT_index << endl;
    }

    cout << "\nMDT (Macro Definition Table):\n";
    for (const auto& entry : MDT) {
        cout << "Index: " << entry.index << ", Definition: " << entry.definition << endl;
    }
}

int main() {
    string inputFile = "input.txt";
    processMacroDefinition(inputFile);
    detectErrors();
    displayTables();
    return 0;
}
