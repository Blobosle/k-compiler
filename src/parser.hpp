#pragma once

#include "lex.hpp"

#include <map>
#include <memory>
#include <string>

class AST;
class PrototypeAST;

class Parser {
    private:
        Lexer& lex;
        int cur_tok;

        int next_token() { return cur_tok = lex.get_tok(); }

        std::unique_ptr<AST> log_err(const std::string&);
        std::unique_ptr<PrototypeAST> log_proto_err(const std::string&);

        std::map<char, int> binop_precedence;

    public:
        Parser(Lexer& lex) : lex(lex) {
            next_token();
        }

        std::unique_ptr<AST> parse_primary();
        std::unique_ptr<AST> parse_num();
        std::unique_ptr<AST> parse_paren();
        std::unique_ptr<AST> parse_identifier();

        std::unique_ptr<AST> parse_expression();

        int get_tok_prec();

        std::unique_ptr<AST> parse_binops_right(int, std::unique_ptr<AST>);
};

class AST {
    public:
        virtual ~AST() = default;
};

class NumberAST : public AST {
    private:
        double val;
    public:
        NumberAST(double val) : val(val) {}
};

class VariableAST : public AST {
    private:
        std::string name;
    public:
        VariableAST(const std::string& name) : name(name) {}
};

class BinaryAST : public AST {
    private:
        char op;
        std::unique_ptr<AST> left;
        std::unique_ptr<AST> right;
    public:
        BinaryAST(char op, std::unique_ptr<AST> left, std::unique_ptr<AST> right)
            : op(op), left(std::move(left)), right(std::move(right)) {}
};

class CallAST : public AST {
    private:
        std::string callee;
        std::vector<std::unique_ptr<AST>> args;
    public:
        CallAST(const std::string& callee,
                std::vector<std::unique_ptr<AST>> args)
            : callee(callee), args(std::move(args)) {}
};

class PrototypeAST {
    private:
        std::string name;
        std::vector<std::string> args;
    public:
        PrototypeAST(const std::string& name, std::vector<std::string> args)
            : name(name), args(args) {}

        const std::string& get_name() {
            return name;
        }
};


class FunctionAST {
    private:
        std::unique_ptr<PrototypeAST> proto;
        std::unique_ptr<AST> body;
    public:
        FunctionAST(std::unique_ptr<PrototypeAST> proto, std::unique_ptr<AST> body)
            : proto(std::move(proto)), body(std::move(body)) {}
};
