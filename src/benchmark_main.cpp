#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "nfa_pointer.h"

int main(int argc, char *argv[])
{
    size_t set = std::stoi(argv[2]);
    std::cout<<set<<"\n";
    std::string regex = argv[1];
    State* nfa = post2nfaE(regex);
    std::fstream file;
    file.open(argv[3], std::ios::out);
    while(set > 0)
    {
        std::string word = getRandomWord(nfa);
        file << word;
        for(size_t i = word.size(); i < 80; i++)
        {
            file << 'A';
        }
        file <<"\n" ;   
        set--;
    }
    file.close();
}