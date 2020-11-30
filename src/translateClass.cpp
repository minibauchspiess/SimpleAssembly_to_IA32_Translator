#include "translateClass.h"

translateClass::translateClass(string inFileName){
    //Recupera o nome, sem a extensao   (para criar o .s a partir do mesmo)
    string rawInFileName = inFileName.substr(0, inFileName.find_last_of('.'));  //Mesma string ate a ultima ocorrencia de '.'

    //Abre os arquivos de leitura e de saida (.obj)
    inputFile.open(inFileName);
    outFile.open(rawInFileName+".s");

    //Inicializa as strings auxiliares com seus respectivos comecos
    InitSecStrings();

    //Inicializa a tabela com traducoes
    InitTranslateTable();

}

translateClass::~translateClass(){
    //Libera os mapas
    translations.clear();
}

void translateClass::Run(){

    string line;    
    do{
        //Busca a proxima linha
        getline(inputFile, line);

        //Traduz a linha (sera vazia caso seja de SECTION TEXT)
        line = TranslateLine(line);

        //Insere a linha na string apropriada (a funcao reconhece a localicacao correta)
        WriteLine(line);

    }while(!inputFile.eof());

    //Com todas as strings completas, estas sao inseridas no arquivo de saida
    outFile<<secData;
    outFile<<secBss;
    outFile<<secText;

    //Fecha os arquivos utilizados
    if(inputFile.is_open()){
        inputFile.close();
    }
    if(outFile.is_open()){
        outFile.close();
    }
}

void translateClass::InitSecStrings(){
    //Secao .data
    secData = "section .data";
    secData = secData + "\nzero db 0";

    //Secao .bss
    secBss = "\nsection .bss";
    secBss = "\nacc resb 1";    //Para substituir o acumulador presente no assembly inventado

    //Secao .text
    secText = "\nsection .text\n";
    secText = secText + "";

}

void translateClass::InitTranslateTable(){
    //Insercao uma a uma das traducoes corretas na tabelas de traducoes
    //ADD END -> add [acc], END
    //SUB END -> sub [acc], END
    //MULT END -> mul [acc], END
    //DIV END -> div [acc], END
    //JMP END -> jmp END
    //JMPN END -> cmp [acc], zero \n jl END
    //JMPP END -> cmp [acc], zero \n jg END
    //JMPZ END -> cmp [acc], zero \n je END
    //COPY END1, END2 -> 
    //LOAD END -> 
    //STORE END -> 
    //INPUT END -> 
    //OUTPUT END -> 
    //C_INPUT END -> 
    //C_OUTPUT END -> 
    //S_INPUT END, IM -> 
    //S_OUTPUT END, IM -> 
    translations.insert({"ADD","add"});

}

string translateClass::TranslateLine(string line){
    return line;
}

void translateClass::WriteLine(string line){

}
