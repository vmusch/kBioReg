#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <math.h>
#include <fstream>
#include <string>
#include <stack>
#include <vector>
#include <filesystem>

#include <seqan3/core/debug_stream.hpp>
#include <seqan3/alphabet/nucleotide/all.hpp>
#include <seqan3/io/sequence_file/all.hpp>
#include <seqan3/search/dream_index/interleaved_bloom_filter.hpp>


// Types
using record_pair = typename std::pair<std::string, seqan3::dna5_vector>;
using record_list = typename std::vector<record_pair>;


std::vector<char> getAlphabet(const std::string& regex);

//soll alle nötigen qgramme finden
std::vector<std::string> getQgramAlphabet(const std::vector<std::vector<std::string>>& matrix);


//uint shiftValue(const uint& input);


//uint32_t getHash(const std::vector<char>& alphabet, const std::string& input, const uint& sValue);


void matrixTotxt(const std::vector<std::vector<std::string>>& matrix, std::string& filename);


void matrixTXT(const std::vector<std::vector<std::string>>& matrix,
                const std::vector<char>& alphabet);


template <class IBFType>
void store_ibf(IBFType const & ibf, std::filesystem::path opath)
{
    std::ofstream os{opath, std::ios::binary};
    cereal::BinaryOutputArchive oarchive{os};
    oarchive(seqan3::interleaved_bloom_filter(ibf));
}


template <class IBFType>
void load_ibf(IBFType & ibf, std::filesystem::path ipath)
{
    std::ifstream is{ipath, std::ios::binary};
    cereal::BinaryInputArchive iarchive{is};
    iarchive(ibf);
}

#endif