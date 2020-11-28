#include "mountClass.h"

mountClass::mountClass(string inFileName){
    //Recupera o nome, sem a extensao   (para criar o .obj a partir do mesmo)
    string rawInFileName = inFileName.substr(0, inFileName.find_last_of('.'));  //Mesma string ate a ultima ocorrencia de '.'

    //Abre os arquivos de leitura e de saida (.obj)
    inputFile.open(inFileName);
    objFile.open(rawInFileName+".obj");

    //Monta as tabelas basicas
    CreateTables();

    //Inicializacao do contador e da flag
    memPos = 0;
    firstLine = true;
}

mountClass::~mountClass(){
    //Libera os mapas
    directives.clear();
    opcodes.clear();
    symbols.clear();

}

void mountClass::Run(){
    cout<<"Fazendo a primeira passagem..."<<endl;
    FirstPassage();
    cout<<"Primeira passagem concluida\nComecando segunda passagem..."<<endl;
    SecondPassage();
    cout<<"Segunda passagem concluida"<<endl;

    //Fecha os arquivos
    if(inputFile.is_open()){
        inputFile.close();
    }
    if(objFile.is_open()){
        objFile.close();
    }
}

void mountClass::FirstPassage(){

    //Percorre o arquivo inteiro procurando labels
    string line;
    vector<string> tokens;
    do{
        //Busca a proxima linha
        getline(inputFile, line);

        //Recupera seus tokens
        tokens = lineOperations::GetTokens(line);

        //Se tiver o token "SECTION", linha sera ignorada
        if(tokens[0] != "SECTION"){
            //Se tiver uma label, salva ela na tabela de simbolos
            if(lineOperations::IsLabel(tokens[0])){
                //Funcao de tratamento para salvar simbolo
                SaveSymbol(tokens);

                //Reduz o contador de posicao na memoria em 1, para a atualizacao com o tamanho de tokens desconsiderar o label
                memPos--;

                //Reduz mais o contador se a linha possuir CONST (3 tokens na linha, mas apenas 1 espaco de memoria)
                if(tokens[1] == "CONST") memPos--;
            }

            //Atualiza o contador de posicao de memoria
            memPos += tokens.size();
        }

    }while(!inputFile.eof());

    //Retorna o iterador de leitura do arquivo ao comeco
    inputFile.seekg(0);
}

void mountClass::SecondPassage(){
    
    string line;
    vector<string> tokens;    

    //Enquanto nao chegar ao fim do arquivo, permanece lendo as linhas e as traduzindo para o .obj
    do{
        //Busca a proxima linha a ser analizada
        getline(inputFile, line);

        //Recupera os tokens da linha
        tokens = lineOperations::GetTokens(line);

        //Se o primeiro token for "SECTION", ignora a linha inteira. Senao, processa a linha
        if(tokens[0] != "SECTION"){

            //Compara cada token utilizado com as tabelas, substitui o que for necessario
            map<string,string>::iterator it;
            for(uint i=0; i<tokens.size(); i++){
                //Caso o primeiro token seja uma label, ele deve ser desconsiderado
                if(lineOperations::IsLabel(tokens[0])) tokens.erase(tokens.begin());

                //Diretivas
                it = directives.find(tokens[i]);
                if(it != directives.end()){
                    tokens = lineOperations::ReplaceToken(tokens, tokens[i], it->second);
                }

                //Opcodes
                it = opcodes.find(tokens[i]);
                if(it != opcodes.end()){
                    tokens = lineOperations::ReplaceToken(tokens, tokens[i], it->second);
                }

                //Simbolos
                it = symbols.find(tokens[i]);
                if(it != symbols.end()){
                    tokens = lineOperations::ReplaceToken(tokens, tokens[i], it->second);
                }

                //Caso haja um token vazio (caso de "CONST", que eh apagado), elimina ele da lista de tokens
                if(tokens[0] == "") tokens.erase(tokens.begin());

            }
            //Remonta a linha obtida e que fora substituida por numeros, sem virgulas
            line = lineOperations::RebuildLine(tokens, false);  //"false" de "nao use virgula"

            //Se nao for a primeira linha criada no arquivo final e nao for uma diretiva const (ja possuira o espaco embutido, "CONST" vira "" no mapa, mas o espaco entre "CONST" e o valor permanece), acrescente um " " antes inserir a linha
            if((!firstLine)) objFile<<" ";
            else firstLine = false;

            //Insere a linha criada no arquivo final
            objFile<<line;
        }

    }while(!inputFile.eof());
}


void mountClass::CreateTables(){
    //Insere as diretivas
    directives.insert({"SPACE","00"});
    directives.insert({"CONST",""});    //O valor da constante eh inserido na leitura do token seguinte
    //A diretiva "SECTION" recebe um tratamento especial, sua linha inteira deve ser desconsiderada (TEXT ou DATA), e isso eh feito na funcao principal

    //Insere os opcodes
    opcodes.insert({"ADD","1"});
    opcodes.insert({"SUB","2"});
    opcodes.insert({"MULT","3"});
    opcodes.insert({"DIV","4"});
    opcodes.insert({"JMP","5"});
    opcodes.insert({"JMPN","6"});
    opcodes.insert({"JMPP","7"});
    opcodes.insert({"JMPZ","8"});
    opcodes.insert({"COPY","9"});
    opcodes.insert({"LOAD","10"});
    opcodes.insert({"STORE","11"});
    opcodes.insert({"INPUT","12"});
    opcodes.insert({"OUTPUT","13"});
    opcodes.insert({"STOP","14"});

}

void mountClass::SaveSymbol(vector<string> tokens){
    //Remove o ":" da label
    tokens[0].pop_back();
    
    //Insere a label na tabela, de acordo com o contador de posicoes do endereco
    symbols.insert({tokens[0],to_string(memPos)});
}
