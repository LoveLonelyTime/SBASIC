#include "language.h"
#include <string>
#include <cmath>
#include <iostream>

namespace SBASIC
{
//Convert function
sbasic_decimal_type string_to_decimal(const std::string & str)
{
#if defined SBASIC_DECIMAL_TYPE_LONG_DOUBLE
    return std::stold(str);
#elif defined SBASIC_DECIMAL_TYPE_FLOAT
    return std::stof(str);
#elif defined SBASIC_DECIMAL_TYPE_DOUBLE
    return std::stod(str);
#endif // SBASIC_DECIMAL_TYPE_LONG_DOUBLE
}
KEYWORD string_to_keyword(const std::string & str)
{
    if(str == "INPUT")
    {
        return KEYWORD::INPUT;
    }
    else if(str == "PRINT")
    {
        return KEYWORD::PRINT;
    }
    else if(str == "END")
    {
        return KEYWORD::END;
    }
    else if(str == "IF")
    {
        return KEYWORD::IF;
    }
    else if(str == "THEN")
    {
        return KEYWORD::THEN;
    }
    else if(str == "ELSE")
    {
        return KEYWORD::ELSE;
    }
    else if(str == "DO")
    {
        return KEYWORD::DO;
    }
    else if(str == "LOOP")
    {
        return KEYWORD::LOOP;
    }
    else if(str == "UNTIL")
    {
        return KEYWORD::UNTIL;
    }
    else if(str == "WHILE")
    {
        return KEYWORD::WHILE;
    }
    else
    {
        return KEYWORD::WEND;
    }
}
OPERATOR string_to_letter_operator(const std::string & str)
{
    if(str == "AND")
    {
        return OPERATOR::AND;
    }
    else if(str == "OR")
    {
        return OPERATOR::OR;
    }
    else if(str == "NOT")
    {
        return OPERATOR::NOT;
    }
    else
    {
        return OPERATOR::MOD;
    }
}

//Is function
bool is_keyword(const std::string & str)
{
    return str == "INPUT" || str == "PRINT" || str == "END" || str == "IF" || str == "THEN" || str == "ELSE" || str == "DO" || str == "LOOP" || str == "UNTIL"|| str == "WHILE" || str == "WEND";
}

bool is_letter_operator(const std::string & str)
{
    return str == "AND" || str == "OR" || str == "NOT" || str == "MOD";
}
bool is_prefix_operator(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '\\' || c == '%' || c == '^' || c == '=' || c == '<' || c == '>';
}
bool is_delimiter(char c)
{
    return c == '\n' || c == ';' || c == ',' || c == '(' || c == ')';
}

//Table class
sbasic_decimal_type VariableTable::get_variable(const std::string & variable_name) throw(std::string)
{
    for(VariableTable * variable_table_ptr = this; variable_table_ptr != nullptr; variable_table_ptr = (variable_table_ptr->m_previous_variable_table_ptr))
    {
        if(variable_table_ptr->m_variables.count(variable_name))
        {
            return variable_table_ptr->m_variables[variable_name];
        }
    }
    throw "The \"" + variable_name + "\" variable not found";
}

void VariableTable::assign_variable(const std::string & var_name, sbasic_decimal_type sdt)
{
    for(VariableTable * variable_table_ptr = this; variable_table_ptr != nullptr; variable_table_ptr = (variable_table_ptr->m_previous_variable_table_ptr))
    {
        if(variable_table_ptr->m_variables.count(var_name))
        {
            variable_table_ptr->m_variables[var_name] = sdt;
            return;
        }
    }
    m_variables[var_name] = sdt;
}

FunctionTable::FunctionTable()
{
    //Init standard functions

}

sbasic_function_pointer FunctionTable::get_function(const std::string & function_name) throw(std::string)
{
    if(m_functions.count(function_name))
    {
        return m_functions[function_name];
    }
    throw "The \"" + function_name + "\" function not found";
}

//Program class
sbasic_decimal_type UnaryExpression::compute(FunctionTable & function_table,VariableTable * variable_table) throw(std::string)
{
    if(get_operator() == OPERATOR::PLUS)
    {
        return get_expression()->compute(function_table,variable_table);
    }
    else if(get_operator() == OPERATOR::SUBSTRACT)
    {
        return -(get_expression()->compute(function_table,variable_table));
    }
    else
    {
        if(get_expression()->compute(function_table,variable_table) == sbasic_false)
        {
            return sbasic_true;
        }
        else
        {
            return sbasic_false;
        }
    }
}

sbasic_decimal_type VariableExpression::compute(FunctionTable & function_table,VariableTable * variable_table) throw(std::string)
{
    return variable_table->get_variable(m_var_name);
}

sbasic_decimal_type BinaryExpression::compute(FunctionTable & function_table,VariableTable * variable_table) throw(std::string)
{
    if(m_operator == OPERATOR::PLUS)
    {
        return m_left_expression->compute(function_table,variable_table) + m_right_expression->compute(function_table,variable_table);
    }
    else if(m_operator == OPERATOR::SUBSTRACT)
    {
        return m_left_expression->compute(function_table,variable_table) - m_right_expression->compute(function_table,variable_table);
    }
    else if(m_operator == OPERATOR::MULTIPLY)
    {
        return m_left_expression->compute(function_table,variable_table) * m_right_expression->compute(function_table,variable_table);
    }
    else if(m_operator == OPERATOR::DIVIDE)
    {
        return m_left_expression->compute(function_table,variable_table) / m_right_expression->compute(function_table,variable_table);
    }
    else if(m_operator == OPERATOR::DIVIDE_EXACTLY)
    {
        return int(m_left_expression->compute(function_table,variable_table) / m_right_expression->compute(function_table,variable_table));
    }
    else if(m_operator == OPERATOR::POWER)
    {
        return std::pow(m_left_expression->compute(function_table,variable_table), m_right_expression->compute(function_table,variable_table));
    }
    else if(m_operator == OPERATOR::MOD)
    {
        return int(m_left_expression->compute(function_table,variable_table)) % int(m_right_expression->compute(function_table,variable_table));
    }
    else if(m_operator == OPERATOR::EQUAL)
    {
        return m_left_expression->compute(function_table,variable_table) == m_right_expression->compute(function_table,variable_table) ? sbasic_true : sbasic_false;
    }
    else if(m_operator == OPERATOR::GREATER_THEN)
    {
        return m_left_expression->compute(function_table,variable_table) > m_right_expression->compute(function_table,variable_table) ? sbasic_true : sbasic_false;
    }
    else if(m_operator == OPERATOR::GREATER_THEN_OR_EQUAL)
    {
        return m_left_expression->compute(function_table,variable_table) >= m_right_expression->compute(function_table,variable_table) ? sbasic_true : sbasic_false;
    }
    else if(m_operator == OPERATOR::LESS_THEN)
    {
        return m_left_expression->compute(function_table,variable_table) < m_right_expression->compute(function_table,variable_table) ? sbasic_true : sbasic_false;
    }
    else if(m_operator == OPERATOR::LESS_THEN_OR_EQUAL)
    {
        return m_left_expression->compute(function_table,variable_table) <= m_right_expression->compute(function_table,variable_table) ? sbasic_true : sbasic_false;
    }
    else if(m_operator == OPERATOR::NOT_EQUAL)
    {
        return m_left_expression->compute(function_table,variable_table) != m_right_expression->compute(function_table,variable_table) ? sbasic_true : sbasic_false;
    }
    else if(m_operator == OPERATOR::AND)
    {
        return (m_left_expression->compute(function_table,variable_table)==sbasic_true) && (m_right_expression->compute(function_table,variable_table)==sbasic_true) ? sbasic_true : sbasic_false;
    }
    else
    {
        return (m_left_expression->compute(function_table,variable_table)==sbasic_true) || (m_right_expression->compute(function_table,variable_table)==sbasic_true) ? sbasic_true : sbasic_false;
    }
}
sbasic_decimal_type CallExpression::compute(FunctionTable & function_table,VariableTable * variable_table) throw(std::string)
{
    std::vector<sbasic_decimal_type> args;
    for(auto it =  m_expressions.begin() ; it != m_expressions.end() ; ++it)
    {
        args.push_back((**it).compute(function_table,variable_table));
    }
    return function_table.get_function(m_function_name)(args);
}

void PrintStmt::execute(FunctionTable & function_table,VariableTable * variable_table) throw(std::string)
{
    if(has_prompt)
    {
        std::cout << m_prompt << std::endl;
    }
    for(auto it =  m_expressions.begin() ; it != m_expressions.end() ; ++it)
    {
        std::cout << (**it).compute(function_table,variable_table) << std::endl;
    }
}
void InputStmt::execute(FunctionTable & function_table,VariableTable * variable_table) throw(std::string)
{
    if(has_prompt)
    {
        std::cout << m_prompt << "?";
    }
    else
    {
        std::cout << "?";
    }
    for(auto it =  m_expressions.begin() ; it != m_expressions.end() ; ++it)
    {
        sbasic_decimal_type sdt;
        std::cin >> sdt;
        variable_table->assign_variable((**it).get_variable_name(),sdt);
    }
}
void AssignmentStmt::execute(FunctionTable & function_table,VariableTable * variable_table) throw(std::string)
{
    variable_table->assign_variable(m_variable_expression->get_variable_name(),m_expression->compute(function_table,variable_table));
}
void DOIteratorStmt::execute(FunctionTable & function_table,VariableTable * variable_table) throw(std::string)
{
    do
    {
        VariableTable * var_tb = new VariableTable(variable_table);
        for(auto it =  m_stmts.begin() ; it != m_stmts.end() ; ++it)
        {
            (**it).execute(function_table,var_tb);
        }
        delete var_tb;
    }
    while(m_condition->compute(function_table,variable_table) == sbasic_false);
}
void SelectionStmt::execute(FunctionTable & function_table,VariableTable * variable_table) throw(std::string)
{
    VariableTable * var_tb = new VariableTable(variable_table);
    if(m_condition->compute(function_table,variable_table) == sbasic_true)
    {
        for(auto it =  m_true_stmts.begin() ; it != m_true_stmts.end() ; ++it)
        {
            (**it).execute(function_table,var_tb);
        }
    }
    else
    {
        for(auto it =  m_false_stmts.begin() ; it != m_false_stmts.end() ; ++it)
        {
            (**it).execute(function_table,var_tb);
        }
    }
    delete var_tb;
}
void WHILEIteratorStmt::execute(FunctionTable & function_table,VariableTable * variable_table) throw(std::string)
{
    while(m_condition->compute(function_table,variable_table) == sbasic_true)
    {
        VariableTable * var_tb = new VariableTable(variable_table);
        for(auto it =  m_stmts.begin() ; it != m_stmts.end() ; ++it)
        {
            (**it).execute(function_table,var_tb);
        }
        delete var_tb;
    }
}
}
