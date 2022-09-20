#include "utils.h"


std::vector<char> getAlphabet(const std::string& regex)
{
  std::vector<char> alphabet{};
  for(auto e : regex)
  {
    if(e != '.' && e != '|' && e != '?' && e != '+' && e != '*')
    {
      alphabet.push_back(e);
    }
  }
  std::sort(alphabet.begin(), alphabet.end());
  auto last = std::unique(alphabet.begin(), alphabet.end());
  alphabet.erase(last, alphabet.end());
  return alphabet;
}


void matrixTotxt(const std::vector<std::vector<std::string>>& matrix, std::string& filename)
{
  std::fstream f;
  
  f.open(filename += ".txt", std::ios::out);
  if(f.good())
  {
    for(auto z : matrix)
    {
      for(auto s : z)
      {
        f << s <<" ";
      }
      f << "\n";
    }
  }
}

// bool isFunction(const char& a)
// {
//       switch(a){
//       default: //char
//               return false;
//               break;
//       case '(':  
//               return true;
//               break;
//       case ')':  
//               return true;
//               break;
//       case '|':  
//               return true;
//               break;
//       case '?': 
//               return true;
//               break;
//       case '*': 
//               return true;
//               break;
//       case '+':
//               return true;
// 							break;
//     }
// }

// std::string shuntingYard(const std::string& regex)
// {
//   std::string out;
//   std::stack<char> stack{};
//   for(auto elem : regex)
//   {
//     isFunction(elem) ? stack.push(elem) : out += elem ;
//     while()
//   }

// }


//Not used
//uint getValue(const std::vector<char>& alphabet, const char& input)
//{
//  for(uint i = 0; i < alphabet.size(); i++)
//  {
//    if(alphabet[i] == input)
//    {
//      return i;
//    }
//  }
//  return -1;
//}

//Not used
//uint shiftValue(const uint& input)//last index of alphabet biggest posibil nr
//{
//  uint i = 1;
//  while(input >= pow(2,i)-1)
//  {
//    i++;
//  }
//  return i;
//}
//NOT Used
//Hash funkction that only works with an Alphabet of the size 2^n
//uint32_t getHash(const std::vector<char>& alphabet, const std::string& input, const uint& sValue)
//{
//  uint32_t hash = 0;
//  for(uint i = 0; i < input.size(); i++)
//  {
//    uint ordValue = getValue(alphabet, input[i]);
//    hash <<= sValue;
//    hash |= ordValue;
//  }
//  return hash;
//}

//NOT Used
//std::string getQgram(const std::vector<char>& alphabet, uint32_t input, const uint& sValue, uint qlength)
//{
//  std::string out = "";
//  uint mask = pow(2,sValue)-1;
//  uint ord = mask;
//  while(qlength > 0)
//  {
//    ord &= input;
//    input >>= sValue;
//    out += alphabet[ord];
//    ord = mask;
//    qlength--;
//  }
//  std::reverse(out.begin(), out.end());
//  return out;
//}

//NOT Used
//void matrixTXT(const std::vector<std::vector<std::string>>& matrix,
//                const std::vector<char>& alphabet)
//{
//  uint hashP = pow(alphabet.size(), matrix[0][0].size());
//  uint sValue = shiftValue(alphabet.size()-1);
//
//  std::vector<uint16_t> line(hashP,0);
//  std::fstream f;
//
//  f.open("Matrix_out.txt", std::ios::out);
//  if(f.good())
//  {
//    for(uint i = 0; i < hashP; i++)
//    {
//      std::cout<<getQgram(alphabet, i, sValue, matrix[0][0].size())<<"\n";
//      //f << getQgram(alphabet, i, sValue, matrix[0][0].size()) << " ";
//    }
//    f << "\n";
//
//    for(auto l : matrix)
//    {
//      for(auto v : l)
//      {
//        line[getHash(alphabet, v, sValue)] = 1;
//      }
//      for(auto e : line)
//      {
//        f<<e<<" ";
//      }
//      f << "\n";
//      std::fill(line.begin(), line.end(), 0);
//    }
//  }
//}

//NOT Used
bool allCharactersSame(const std::string& s)
{
    int n = s.length();
    for (int i = 1; i < n; i++)
        if (s[i] != s[0])
            return false;

    return true;
}



/*
int main()
{
  std::vector<char> alphabet = {'a','b','c','d','e','f'};
  uint hashP = pow(alphabet.size(), 5);
  uint sValue = shiftValue(alphabet.size()-1);
    for(uint i = 0; i < hashP; i++)
    {
      std::string w = getQgram(alphabet, i, sValue, 5);
      uint z = getHash(alphabet, w, sValue);
      std::cout<<w<<" "<<z<<" ";
      //f << getQgram(alphabet, i, sValue, matrix[0][0].size()) << " ";
    }
}*/
