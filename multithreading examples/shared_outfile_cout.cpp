/*
	This program uses LogFile class to allow threads writing to same file in a syncrynized way
	Output of this file will be saved in "log.txt"
	mutex m is protecting the ofstream f.

	This program also uses unique lock, similar to lock_guard
	- can be unlocked
	- mutext doesn't have to be locked when unique lock is created


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
	ofstream f2;
	mutex m_open;

public: 
	LogFile(){
		f.open("log.txt");
	}

	/* A shared function used by many threads concurrently */
	void print_function(string msg, int num){
		lock_guard<mutex> locker(m);
		f << msg << num << endl;  /* write into a file */
		cout << msg << num << endl;
	}

	void shared_function(string msg, int num){


		{
			unique_lock<mutex> locker_open_file(m_open); 
			if(!f2.is_open()){
				/* if unique lock is inside if statement, file will be opened more than once */
				// unique_lock<mutex> locker_open_file(m_open);  
				f.open("share_function.txt")
			}
		}

	/**************** Unique Lock *************************/
		/* unique lock, defer_lock allows mutex not to be locked at this moment, it can also be unlocked at any moment*/
		unique_lock<mutex> locker(mu, defer_lock);


		// you can do something before locking the mutex
		locker.lock();
		// you can lock and unlock using unique lock which cannot be done by lock_guard
		locker.unlock();

		locker.lock();

		locker.unlock();
		


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