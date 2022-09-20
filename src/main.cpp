#include "utils.h"
#include "arg_parse.h"
#include "index.h"
#include "korotkov_nfa.h"
#include "graphMaker.h"
#include "nfa_pointer.h"


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
    seqan3::debug_stream << "Reading Index from Disk... ";
    seqan3::interleaved_bloom_filter<seqan3::data_layout::uncompressed> ibf{seqan3::bin_count{11},
                                                                            seqan3::bin_size{8192},
                                                                            seqan3::hash_function_count{2}};
    load_ibf(ibf, cmd_args.idx);
    seqan3::debug_stream << "DONE" << std::endl;

    // Evaluate and search for Regular Expression
    seqan3::debug_stream << "Querying" << std::endl;
    int qlength = cmd_args.k;
    std::string query = cmd_args.query;
    std::vector<char> a = getAlphabet(query);
    State* nfa = post2nfaE(query);
    std::vector<kState *> knfa = nfa2knfa(nfa, qlength);
    deleteGraph(nfa);
    std::vector<std::vector<std::string>> matrix{};
    for(auto i : knfa)
    {
        dfs(i,matrix);
    }
    uMatrix(matrix);
//    for(auto e : a)
//    {
//        std::cout<<e<<" ";
//    }
//    std::cout<<"\n";
    for(auto i : matrix)
    {
        for(auto j : i)
        {
            std::cout<<j<<" ";
        }
        std::cout<<"\n";
    }
}


int main(int argc, char *argv[])
{
    seqan3::argument_parser top_level_parser{"kbioreg", argc, argv,
                                             seqan3::update_notifications::off,
                                             {"index", "query"}};
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