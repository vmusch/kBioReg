//
// Created by Remy Schwab on 20.09.22.
//

#include "query.h"



void query_ibf(IndexStructure &ibf, std::vector<std::pair<std::string, uint64_t>> &path)
{
    seqan3::debug_stream << path << ":::";
    
    seqan3::interleaved_bloom_filter<seqan3::data_layout::uncompressed>::membership_agent::binning_bitvector hit_vector{ibf.getBinCount()};
    std::fill(hit_vector.begin(), hit_vector.end(), true);
    
    auto && ibf_ref = ibf.getIBF();
    auto agent = ibf_ref.membership_agent();

    for (auto && kmer : path)
    {
        auto & result = agent.bulk_contains(kmer.second);
        hit_vector.raw_data() &= result.raw_data();
    }
    seqan3::debug_stream << hit_vector << std::endl;
}


std::string collapse_kmers(uint8_t const &k, std::vector<std::pair<std::string, uint64_t>> const &kvec)
{
    std::string assembly = kvec[0].first;
    for (size_t i = 1; i < kvec.size(); i++)
        assembly += kvec[i].first.substr(k-1, 1);
    return assembly;
}
