// in this folder you will create you files
// for pengu how to build your files 
// have you remember ? i got a doubt 
// can you paste the commands here
// cl /EHsc name.cpp
// that it ?? 
// ok 
// in this folder you can create your files for the ches game
#include<iostream>
#include<string>
#include "parsingfen.h"
using namespace std;
int main()
{
    //string fenString = "1q4n1/1n4Q1/pp2pk2/PP5p/3P2PP/1pr5/4P1K1/2b2B1R b - - 5 41";
    string fenString = "1rbq3r/ppp1npb1/4k3/3pp2p/NR1P2P1/2P5/P3PP1P/B3KB1R b K - 1 18";
    Algo::timeLimitedIterativeDeepeningABMInMax(fenString);
    //cout<<c.getRandomMove()<<endl;
    cout << "Bye!" << endl;
    return 0;
}