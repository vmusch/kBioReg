//
// Created by Remy Schwab on 13.09.22.
//

#include "index.h"

void drive_index(const index_arguments &cmd_args)
{
    std::filesystem::path acid_lib = cmd_args.acid_lib;
    if(cmd_args.molecule == "na")
    {
        record_list<seqan3::dna5_vector> records;
        uint32_t bin_count = parse_reference_na(acid_lib, records);
        
        seqan3::debug_stream << "Indexing " << bin_count << " genomes... ";
        IndexStructure ibf = create_index(records, bin_count, cmd_args);
        seqan3::debug_stream << "DONE" << std::endl;

        seqan3::debug_stream << "Writing to disk... ";
        std::filesystem::path output_path{cmd_args.ofile+".ibf"};
        store_ibf(ibf, output_path);
        seqan3::debug_stream << "DONE" << std::endl;
    } else
    {
        record_list<seqan3::aa27_vector> records;
        uint32_t bin_count = parse_reference_aa(acid_lib, records);
        
        seqan3::debug_stream << "Indexing " << bin_count << " genomes... ";
        IndexStructure ibf = create_index(records, bin_count, cmd_args);
        seqan3::debug_stream << "DONE" << std::endl;

        seqan3::debug_stream << "Writing to disk... ";
        std::filesystem::path output_path{cmd_args.ofile+".ibf"};
        store_ibf(ibf, output_path);
        seqan3::debug_stream << "DONE" << std::endl;
    }
}
