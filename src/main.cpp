
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <locale>
#include <map>
#include <vector>

#include "translateClass.h"

using namespace std;


int main(int argc, char* argv[]){

    translateClass transObj(argv[1]);
    transObj.Run();


    return 1;
}