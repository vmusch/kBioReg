//
// Created by Remy Schwab on 20.09.22.
//

#ifndef KBIOREG_QUERY_H
#define KBIOREG_QUERY_H

#include "utils.h"
#include "index.h"
#include "arg_parse.h"
#include "korotkov_nfa.h"
#include "graphMaker.h"
#include "nfa_pointer.h"


using bitvector = seqan3::interleaved_bloom_filter<seqan3::data_layout::uncompressed>::membership_agent::binning_bitvector;

bitvector query_ibf(IndexStructure &ibf, std::vector<std::pair<std::string, uint64_t>> &path);

bitvector drive_query(const query_arguments & cmd_args);

bitvector drive_query_benchmark(const query_arguments & cmd_args, std::fstream &benchmark_table);

std::string collapse_kmers(uint8_t const &k, std::vector<std::pair<std::string, uint64_t>> const &kvec);

#endif //KBIOREG_QUERY_H
