#include "utils.h"
#include "arg_parse.h"
#include "index.h"
#include "query.h"
#include <fstream>
#include <omp.h>
#include <stdlib.h>


void run_index(seqan3::argument_parser &parser)
{
    index_arguments cmd_args{};
    initialise_index_parser(parser, cmd_args);
    try
    {
        parser.parse();
    }
    catch (seqan3::argument_parser_error const & ext)
    {
        seqan3::debug_stream << "[Indexing Parser Error] " << ext.what() << "\n";
        return;
    }
    drive_index(cmd_args);
}


void run_index_no_parser(index_arguments &cmd_args)
{
    if(cmd_args.molecule == "na")
    {
        record_list<seqan3::dna5_vector> records;
        std::filesystem::path acid_lib = cmd_args.acid_lib;
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
        std::filesystem::path acid_lib = cmd_args.acid_lib;
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

void run_query(seqan3::argument_parser &parser)
{
    // Parse Arguments
    query_arguments cmd_args{};
    initialise_query_parser(parser, cmd_args);
    try
    {
        parser.parse();
    }
    catch (seqan3::argument_parser_error const & ext)
    {
        seqan3::debug_stream << "[Error kBioReg Query module " << ext.what() << "\n";
        return;
    }
    drive_query(cmd_args);
}

void run_benchmark(seqan3::argument_parser &parser)
{
    benchmark_arguments cmd_args{};
    initialise_benchmark_parser(parser, cmd_args);
    try
    {
        parser.parse(); // trigger command line parsing
    }
    catch (seqan3::argument_parser_error const & ext) // catch user errors
    {
        seqan3::debug_stream << "[Error] " << ext.what() << "\n"; // customise your error message
        return ;
    }
    seqan3::debug_stream << "=======Start Benchmark========" << std::endl;
    // double t1, t2;
    // t1 = omp_get_wtime();
    seqan3::debug_stream << "=======Make Genome========" << std::endl;
    
    seqan3::debug_stream << "Querying:" << std::endl; 
    uint8_t number = cmd_args.w; //number of words
    //std::string query = cmd_args.query; //regex
    std::string bashC ="../bash_scripts/simulate.sh ";
    bashC += cmd_args.p;
    
    // Postfix to Thompson NFA
    seqan3::debug_stream << "   - Constructing Thompson NFA from RegEx... ";
    State* nfa = post2nfaE(cmd_args.query);
    seqan3::debug_stream << "DONE" << std::endl;

    // Make a file
    seqan3::debug_stream << "   - Constructing words.txt ";
    std::fstream file;
    file.open("words.txt", std::ios::out);
    seqan3::debug_stream << "DONE" << std::endl;

    // write the file
    seqan3::debug_stream << "   - write word.txt ";
    while(number > 0)
    {
        
        std::string word = getRandomWord(nfa);
        file << word <<"\n" ;   
        number--;
    }
    file.close();
    seqan3::debug_stream << "DONE" << std::endl;

    // bash script
    seqan3::debug_stream << "   - start script ";
    system(bashC.c_str());
    seqan3::debug_stream << "DONE" << std::endl;

    //write parser
    seqan3::debug_stream << "   - write parser";

    index_arguments index{};
    index.k = cmd_args.k;
    index.t = cmd_args.t;
    index.ofile = cmd_args.ofile;
    index.acid_lib = cmd_args.acid_lib;
    index.molecule = cmd_args.molecule;
    index.bin_size = cmd_args.bin_size;
    index.hash_count = cmd_args.hash_count;

    query_arguments query{};
    query.idx = cmd_args.idx;
    query.query = cmd_args.query;

    seqan3::debug_stream << "DONE" << std::endl;


    seqan3::debug_stream << "=======Genome Done========" << std::endl;
    
    seqan3::debug_stream << "=======start search========" << std::endl;

    //seqan3::argument_parser query_parser = {};
    //indexing ./bin/kbioreg benchmark -k 4 -w 30 -p 10 words.txt "AC.G+.T."
    drive_index(index);
    //querry
    drive_query(query);
    //regexsearch c++

    //time + rate

    //only regex search c++

    //time rate
    //t2 = omp_get_wtime();
    //seqan3::debug_stream<<"Time: "<< t2-t1 << std::endl;
}

int main(int argc, char *argv[])
{
    seqan3::argument_parser top_level_parser{"kbioreg", argc, argv,
                                             seqan3::update_notifications::off,
                                             {"index", "query", "benchmark"}};
    top_level_parser.info.description.push_back("Index a NA|AA FASTA library or search a regular expression.");
    try
    {
        top_level_parser.parse(); // trigger command line parsing
    }
    catch (seqan3::argument_parser_error const & ext) // catch user errors
    {
        seqan3::debug_stream << "[Error] " << ext.what() << "\n"; // customise your error message
        return -1;
    }

    seqan3::argument_parser & sub_parser = top_level_parser.get_sub_parser(); // hold a reference to the sub_parser
    if (sub_parser.info.app_name == std::string_view{"kbioreg-index"})
        run_index(sub_parser);
    else if (sub_parser.info.app_name == std::string_view{"kbioreg-query"})
        run_query(sub_parser);
    else if (sub_parser.info.app_name == std::string_view{"kbioreg-benchmark"})
        run_benchmark(sub_parser);
    else
        std::cout << "Unhandled subparser named " << sub_parser.info.app_name << '\n';
  return 0;
}
