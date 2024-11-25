#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <regex>

using namespace std;

// Data structures
map<string, int> symbolTable;
vector<string> intermediateCode;
int locationCounter = 0;
vector<string> errors;

// Mnemonic Opcode Table (MOT)
map<string, string> ad = {{"START", "01"}, {"END", "02"}, {"ORIGIN", "03"}, {"EQU", "04"}, {"LTORG", "05"}};
map<string, string> is = {{"MOVER", "04"}, {"ADD", "01"}, {"SUB", "02"}, {"MUL", "03"}, {"DIV", "04"}, {"JMP", "05"}, {"CMP", "06"}};
map<string, string> dl = {{"DC", "01"}, {"DS", "02"}};

// Function to check if a string is a number
bool isNumber(const string& str) {
    return regex_match(str, regex("[0-9]+"));
}

// Function to process the ALP
void processAlp(const vector<string>& alp) {
    for (const string& line : alp) {
        stringstream ss(line);
        vector<string> tokens;
        string token;
        while (ss >> token) {
            tokens.push_back(token);
        }

        if (tokens.empty()) continue;

        if (ad.find(tokens[0]) != ad.end()) {
            if (tokens[0] == "START") {
                if (tokens.size() != 2 || !isNumber(tokens[1])) {
                    errors.push_back("Invalid START instruction format: " + line);
                    continue;
                }
                locationCounter = stoi(tokens[1]);
                intermediateCode.push_back("(AD," + ad[tokens[0]] + ") (C," + tokens[1] + ")");
            } else if (tokens[0] == "END") {
                intermediateCode.push_back("(AD," + ad[tokens[0]] + ")");
            } else if (tokens[0] == "ORIGIN") {
                // Assuming ORIGIN instruction is in the format ORIGIN <address>
                if (tokens.size() != 2) {
                    errors.push_back("Invalid ORIGIN instruction format: " + line);
                    continue;
                }
                // Handle symbol or numeric address
                if (isNumber(tokens[1])) {
                    locationCounter = stoi(tokens[1]);
                } else if (symbolTable.find(tokens[1]) != symbolTable.end()) {
                    locationCounter = symbolTable[tokens[1]];
                } else {
                    errors.push_back("Undefined symbol in ORIGIN instruction: " + tokens[1]);
                    continue;
                }
                intermediateCode.push_back("(AD," + ad[tokens[0]] + ") (C," + tokens[1] + ")");
            } else if (tokens[0] == "EQU") {
                // Assuming EQU instruction is in the format <symbol> EQU <address/value>
                if (tokens.size() != 3) {
                    errors.push_back("Invalid EQU instruction format: " + line);
                    continue;
                }
                if (isNumber(tokens[2])) {
                    symbolTable[tokens[1]] = stoi(tokens[2]);
                } else if (symbolTable.find(tokens[2]) != symbolTable.end()) {
                    symbolTable[tokens[1]] = symbolTable[tokens[2]];
                } else {
                    errors.push_back("Undefined symbol in EQU instruction: " + tokens[2]);
                    continue;
                }
                intermediateCode.push_back("(AD," + ad[tokens[0]] + ") (S," + tokens[1] + ") (C," + tokens[2] + ")");
            } else if (tokens[0] == "LTORG") {
                // Handle LTORG logic if required (not covered in the initial code)
                intermediateCode.push_back("(AD," + ad[tokens[0]] + ")");
            } else {
                errors.push_back("Unsupported AD instruction: " + tokens[0]);
            }
        } else if (is.find(tokens[0]) != is.end()) {
            if (tokens.size() < 3) {
                errors.push_back("Invalid IS instruction format: " + line);
                continue;
            }
            // Assuming instructions are in the format: IS reg, value
            string reg = tokens[1];
            string operand = tokens[2];
            if (!isNumber(operand) && symbolTable.find(operand) == symbolTable.end()) {
                errors.push_back("Undefined symbol: " + operand);
                continue;
            }
            intermediateCode.push_back("(IS," + is[tokens[0]] + ") (R," + reg + ") (C," + operand + ")");
            locationCounter++;
        } else if (dl.find(tokens[1]) != dl.end()) {
            symbolTable[tokens[0]] = locationCounter;
            intermediateCode.push_back("(DL," + dl[tokens[1]] + ") (C," + tokens[2] + ")");
            if (tokens[1] == "DC") {
                locationCounter++;
            } else if (tokens[1] == "DS") {
                locationCounter += stoi(tokens[2]);
            }
        } else {
            errors.push_back("Invalid instruction: " + line);
        }
    }
}

int main() {
    // Read the input assembly language program from input.txt
    ifstream inputFile("input.txt");
    vector<string> inputAlp;
    string line;

    if (inputFile.is_open()) {
        while (getline(inputFile, line)) {
            inputAlp.push_back(line);
        }
        inputFile.close();
    } else {
        cerr << "Unable to open file input.txt" << endl;
        return 1;
    }

    // Process the ALP
    processAlp(inputAlp);

    // Write results to output.txt
    ofstream outputFile("output.txt");
    if (outputFile.is_open()) {
        // Print Symbol Table
        outputFile << "Symbol Table:" << endl;
        for (const auto& entry : symbolTable) {
            outputFile << entry.first << " -> " << entry.second << endl;
        }

        // Print Intermediate Code
        outputFile << "\nIntermediate Code:" << endl;
        for (const string& ic : intermediateCode) {
            outputFile << ic << endl;
        }

        // Print Errors
        if (!errors.empty()) {
            outputFile << "\nErrors:" << endl;
            for (const string& error : errors) {
                outputFile << error << endl;
            }
        }

        outputFile.close();
    } else {
        cerr << "Unable to open file output.txt" << endl;
        return 1;
    }

    return 0;
}
