#pragma once

#include <string.h>
#include <iostream>
#include <vector>

using namespace std;

/*** Classe com funcoes auxiliares de manipulacao de linhas adquiridas de codigo ***
 * 
 * - Possui operacoes de padronizacao de linhas (tirar espacos e '\n' excessivos, tornar uppercase, identificar label e linha vazia...)
 * - Possui operacoes de manipulacao de tokens
*/

class lineOperations
{
private:
    
public:
    lineOperations();
    ~lineOperations();

    //Operacoes com a linha
    static string EraseComment(string line);   //Retorna a linha excluindo tudo escrito apos a ocorrencia de ';' (comentario)
    static bool EmptyLine(string line);    //Indica se a linha esta vazia, ou contem apenas espacos
    static bool LabelSplited(string line); //Indica se a linha possui apenas um label, com o comando inserido em outra linha
    static string SingleSpaced(string line);   //Retira todos os espacos excessivos da linha
    static string MakeUpper(string line);  //Converte todos os caracteres da linha para upper case
    static vector<string> GetLines(string str); //Cada posicao do vetor de strings eh uma das linhas da string passada, sem \n

    //Operacoes de tokens
    static vector<string> GetTokens(string line);  //Divide uma linha em strings, cada uma representando um token da linha
    static vector<string> ReplaceToken(vector<string> tokens, string oldToken, string newToken);   //Substitui um token por outro no vetor
    static string RebuildLine(vector<string> tokens, bool useComma = true);  //Reconstroi uma linha, utilizando os tokens passados
    static bool IsLabel(string token);
    static vector<string> TokensLabelM4(vector<string> tokens);   //Identifica se ha rotulos com + X, e retorna esses 3 tokens como um so, com X*4 (sempre dword)
};
