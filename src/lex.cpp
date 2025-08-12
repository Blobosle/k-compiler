#include "lex.hpp"

#include <cctype>
#include <cstdlib>

#define eof (std::char_traits<char>::eof())

int Lexer::get_tok() {
    /* White space ignorer */
    while (std::isspace(last_char)) {
        last_char = fp.get();
    }

    /* ID: [a-zA-Z] */
    if (std::isalpha(last_char)) {
        identifier_str = static_cast<char>(last_char);

        /* Get whole word */
        while (std::isalnum(last_char = fp.get())) {
            identifier_str += static_cast<char>(last_char);
        }

        if (identifier_str == "def") {
            return tok_def;
        }
        else if (identifier_str == "extern") {
            return tok_extern;
        }
        return tok_identifier;
    }

    /* ID: [0-9.] */
    if (std::isdigit(last_char) || last_char == '.') {
        std::string num_str;

        do {
            num_str += static_cast<char>(last_char);
            last_char = fp.get();
        } while (std::isdigit(last_char) || last_char == '.');

        num_val = std::strtod(num_str.c_str(), 0);
        return tok_number;
    }

    /* Comment handling - ID: [#]*/
    if (last_char == '#') {
        do {
            last_char = fp.get();
        } while (last_char != '\n' && last_char != '\r' && last_char != eof);

        if (last_char != eof) {
            return get_tok();
        }
    }

    /* Returning EOF */
    if (last_char == eof) {
        return tok_eof;
    }

    /* Returning the ascii value as is */
    int ret = last_char;
    last_char = fp.get();

    return ret;
} /* get_tok() */
