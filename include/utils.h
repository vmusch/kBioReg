#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

std::vector<char> getAlphabet(const std::string& regex);

//soll alle nötigen qgramme finden
std::vector<std::string> getQgramAlphabet(const std::vector<std::vector<std::string>>& matrix);

//uint shiftValue(const uint& input);

//uint32_t getHash(const std::vector<char>& alphabet, const std::string& input, const uint& sValue);

void matrixTotxt(const std::vector<std::vector<std::string>>& matrix, std::string& filename);

void matrixTXT(const std::vector<std::vector<std::string>>& matrix,
                const std::vector<char>& alphabet);

#endif