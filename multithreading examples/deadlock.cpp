/* 
	This program implements a Deadlock situation
*/
#include <iostream>
#include <thread>
#include <mutex>
#include <fstream>

using namespace std;

class LogFile{
private:
	mutex m1;    
	mutex m2; 
	ofstream f;

public: 
	LogFile(){}
	/* A shared function used by many threads concurrently */
	void print_function(string msg, int num){
		// lock_guard<mutex> locker1(m1);
		// lock_guard<mutex> locker2(m2);

		/* SOLUTION 2 to avoid a deadlock by using adopt lock */
		lock(m1, m2); /* lock multiple mutex */
		lock_guard<mutex> locker1(m1, adopt_lock); /* locker1 will adopt the lock of m1 */
		lock_guard<mutex> locker2(m2, adopt_lock);

		cout << msg << num << endl;
	}
	void print_function2(string msg, int num){

		/* This will cause a deadlock */
		// lock_guard<mutex> locker1(m1);
		// lock_guard<mutex> locker2(m2);     


		/* Avoiding deadlock:
			1. Prefer locking single mutex
			2. Avoid locking a mutex and then call other function
			3. Use std::lock() to lock more than one mutex
			4. Lock the mutex in same oder
		*/

		/* SOLUTION 1 to avoid a deadlock by using adopt lock */
		// lock_guard<mutex> locker2(m2); 
		// lock_guard<mutex> locker1(m1);

		/* SOLUTION 2 to avoid a deadlock by using adopt lock */
		lock(m1, m2);
		lock_guard<mutex> locker2(m2, adopt_lock);
		lock_guard<mutex> locker1(m1, adopt_lock);


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
		string message = "Hello2, ";
		log.print_function2(message, i); /* calling print function2*/
	}
	t1.join();
	return 0;
}