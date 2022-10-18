//
// Created by Remy Schwab on 12.10.22.
//

#pragma once

#include <seqan3/search/dream_index/interleaved_bloom_filter.hpp>


namespace index_structure
{

    using ibf = seqan3::interleaved_bloom_filter<seqan3::data_layout::uncompressed>;
    using ibf_compressed = seqan3::interleaved_bloom_filter<seqan3::data_layout::compressed>;

    template <typename return_t, typename input_t>
    concept compressible_from = (std::same_as<return_t, ibf_compressed> && std::same_as<input_t, ibf>);

} // namespace index_structure

template <typename data_t = index_structure::ibf>
class IndexStructure
{

private:
    uint8_t bin_count_;
    uint32_t bin_size_{};
    uint8_t hash_count_{};

    seqan3::interleaved_bloom_filter<seqan3::data_layout::uncompressed> ibf_;

public:
    static constexpr seqan3::data_layout data_layout_mode = data_t::data_layout_mode;

    uint8_t k_{};

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
            static_assert(data_layout_mode == seqan3::data_layout::uncompressed);
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

    void emplace(uint64_t val, uint8_t idx)
    {
        ibf_.emplace(val, seqan3::bin_index{idx});
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
