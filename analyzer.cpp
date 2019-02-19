#include "analyzer.h"
namespace SBASIC
{
SyntaxAnalyer::SyntaxAnalyer(TokenReader & token_reader) : m_token_reader(token_reader), m_token_ptr(nullptr)
{
    read_token();
}

void SyntaxAnalyer::read_token()
{
    if(m_token_ptr != nullptr)
    {
        delete m_token_ptr;
    }
    m_token_ptr = m_token_reader.read_token();
}

SyntaxAnalyer::~SyntaxAnalyer()
{
    if(m_token_ptr != nullptr)
    {
        delete m_token_ptr;
    }
}

Program * SyntaxAnalyer::analyer() throw(std::string)
{
    Program * program_ptr = new Program();
    std::vector<Stmt *> stmt_vector;
    stmts(stmt_vector);
    for(auto it = stmt_vector.begin(); it != stmt_vector.end(); ++it)
    {
        program_ptr->add_stmt(*it);
    }
    if(!(m_token_ptr->get_token_type() == TOKEN::KEYWORD_TOKEN && dynamic_cast<KeywordToken *>(m_token_ptr)->get_keyword() == KEYWORD::END))
    {
        throw create_error("Not found END",m_token_ptr->get_line_number());
    }
    return program_ptr;
}

void SyntaxAnalyer::stmts(std::vector<Stmt *> & stmt_vector)throw (std::string)
{
    Stmt * stmt_ptr = stmt();
    if(stmt_ptr != nullptr)
    {
        stmt_vector.push_back(stmt_ptr);
        if(!(m_token_ptr->get_token_type() == TOKEN::DELIMITER_TOKEN && dynamic_cast<DelimiterToken *>(m_token_ptr)->get_delimiter() == DELIMITER::NEW_LINE))
        {
            throw create_error("Lack of \\n",m_token_ptr->get_line_number());
        }
        else
        {
            read_token();
            stmts(stmt_vector);
        }
    }
}

void SyntaxAnalyer::exps(std::vector<Expression *> & exp_vector)throw(std::string)
{
    exp_vector.push_back(exp());
    exp_tail(exp_vector);
}

void SyntaxAnalyer::exp_tail(std::vector<Expression *> & exp_vector)throw(std::string)
{
    if(m_token_ptr->get_token_type() == TOKEN::DELIMITER_TOKEN && dynamic_cast<DelimiterToken *>(m_token_ptr)->get_delimiter() == DELIMITER::COMMA)
    {
        read_token();
        exp_vector.push_back(exp());
        exp_tail(exp_vector);
    }
}

Stmt * SyntaxAnalyer::stmt()throw (std::string)
{
    if(m_token_ptr->get_token_type() == TOKEN::KEYWORD_TOKEN && dynamic_cast<KeywordToken *>(m_token_ptr)->get_keyword() == KEYWORD::PRINT)
    {
        PrintStmt * print_stmt_ptr = new PrintStmt();
        read_token();
        if(m_token_ptr->get_token_type() == TOKEN::STRING_TOKEN)
        {
            print_stmt_ptr->set_prompt(dynamic_cast<StringToken *>(m_token_ptr)->get_string());
            read_token();
            if(m_token_ptr->get_token_type() == TOKEN::DELIMITER_TOKEN && dynamic_cast<DelimiterToken *>(m_token_ptr)->get_delimiter() == DELIMITER::SEMICOLON)
            {
                read_token();
                std::vector<Expression *> exp_vector;
                exps(exp_vector);
                for(auto it = exp_vector.begin(); it != exp_vector.end(); ++it)
                {
                    print_stmt_ptr->add_expression(*it);
                }
            }
        }
        else
        {
            std::vector<Expression *> exp_vector;
            exps(exp_vector);
            for(auto it = exp_vector.begin(); it != exp_vector.end(); ++it)
            {
                print_stmt_ptr->add_expression(*it);
            }
        }
        return print_stmt_ptr;
    }
    else if(m_token_ptr->get_token_type() == TOKEN::KEYWORD_TOKEN && dynamic_cast<KeywordToken *>(m_token_ptr)->get_keyword() == KEYWORD::INPUT)
    {
        InputStmt * input_stmt_ptr = new InputStmt();
        read_token();

        std::string * prompt_ptr = prompt();
        if(prompt_ptr != nullptr)
        {
            input_stmt_ptr->set_prompt(*prompt_ptr);
            delete prompt_ptr;
        }
        std::vector<VariableExpression *> var_vector;
        ids(var_vector);
        for(auto it = var_vector.begin(); it != var_vector.end(); ++it)
        {
            input_stmt_ptr->add_expression(*it);
        }
        return input_stmt_ptr;
    }
    else if(m_token_ptr->get_token_type() == TOKEN::IDENTIFIER_TOKEN)
    {
        AssignmentStmt * assignment_stmt_ptr = new AssignmentStmt();
        VariableExpression * var_exp_ptr = new VariableExpression();
        var_exp_ptr->set_line_number(m_token_ptr->get_line_number());
        var_exp_ptr->set_variable_name(dynamic_cast<IdentifierToken *>(m_token_ptr)->get_identifier());
        assignment_stmt_ptr->set_variable_expression(var_exp_ptr);
        read_token();
        if(m_token_ptr->get_token_type() == TOKEN::OPERATOR_TOKEN && dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator() == OPERATOR::EQUAL)
        {
            read_token();
            assignment_stmt_ptr->set_expression(exp());
            return assignment_stmt_ptr;
        }
        else
        {
            throw create_error("Lack of =",m_token_ptr->get_line_number());
        }
    }
    else if(m_token_ptr->get_token_type() == TOKEN::KEYWORD_TOKEN && dynamic_cast<KeywordToken *>(m_token_ptr)->get_keyword() == KEYWORD::IF)
    {
        SelectionStmt * selection_stmt_ptr = new SelectionStmt();
        read_token();
        selection_stmt_ptr->set_condition(exp());
        if(m_token_ptr->get_token_type() == TOKEN::KEYWORD_TOKEN && dynamic_cast<KeywordToken *>(m_token_ptr)->get_keyword() == KEYWORD::THEN)
        {
            read_token();
            read_token();
            std::vector<Stmt *> true_stmt_vector;
            stmts(true_stmt_vector);
            for(auto it = true_stmt_vector.begin(); it != true_stmt_vector.end(); ++it)
            {
                selection_stmt_ptr->add_true_stmt(*it);
            }
            if(m_token_ptr->get_token_type() == TOKEN::KEYWORD_TOKEN && dynamic_cast<KeywordToken *>(m_token_ptr)->get_keyword() == KEYWORD::ELSE)
            {
                read_token();
                read_token();
                std::vector<Stmt *> false_stmt_vector;
                stmts(false_stmt_vector);
                for(auto it = false_stmt_vector.begin(); it != false_stmt_vector.end(); ++it)
                {
                    selection_stmt_ptr->add_false_stmt(*it);
                }
                if(m_token_ptr->get_token_type() == TOKEN::KEYWORD_TOKEN && dynamic_cast<KeywordToken *>(m_token_ptr)->get_keyword() == KEYWORD::END)
                {
                    read_token();
                    if(m_token_ptr->get_token_type() == TOKEN::KEYWORD_TOKEN && dynamic_cast<KeywordToken *>(m_token_ptr)->get_keyword() == KEYWORD::IF)
                    {
                        read_token();
                        return selection_stmt_ptr;
                    }
                    else
                    {
                        throw create_error("Lack of END IF",m_token_ptr->get_line_number());
                    }
                }
                else
                {
                    throw create_error("Lack of END IF",m_token_ptr->get_line_number());
                }
            }
            else
            {
                if(m_token_ptr->get_token_type() == TOKEN::KEYWORD_TOKEN && dynamic_cast<KeywordToken *>(m_token_ptr)->get_keyword() == KEYWORD::END)
                {
                    read_token();
                    if(m_token_ptr->get_token_type() == TOKEN::KEYWORD_TOKEN && dynamic_cast<KeywordToken *>(m_token_ptr)->get_keyword() == KEYWORD::IF)
                    {
                        read_token();
                        return selection_stmt_ptr;
                    }
                    else
                    {
                        throw create_error("Lack of END IF",m_token_ptr->get_line_number());
                    }
                }
                else
                {
                    throw create_error("Lack of END IF",m_token_ptr->get_line_number());
                }
            }
        }
        else
        {
            throw create_error("Lack of THEN",m_token_ptr->get_line_number());
        }
    }
    else if(m_token_ptr->get_token_type() == TOKEN::KEYWORD_TOKEN && dynamic_cast<KeywordToken *>(m_token_ptr)->get_keyword() == KEYWORD::WHILE)
    {
        WHILEIteratorStmt * while_stmt_ptr = new WHILEIteratorStmt();
        read_token();
        while_stmt_ptr->set_condition(exp());
        read_token();
        std::vector<Stmt *> stmt_vector;
        stmts(stmt_vector);
        for(auto it = stmt_vector.begin(); it != stmt_vector.end(); ++it)
        {
            while_stmt_ptr->add_stmt(*it);
        }
        if(m_token_ptr->get_token_type() == TOKEN::KEYWORD_TOKEN && dynamic_cast<KeywordToken *>(m_token_ptr)->get_keyword() == KEYWORD::WEND)
        {
            read_token();
            return while_stmt_ptr;
        }
        else
        {
            throw create_error("Lack of WEND",m_token_ptr->get_line_number());
        }
    }
    else if(m_token_ptr->get_token_type() == TOKEN::KEYWORD_TOKEN && dynamic_cast<KeywordToken *>(m_token_ptr)->get_keyword() == KEYWORD::DO)
    {
        DOIteratorStmt * do_stmt_ptr = new DOIteratorStmt();
        read_token();
        read_token();
        std::vector<Stmt *> stmt_vector;
        stmts(stmt_vector);
        for(auto it = stmt_vector.begin(); it != stmt_vector.end(); ++it)
        {
            do_stmt_ptr->add_stmt(*it);
        }
        if(m_token_ptr->get_token_type() == TOKEN::KEYWORD_TOKEN && dynamic_cast<KeywordToken *>(m_token_ptr)->get_keyword() == KEYWORD::LOOP)
        {
            read_token();
            if(m_token_ptr->get_token_type() == TOKEN::KEYWORD_TOKEN && dynamic_cast<KeywordToken *>(m_token_ptr)->get_keyword() == KEYWORD::UNTIL)
            {
                read_token();
                do_stmt_ptr->set_condition(exp());
                return do_stmt_ptr;
            }
            else
            {
                throw create_error("Lack of LOOP UNTIL",m_token_ptr->get_line_number());
            }
        }
        else
        {
            throw create_error("Lack of LOOP UNTIL",m_token_ptr->get_line_number());
        }
    }
    else if(m_token_ptr->get_token_type() == TOKEN::DELIMITER_TOKEN && dynamic_cast<DelimiterToken *>(m_token_ptr)->get_delimiter() == DELIMITER::NEW_LINE)
    {
        read_token();
        return stmt();
    }
    else
    {
        return nullptr;
    }
}

std::string * SyntaxAnalyer::prompt()throw (std::string)
{
    if(m_token_ptr->get_token_type() == TOKEN::STRING_TOKEN)
    {

        std::string * str = new std::string(dynamic_cast<StringToken *>(m_token_ptr)->get_string());
        read_token();
        if(!(m_token_ptr->get_token_type() == TOKEN::DELIMITER_TOKEN && dynamic_cast<DelimiterToken *>(m_token_ptr)->get_delimiter() == DELIMITER::SEMICOLON))
        {
            throw create_error("Lack of ;",m_token_ptr->get_line_number());
        }
        read_token();
        return str;
    }
    else
    {
        return nullptr;
    }
}

void SyntaxAnalyer::ids(std::vector<VariableExpression *> & vars_vector)throw(std::string)
{
    if(m_token_ptr->get_token_type() == TOKEN::IDENTIFIER_TOKEN)
    {
        VariableExpression * var_exp_ptr = new VariableExpression();
        var_exp_ptr->set_line_number(m_token_ptr->get_line_number());
        var_exp_ptr->set_variable_name(dynamic_cast<IdentifierToken *>(m_token_ptr)->get_identifier());
        vars_vector.push_back(var_exp_ptr);
        read_token();
        id_Tail(vars_vector);
    }
    else
    {
        throw create_error("Lack of variable",m_token_ptr->get_line_number());
    }
}

void SyntaxAnalyer::id_Tail(std::vector<VariableExpression *> & vars_vector)throw(std::string)
{
    if(m_token_ptr->get_token_type() == TOKEN::DELIMITER_TOKEN && dynamic_cast<DelimiterToken *>(m_token_ptr)->get_delimiter() == DELIMITER::COMMA)
    {
        read_token();
        if(m_token_ptr->get_token_type() == TOKEN::IDENTIFIER_TOKEN)
        {
            VariableExpression * var_exp_ptr = new VariableExpression();
            var_exp_ptr->set_line_number(m_token_ptr->get_line_number());
            var_exp_ptr->set_variable_name(dynamic_cast<IdentifierToken *>(m_token_ptr)->get_identifier());
            vars_vector.push_back(var_exp_ptr);
            read_token();
            id_Tail(vars_vector);
        }
        else
        {
            throw create_error("Lack of ,",m_token_ptr->get_line_number());
        }
    }
}

Expression * SyntaxAnalyer::exp()throw(std::string)
{
    Expression * or_exp_ptr = or_exp();
    if(m_token_ptr->get_token_type() == TOKEN::OPERATOR_TOKEN && dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator() == OPERATOR::AND)
    {
        BinaryExpression * binary_exp = new BinaryExpression();
        binary_exp->set_left_expression(or_exp_ptr);
        binary_exp->set_operator(dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator());
        read_token();
        binary_exp->set_right_expression(or_exp());
        return binary_exp;
    }
    else
    {
        return or_exp_ptr;
    }
}

Expression * SyntaxAnalyer::primary_exp()throw(std::string)
{
    if(m_token_ptr->get_token_type() == TOKEN::IDENTIFIER_TOKEN)
    {
        IdentifierToken id_token(m_token_ptr->get_line_number(),dynamic_cast<IdentifierToken *>(m_token_ptr)->get_identifier());
        read_token();
        if(m_token_ptr->get_token_type() == TOKEN::DELIMITER_TOKEN && dynamic_cast<DelimiterToken *>(m_token_ptr)->get_delimiter() == DELIMITER::LEFT_PARENTHESIS)
        {
            CallExpression * call_exp_ptr = new CallExpression();
            call_exp_ptr->set_line_number(id_token.get_line_number());
            call_exp_ptr->set_function_name(id_token.get_identifier());
            read_token();
            std::vector<Expression *> arg_vector;
            param(arg_vector);
            for(auto it = arg_vector.begin(); it!=arg_vector.end(); ++it)
            {
                call_exp_ptr->add_expression(*it);
            }
            return call_exp_ptr;
        }
        else
        {
            VariableExpression * var_exp_ptr = new VariableExpression();
            var_exp_ptr->set_line_number(id_token.get_line_number());
            var_exp_ptr->set_variable_name(id_token.get_identifier());
            return var_exp_ptr;
        }
    }
    else if(m_token_ptr->get_token_type() == TOKEN::DECIMAL_TOKEN)
    {
        DecimalExpression * decimal_exp_ptr = new DecimalExpression();
        decimal_exp_ptr->set_decimal(dynamic_cast<DecimalToken *>(m_token_ptr)->get_decimal());
        read_token();
        return decimal_exp_ptr;
    }
    else if(m_token_ptr->get_token_type() == TOKEN::DELIMITER_TOKEN && dynamic_cast<DelimiterToken *>(m_token_ptr)->get_delimiter() == DELIMITER::LEFT_PARENTHESIS)
    {
        read_token();
        Expression * exp_ptr = exp();
        if(m_token_ptr->get_token_type() == TOKEN::DELIMITER_TOKEN && dynamic_cast<DelimiterToken *>(m_token_ptr)->get_delimiter() == DELIMITER::RIGHT_PARENTHESIS)
        {
            read_token();
            return exp_ptr;
        }
        else
        {
            throw create_error("Lack of )",m_token_ptr->get_line_number());
        }
    }
    else
    {
        throw create_error("Not id, id(), (expression) or decimal",m_token_ptr->get_line_number());
    }
}

void SyntaxAnalyer::param(std::vector<Expression *> & exp_vector)throw(std::string)
{
    if(m_token_ptr->get_token_type() == TOKEN::DELIMITER_TOKEN && dynamic_cast<DelimiterToken *>(m_token_ptr)->get_delimiter() == DELIMITER::RIGHT_PARENTHESIS)
    {
        read_token();
    }
    else
    {
        std::vector<Expression *> n_exp_vector;
        args(n_exp_vector);
        if(m_token_ptr->get_token_type() == TOKEN::DELIMITER_TOKEN && dynamic_cast<DelimiterToken *>(m_token_ptr)->get_delimiter() == DELIMITER::RIGHT_PARENTHESIS)
        {
            read_token();
            for(auto it = n_exp_vector.begin(); it!=n_exp_vector.end(); ++it)
            {
                exp_vector.push_back(*it);
            }
        }
        else
        {
            throw create_error("Lack of )",m_token_ptr->get_line_number());
        }
    }
}

void SyntaxAnalyer::args(std::vector<Expression *> & exp_vector)throw(std::string)
{
    exp_vector.push_back(exp());
    arg_tail(exp_vector);
}

void SyntaxAnalyer::arg_tail(std::vector<Expression *> & exp_vector)throw(std::string)
{
    if(m_token_ptr->get_token_type() == TOKEN::DELIMITER_TOKEN && dynamic_cast<DelimiterToken *>(m_token_ptr)->get_delimiter() == DELIMITER::COMMA)
    {
        read_token();
        exp_vector.push_back(exp());
        arg_tail(exp_vector);
    }
}

Expression * SyntaxAnalyer::unary_exp()throw(std::string)
{
    if(m_token_ptr->get_token_type() == TOKEN::OPERATOR_TOKEN && (dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator() == OPERATOR::PLUS || dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator() == OPERATOR::SUBSTRACT || dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator() == OPERATOR::NOT))
    {
        UnaryExpression * unary_exp_ptr = new UnaryExpression();
        unary_exp_ptr->set_operator(dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator());
        read_token();
        unary_exp_ptr->set_expression(primary_exp());
        return unary_exp_ptr;
    }
    else
    {
        return primary_exp();
    }
}

Expression * SyntaxAnalyer::power_exp()throw(std::string)
{
    Expression * unary_exp_ptr = unary_exp();
    TailExpression * power_exp_ptr = power_exp_tail();
    if(power_exp_ptr != nullptr)
    {
        BinaryExpression * binary_exp = new BinaryExpression();
        binary_exp->set_left_expression(unary_exp_ptr);
        binary_exp->set_operator(power_exp_ptr->get_operator());
        binary_exp->set_right_expression(power_exp_ptr->get_expression());
        delete power_exp_ptr;
        return binary_exp;
    }
    else
    {
        return unary_exp_ptr;
    }
}

TailExpression * SyntaxAnalyer::power_exp_tail()throw(std::string)
{
    if(m_token_ptr->get_token_type() == TOKEN::OPERATOR_TOKEN && dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator() == OPERATOR::POWER)
    {
        TailExpression * tail_exp_ptr = new TailExpression();
        tail_exp_ptr->set_operator(OPERATOR::POWER);
        read_token();
        Expression * unary_exp_ptr = unary_exp();
        TailExpression * power_exp_tail_ptr = power_exp_tail();
        if(power_exp_tail_ptr != nullptr)
        {
            BinaryExpression * binary_exp_ptr = new BinaryExpression();
            binary_exp_ptr->set_left_expression(unary_exp_ptr);
            binary_exp_ptr->set_operator(power_exp_tail_ptr->get_operator());
            binary_exp_ptr->set_right_expression(power_exp_tail_ptr->get_expression());
            tail_exp_ptr->set_expression(binary_exp_ptr);
            delete power_exp_tail_ptr;
            return tail_exp_ptr;
        }
        else
        {
            tail_exp_ptr->set_expression(unary_exp_ptr);
            return tail_exp_ptr;
        }
    }
    else
    {
        return nullptr;
    }
}

Expression * SyntaxAnalyer::mult_exp()throw(std::string)
{
    Expression * power_exp_ptr = power_exp();
    TailExpression * mult_exp_tail_ptr = mult_exp_tail();
    if(mult_exp_tail_ptr != nullptr)
    {
        BinaryExpression * binary_exp_ptr = new BinaryExpression();
        binary_exp_ptr->set_left_expression(power_exp_ptr);
        binary_exp_ptr->set_operator(mult_exp_tail_ptr->get_operator());
        binary_exp_ptr->set_right_expression(mult_exp_tail_ptr->get_expression());
        delete mult_exp_tail_ptr;
        return binary_exp_ptr;
    }
    else
    {
        return power_exp_ptr;
    }
}

TailExpression * SyntaxAnalyer::mult_exp_tail()throw(std::string)
{
    if(m_token_ptr->get_token_type() == TOKEN::OPERATOR_TOKEN && (dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator() == OPERATOR::MULTIPLY || dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator() == OPERATOR::DIVIDE || dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator() == OPERATOR::DIVIDE_EXACTLY))
    {
        TailExpression * tail_exp_ptr = new TailExpression();
        tail_exp_ptr->set_operator(dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator());
        read_token();
        Expression * power_exp_ptr = power_exp();
        //
        TailExpression * mult_exp_tail_ptr = mult_exp_tail();
        if(mult_exp_tail_ptr != nullptr)
        {
            BinaryExpression * binary_exp_ptr = new BinaryExpression();
            binary_exp_ptr->set_left_expression(power_exp_ptr);
            binary_exp_ptr->set_operator(mult_exp_tail_ptr->get_operator());
            binary_exp_ptr->set_right_expression(mult_exp_tail_ptr->get_expression());
            tail_exp_ptr->set_expression(binary_exp_ptr);
            delete mult_exp_tail_ptr;
            return tail_exp_ptr;
        }
        else
        {
            tail_exp_ptr->set_expression(power_exp_ptr);
            return tail_exp_ptr;
        }
    }
    else
    {
        return nullptr;
    }
}

Expression * SyntaxAnalyer::mod_exp()throw(std::string)
{
    Expression * mult_exp_ptr = mult_exp();
    TailExpression * mod_exp_tail_ptr = mod_exp_tail();
    if(mod_exp_tail_ptr != nullptr)
    {
        BinaryExpression * binary_exp_ptr = new BinaryExpression();
        binary_exp_ptr->set_left_expression(mult_exp_ptr);
        binary_exp_ptr->set_operator(mod_exp_tail_ptr->get_operator());
        binary_exp_ptr->set_right_expression(mod_exp_tail_ptr->get_expression());
        delete mod_exp_tail_ptr;
        return binary_exp_ptr;
    }
    else
    {
        return mult_exp_ptr;
    }
}

TailExpression * SyntaxAnalyer::mod_exp_tail()throw(std::string)
{
    if(m_token_ptr->get_token_type() == TOKEN::OPERATOR_TOKEN && dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator() == OPERATOR::MOD)
    {
        TailExpression * tail_exp_ptr = new TailExpression();
        tail_exp_ptr->set_operator(dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator());
        read_token();
        Expression * mult_exp_ptr = mult_exp();

        TailExpression * mod_exp_tail_ptr = mod_exp_tail();
        if(mod_exp_tail_ptr != nullptr)
        {
            BinaryExpression * binary_exp_ptr = new BinaryExpression();
            binary_exp_ptr->set_left_expression(mult_exp_ptr);
            binary_exp_ptr->set_operator(mod_exp_tail_ptr->get_operator());
            binary_exp_ptr->set_right_expression(mod_exp_tail_ptr->get_expression());
            tail_exp_ptr->set_expression(binary_exp_ptr);
            delete mod_exp_tail_ptr;
            return tail_exp_ptr;
        }
        else
        {
            tail_exp_ptr->set_expression(mult_exp_ptr);
            return tail_exp_ptr;
        }
    }
    else
    {
        return nullptr;
    }
}

Expression * SyntaxAnalyer::add_exp()throw(std::string)
{
    Expression * mod_exp_ptr = mod_exp();
    TailExpression * add_exp_tail_ptr = add_exp_tail();
    if(add_exp_tail_ptr != nullptr)
    {
        BinaryExpression * binary_exp_ptr = new BinaryExpression();
        binary_exp_ptr->set_left_expression(mod_exp_ptr);
        binary_exp_ptr->set_operator(add_exp_tail_ptr->get_operator());
        binary_exp_ptr->set_right_expression(add_exp_tail_ptr->get_expression());
        delete add_exp_tail_ptr;
        return binary_exp_ptr;
    }
    else
    {
        return mod_exp_ptr;
    }
}

TailExpression * SyntaxAnalyer::add_exp_tail()throw(std::string)
{
    if(m_token_ptr->get_token_type() == TOKEN::OPERATOR_TOKEN && (dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator() == OPERATOR::PLUS || dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator() == OPERATOR::SUBSTRACT))
    {
        TailExpression * tail_exp_ptr = new TailExpression();
        tail_exp_ptr->set_operator(dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator());
        read_token();
        Expression * mod_exp_ptr = mod_exp();

        TailExpression * add_exp_tail_ptr = add_exp_tail();
        if(add_exp_tail_ptr != nullptr)
        {
            BinaryExpression * binary_exp_ptr = new BinaryExpression();
            binary_exp_ptr->set_left_expression(mod_exp_ptr);
            binary_exp_ptr->set_operator(add_exp_tail_ptr->get_operator());
            binary_exp_ptr->set_right_expression(add_exp_tail_ptr->get_expression());
            tail_exp_ptr->set_expression(binary_exp_ptr);
            delete add_exp_tail_ptr;
            return tail_exp_ptr;
        }
        else
        {
            tail_exp_ptr->set_expression(mod_exp_ptr);
            return tail_exp_ptr;
        }
    }
    else
    {
        return nullptr;
    }
}

Expression * SyntaxAnalyer::relationship_exp()throw(std::string)
{
    Expression * add_exp_ptr = add_exp();
    TailExpression * relationship_exp_tail_ptr = relationship_exp_tail();
    if(relationship_exp_tail_ptr != nullptr)
    {
        BinaryExpression * binary_exp_ptr = new BinaryExpression();
        binary_exp_ptr->set_left_expression(add_exp_ptr);
        binary_exp_ptr->set_operator(relationship_exp_tail_ptr->get_operator());
        binary_exp_ptr->set_right_expression(relationship_exp_tail_ptr->get_expression());
        delete relationship_exp_tail_ptr;
        return binary_exp_ptr;
    }
    else
    {
        return add_exp_ptr;
    }
}

TailExpression * SyntaxAnalyer::relationship_exp_tail()throw(std::string)
{
    if(m_token_ptr->get_token_type() == TOKEN::OPERATOR_TOKEN && (dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator() == OPERATOR::EQUAL
            || dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator() == OPERATOR::GREATER_THEN
            || dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator() == OPERATOR::LESS_THEN
            || dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator() == OPERATOR::NOT_EQUAL
            || dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator() == OPERATOR::LESS_THEN_OR_EQUAL
            || dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator() == OPERATOR::GREATER_THEN_OR_EQUAL))
    {
        TailExpression * tail_exp_ptr = new TailExpression();
        tail_exp_ptr->set_operator(dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator());
        read_token();
        Expression * add_exp_ptr = add_exp();

        TailExpression * relationship_exp_tail_ptr = relationship_exp_tail();
        if(relationship_exp_tail_ptr != nullptr)
        {
            BinaryExpression * binary_exp_ptr = new BinaryExpression();
            binary_exp_ptr->set_left_expression(add_exp_ptr);
            binary_exp_ptr->set_operator(relationship_exp_tail_ptr->get_operator());
            binary_exp_ptr->set_right_expression(relationship_exp_tail_ptr->get_expression());
            tail_exp_ptr->set_expression(binary_exp_ptr);
            delete relationship_exp_tail_ptr;
            return tail_exp_ptr;
        }
        else
        {
            tail_exp_ptr->set_expression(add_exp_ptr);
            return tail_exp_ptr;
        }
    }
    else
    {
        return nullptr;
    }
}

Expression * SyntaxAnalyer::or_exp()throw(std::string)
{
    Expression * relationship_exp_ptr = relationship_exp();
    TailExpression * or_exp_tail_ptr = or_exp_tail();
    if(or_exp_tail_ptr != nullptr)
    {
        BinaryExpression * binary_exp_ptr = new BinaryExpression();
        binary_exp_ptr->set_left_expression(relationship_exp_ptr);
        binary_exp_ptr->set_operator(or_exp_tail_ptr->get_operator());
        binary_exp_ptr->set_right_expression(or_exp_tail_ptr->get_expression());
        delete or_exp_tail_ptr;
        return binary_exp_ptr;
    }
    else
    {
        return relationship_exp_ptr;
    }
}

TailExpression * SyntaxAnalyer::or_exp_tail()throw(std::string)
{
    if(m_token_ptr->get_token_type() == TOKEN::OPERATOR_TOKEN && dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator() == OPERATOR::OR)
    {
        TailExpression * tail_exp_ptr = new TailExpression();
        tail_exp_ptr->set_operator(dynamic_cast<OperatorToken *>(m_token_ptr)->get_operator());
        read_token();
        Expression * relationship_exp_ptr = relationship_exp();

        TailExpression * or_exp_tail_ptr = or_exp_tail();
        if(or_exp_tail_ptr != nullptr)
        {
            BinaryExpression * binary_exp_ptr = new BinaryExpression();
            binary_exp_ptr->set_left_expression(relationship_exp_ptr);
            binary_exp_ptr->set_operator(or_exp_tail_ptr->get_operator());
            binary_exp_ptr->set_right_expression(or_exp_tail_ptr->get_expression());
            tail_exp_ptr->set_expression(binary_exp_ptr);
            delete or_exp_tail_ptr;
            return tail_exp_ptr;
        }
        else
        {
            tail_exp_ptr->set_expression(relationship_exp_ptr);
            return tail_exp_ptr;
        }
    }
    else
    {
        return nullptr;
    }
}
}
