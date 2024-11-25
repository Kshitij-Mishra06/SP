#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>

using namespace std;

// Data structures
map<string, int> symbolTable;
map<string, int> literalTable;
vector<string> intermediateCode;
int locationCounter = 0;
vector<string> machineCode;

// Mnemonic Opcode Table (MOT)
map<string, string> is = {{"MOVER", "04"}, {"ADD", "01"}, {"SUB", "02"}, {"MUL", "03"}, {"DIV", "04"}, {"JMP", "05"}, {"CMP", "06"}};
map<string, string> reg = {{"AREG", "01"}, {"BREG", "02"}, {"CREG", "03"}, {"DREG", "04"}};

// Function to process the intermediate code and generate machine code
void processIntermediateCode() {
    for (const string& line : intermediateCode) {
        stringstream ss(line);
        string code;
        vector<string> tokens;

        while (ss >> code) {
            tokens.push_back(code);
        }

        if (tokens.empty()) continue;

        // End of Program
        if (tokens[0] == "(AD,02)") {
            break;
        }

        // Process IS Instructions
        if (tokens[0].substr(1, 2) == "IS") {
            string opcode = tokens[0].substr(4, 2);
            string regCode = reg[tokens[1].substr(3, tokens[1].size() - 4)];
            string operandCode;

            // Extract and process operand code
            if (tokens[2].substr(1, 1) == "S") { // Symbol
                operandCode = to_string(symbolTable[tokens[2].substr(3, tokens[2].size() - 4)]);
            } else if (tokens[2].substr(1, 1) == "L") { // Literal
                operandCode = to_string(literalTable[tokens[2].substr(3, tokens[2].size() - 4)]);
            } else if (tokens[2].substr(1, 1) == "C") { // Constant
                operandCode = tokens[2].substr(3, tokens[2].size() - 4);
                // Ensure that operandCode is formatted correctly
                if (operandCode.length() < 2) operandCode = "0" + operandCode;
            } else {
                operandCode = "00"; // Default value if unknown type
            }

            // Format the machine code line
            string mc = to_string(locationCounter) + " " + opcode + " " + regCode + " " + operandCode;
            machineCode.push_back(mc);
            locationCounter++;
        } 

        // Process DL Instructions
        else if (tokens[0].substr(1, 2) == "DL") {
            if (tokens[1].substr(1, 2) == "01") { // DC
                string operand = tokens[2].substr(3, tokens[2].size() - 4);
                string mc = to_string(locationCounter) + " 00 00 " + operand;
                machineCode.push_back(mc);
                locationCounter++;
            }
        } 

        // Process AD Instructions
        else if (tokens[0].substr(1, 2) == "AD" && tokens[0].substr(4, 2) == "01") { // ORIGIN
            if (tokens[1].substr(1, 1) == "C") {
                locationCounter = stoi(tokens[1].substr(3, tokens[1].size() - 4));
            } else if (tokens[1].substr(1, 1) == "S") {
                locationCounter = symbolTable[tokens[1].substr(3, tokens[1].size() - 4)];
            }
        }
    }
}

int main() {
    // Read the Intermediate Code from IC.txt
    ifstream icFile("IC.txt");
    string line;

    if (icFile.is_open()) {
        while (getline(icFile, line)) {
            intermediateCode.push_back(line);
        }
        icFile.close();
    } else {
        cerr << "Unable to open file IC.txt" << endl;
        return 1;
    }

    // Read the Symbol Table from SYMTAB.txt
    ifstream symFile("SYMTAB.txt");

    if (symFile.is_open()) {
        while (getline(symFile, line)) {
            stringstream ss(line);
            string symbol;
            int address;
            ss >> symbol >> address;
            symbolTable[symbol] = address;
        }
        symFile.close();
    } else {
        cerr << "Unable to open file SYMTAB.txt" << endl;
        return 1;
    }

    // Read the Literal Table from LITTAB.txt
    ifstream litFile("LITTAB.txt");

    if (litFile.is_open()) {
        while (getline(litFile, line)) {
            stringstream ss(line);
            string literal;
            int address;
            ss >> literal >> address;
            literalTable[literal] = address;
        }
        litFile.close();
    } else {
        cerr << "Unable to open file LITTAB.txt" << endl;
        return 1;
    }

    // Process the Intermediate Code to generate Machine Code
    processIntermediateCode();

    // Write the results to machine_code.txt
    ofstream mcFile("machine_code.txt");
    if (mcFile.is_open()) {
        mcFile << "Machine Code:" << endl;
        for (const string& mc : machineCode) {
            mcFile << mc << endl;
        }
        mcFile.close();
    } else {
        cerr << "Unable to open file machine_code.txt" << endl;
        return 1;
    }

    return 0;
}
