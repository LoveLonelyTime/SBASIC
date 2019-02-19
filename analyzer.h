#ifndef ANALYZER_H_INCLUDED
#define ANALYZER_H_INCLUDED

#include "lexer.h"
#include <string>
#include <vector>
namespace SBASIC
{
class SyntaxAnalyer
{
private:
    TokenReader & m_token_reader;
    Token * m_token_ptr;
    void read_token();
    void stmts(std::vector<Stmt *> & stmt_vector)throw (std::string);
    void exps(std::vector<Expression *> & exp_vector)throw(std::string);
    void exp_tail(std::vector<Expression *> & exp_vector)throw(std::string);
    Stmt * stmt()throw(std::string);
    std::string * prompt()throw (std::string);
    void ids(std::vector<VariableExpression *> & vars_vector)throw(std::string);
    void id_Tail(std::vector<VariableExpression *> & vars_vector)throw(std::string);
    Expression * exp()throw(std::string);
    Expression * primary_exp()throw(std::string);
    void param(std::vector<Expression *> & exp_vector)throw(std::string);
    void args(std::vector<Expression *> & exp_vector)throw(std::string);
    void arg_tail(std::vector<Expression *> & exp_vector)throw(std::string);
    Expression * unary_exp()throw(std::string);
    Expression * power_exp()throw(std::string);
    TailExpression * power_exp_tail()throw(std::string);
    Expression * mult_exp()throw(std::string);
    TailExpression * mult_exp_tail()throw(std::string);
    Expression * mod_exp()throw(std::string);
    TailExpression * mod_exp_tail()throw(std::string);
    Expression * add_exp()throw(std::string);
    TailExpression * add_exp_tail()throw(std::string);
    Expression * relationship_exp()throw(std::string);
    TailExpression * relationship_exp_tail()throw(std::string);
    Expression * or_exp()throw(std::string);
    TailExpression * or_exp_tail()throw(std::string);
    std::string create_error(const std::string & error,line_number ln)
    {
        return "Line: " + std::to_string(ln) + ", Error: " + error;
    }
public:
    SyntaxAnalyer(TokenReader & token_reader);
    ~SyntaxAnalyer();
    Program * analyer() throw(std::string);
    void delete_program(Program * program)
    {
        delete program;
    }
};
}

#endif // ANALYZER_H_INCLUDED
