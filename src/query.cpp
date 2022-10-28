//
// Created by Remy Schwab on 20.09.22.
//

#include "query.h"



void query_ibf(IndexStructure &ibf, std::vector<std::pair<std::string, uint64_t>> &path)
{
    seqan3::debug_stream << path << ":::";
    auto agent = ibf.getIBF().membership_agent();
    std::vector<uint32_t> counter;
    counter.assign(ibf.getBinCount(), 0);
    for (auto && kmer : path)
        std::transform (counter.begin(), counter.end(),
                        agent.bulk_contains(kmer.second).begin(),
                        counter.begin(), std::plus<int>());
    seqan3::debug_stream << counter << std::endl;
}


std::string collapse_kmers(uint8_t const &k, std::vector<std::pair<std::string, uint64_t>> const &kvec)
{
    std::string assembly = kvec[0].first;
    for (size_t i = 1; i < kvec.size(); i++)
        assembly += kvec[i].first.substr(k-1, 1);
    return assembly;
}
