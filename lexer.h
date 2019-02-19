#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED

#include <iostream>
#include <string>
#include "language.h"
namespace SBASIC
{
//Token
class Token
{
private:
    line_number m_line_number;
public:
    Token(line_number ln) : m_line_number(ln) {};
    virtual ~Token() {}
    line_number get_line_number() const
    {
        return m_line_number;
    }
    virtual TOKEN get_token_type() const =0;
};
class DecimalToken : public Token
{
private:
    sbasic_decimal_type m_decimal;
public:
    DecimalToken(line_number ln,sbasic_decimal_type decimal) : Token(ln),m_decimal(decimal) {}
    sbasic_decimal_type get_decimal() const
    {
        return m_decimal;
    }
    TOKEN get_token_type() const
    {
        return TOKEN::DECIMAL_TOKEN;
    }
};
class IdentifierToken : public Token
{
private:
    std::string m_identifier;
public:
    IdentifierToken(line_number ln,const std::string & id) : Token(ln),m_identifier(id) {}
    std::string get_identifier() const
    {
        return m_identifier;
    }
    TOKEN get_token_type() const
    {
        return TOKEN::IDENTIFIER_TOKEN;
    }
};
class NewlineToken : public Token
{
public:
    NewlineToken(line_number ln) : Token(ln) {}
    TOKEN get_token_type() const
    {
        return TOKEN::NEWLINE_TOKEN;
    }
};
class StringToken : public Token
{
private:
    std::string m_string;
public:
    StringToken(line_number ln, const std::string & str) : Token(ln), m_string(str) {}
    std::string get_string() const
    {
        return m_string;
    }
    TOKEN get_token_type() const
    {
        return TOKEN::STRING_TOKEN;
    }
};
class KeywordToken : public Token
{
private:
    KEYWORD m_keyword;
public:
    KeywordToken(line_number ln, KEYWORD keyword) : Token(ln),m_keyword(keyword) {}
    KEYWORD get_keyword() const
    {
        return m_keyword;
    }
    TOKEN get_token_type() const
    {
        return TOKEN::KEYWORD_TOKEN;
    }
};
class EOFToken : public Token
{
public:
    EOFToken(line_number ln) : Token(ln) {}
    TOKEN get_token_type() const
    {
        return TOKEN::EOF_TOKEN;
    }
};
class OperatorToken : public Token
{
private:
    OPERATOR m_operator;
public:
    OperatorToken(line_number ln, OPERATOR op) : Token(ln),m_operator(op) {}
    OPERATOR get_operator() const
    {
        return m_operator;
    }
    TOKEN get_token_type() const
    {
        return TOKEN::OPERATOR_TOKEN;
    }
};
class DelimiterToken : public Token
{
private:
    DELIMITER m_delimiter;
public:
    DelimiterToken(line_number ln, DELIMITER delimiter) : Token(ln),m_delimiter(delimiter) {}
    DELIMITER get_delimiter() const
    {
        return m_delimiter;
    }
    TOKEN get_token_type() const
    {
        return TOKEN::DELIMITER_TOKEN;
    }
};

//TokenReader
class TokenReader
{
private:
    std::istream & m_is;
    char m_current_char;
    line_number m_line_number;
    void read_char();
    std::string read_digit();
    std::string read_string() throw(std::string);
    std::string read_word();
    std::string create_error(const std::string & error,line_number ln)
    {
        return "Line: " + std::to_string(ln) + ", Error: " + error;
    }
public:
    TokenReader(std::istream & is);
    Token * read_token() throw(std::string);
    void delete_token(Token * token_ptr)
    {
        delete token_ptr;
    }
};
}

#endif // LEXER_H_INCLUDED
