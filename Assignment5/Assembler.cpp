#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

// Structure for Macro Name Table (MNT)
struct MNTEntry {
    string macroName;
    int mdtIndex;
    int numParams;
};

// Structure for Macro Definition Table (MDT)
struct MDTEntry {
    int index;
    string definition;
};

// Actual Parameter Table (APTAB) implemented as an unordered map
unordered_map<string, string> APTAB;

// Global tables
vector<MNTEntry> MNT;
vector<MDTEntry> MDT;

// Function to initialize MNT and MDT from the input macro definitions file
void initializeTables() {
    ifstream macroDefFile("macro_definitions.txt");
    string line;
    int mdtIndex = 0;

    // Read macro definitions and populate MDT and MNT
    while (getline(macroDefFile, line)) {
        // Check if line contains "MACRO" to start a new macro definition
        if (line.find("MACRO") != string::npos) {
            stringstream ss(line);
            string macroKeyword, macroName, param;
            vector<string> params;

            ss >> macroKeyword >> macroName;  // Read "MACRO" and the actual macro name

            // Read parameters (if any)
            while (getline(ss, param, ',')) {
                param.erase(remove(param.begin(), param.end(), ' '), param.end());
                params.push_back(param);
            }

            // Add entry to MNT with correct macro name
            MNT.push_back({macroName, mdtIndex, static_cast<int>(params.size())});

            // Add macro definition line to MDT
            MDT.push_back({mdtIndex++, line});

            // Read the following lines until "MEND"
            while (getline(macroDefFile, line) && line.find("MEND") == string::npos) {
                MDT.push_back({mdtIndex++, line});
            }
            // Add "MEND" to MDT
            MDT.push_back({mdtIndex++, line});
        }
    }
    macroDefFile.close();
}

// Function to display tables
void displayTables() {
    // Display MNT
    cout << "\nMacro Name Table (MNT):\n";
    cout << "Macro Name\tMDT Index\tNumber of Parameters\n";
    for (const auto& entry : MNT) {
        cout << entry.macroName << "\t\t" << entry.mdtIndex << "\t\t" << entry.numParams << endl;
    }

    // Display MDT
    cout << "\nMacro Definition Table (MDT):\n";
    cout << "Index\tDefinition\n";
    for (const auto& entry : MDT) {
        cout << entry.index << "\t" << entry.definition << endl;
    }
}

// Function to expand a macro call
void expandMacro(const string& macroName, const vector<string>& actualParams) {
    // Find the macro in MNT
    auto it = find_if(MNT.begin(), MNT.end(), [&](const MNTEntry& entry) {
        return entry.macroName == macroName;
    });

    if (it == MNT.end()) {
        cout << "Macro " << macroName << " not found!\n";
        return;
    }

    // Load actual parameters into APTAB
    for (int i = 0; i < it->numParams; i++) {
        APTAB["&ARG" + to_string(i + 1)] = actualParams[i];
    }

    // Expand the macro using MDT
    cout << "\nExpanded Code for " << macroName << ":\n";
    for (int i = it->mdtIndex + 1; i < MDT.size(); i++) {
        string line = MDT[i].definition;
        if (line.find("MEND") != string::npos) break;  // Stop at the next macro definition

        // Replace formal parameters with actual parameters
        for (const auto& param : APTAB) {
            size_t pos;
            while ((pos = line.find(param.first)) != string::npos) {
                line.replace(pos, param.first.length(), param.second);
            }
        }
        cout << line << endl;
    }

    // Clear APTAB for the next macro call
    APTAB.clear();
}

// Function to process macro calls from the input file
void processMacroCalls() {
    ifstream macroCallsFile("macro_calls.txt");
    string line;

    // Read each line containing a macro call
    while (getline(macroCallsFile, line)) {
        stringstream ss(line);
        string macroName, param;
        vector<string> actualParams;

        // Read macro name and actual parameters
        ss >> macroName;
        while (getline(ss, param, ',')) {
            param.erase(remove(param.begin(), param.end(), ' '), param.end());
            actualParams.push_back(param);
        }

        // Expand the macro
        expandMacro(macroName, actualParams);
    }
    macroCallsFile.close();
}

int main() {
    // Initialize tables with data from the macro definitions file
    initializeTables();
    displayTables();

    // Process and expand macro calls from the macro calls file
    processMacroCalls();

    return 0;
}
