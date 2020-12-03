#pragma once


#include <fstream>  //Bib para leitura e escrita de arquivos
#include <map>

#include "lineOperations.h"

//Definicoes para auxilio no uso da flag secFlag
#define SEC_DATA 0
#define SEC_BSS 1
#define SEC_TEXT 2


using namespace std;


/*
 *** Classe que traduz um arquivo na linguagem inventada assembly (.asm) para linguagem IA32 (.s) ***

 * Recebe como argumento o nome do arquivo com o codigo assembly inventado
 * 
 *  - Tratamento de SECTION TEXT e SECTION DATA:
 *      - Existem 3 strings auxiliares, utilizadas para armazenar o código traduzido que entram nas secoes .text, .data e .bss
 *      - O codigo eh traduzido por completo para essas strings (selecionadas a depender da secao sendo lida)
 *      - Apos traduzido, as 3 strings sao inseridas na ordem certa no arquivo de saida
 *  - Tradução:
 *      - Cada instrução no assembly inventado é mapeado a um conjunto de instruções em IA32 (comando switch)
 * 
 * 
*/
class translateClass
{
private:

    //Arquivos manipulados
    ifstream inputFile;  //Arquivo original assembly, .asm
    ofstream outFile;     //Arquivo traduzido, .s


    //Secoes do codigo (texto, dados e variaveis)
    string secText;   //section .text do IA32 (escrita no arquivo .s)
    string secData;   //section .data do IA32 (escrita no arquivo .s)
    string secBss;    //section .bss do IA32 (escrita no arquivo .s)


    //Flags de controle
    int secFlag;    //Flag indicando a qual sessao o trecho traduzido pertence (0 para .data, 1 para .bss e 2 para .text)


public:

    translateClass(string inFileName);  //Abre os arquivos de entrada e saída, inicializa as strings de sessao
    ~translateClass();

    //Principal funcao. Le linha e chama ciclicamente a funcao de traducao de instrucao, e insere as linhas traduzidas na posicao correta do codigo de saida
    void Run();

    //Funcoes de inicializacao
    void InitSecStrings();  //Insere .data, .bss ou .text no comeco, insere as funcoes de escrita/leitura e possiveis \n no comeco
    
    //Funcao que determina a traducao apropriada da linha, baseada na tabela de traducoes, e atualiza secFlag de acordo com o que tiver sido lido
    string TranslateLine(string line);
    void WriteLine(string line);

};

