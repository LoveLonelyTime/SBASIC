#include <iostream>
#include <fstream>
#include "language.h"
#include "analyzer.h"
#include "lexer.h"

using namespace std;
using namespace SBASIC;

int main(int argc,char *argv[])
{
    if(argc == 2)
    {
        try
        {
            char * file_name = argv[1];
            ifstream ifs(file_name);
            TokenReader token_reader(ifs);
            SyntaxAnalyer syntax_analyer(token_reader);
            Program * program = syntax_analyer.analyer();
            FunctionTable function_table;
            VariableTable variable_table(nullptr);
            program->run(function_table,&variable_table);
            syntax_analyer.delete_program(program);
        }
        catch(string & err)
        {
            std::cout << err << endl;
        }
    }
    else
    {
        std::cout << "Need One SBASIC File" << endl;
    }
    return 0;
}
