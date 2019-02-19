#ifndef LANGUAGE_H_INCLUDED
#define LANGUAGE_H_INCLUDED

#include <string>
#include <map>
#include <vector>

#define SBASIC_DECIMAL_TYPE_DOUBLE
namespace SBASIC
{
//Type line_number
typedef unsigned int line_number;

//Enum
enum class TOKEN
{
    DECIMAL_TOKEN, IDENTIFIER_TOKEN, NEWLINE_TOKEN, STRING_TOKEN, KEYWORD_TOKEN, EOF_TOKEN, OPERATOR_TOKEN, DELIMITER_TOKEN
};
enum class KEYWORD
{
    INPUT, PRINT, END, IF, THEN, ELSE, DO, LOOP, UNTIL, WHILE, WEND
};
enum class DELIMITER
{
    NEW_LINE, SEMICOLON, COMMA, LEFT_PARENTHESIS, RIGHT_PARENTHESIS
};
enum class OPERATOR
{
    PLUS, SUBSTRACT, MULTIPLY, DIVIDE, DIVIDE_EXACTLY, POWER, MOD, EQUAL, GREATER_THEN, LESS_THEN, NOT_EQUAL, GREATER_THEN_OR_EQUAL, LESS_THEN_OR_EQUAL, AND, OR, NOT
};

//SBASIC decimal type
#if defined SBASIC_DECIMAL_TYPE_LONG_DOUBLE
typedef long double sbasic_decimal_type;
#elif defined SBASIC_DECIMAL_TYPE_FLOAT
typedef float sbasic_decimal_type;
#elif defined SBASIC_DECIMAL_TYPE_DOUBLE
typedef double sbasic_decimal_type;
#endif // SBASIC_DECIMAL_TYPE_LONG_DOUBLE

//SBASIC function pointer
typedef sbasic_decimal_type (*sbasic_function_pointer)(const std::vector<sbasic_decimal_type> & args);

//True or False
const sbasic_decimal_type sbasic_true = 1;
const sbasic_decimal_type sbasic_false = 0;

//Convert function
sbasic_decimal_type string_to_decimal(const std::string & str);
KEYWORD string_to_keyword(const std::string & str);
OPERATOR string_to_letter_operator(const std::string & str);

//Is function
bool is_keyword(const std::string & str);
bool is_letter_operator(const std::string & str);
bool is_prefix_operator(char c);
bool is_delimiter(char c);

//Table class
class VariableTable
{
private:
    std::map<std::string,sbasic_decimal_type> m_variables;
    VariableTable * m_previous_variable_table_ptr;
public:
    VariableTable(VariableTable * previous_variable_table_ptr) : m_previous_variable_table_ptr(previous_variable_table_ptr) {}
    sbasic_decimal_type get_variable(const std::string & variable_name) throw(std::string);
    void assign_variable(const std::string & var_name, sbasic_decimal_type sdt);

};

class FunctionTable
{
private:
    std::map<std::string,sbasic_function_pointer> m_functions;
public:
    FunctionTable();
    sbasic_function_pointer get_function(const std::string & function_name) throw(std::string);
};

//Program class
class Expression
{
public:
    virtual sbasic_decimal_type compute(FunctionTable & function_table,VariableTable * variable_table) throw(std::string) =0;
    virtual ~Expression() {}
};

class TailExpression
{
private:
    OPERATOR m_operator;
    Expression * m_expression;
public:
    OPERATOR get_operator() const
    {
        return m_operator;
    }
    Expression * get_expression() const
    {
        return m_expression;
    }
    void set_operator(OPERATOR op)
    {
        m_operator = op;
    }
    void set_expression(Expression * expression)
    {
        m_expression = expression;
    }
};

class UnaryExpression : public Expression, public TailExpression
{
    sbasic_decimal_type compute(FunctionTable & function_table,VariableTable * variable_table) throw(std::string);
};

class VariableExpression : public Expression
{
private:
    std::string m_var_name;
    line_number m_line_number;
public:
    std::string get_variable_name() const
    {
        return m_var_name;
    }
    line_number get_line_number() const
    {
        return m_line_number;
    }
    void set_variable_name(const std::string & variable_name)
    {
        m_var_name = variable_name;
    }
    void set_line_number(line_number ln)
    {
        m_line_number = ln;
    }
    sbasic_decimal_type compute(FunctionTable & function_table,VariableTable * variable_table) throw(std::string);
};

class BinaryExpression : public Expression
{
private:
    OPERATOR m_operator;
    Expression * m_left_expression;
    Expression * m_right_expression;
public:
    ~BinaryExpression()
    {
        delete m_left_expression;
        delete m_right_expression;
    }
    OPERATOR get_operator() const
    {
        return m_operator;
    }
    Expression * get_left_expression() const
    {
        return m_left_expression;
    }
    Expression * get_right_expression() const
    {
        return m_right_expression;
    }
    void set_operator(OPERATOR op)
    {
        m_operator = op;
    }
    void set_left_expression(Expression * expression)
    {
        m_left_expression = expression;
    }
    void set_right_expression(Expression * expression)
    {
        m_right_expression = expression;
    }
    sbasic_decimal_type compute(FunctionTable & function_table,VariableTable * variable_table) throw(std::string);
};
class CallExpression : public Expression
{
private:
    std::string m_function_name;
    std::vector<Expression *> m_expressions;
    line_number m_line_number;
public:
    ~CallExpression()
    {
        for(auto it =  m_expressions.begin() ; it != m_expressions.end() ; ++it)
        {
            delete (*it);
        }
    }
    void add_expression(Expression * expression)
    {
        m_expressions.push_back(expression);
    }
    line_number get_line_number() const
    {
        return m_line_number;
    }
    std::string get_function_name() const
    {
        return m_function_name;
    }
    void set_function_name(const std::string & function_name)
    {
        m_function_name = function_name;
    }
    void set_line_number(line_number ln)
    {
        m_line_number = ln;
    }
    sbasic_decimal_type compute(FunctionTable & function_table,VariableTable * variable_table) throw(std::string);
};
class DecimalExpression : public Expression
{
private:
    sbasic_decimal_type m_sdt;
public:
    sbasic_decimal_type get_decimal() const
    {
        return m_sdt;
    }
    void set_decimal(sbasic_decimal_type decimal)
    {
        m_sdt = decimal;
    }
    sbasic_decimal_type compute(FunctionTable & function_table,VariableTable * variable_table) throw(std::string)
    {
        return m_sdt;
    }
};

class Stmt
{
public:
    virtual void execute(FunctionTable & function_table,VariableTable * variable_table) throw(std::string) =0;
    virtual ~Stmt() {}
};

class PrintStmt : public Stmt
{
private:
    std::string m_prompt;
    bool has_prompt;
    std::vector<Expression *> m_expressions;
public:
    ~PrintStmt()
    {
        for(auto it =  m_expressions.begin() ; it != m_expressions.end() ; ++it)
        {
            delete (*it);
        }
    }
    std::string get_prompt() const
    {
        return m_prompt;
    }
    void set_prompt(const std::string & prompt)
    {
        m_prompt = prompt;
        has_prompt = true;
    }
    void add_expression(Expression * expression)
    {
        m_expressions.push_back(expression);
    }
    void execute(FunctionTable & function_table,VariableTable * variable_table) throw(std::string);
};

class InputStmt : public Stmt
{
private:
    std::string m_prompt;
    bool has_prompt;
    std::vector<VariableExpression *> m_expressions;
public:
    ~InputStmt()
    {
        for(auto it =  m_expressions.begin() ; it != m_expressions.end() ; ++it)
        {
            delete (*it);
        }
    }
    std::string get_prompt() const
    {
        return m_prompt;
    }
    void set_prompt(const std::string & prompt)
    {
        m_prompt = prompt;
        has_prompt = true;
    }
    void add_expression(VariableExpression * expression)
    {
        m_expressions.push_back(expression);
    }
    void execute(FunctionTable & function_table,VariableTable * variable_table) throw(std::string);
};

class AssignmentStmt : public Stmt
{
private:
    VariableExpression * m_variable_expression;
    Expression * m_expression;
public:
    ~AssignmentStmt()
    {
        delete m_expression;
        delete m_variable_expression;
    }
    VariableExpression * get_variable_expression() const
    {
        return m_variable_expression;
    }
    void set_variable_expression(VariableExpression * variable_expression)
    {
        m_variable_expression = variable_expression;
    }
    Expression * get_expression() const
    {
        return m_expression;
    }
    void set_expression(Expression * expression)
    {
        m_expression = expression;
    }
    void execute(FunctionTable & function_table,VariableTable * variable_table) throw(std::string);
};

class DOIteratorStmt : public Stmt
{
private:
    Expression * m_condition;
    std::vector<Stmt *> m_stmts;
public:
    ~DOIteratorStmt()
    {
        delete m_condition;
        for(auto it =  m_stmts.begin() ; it != m_stmts.end() ; ++it)
        {
            delete (*it);
        }
    }
    Expression * get_condition() const
    {
        return m_condition;
    }
    void set_condition(Expression * expression)
    {
        m_condition = expression;
    }
    void add_stmt(Stmt * stmt)
    {
        m_stmts.push_back(stmt);
    }
    void execute(FunctionTable & function_table,VariableTable * variable_table) throw(std::string);
};
class SelectionStmt : public Stmt
{
private:
    Expression * m_condition;
    std::vector<Stmt *> m_true_stmts;
    std::vector<Stmt *> m_false_stmts;
public:
    ~SelectionStmt()
    {
        delete m_condition;
        for(auto it =  m_true_stmts.begin() ; it != m_true_stmts.end() ; ++it)
        {
            delete (*it);
        }
        for(auto it =  m_false_stmts.begin() ; it != m_false_stmts.end() ; ++it)
        {
            delete (*it);
        }
    }
    Expression * get_condition() const
    {
        return m_condition;
    }
    void set_condition(Expression * expression)
    {
        m_condition = expression;
    }
    void add_true_stmt(Stmt * stmt)
    {
        m_true_stmts.push_back(stmt);
    }
    void add_false_stmt(Stmt * stmt)
    {
        m_false_stmts.push_back(stmt);
    }
    void execute(FunctionTable & function_table,VariableTable * variable_table) throw(std::string);
};
class WHILEIteratorStmt : public Stmt
{
private:
    Expression * m_condition;
    std::vector<Stmt *> m_stmts;
public:
    ~WHILEIteratorStmt()
    {
        delete m_condition;
        for(auto it =  m_stmts.begin() ; it != m_stmts.end() ; ++it)
        {
            delete (*it);
        }
    }
    Expression * get_condition() const
    {
        return m_condition;
    }
    void set_condition(Expression * expression)
    {
        m_condition = expression;
    }
    void add_stmt(Stmt * stmt)
    {
        m_stmts.push_back(stmt);
    }
    void execute(FunctionTable & function_table,VariableTable * variable_table) throw(std::string);
};
class Program
{
private:
    std::vector<Stmt *> m_stmts;
public:
    ~Program()
    {
        for(auto it =  m_stmts.begin() ; it != m_stmts.end() ; ++it)
        {
            delete (*it);
        }
    }
    void add_stmt(Stmt * stmt)
    {
        m_stmts.push_back(stmt);
    }
    void run(FunctionTable & function_table,VariableTable * variable_table) throw(std::string)
    {
        for(auto it =  m_stmts.begin() ; it != m_stmts.end() ; ++it)
        {
            (**it).execute(function_table,variable_table);
        }
    }
};
}

#endif // LANGUAGE_H_INCLUDED
