#include <iostream>
#include <string>

#include<stdio.h>

using namespace std;
int main(int argc, char *argv[])
{
    string line;
    while(getline(cin, line)){
    	cout << line <<endl;
    }

}