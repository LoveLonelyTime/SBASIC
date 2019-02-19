#include "lexer.h"
#include "language.h"
#include <cctype>
#include <string>
#include <cmath>

namespace SBASIC
{
TokenReader::TokenReader(std::istream & is) : m_is(is), m_current_char('\0'),m_line_number(1)
{
    read_char();
}

std::string TokenReader::read_digit()
{
    std::string str;
    str += m_current_char;
    for(read_char(); std::isdigit(m_current_char); read_char())
    {
        str += m_current_char;
    }
    return str;
}

std::string TokenReader::read_string() throw(std::string)
{
    std::string n_str;
    n_str += m_current_char;
    for(read_char(); m_current_char != '"'; read_char())
    {
        if(m_current_char == '0' || m_current_char == '\n')
        {
            throw create_error("Not found end \"",m_line_number);
        }
        n_str += m_current_char;
    }
    read_char();
    return n_str;
}

std::string TokenReader::read_word()
{
    std::string str;
    str += m_current_char;
    for(read_char(); std::isalpha(m_current_char) || std::isdigit(m_current_char) || m_current_char == '.'; read_char())
    {
        str += m_current_char;
    }
    return str;
}

void TokenReader::read_char()
{
    static bool CRLF = false;
    if(m_is.get(m_current_char))
    {
        //Success
        if(m_current_char == '\n' || CRLF)
        {
            CRLF = false;
            m_line_number++;
        }
        else if(m_current_char == '\r')
        {
            CRLF = true;
            read_char();
        }
    }
    else
    {
        //EOF
        m_current_char = '\0';
    }
}

Token * TokenReader::read_token() throw(std::string)
{
    //Ignore white characters
    for(; m_current_char == ' ' || m_current_char == '\t'; read_char())
    {
        continue;
    }

    if(std::isdigit(m_current_char))
    {
        // Decimal
        std::string integer = read_digit();
        if (m_current_char == '.')
        {
            read_char();
            if(std::isdigit(m_current_char))
            {
                std::string decimal = read_digit();
                if(m_current_char == 'E' || m_current_char == 'e')
                {
                    read_char();
                    bool negative = false;
                    if(m_current_char == '+' || m_current_char == '-')
                    {
                        negative = (m_current_char == '-');
                        read_char();
                    }
                    if(std::isdigit(m_current_char))
                    {
                        std::string exponent = read_digit();
                        //Integer, decimal and exponent
                        return new DecimalToken(m_line_number,string_to_decimal(integer + "." + decimal) * std::pow(10,negative ? -std::stoi(exponent) : std::stoi(exponent)));
                    }
                    else
                    {
                        throw create_error("Lack of exponent",m_line_number);
                    }
                }
                else
                {
                    //Integer and decimal
                    return new DecimalToken(m_line_number,string_to_decimal(integer + "." + decimal));
                }
            }
            else
            {
                throw create_error("Lack of decimal",m_line_number);
            }
        }
        else if(m_current_char == 'E' || m_current_char == 'e')
        {
            read_char();
            bool negative = false;
            if(m_current_char == '+' || m_current_char == '-')
            {
                negative = (m_current_char == '-');
                read_char();
            }
            if(std::isdigit(m_current_char))
            {
                std::string exponent = read_digit();
                //Integer and exponent
                return new DecimalToken(m_line_number,string_to_decimal(integer) * std::pow(10,negative ? -std::stoi(exponent) : std::stoi(exponent)));
            }
            else
            {
                throw create_error("Lack of exponent",m_line_number);
            }
        }
        else
        {
            //Only integer
            return new DecimalToken(m_line_number,string_to_decimal(integer));
        }
    }
    else if(m_current_char == '"')
    {
        //String
        return new StringToken(m_line_number,read_string());
    }
    else if(std::isalpha(m_current_char))
    {
        //Word
        std::string word = read_word();
        if(is_keyword(word))
        {
            //Keyword
            return new KeywordToken(m_line_number,string_to_keyword(word));
        }
        else if(is_letter_operator(word))
        {
            //Letter operator
            return new OperatorToken(m_line_number,string_to_letter_operator(word));
        }
        else if(word == "REM")
        {
            //Comment
            for(; m_current_char != '\n' && m_current_char != '\0'; read_char())
            {
                continue;
            }
            if(m_current_char == '\n')
            {
                read_char();
            }
            return read_token();
        }
        else
        {
            //Identifier
            return new IdentifierToken(m_line_number,word);
        }
    }
    else if(is_prefix_operator(m_current_char))
    {
        //Operator
        if(m_current_char == '+')
        {
            read_char();
            return new OperatorToken(m_line_number,OPERATOR::PLUS);
        }
        else if(m_current_char == '-')
        {
            read_char();
            return new OperatorToken(m_line_number,OPERATOR::SUBSTRACT);
        }
        else if(m_current_char == '*')
        {
            read_char();
            return new OperatorToken(m_line_number,OPERATOR::MULTIPLY);
        }
        else if(m_current_char == '/')
        {
            read_char();
            return new OperatorToken(m_line_number,OPERATOR::DIVIDE);
        }
        else if(m_current_char == '\\')
        {
            read_char();
            return new OperatorToken(m_line_number,OPERATOR::DIVIDE_EXACTLY);
        }
        else if(m_current_char == '^')
        {
            read_char();
            return new OperatorToken(m_line_number,OPERATOR::POWER);
        }
        else if(m_current_char == '%')
        {
            read_char();
            return new OperatorToken(m_line_number,OPERATOR::MOD);
        }
        else if(m_current_char == '=')
        {
            read_char();
            return new OperatorToken(m_line_number,OPERATOR::EQUAL);
        }
        else if(m_current_char == '>')
        {
            read_char();
            if(m_current_char == '=')
            {
                read_char();
                return new OperatorToken(m_line_number,OPERATOR::GREATER_THEN_OR_EQUAL);
            }
            else
            {
                return new OperatorToken(m_line_number,OPERATOR::GREATER_THEN);
            }
        }
        else
        {
            read_char();
            if(m_current_char == '=')
            {
                read_char();
                return new OperatorToken(m_line_number,OPERATOR::LESS_THEN_OR_EQUAL);
            }
            else if(m_current_char == '>')
            {
                read_char();
                return new OperatorToken(m_line_number,OPERATOR::NOT_EQUAL);
            }
            else
            {
                return new OperatorToken(m_line_number,OPERATOR::LESS_THEN);
            }
        }
    }
    else if(is_delimiter(m_current_char))
    {
        //Delimiter
        if(m_current_char == '\n')
        {
            read_char();
            return new DelimiterToken(m_line_number,DELIMITER::NEW_LINE);
        }
        else if(m_current_char == ';')
        {
            read_char();
            return new DelimiterToken(m_line_number,DELIMITER::SEMICOLON);
        }
        else if(m_current_char == ',')
        {
            read_char();
            return new DelimiterToken(m_line_number,DELIMITER::COMMA);
        }
        else if(m_current_char == '(')
        {
            read_char();
            return new DelimiterToken(m_line_number,DELIMITER::LEFT_PARENTHESIS);
        }
        else
        {
            read_char();
            return new DelimiterToken(m_line_number,DELIMITER::RIGHT_PARENTHESIS);
        }
    }
    else if(m_current_char == '\0')
    {
        //EOF
        return new EOFToken(m_line_number);
    }
    else
    {
        //Error
        std::string error = "Unknown head character : ";
        error += m_current_char;
        throw create_error(error,m_line_number);
    }
}
}
