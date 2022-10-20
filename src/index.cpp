//
// Created by Remy Schwab on 13.09.22.
//

#include "index.h"


uint8_t parse_reference(std::filesystem::path &ref_file, record_list &refs)
{
    uint8_t bin_count = 0;
    seqan3::sequence_file_input fin{ref_file};
    record_pair record_desc;
    for(auto & record: fin) {
        bin_count++;
        record_desc = std::make_pair(record.id(), record.sequence());
        refs.push_back(record_desc);
    }
    return bin_count;
}


IndexStructure create_index(record_list &refs, uint8_t &bin_count, index_arguments args)
{
    uint8_t k = args.k;
    IndexStructure ibf(k, bin_count, args.bin_size, args.hash_count);

    auto hash_adaptor = seqan3::views::kmer_hash(seqan3::ungapped{k});
    for (uint8_t i = 0; i < bin_count; i++)
        for (auto && value : refs[i].second | hash_adaptor)
        {
            ibf.emplace(value, i);
        }
    return ibf;
}

