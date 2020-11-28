
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <locale>
#include <map>
#include <vector>

#include "preprocessClass.h"
#include "mountClass.h"

using namespace std;


int main(int argc, char* argv[]){

    //Caso passe uma diretiva de compilacao (-p ou -o), executa somente a etapa requerida
    if(argc==3){
        string fileName(argv[2]);

        string ppCommand="-p", mComannd = "-o";

        //Caso seja -p, apenas preprocessamento eh feito, fileName eh um .asm
        if(argv[1] == ppCommand){
            preprocessClass ppObj(fileName);
            ppObj.Run();
        }
        //Caso seja -o, apenas montagem, filename eh um .pre
        else if(argv[1] == mComannd){
            mountClass mountObj(fileName);
            mountObj.Run();
        }
    }
    //Caso nao haja uma diretiva de compilacao, preprocessa e monta
    else{
        string fileName(argv[1]), ppFileName;

        //Preprocessa
        preprocessClass ppObj(fileName);
        ppFileName = ppObj.Run();

        //Monta
        mountClass mountObj(ppFileName);
        mountObj.Run();
    }

    return 1;
}