#include "utils.h"
#include "arg_parse.h"
#include "index.h"
#include "korotkov_nfa.h"
#include "graphMaker.h"
#include "nfa_pointer.h"
#include "query.h"


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

    // Load index from disk
    seqan3::debug_stream << "Reading Index from Disk... ";
    IndexStructure ibf;
    load_ibf(ibf, cmd_args.idx);
    seqan3::debug_stream << "DONE" << std::endl;

    // Evaluate and search for Regular Expression
    seqan3::debug_stream << "Querying:" << std::endl;
    uint8_t qlength = ibf.k_;
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
        //seqan3::debug_stream << collapse_kmers(qlength, path) << ":::";
        query_ibf(ibf, path);
    }
    
    seqan3::debug_stream << "DONE" << std::endl;
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
  return 0;
}
