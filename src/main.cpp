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
    cmd_args.query = translate(cmd_args.regex);
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

    cmd_args.query = translate(cmd_args.regex);
    // cmd_args.query = cmd_args.regex;

    seqan3::debug_stream << "=======Start Benchmark========" << std::endl;
    seqan3::debug_stream << "=======Make Genome========" << std::endl;
    uint8_t number = cmd_args.w; //number of words
    //std::string query = cmd_args.query; //regex
    std::string bashC ="../bash_scripts/simulate.sh ";
    bashC += cmd_args.p;
    
    // Postfix to Thompson NFA
    seqan3::debug_stream <<"   - Constructing Thompson NFA from RegEx... ";
    State* nfa = post2nfaE(cmd_args.query);
    seqan3::debug_stream << "DONE" << std::endl;

    // Make a file
    seqan3::debug_stream << "   - Write words.txt ";
    std::fstream file;
    file.open("words.txt", std::ios::out);
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
    query.regex = cmd_args.regex;
    query.query = cmd_args.query;
    query.graph = cmd_args.graph;

    seqan3::debug_stream << "DONE" << std::endl;


    seqan3::debug_stream << "=======Genome Done========" << std::endl;
    
    seqan3::debug_stream << "=======start search========" << std::endl;
    double t1,t2;
    //seqan3::argument_parser query_parser = {};
    //indexing ./bin/kbioreg benchmark -k 4 -w 30 -p 10 words.txt "AC.G+.T."
    t1 = omp_get_wtime();
    drive_index(index);
    t2 = omp_get_wtime();
    seqan3::debug_stream << "[INDEXING TIME]: " << t2-t1 << std::endl;
    //querry
    t1 = omp_get_wtime();
    std::string str = cmd_args.regex;
    std::regex reg(str);
    bitvector hits = drive_query(query);
    for(size_t i = 0; i < 64; i++) // Bin Count hardcoded here
    {
        if(hits[i])
        {
            std::stringstream bin_file_stream;
            std::string bin_file;
            if(i < 10)
            {
                bin_file_stream << "bin_0"<<i<< ".fa";
                bin_file = bin_file_stream.str();
            } else
            {
                bin_file_stream << "bin_"<<i<< ".fa";
                bin_file = bin_file_stream.str();
            }
            std::string filepath = "64/bins/"+bin_file;
            matches(stream_as_string(filepath), reg, "kbioreg_search.txt");
        }
    }
    t2 = omp_get_wtime();
    seqan3::debug_stream << "[KBIOREG QUERY TIME]: " << t2-t1 << std::endl;
    
    // Standard library RegEx Searc
    t1 = omp_get_wtime();
    matches(stream_as_string("64/bins/all_bins.fa"), reg, "standard_search.txt");
    t2 = omp_get_wtime();
    seqan3::debug_stream<<"[STDREGEX TIME]: "<< t2-t1 << std::endl;
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
