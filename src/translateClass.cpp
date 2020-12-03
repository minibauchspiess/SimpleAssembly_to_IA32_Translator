#include "translateClass.h"

translateClass::translateClass(string inFileName){
    //Recupera o nome, sem a extensao   (para criar o .s a partir do mesmo)
    string rawInFileName = inFileName.substr(0, inFileName.find_last_of('.'));  //Mesma string ate a ultima ocorrencia de '.'

    //Abre os arquivos de leitura e de saida (.obj)
    inputFile.open(inFileName);
    outFile.open(rawInFileName+".s");

    //Inicializa as strings auxiliares com seus respectivos comecos
    InitSecStrings();

    //Para retirar lixo de memoria de secFlag
    secFlag = 0;

}

translateClass::~translateClass(){
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
        secBss.append(line);
        secBss.append("\n");
    }while(!secFile.eof());
    secFile.close();

    //Secao .text
    secFile.open("IA32_Sections/Text");
    do{
        getline(secFile, line);
        secText.append(line);
        secText.append("\n");
    }while(!secFile.eof());
    secFile.close();

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


    //Recupera a instrucao (eh ou o primeiro ou o segundo token (caso tenha label no comeco))
    string inst;
    int arg1Pos;    //Onde encontrar o proximo argumento da instrucao
    if(lineOperations::IsLabel(tokens[0])){
        inst = tokens[1];
        transLine = tokens[0];          //tokens[0] possui ':' no final, que sera retirado a depender da instrucao
        arg1Pos = 2;
    }
    else{
        inst = tokens[0];
        transLine = "";
        arg1Pos = 1;
    }

    if(inst == "SPACE"){
        secFlag = SEC_BSS;
        transLine.pop_back();       //Nessas instrucoes, a label nao possui ":"

        if(tokens.size() == 3){
            transLine = transLine + " resd "+tokens[arg1Pos];    //Para SPACE e CONST, sempre ha uma label no comeco
        }
        else{
            transLine = transLine + " resd 1";
        }
    }
    else if(inst == "CONST"){
        secFlag = SEC_DATA;
        transLine.pop_back();
        transLine = transLine + " dd " + tokens[arg1Pos];
    }
    else{
        //Todas as seguintes instrucoes entram em .text
        secFlag = SEC_TEXT;

        //E eis o switch enorme com todas as traducoes (if else pq switch so aceita numero)
        if(inst == "ADD"){
            transLine = transLine + "add eax, [" + tokens[arg1Pos] + "]";
        }
        else if(inst == "SUB"){
            transLine = transLine + "sub eax, [" + tokens[arg1Pos] + "]";
        }
        else if(inst == "MULT"){
            transLine = transLine                               //Caso haja uma label, transLine ja iniciara com ela
                    +   string("push edx\n")                    //edx eh utilizado no imul, seu valor deve ser preservado
                    +   string("imul dword [") + tokens[arg1Pos] + "]\n"    //Instrucao em si
                    +   string("jo _Overflow\n")                //Checagem de overflow
                    +   string("pop edx");                      //Recupera valor de edx
        }
        else if(inst == "DIV"){
            transLine = transLine                               //Caso haja uma label, transLine ja iniciara com ela
                    +   string("push edx\n")                    //edx eh utilizado no idiv, seu valor deve ser preservado
                    +   string("cdq\n")                         //Extende sinal de eax para edx
                    +   string("idiv dword [") + tokens[arg1Pos] + "]\n"    //Instrucao em si
                    +   string("pop edx");                      //Recupera valor de edx
        }
        else if(inst == "JMP"){
            transLine = transLine + "jmp " + tokens[arg1Pos];
        }
        else if(inst == "JMPN"){
            transLine = transLine 
                    +   string("cmp eax, 0\n")
                    +   string("jl " + tokens[arg1Pos]);
        }
        else if(inst == "JMPP"){
            transLine = transLine 
                    +   string("cmp eax, 0\n")
                    +   string("jg " + tokens[arg1Pos]);
        }
        else if(inst == "JMPZ"){
            transLine = transLine 
                    +   string("cmp eax, 0\n")
                    +   string("je " + tokens[arg1Pos]);
        }
        else if(inst == "COPY"){
            transLine = transLine 
                    +   string("push ebx\n")
                    +   string("mov ebx, [") + tokens[arg1Pos] + string("]\n")
                    +   string("mov [")+tokens[arg1Pos+1]+string("], ebx\n")
                    +   string("pop ebx");
        }
        else if(inst == "LOAD"){
            transLine = transLine + string("mov eax, [") + tokens[arg1Pos] + string("]");
        }
        else if(inst == "STORE"){
            transLine = transLine + string("mov [") + tokens[arg1Pos] + string("], eax");
        }
        else if(inst == "INPUT"){
            transLine = transLine 
                    +   string("push eax\n")
                    +   string("push dword ") + tokens[arg1Pos] + string("\n")
                    +   string("call _LeerInteiro\n")
                    +   string("mov [") + tokens[arg1Pos] + string("], eax\n")  //Saida da funcao em eax
                    +   string("pop eax");
        }
        else if(inst == "OUTPUT"){
            transLine = transLine 
                    +   string("push eax\n")
                    +   string("push dword ") + tokens[arg1Pos] + string("\n")
                    +   string("call _EscreverInteiro\n")
                    +   string("pop eax");
        }
        else if(inst == "C_INPUT"){
            transLine = transLine 
                    +   string("push eax\n")
                    +   string("push dword ") + tokens[arg1Pos] + string("\n")
                    +   string("call _LeerChar\n")
                    +   string("pop eax");
        }
        else if(inst == "C_OUTPUT"){
            transLine = transLine 
                    +   string("push eax\n")
                    +   string("push dword ") + tokens[arg1Pos] + string("\n")
                    +   string("call _EscreverChar\n")
                    +   string("pop eax");
        }
        else if(inst == "S_INPUT"){
            transLine = transLine 
                    +   string("push eax\n")
                    +   string("push dword ") + tokens[arg1Pos] + string("\n")
                    +   string("push dword ") + tokens[arg1Pos+1] + string("\n")
                    +   string("call _LeerString\n")
                    +   string("pop eax");
        }
        else if(inst == "S_OUTPUT"){
            transLine = transLine 
                    +   string("push eax\n")
                    +   string("push dword ") + tokens[arg1Pos] + string("\n")
                    +   string("push dword ") + tokens[arg1Pos+1] + string("\n")
                    +   string("call _EscreverString\n")
                    +   string("pop eax");
        }
        else if(inst == "STOP"){
            transLine = transLine 
                    +   string("mov eax, 1\n")
                    +   string("mov ebx, 0\n")
                    +   string("int 80h");
        }
        else{
            return line;
        }
        
    }

    return transLine;
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
