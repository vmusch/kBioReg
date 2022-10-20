//
// Created by Remy Schwab on 20.09.22.
//

#ifndef KBIOREG_QUERY_H
#define KBIOREG_QUERY_H

#include "utils.h"
#include "index.h"


void query_ibf(IndexStructure &ibf, std::vector<std::pair<std::string, uint64_t>> &path);
std::string collapse_kmers(uint8_t const &k, std::vector<std::pair<std::string, uint64_t>> const &kvec);

#endif //KBIOREG_QUERY_H
