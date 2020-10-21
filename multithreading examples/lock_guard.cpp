/* 
	This program uses lock_guard to replace m.lock() and m.unlock()
	lock will be released once it goes out of scope 
*/
#include <thread>
#include <string>
#include <mutex>
#include <iostream>
#include <fstream>
using namespace std;


mutex m;
	
/* A shared function used by many threads concurrently */
void print_function(string msg, int num){
	lock_guard<mutex> locker(m);
	cout << msg << num << endl;
}


void function_1(){
	for(int i = 0; i < 100; i++){
		string message = "Hello, ";
		print_function(message, i);
	}
}

int main(){

	thread t1(function_1);
	for(int i = 0; i >-100; i--){
		string message = "Hello, ";
		print_function(message, i);
	}
	t1.join();
}
