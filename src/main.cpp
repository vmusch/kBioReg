#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <stack>
#include "korotkov_nfa.h"
#include "graphMaker.h"
#include "nfa_pointer.h"
#include "utils.h"
#include "arg_parse.h"
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/search/dream_index/interleaved_bloom_filter.hpp>


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
        seqan3::debug_stream << "[Error git pull] " << ext.what() << "\n";
        return;
    }
    seqan3::debug_stream << "Indexing" << std::endl;

    seqan3::interleaved_bloom_filter ibf{seqan3::bin_count{12u}, seqan3::bin_size{8192u}};
    ibf.emplace(126, seqan3::bin_index{0u});
    ibf.emplace(712, seqan3::bin_index{3u});
    ibf.emplace(237, seqan3::bin_index{9u});


    //Capture the result by reference to avoid copies.
    auto agent = ibf.membership_agent();
    auto & result = agent.bulk_contains(712);
    seqan3::debug_stream << result << '\n'; // prints [0,0,0,1,0,0,0,0,0,0,0,0]
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
        seqan3::debug_stream << "[Error git pull] " << ext.what() << "\n";
        return;
    }
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
    for(auto e : a)
    {
        std::cout<<e<<" ";
    }
    std::cout<<"\n";
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






//   std::string regex;
//   int qlength = 0;
//   std::cout<<"Enter Regex in RPN:"<<"\n";
//   std::cin>>regex;
//   std::cout<<"Enter qGram length:"<<"\n";
//   std::cin>>qlength;
//   State* nfa = post2nfaE(regex);

//   std::vector<kState *> knfa = nfa2knfa(nfa, qlength);

//   std::vector<char> a = getAlphabet(regex);
//   for(auto e : a)
//   {
//     std::cout<<e<<" ";
//   }

//   std::cout<<"\n";
//   std::vector<std::vector<std::string>> matrix{};

//   for(auto i : knfa)
//   {
//     dfs(i,matrix);
//   }
//   uMatrix(matrix);
//   for(auto i : matrix)
//   {
//     for(auto j : i)
//     {
//       std::cout<<j<<" ";
//     }
//     std::cout<<"\n";
//   }
//   std::string h = "out";
//   matrixTotxt(matrix, h);
//   printGraph(knfa,"out.dot");
//   return 0;
//   //at.g.    at| gc| |    at| gc| | |    at| gc| |   |.  ta.g.tg.a.|ta.a.|.
// }
// at.g.at|gc||at|gc|||at|gc|||*.ta.g.tg.a.|ta.a.|.
