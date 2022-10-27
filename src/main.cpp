#include "utils.h"
#include "arg_parse.h"
#include "index.h"
#include "korotkov_nfa.h"
#include "graphMaker.h"
#include "nfa_pointer.h"
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
    record_list records;
    std::filesystem::path acid_lib = cmd_args.acid_lib;
    uint8_t bin_count = parse_reference(acid_lib, records);
    // Create IBF with one BF for each contig in library
    seqan3::interleaved_bloom_filter<seqan3::data_layout::uncompressed> ibf{seqan3::bin_count{bin_count},
                                         seqan3::bin_size{cmd_args.bin_size},
                                         seqan3::hash_function_count{cmd_args.hash_count}};
    seqan3::debug_stream << "Indexing " << bin_count << " genomes... ";
    create_index(ibf, records, bin_count, cmd_args.k);
    seqan3::debug_stream << "DONE" << std::endl;

    seqan3::debug_stream << "Writing to disk... ";
    store_ibf(ibf, "index.ibf");
    seqan3::debug_stream << "DONE" << std::endl;
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

    // Load index from disk
    seqan3::debug_stream << "Reading Index from Disk... ";
    seqan3::interleaved_bloom_filter<seqan3::data_layout::uncompressed> ibf{};
    load_ibf(ibf, cmd_args.idx);
    seqan3::debug_stream << "DONE" << std::endl;

    // Evaluate and search for Regular Expression
    seqan3::debug_stream << "Querying:" << std::endl;
    uint8_t qlength = cmd_args.k;
    std::string query = cmd_args.query;
    std::vector<char> a = getAlphabet(query);

    // Postfix to Thompson NFA
    seqan3::debug_stream << "   - Constructing Thompson NFA from RegEx... ";
    State* nfa = post2nfaE(query);
    seqan3::debug_stream << "DONE" << std::endl;

    // Thompson NFA to Korotkov NFA
    seqan3::debug_stream << "   - Construction kNFA from Thompson NFA... ";
    std::vector<kState *> knfa = nfa2knfa(nfa, qlength);
    seqan3::debug_stream << "DONE" << std::endl;
    //deleteGraph(nfa);

    // Create kmer path matrix from kNFA
    seqan3::debug_stream << "   - Computing kmer path matrix from kNFA... ";
    std::vector<std::vector<std::string>> matrix{};
    for(auto i : knfa)
    {
        dfs(i,matrix);
    }
    uMatrix(matrix);
    seqan3::debug_stream << "DONE" << std::endl;

    // make bitvecs
    seqan3::debug_stream << "   - compute bit... ";
    
    seqan3::debug_stream << std::endl;


    // Search kmer paths in index
    seqan3::debug_stream << "   - Search kmers in index... ";
    seqan3::debug_stream << std::endl;

    // A vector to store kNFA paths as hashed constituent kmers eg one element would be. <78, 45, 83...> <--> AC, CG, GT
    auto hash_adaptor = seqan3::views::kmer_hash(seqan3::ungapped{qlength});
    std::vector<std::vector<std::pair<std::string, uint64_t>>> paths_vector;
    for(auto i : matrix)
    {
        std::vector<std::pair<std::string, uint64_t>> hash_vector;
        for(auto j : i)
        {
            std::vector<seqan3::dna5> acid_vec = convertStringToDNA(j);
            auto digest = acid_vec | hash_adaptor;
            // Create a vector of kmer hashes that correspond
            hash_vector.push_back(std::make_pair(j, digest[0]));
        }
        paths_vector.push_back(hash_vector);
    }
    for (auto path : paths_vector)
    {
        seqan3::debug_stream << collapse_kmers(qlength, path) << ":::";
        query_ibf(ibf, path);
    }
    seqan3::debug_stream << "DONE" << std::endl;
}

void run_wordFile(seqan3::argument_parser &parser)
{
    query_arguments cmd_args{};
    initialise_query_parser(parser, cmd_args);
    try
    {
        parser.parse(); // trigger command line parsing
    }
    catch (seqan3::argument_parser_error const & ext) // catch user errors
    {
        seqan3::debug_stream << "[Error] " << ext.what() << "\n"; // customise your error message
        return ;
    }
    
    seqan3::debug_stream << "Querying:" << std::endl;
    uint8_t number = cmd_args.k;
    std::string query = cmd_args.query;

    // Postfix to Thompson NFA
    seqan3::debug_stream << "   - Constructing Thompson NFA from RegEx... ";
    State* nfa = post2nfaE(query);
    seqan3::debug_stream << "DONE" << std::endl;

    // Make a file
    seqan3::debug_stream << "   - Constructing words.txt ";
    std::fstream file;
    file.open(cmd_args.idx, std::ios::out);
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
    uint8_t qlength = cmd_args.k;   
    uint8_t number = cmd_args.w; //number of words
    std::string query = cmd_args.query; //regex
    std::string bashC ="../bash_scripts/simulate.sh ";
    bashC += cmd_args.p;
    
    // Postfix to Thompson NFA
    seqan3::debug_stream << "   - Constructing Thompson NFA from RegEx... ";
    State* nfa = post2nfaE(query);
    seqan3::debug_stream << "DONE" << std::endl;

    // Make a file
    seqan3::debug_stream << "   - Constructing words.txt ";
    std::fstream file;
    file.open(cmd_args.idx, std::ios::out);
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
    seqan3::debug_stream << "=======Genome Done========" << std::endl;
    
    seqan3::debug_stream << "=======start search========" << std::endl;

    seqan3::argument_parser indexparser = {};
    seqan3::argument_parser query parser = {};
    //indexing
    run_index();
    //querry
    run_query();
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
                                             {"index", "query", "words", "benchmark"}};
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
    else if (sub_parser.info.app_name == std::string_view{"kbioreg-words"})
        run_wordFile(sub_parser);
    else if (sub_parser.info.app_name == std::string_view{"kbioreg-benchmark"})
        run_benchmark(sub_parser);
    else
        std::cout << "Unhandled subparser named " << sub_parser.info.app_name << '\n';

    // Return error for bad invocation
//    if(argc > 5 || argc < 3)
//    {
//    std::cerr<<"error";
//    return -1;
//    }
//
//    int qlength = std::stoi(argv[2]);
//    std::vector<char> a = getAlphabet(argv[1]);
//
//    State* nfa = post2nfaE(argv[1]);
//    std::vector<kState *> knfa = nfa2knfa(nfa, qlength);
//
//    deleteGraph(nfa);
//
//    std::vector<std::vector<std::string>> matrix{};
//    for(auto i : knfa)
//    {
//      dfs(i,matrix);
//    }
//    uMatrix(matrix);
//    if(argc >= 4)
//    {
//      std::string matrixfile = argv[3];
//      matrixTotxt(matrix, matrixfile);
//    }
//    else
//    {
//      std::vector<char> a = getAlphabet(argv[1]);
//      for(auto e : a)
//      {
//        std::cout<<e<<" ";
//      }
//      std::cout<<"\n";
//      for(auto i : matrix)
//      {
//        for(auto j : i)
//        {
//          std::cout<<j<<" ";
//        }
//        std::cout<<"\n";
//      }
//    }
//    if(argc == 5)
//    {
//      std::string dotfile = argv[4];
//      dotfile += ".dot";
//      printGraph(knfa, dotfile);
//    }
  return 0;
}