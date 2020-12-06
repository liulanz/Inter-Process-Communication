#include <iostream>
using namespace std;

int main(int argc, char* argv[]){
	char *p;
	long arg = strtol(argv[1], &p, 10);
	int x = arg;
	for(int i = 0; i < x; i++){
		cout << "hello world" <<endl;
	}
	
}