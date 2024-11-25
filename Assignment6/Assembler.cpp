#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <map>
#include <vector>
#include <sstream>

// Define token types
enum TokenType {
    KEYWORD, IDENTIFIER, OPERATOR, LITERAL, UNKNOWN
};

// Structure for tokens
struct Token {
    int line;
    std::string lexeme;
    TokenType type;
    std::string value;
};

// Keywords for subset of "C"
std::map<std::string, TokenType> keywords = {
    {"int", KEYWORD}, {"float", KEYWORD}, {"if", KEYWORD}, {"else", KEYWORD}
};

// Function to classify and tokenize
TokenType classify(const std::string& lexeme) {
    if (keywords.find(lexeme) != keywords.end()) return KEYWORD;
    if (std::regex_match(lexeme, std::regex("[a-zA-Z_][a-zA-Z0-9_]*"))) return IDENTIFIER;
    if (std::regex_match(lexeme, std::regex("[+-/*=<>!]"))) return OPERATOR;
    if (std::regex_match(lexeme, std::regex("[0-9]+"))) return LITERAL;
    return UNKNOWN;
}

// Main function to read file, tokenize, and output
int main() {
    std::ifstream file("input.txt"); // Input C program file
    if (!file) {
        std::cerr << "Error opening input file.\n";
        return 1;
    }

    std::vector<Token> tokens;
    std::string line;
    int lineNo = 1;

    while (std::getline(file, line)) {
        std::istringstream stream(line);
        std::string lexeme;
        while (stream >> lexeme) {
            Token token;
            token.line = lineNo;
            token.lexeme = lexeme;
            token.type = classify(lexeme);
            token.value = lexeme; // For simplicity, use lexeme as value
            tokens.push_back(token);
        }
        lineNo++;
    }

    // Output tokens
    std::cout << "Line No\tLexeme\tToken\tToken_Value\n";
    for (const auto& token : tokens) {
        std::cout << token.line << "\t" << token.lexeme << "\t" 
                  << token.type << "\t" << token.value << "\n";
    }
    file.close();
    return 0;
}
