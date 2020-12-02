
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <locale>
#include <map>
#include <vector>

#include "lineOperations.h"
#include "preprocessClass.h"
#include "translateClass.h"

using namespace std;


int main(int argc, char* argv[]){
    preprocessClass ppObj(argv[1]);
    string ppFileName = ppObj.Run();
    
    translateClass transObj(ppFileName);
    transObj.Run();


    return 1;
}