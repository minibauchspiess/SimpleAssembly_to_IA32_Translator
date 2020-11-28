#pragma once


#include <fstream>  //Bib para leitura e escrita de arquivos
#include <map>

#include "lineOperations.h"


using namespace std;

/*
 *** Classe que lida com o montador de 2 passagens ***

 * Recebe como argumento o nome do arquivo com o codigo preprocessado
 * 
 * Monta a tabela de simbolos de labels na primeira passagem
 * 
 * Na segunda passagem, substitui as diretivas, opcodes e simbolos por seus valores respectivos
*/
class mountClass
{
private:
    //Arquivos manipulados
    ifstream inputFile;  //Arquivo original preprocessado, .pre
    ofstream objFile;     //Arquivo montado, .obj

    //Tabelas
    map<string,string> symbols;
    map<string,string> directives;
    map<string,string> opcodes;

    //Contador de espacos de memoria
    uint memPos;

    //Indicador se eh a primeira linha a ser passada ao objeto de saida
    bool firstLine;


public:
    mountClass(string inFileName);
    ~mountClass();

    void Run();     //Executa as duas passagens

    void FirstPassage();    //Realiza a primeira passagem pelo arquivo, salvando simbolos na tabela de simbolos
    void SecondPassage();   //Gera o arquivo de saida, baseado na tabela de diretivas, opcodes e simbolos

    void CreateTables();    //Insere dados nas tabelas de diretivas e opcodes
    void SaveSymbol(vector<string> tokens); //Salva o simbolo na tabela
    
};

