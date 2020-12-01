
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

    string a = string("um dois")
    + string(", feijao com arroz")
    + string("\n5, 6")
    + string(", molho ingles")
    ;
    //a = a+", feijao com arroz";

    cout<<a<<endl;

    return 1;
}