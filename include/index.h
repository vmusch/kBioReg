//
// Created by Remy Schwab on 13.09.22.
//

#pragma once


#include "utils.h"
#include "arg_parse.h"

#include <seqan3/core/debug_stream.hpp>
#include <seqan3/search/views/kmer_hash.hpp>
#include <seqan3/alphabet/nucleotide/all.hpp>



namespace index_structure
{

    using ibf = seqan3::interleaved_bloom_filter<seqan3::data_layout::uncompressed>;
    using ibf_compressed = seqan3::interleaved_bloom_filter<seqan3::data_layout::compressed>;

    template <typename return_t, typename input_t>
    concept compressible_from = (std::same_as<return_t, ibf_compressed> && std::same_as<input_t, ibf>);

} // namespace index_structure

class IndexStructure
{

private:
    uint8_t bin_count_;
    uint32_t bin_size_{};
    uint8_t hash_count_{};
    seqan3::interleaved_bloom_filter<seqan3::data_layout::uncompressed> ibf_;

public:

    uint8_t k_;

    IndexStructure() = default;

    explicit IndexStructure(uint8_t k,
                            uint8_t bc,
                            uint32_t bs,
                            uint8_t hc) :
            k_{k},
            bin_count_{bc},
            bin_size_{bs},
            hash_count_{hc},
            ibf_{seqan3::bin_count{bin_count_},
                 seqan3::bin_size{bin_size_},
                 seqan3::hash_function_count{hash_count_}}
    {
        //static_assert(data_layout_mode == seqan3::data_layout::uncompressed);
    }

    uint8_t getBinCount()
    {
        return bin_count_;
    }

    uint32_t getBinSize()
    {
        return bin_size_;
    }

    uint8_t getHashCount()
    {
        return hash_count_;
    }

   seqan3::interleaved_bloom_filter<seqan3::data_layout::uncompressed> getIBF()
   {
    return ibf_;
   } 

    void emplace(uint64_t val, uint8_t idx)
    {
        ibf_.emplace(val, seqan3::bin_index{idx});
    }

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(bin_count_, bin_size_, hash_count_, k_);
    }
};

template <class IndexStructure>
void store_ibf(IndexStructure const & ibf, std::filesystem::path opath)
{
    std::ofstream os{opath, std::ios::binary};
    cereal::BinaryOutputArchive oarchive{os};
    oarchive(ibf);
}

template <class IndexStructure>
void load_ibf(IndexStructure & ibf, std::filesystem::path ipath)
{
    std::ifstream is{ipath, std::ios::binary};
    cereal::BinaryInputArchive iarchive{is};
    iarchive(ibf);
}

uint8_t parse_reference(std::filesystem::path &ref_file, record_list &refs);

IndexStructure create_index(record_list &refs, uint8_t &bin_count, index_arguments args);
