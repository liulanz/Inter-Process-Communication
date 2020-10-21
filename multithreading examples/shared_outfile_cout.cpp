/*
	This program uses LogFile class to allow threads writing to same file in a syncrynized way
	Output of this file will be saved in "log.txt"
	mutex m is protecting the ofstream f.
*/
#include <iostream>
#include <thread>
#include <mutex>
#include <fstream>

using namespace std;

class LogFile{
private:
	mutex m;    /* this mutex is protecting the log */
	ofstream f;

public: 
	LogFile(){
		f.open("log.txt"); /* constructor opens up a log file */
	}
	/* A shared function used by many threads concurrently */
	void print_function(string msg, int num){
		lock_guard<mutex> locker(m);
		f << msg << num << endl;  /* write into a file */
		cout << msg << num << endl;
	}


};


void function_1(LogFile& log){
	for(int i = 0; i < 100; i++){
		string message = "Hello, ";
		log.print_function(message, i);
	}
}

int main(){
	LogFile log;
	thread t1(function_1, ref(log));
	for(int i = 0; i >-100; i--){
		string message = "Hello, ";
		log.print_function(message, i);
	}
	t1.join();
	return 0;
}