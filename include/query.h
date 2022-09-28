//
// Created by Remy Schwab on 20.09.22.
//

#ifndef KBIOREG_QUERY_H
#define KBIOREG_QUERY_H

#include "utils.h"


void
query_ibf(seqan3::interleaved_bloom_filter<seqan3::data_layout::uncompressed> &ibf, std::vector<std::pair<std::string, uint64_t>> &path);


std::string
collapse_kmers(uint8_t const &k, std::vector<std::pair<std::string, uint64_t>> const &kvec);

#endif //KBIOREG_QUERY_H
