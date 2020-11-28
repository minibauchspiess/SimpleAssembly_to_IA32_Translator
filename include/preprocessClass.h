#pragma once


#include <fstream>  //Bib para leitura e escrita de arquivos
#include <map>

#include "lineOperations.h"


using namespace std;


/*
 *** Classe que lida com o preprocessamento de um arquivo .asm ***

 * Recebe como argumento o nome do arquivo com o codigo assembly
 * 
 * Realiza as operacoes de preprocessamento:
 *  - Regulariza cada linha:
 *      -- Retira tudo que vem depois do comentario
 *      -- Desconsidera linhas e em branco
 *      -- Junta linhas de label com comando, se necessario
 *      -- Retira espacos desnecessarios
 *      -- Torna tudo upper case
 *  - Executa as diretivas:
 *      -- EQU: mapeia a label com seu valor, desconsidera a linha
 *      -- IF: executa a comparacao e pega a proxima linha. Desconsidera a primeira linha, insere a seguinte somente se verdadeiro
 *      -- MACRO: gera a Macro Name Table (MNT) e a Macro Definition Table (MDT). Desconsidera linhas ate ENDMACRO
 *  - Substitui EQU por seu valor da tabela, se houver
 *  - Expande a macro, se houver
 *  - Insere o resultado das operacoes acima em um arquivo .pre
*/
class preprocessClass
{
private:

    //Arquivos manipulados
    ifstream inputFile;  //Arquivo original assembly, .asm
    ofstream ppFile;     //Arquivo preprocessado, .pre
    string ppFileName;

    //Tabelas
    map<string,string> equ;   //Valores salvos pelo comando EQU
    map<string, pair<uint,uint> > mnt;  //Macro Name Table, contendo a label, qtd de argumentos e local na mdf com seu corpo
    vector<string> mdt; //Macro Definition Table, contendo em cada posicao o corpo de uma macro


public:

    preprocessClass(string inFileName);
    ~preprocessClass();

    //Principal funcao. Percorre o arquivo inteiro, fazendo as adaptacoes necessarias. Monta o arquivo .pre e retorna seu nome. Fecha os arquivos ao final
    string Run();

    //Recebe uma linha do codigo, e retorna uma linha na forma padronizada
    string StandardLine();
    
    //Executa a diretiva, se houver uma na linha. Retorna a linha a ser inserida no arquivo preprocessado
    string ExeDirective(string line);   //Caso seja uma diretia de mais linhas, ela busca mais linhas do arquivo 

    //Funcoes auxiliares de ExeDirective
    string GetDirective(vector<string> tokens);   //Busca a diretiva na lista de tokens (caso haja uma)
    void SaveEqu(vector<string> tokens);   //Executa a diretiva EQU
    bool DirIf(vector<string> tokens);    //Retorna true se a diretiva IF for verdadeira
    void SaveMacro(vector<string> tokens); //Salva a macro nas tabelas. Le linhas adicionais ate encontrar ENDMACRO
    string ReplaceEqu(string line);   //Se houver valores de EQU na linha, os altera para os valores reais. Nao faz nada, caso contrario
    string ExpandMacro(string line);  //Se houver uma macro na linha, retorna uma string com seu corpo, e seus parametros trocados pelos indicados. Caso nao haja, nao faz nada com a linha

};

