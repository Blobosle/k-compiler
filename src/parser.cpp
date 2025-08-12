#include "parser.hpp"
#include "lex.hpp"

#include <iostream>
#include <memory>

std::unique_ptr<AST> Parser::log_err(const std::string& str) {
    std::cerr << "[ERROR]: " << str << "\n";
    return nullptr;
} /* log_err() */

std::unique_ptr<PrototypeAST> Parser::log_proto_err(const std::string& str) {
    log_err(str);
    return nullptr;
} /* log_proto_err() */

std::unique_ptr<AST> Parser::parse_num() {
    auto ret = std::make_unique<NumberAST>(lex.num_val);
    next_token();
    return std::move(ret);
} /* parse_num() */

std::unique_ptr<AST> Parser::parse_paren() {
    next_token();

    auto v = parse_expression();

    if (!v) {
        return nullptr;
    }

    if (cur_tok != ')') {
        return log_err("expected ')'");
    }

    next_token();
    return v;
} /* parse_paren() */

std::unique_ptr<AST> Parser::parse_identifier() {
    std::string id = lex.identifier_str;

    next_token();

    if (cur_tok != '(') {
        return std::make_unique<VariableAST>(id);
    }

    next_token();
    std::vector<std::unique_ptr<AST>> args;

    if (cur_tok != ')') {
        for (;;) {
            if (auto arg = parse_expression()) {
                args.push_back(std::move(arg));
            }
            else {
                return nullptr;
            }

            if (cur_tok == ')') {
                break;
            }

            if (cur_tok != ',') {
                return log_err("expected ')' or ',' in argument list");
            }

            next_token();
        }
    }

    next_token();

    return std::make_unique<CallAST>(id, std::move(args));
} /* parse_identifier() */

std::unique_ptr<AST> Parser::parse_primary() {
    switch (cur_tok) {
        case tok_identifier:
            return parse_identifier();
        case tok_number:
            return parse_num();
        case '(':
            return parse_paren();
        default:
            return log_err("unknown token parsed");
    }

    return nullptr;
} /* parse_primary() */

std::unique_ptr<AST> Parser::parse_expression() {
    auto left = parse_primary();

    if (!left) {
        return nullptr;
    }

    return parse_binops_right(0, std::move(left));
} /* parse_expression() */

int Parser::get_tok_prec() {
    if (!isascii(cur_tok)) {
        return -1;
    }

    int tok_prec = binop_precedence[cur_tok];

    if (tok_prec <= 0) {
        return -1;
    }

    return tok_prec;
} /* get_tok_prec() */

std::unique_ptr<AST> Parser::parse_binops_right(int expr_prec,
                                                std::unique_ptr<AST> left) {
    for (;;) {
        int tok_prec = get_tok_prec();

        if (tok_prec < expr_prec) {
            return left;
        }

        int binop = cur_tok;
        next_token();

        auto right = parse_primary();
        if (!right) {
            return nullptr;
        }

        int next_prec = get_tok_prec();

        if (tok_prec < next_prec) {
            right = parse_binops_right(tok_prec + 1, std::move(right));
            if (!right) {
                return nullptr;
            }
        }

        left = std::make_unique<BinaryAST>(static_cast<char>(binop),
                                            std::move(left), std::move(right));
    }
}
