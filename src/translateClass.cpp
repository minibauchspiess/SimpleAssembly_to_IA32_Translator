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

        //Caso tenha label + valor, serao 3 tokens se transformando no unico token label+4*valor
        vector<string> tokens = lineOperations::GetTokens(line);
        tokens = lineOperations::TokensLabelM4(tokens);
        line = lineOperations::RebuildLine(tokens);

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
    //As sessoes de inicializacao estao presentes nos arquivos da pasta 'IA32_Sections/'
    ifstream secFile;   //Stream que abrira cada um dos arquivos
    string line;
    //Secao .data
    secFile.open("IA32_Sections/Data");
    do{
        getline(secFile, line);
        secData.append(line);
        secData.append("\n");
    }while(!secFile.eof());
    secFile.close();

    //secData = "section .data";
    //secData = secData + "\nzero db 0";

    //Secao .bss
    secFile.open("IA32_Sections/Bss");
    do{
        getline(secFile, line);
        secData.append(line);
        secData.append("\n");
    }while(!secFile.eof());
    secFile.close();

    //Secao .text
    secFile.open("IA32_Sections/Text");
    do{
        getline(secFile, line);
        secData.append(line);
        secData.append("\n");
    }while(!secFile.eof());
    secFile.close();

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
    //translations.insert({"ADD","add"});

}

string translateClass::TranslateLine(string line){
    //Linha traduzida
    string transLine;
    //Recupera os tokens
    vector<string> tokens = lineOperations::GetTokens(line);
    
    //Se o primeiro token for de sessao, retorna uma linha vazia (sera pulada no arquivo final, mas c'est la vie)
    if(tokens[0] == "SECTION"){
        return "";
    }

    //Tratamento das coisas que vao pra section .data ou .bss
    if(lineOperations::IsLabel(tokens[0])){ //Sendo label, o comando seguinte sera SPACE ou CONST
        //Apaga o ':' no final do token, para inserir no arquivo final
        tokens[0].pop_back();
        
        if(tokens[1] == "SPACE"){
            secFlag = SEC_BSS;
            
            if(tokens.size() == 3){
                transLine = tokens[0]+ " dd "+tokens[2];
            }
            else{
                transLine = tokens[0]+" dd 1";
            }
            
        }
        else{       //Se nao for SPACE, sera CONST (a menos que o usuario seja troll)
            secFlag = SEC_DATA;
            transLine = tokens[0] + " resd " + tokens[2];
        }
    }
    else{   //Qualquer diretiva aqui sera inserida na secao .text
        secFlag = SEC_TEXT;

        //E eis o switch enorme com todas as traducoes (if else pq switch so aceita numero)
        if(tokens[0] == "ADD"){
            
        }

    }

    return line;
}

void translateClass::WriteLine(string line){
    line = string("\n")+line;
    switch(secFlag){
        case SEC_DATA:
            secData = secData+line;
            break;
        case SEC_BSS:
            secBss = secBss+line;
            break;
        case SEC_TEXT:
            secText = secText+line;
            break;
    }
}
