#include "preprocessClass.h"



preprocessClass::preprocessClass(string inFileName){
    //Recupera o nome, sem a extensao
    string rawInFileName = inFileName.substr(0, inFileName.find_last_of('.'));  //Mesma string ate a ultima ocorrencia de '.'

    //Abre os arquivos de leitura e de saida (preprocessado)
    inputFile.open(inFileName);
    ppFileName = (rawInFileName+".pre");
    ppFile.open(ppFileName);
}

preprocessClass::~preprocessClass(){}


string preprocessClass::Run(){

    cout<<"Inicializando preprocessamento..."<<endl;

    string line;

    //Insere a primeira linha sem acrescentar \n primeiro
    line = StandardLine();
    line = ExeDirective(line);
    while(line == ""){  //Enquanto nao encontra uma linha nao vazia para inserir no arquivo final sem '\n', permanece lendo  
        line = StandardLine();
        line = ExeDirective(line);
        //Se tiver chegado ao final do arquivo sem ler nenhuma linha valida, retorna com o arquivo .pre vazio
        if(inputFile.eof()){
            return ppFileName;
        }
    }
    ppFile<<(line);

    //Insere as demais linhas, sempre colocando primeiro uma quebra de linha
    while (!inputFile.eof()){
        //Busca uma linha padronizada de codigo
        line = StandardLine();

        //Executa qualquer diretiva que haja na linha, e recupera as linhas que devem ir no .pre
        line = ExeDirective(line);

        //Insere a quebra de linha e a proxima linha, caso a linha obtida nao esteja vazia
        if(line != ""){
            ppFile<<("\n"+line);
        }
    }
    

    //Fecha os arquivos
    if(inputFile.is_open()){
        inputFile.close();
    }
    if(ppFile.is_open()){
        ppFile.close();
    }
    
    cout<<"Preprocessamento finalizado"<<endl;
    
    //Retorna o nome do arquivo criado
    return ppFileName;

}

//Funcao de padronizacao da linha
string preprocessClass::StandardLine(){
    string stdLine, line;

    //Pega a proxima linha para padronizar
    getline(inputFile, line);

    //Apaga o comentario que houver, para analisar a linha sem comentario
    line = lineOperations::EraseComment(line);

    //Caso seja uma linha vazia, a desconsidera e busca a seguinte
    while(lineOperations::EmptyLine(line)){
        //Se tiver chegado ao fim do arquivo (linhas vazias ao final do mesmo), retorna com uma linha vazia
        if(inputFile.eof()){
            return "";
        }
        getline(inputFile, line);
        line = lineOperations::EraseComment(line);
    }

    stdLine = line;

    //Caso haja um label dividido em mais de uma linha, mistura mais de uma linha na linha final
    if(lineOperations::LabelSplited(line)){
        //Busca nas proximas linhas ate encontrar uma nao vazia
        do{
            getline(inputFile, line);
            line = lineOperations::EraseComment(line);
        }while(lineOperations::EmptyLine(line));
        stdLine.append(" "+line);
    }

    //Remove todos os espacos desnecessarios da linha criada
    stdLine = lineOperations::SingleSpaced(stdLine);


    //Torna todas as letras maiusculas
    stdLine = lineOperations::MakeUpper(stdLine);

    return stdLine;
}

//Funcoes de execucao de diretivas
string preprocessClass::ExeDirective(string line){
    //Se for uma linha vazia (linhas ao final do codigo, StandardLine nao montou uma linha valida, e sim uma vazia)
    if(line == "") return "";

    //Comeca alterando os valores definidos (EQU)
    line = ReplaceEqu(line);

    //Confere se tem alguma diretiva a ser executada na linha
    vector<string> tokens = lineOperations::GetTokens(line);
    string directive = GetDirective(tokens);

    //Caso sejam as diretivas EQU ou MACRO, salva os novos valores na tabela, e envia uma linha vazia (nao entra no arquivo preprocessado)
    if(directive == "EQU"){
        SaveEqu(tokens);
        return "";
    }
    else if(directive == "MACRO"){
        SaveMacro(tokens);
        return "";
    }
    //Caso seja a diretiva IF, le a linha seguinte, e retorna "" caso seja falso, e trata a linha caso seja verdadeira
    else if(directive == "IF"){

        line = StandardLine();
        if(DirIf(tokens)){
            //Substitui qualquer ocorrencia de EQU e expande qualquer MACRO, caso haja. Assume-se que nao sera outra diretiva
            line = ReplaceEqu(line);
            line = ExpandMacro(line);

            //Se a linha seguinte for uma diretiva EQU, executar a diretiva e retornar uma linha vazia
            if(GetDirective(lineOperations::GetTokens(line)) == "EQU"){
                SaveEqu(lineOperations::GetTokens(line));
                return "";
            }
            return line;
        }
        else{
            return "";
        }
    }
    //Caso nao seja nenhuma das diretivas listadas, subsitui as definicoes EQU na linha e expande se for macro
    else{
        line = ReplaceEqu(line);
        line = ExpandMacro(line);

        return line;
    }


}

string preprocessClass::GetDirective(vector<string> tokens){
    string token;

    if(lineOperations::IsLabel(tokens[0])){ //Caso o primeiro token seja uma label, a possivel diretiva estara no segundo token
        token = tokens[1];
    }
    else{
        token = tokens[0];
    }
    
    //Confere se o token eh uma das diretivas a serem preprocessadas
    if((token == "EQU") || (token == "IF") || (token == "MACRO")){
        return token;  //Se for, retorna a diretiva
    }

    return "";  //Caso contrario, retorna uma string vazia

}

void preprocessClass::SaveEqu(vector<string> tokens){
    tokens[0].pop_back();   //Elimina o ':' antes de enviar pra tabela
    equ.insert({tokens[0] ,tokens[2]});  //Insere os novos valores na tabela
}

bool preprocessClass::DirIf(vector<string> tokens){
    //A diretiva IF sera o segundo token da linha caso haja uma label, e o primeiro caso contrario. O parametro de comparacao sera logo em seguida
    //Qualquer valor diferente de "0" equivale a uma comparacao true
    if(tokens[0] == "IF"){
        if(tokens[1] != "0"){
            return true;
        }
        else{
            return false;
        }
    }
    else{
        if(tokens[2] != "0"){
            return true;
        }
        else{
            return false;
        }
    }
}

void preprocessClass::SaveMacro(vector<string> tokens){
    //Busca os parametros para serem passados pra MNT
    uint nArg, pos;     //Numero de argumentos passados para a macro, e posicao dela na MDT

    nArg = tokens.size() - 2;
    pos = mdt.size();   //Ultima posicao + 1, posicao onde a instrucao sera inserida

    tokens[0].pop_back(); //O primeiro token eh o label, e este permanece com ':' ao final. A label deve ser salva sem ':'

    //Insere o nome da macro (chave), junto com o par de numero de argumentos com posicao do corpo da macro
    mnt.insert({tokens[0], pair<uint,uint>(nArg,pos)});


    //Permanece lendo linhas, ate encontrar a linha em que esta escrito "ENDMACRO", e armazena todas em uma unica string, a ser salva em mdt
    string macroBody, curLine;
    vector<string> newTokens;
    curLine = StandardLine();

    while(curLine != "ENDMACRO"){
        //Recupera os tokens sinalizados na linha
        newTokens = lineOperations::GetTokens(curLine);

        //Substitui os parametros extras por #numerodoparametro
        for(uint i=2; i<tokens.size(); i++){
            newTokens = lineOperations::ReplaceToken(newTokens, tokens[i], "#"+to_string(i-1));
        }
        //Recupera a string, com os parametros atualizados
        curLine = lineOperations::RebuildLine(newTokens);

        //Insere a linha na linha de saida, junto com a quebra de linha, e busca a proxima
        macroBody.append(curLine);
        macroBody.push_back('\n');

        curLine = StandardLine();
    }
    //Retira a quebra de linha da ultima linha da macro (funcoes externas que devem decidir se havera ou nao a quebra de linha)
    macroBody.pop_back();

    //Insere o corpo da macro na Macro Definition Table
    mdt.push_back(macroBody);

}

string preprocessClass::ReplaceEqu(string line){
    //Recupera os tokens, para realizar a troca das definicoes
    vector<string> tokens = lineOperations::GetTokens(line);

    //Identifica a ocorrencia de um valor definido em equ
    map<string,string>::iterator it;
    for(uint i=0; i<tokens.size(); i++){
        it = equ.find(tokens[i]);
        if(it != equ.end()){    //Caso tenha encontrado a definicao, altera o valor na lista de tokens
            tokens = lineOperations::ReplaceToken(tokens, tokens[i], it->second);       //Na lista de token, todos os tokens iguais ao da posicao i se transformarao no valor mapeado em equ
        }
    }

    //Transforma novamente em uma string, e a retorna
    return lineOperations::RebuildLine(tokens);
}

string preprocessClass::ExpandMacro(string line){
    //Recupera os tokens, para facilitar a comparacao e substituicao de tokens
    vector<string> tokens = lineOperations::GetTokens(line);

    //Confere se a macro existe na tabela de macros definida
    map<string,pair<uint,uint>>::iterator it;
    it = mnt.find(tokens[0]);

    //Se nao existir, retorna a linha inalterada
    if(it == mnt.end()){
        return line;
    }
    //Se existir, recupera o corpo da macro
    string body = mdt[it->second.second];

    //Se houverem parametros a serem substituidos, separa as linhas do corpo da macro, para a substituicao
    if(it->second.first>0){
        //Divide o corpo da macro em linhas
        vector<string> lines = lineOperations::GetLines(body);

        //Vetor para receber os tokens da nova linha
        vector<string> lineTokens;

        //Para cada linha, substitui os parametros passados no corpo
        for(uint i=0; i<lines.size(); i++){
            //Separa a linha em tokens, para facilitar a substituicao
            lineTokens = lineOperations::GetTokens(lines[i]);

            //Tenta fazer a substituicao para cada argumento possivel
            for(uint paramInd=1; paramInd<=it->second.first; paramInd++){
                lineTokens = lineOperations::ReplaceToken(lineTokens, ("#"+to_string(paramInd)), tokens[paramInd]);
            }

            //Atualiza a linha com seus parametros inseridos
            lines[i] = lineOperations::RebuildLine(lineTokens);
        }

        //Insere todas as linhas em uma unica string, com \n
        body = lines[0];
        for(uint i=1; i<lines.size(); i++){
            body.push_back('\n');
            body.append(lines[i]);
        }
    }

    return body;

}

