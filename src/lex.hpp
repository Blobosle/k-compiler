#pragma once

#include <istream>
#include <string>

enum token_t {
    tok_eof = -1,
    tok_def = -2,
    tok_extern = -3,
    tok_identifier = -4,
    tok_number = -5,
};

class Lexer {
    private:
        int last_char;
        std::istream& fp;

    public:
        std::string identifier_str;
        double num_val;

        Lexer(std::istream& in) : fp(in), last_char(' '), identifier_str(), num_val(0.0) {}

        int get_tok();
};
