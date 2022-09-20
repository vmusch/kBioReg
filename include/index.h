//
// Created by Remy Schwab on 13.09.22.
//

#ifndef KBIOREG_INDEX_H
#define KBIOREG_INDEX_H


#include "utils.h"
#include "arg_parse.h"

#include <seqan3/core/debug_stream.hpp>
#include <seqan3/search/views/kmer_hash.hpp>
#include <seqan3/alphabet/nucleotide/all.hpp>
#include <seqan3/search/dream_index/interleaved_bloom_filter.hpp>


uint8_t parse_reference(std::filesystem::path &ref_file, record_list &refs);
void create_index(seqan3::interleaved_bloom_filter<seqan3::data_layout::uncompressed> &ibf, record_list &refs, uint8_t &bin_count, uint8_t k);

template <class IBFType>
void store_ibf(IBFType const & ibf, std::filesystem::path opath)
{
    std::ofstream os{opath, std::ios::binary};
    cereal::BinaryOutputArchive oarchive{os};
    oarchive(seqan3::interleaved_bloom_filter(ibf));
}

#endif //KBIOREG_INDEX_H
