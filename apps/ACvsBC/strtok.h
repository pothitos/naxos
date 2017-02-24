// Part of https://github.com/pothitos/naxos

#ifndef STRTOK_H
#define STRTOK_H

#include <string>

class StrTokenizer {
    private:
        const std::string str;   // string to break up into tokens
        const std::string delim; // the characters in `delim'
                                 //   separate the tokens of `str'
        unsigned c;              // current character

    public:
        StrTokenizer(const std::string str_init,
                     const std::string delim_init = " ")
          : str(str_init), delim(delim_init), c(0)
        {
                //  Eating up delimiters (in the begining of `str')
                while (c < str.size() &&
                       delim.find(str[c]) != std::string::npos)
                        ++c;
        }

        std::string next(void) // returns the next token
        {
                //  Creating character by character the `token'
                std::string token;
                while (c < str.size() &&
                       delim.find(str[c]) == std::string::npos) {
                        token += str[c];
                        ++c;
                }

                //  Eating up delimiters
                while (c < str.size() &&
                       delim.find(str[c]) != std::string::npos)
                        ++c;
                return token;
        }
};

#endif // STRTOK_H
