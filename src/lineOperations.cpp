#include "lineOperations.h"

lineOperations::lineOperations(){}

lineOperations::~lineOperations(){}


string lineOperations::EraseComment(string line){
    //Busca a posicao de ';' na string
    size_t comentPos = line.find_first_of(';');

    //Caso tenha encontrado, apaga tudo que esta escrito apos o ';'
    if(comentPos != string::npos){
        line.erase(comentPos);
    }

    //Retorna a linha reduzida
    return line;
}

bool lineOperations::EmptyLine(string line){
    //Percorre a string procurando algum char diferente de ' '
    size_t pos = line.find_first_not_of(' ');

    //Caso nao tenha encontrado, retorna que a linha esta vazia (true)
    if(pos == string::npos){
        return true;
    }
    //Caso contrario, a linha nao esta vazia
    else{
        return false;
    }
}

bool lineOperations::LabelSplited(string line){
    //Detecta a presenca de um label (indicado pela presenca de ':')
    size_t labelPos = line.find_first_of(':');

    //Caso nao tenha achado nenhuma label, a linha ja nao estara dividida
    if(labelPos == string::npos){
        return false;
    }

    //Caso tenha encontrado uma label, verifica se ha algo escrito apos ela
    size_t comPos = line.find_first_not_of(' ', labelPos+1);

    //Caso haja um comando depois da label, a linha nao esta dividida
    if(comPos != string::npos){
        return false;
    }
    //Caso contrario, o resto do comando esta em linhas posteriores
    else{
        return true;
    }
}

string lineOperations::SingleSpaced(string line){
    //Char que percorre a string comparando cada elemento seu
    char c, lastC = 'a';

    //Indice que percorre a string
    size_t pos = line.find_first_not_of(' ');

    //String compactada, de saida
    string outString;

    //Percorre a string enquanto ainda houverem elementos em seu interior a serem analisados
    while ((pos < line.length()) && (pos != string::npos)){
        c = line[pos];  //Busca novo valor a ser analizado

        //Dependendo do char lido, o padrao de tirar linhas podera ser diferente. Todos os espacos da string sao desconsiderados, e sao adicionados novos onde necesario
        if((c == ':') || (c == ',')){   //Padrao: sem espacos antes, com espaco depois. Depois pula para o proximo item
            outString.push_back(c);
            outString.push_back(' ');

            lastC = c;
            pos = line.find_first_not_of(' ', pos+1);
        }
        else if(c == ' '){  //Padrao: Ignora. Depois pula para o proximo valor que nao e espaco
            lastC = c;
            pos = line.find_first_not_of(' ', pos+1);
        }
        else if(lastC == ' '){  //Eh uma letra ou '_' e o char anterior foi ' ' (indica comeco da nova palavra). Padrao: insere o espaco e a letra. Depois segue adiante
            outString.push_back(' ');
            outString.push_back(c);

            lastC = c;
            pos++;
        }
        else{   //Eh uma letra ou '_', e o char anterior tambem o foi (indica meio da palavra). Padrao: apenas insere o char
            outString.push_back(c);

            lastC = c;
            pos++;
        }
    }
    
    //Retorna a string montada
    return outString;

}

string lineOperations::MakeUpper(string line){
    locale loc;
    for(uint i=0; i<line.length(); i++){
        line[i] = toupper(line[i], loc);
    }
    return line;
}

vector<string> lineOperations::GetLines(string str){
    size_t startPos, endPos;
    vector<string> lines;

    //Busca as posicoes onde comeca e termina a linha
    startPos = 0;
    endPos = str.find_first_of('\n', startPos);

    //Enquanto a string nao tiver acabado, permanece lendo as linhas
    while (endPos != string::npos){
        //Transfere o trecho da string para uma substring, inserida no final do vetor
        lines.push_back(str.substr(startPos, endPos-startPos));

        //Atualiza os iteradores de str
        startPos = str.find_first_not_of('\n', endPos);
        endPos = str.find_first_of('\n', startPos);
    }

    //Insere o ultimo item (startPos esta no lugar certo, endPos encontrou o final da string)
    if(startPos != string::npos){   //Se nao cair nessa condicao, eh pq o normalizador de linhas nao fez um bom trabalho (tem espaco em branco no final)
        lines.push_back(str.substr(startPos));
    }
    
    return lines;
}


//Funcoes de operacoes com tokens
vector<string> lineOperations::GetTokens(string line){
    size_t startPos, endPos;
    vector<string> tokens;

    //Busca as posicoes onde comeca e termina a string
    startPos = line.find_first_not_of(' ');
    endPos = line.find_first_of(' ', startPos);

    //Enquanto a string nao tiver acabado, permanece lendo os tokens
    while (endPos != string::npos){
        //Transfere o trecho da string para uma substring, inserida no final do vetor
        tokens.push_back(line.substr(startPos, endPos-startPos));

        //Elimina a presenca de ',', caso haja (':' eh mantido, para reconhecer se eh uma label)
        if(tokens.back().back() == ','){    //Ultimo char do ultimo elemento do vetor
            tokens.back().pop_back();
        }

        //Atualiza os iteradores de line
        startPos = line.find_first_not_of(' ', endPos);
        endPos = line.find_first_of(' ', startPos);
    }

    //Insere o ultimo item (startPos esta no lugar certo, endPos encontrou o final da string)
    if(startPos != string::npos){   //Se nao cair nessa condicao, eh pq o normalizador de linhas nao fez um bom trabalho (tem espaco em branco no final)
        tokens.push_back(line.substr(startPos));
    }
    
    return tokens;
}

vector<string> lineOperations::ReplaceToken(vector<string> tokens, string oldToken, string newToken){
    //Percorre o vetor. Caso encontre uma ocorrencia de oldToken, a substitui por newToken
    for(uint i=0; i<tokens.size(); i++){
        if(tokens[i] == oldToken){
            tokens[i] = newToken;
        }
    }

    return tokens;
}

string lineOperations::RebuildLine(vector<string> tokens, bool useComma){
    uint labelAdd=0;
    if(IsLabel(tokens[0])) labelAdd = 1;    //Usado na logica para inserir as virgulas

    string outString = tokens[0];

    for(uint i=1; i<tokens.size(); i++){
        //Se nao for o ultimo valor (continua no for), mas esta numa posicao onde deveria haver ',', insere a ','
        if((i >= 2+labelAdd) && useComma){
            outString.push_back(',');
        }
        //Insere espaco e o proximo token
        outString.push_back(' ');
        outString = outString.append(tokens[i]);
    }

    return outString;
}

bool lineOperations::IsLabel(string token){
    return token.back() == ':';
}
