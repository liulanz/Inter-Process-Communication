/*
	This program is a very simple example of how threads are being created
*/
#include <thread>
#include <iostream>
#include <mutex>
using namespace std;


void exampleFunction1(){
	cout << "Thead:" <<this_thread::get_id() << " is here " << endl;
}

/* Taking a int and double the value, in this case num is passed by reference*/
void exampleFunctionWithParamater(int &num){
	num *= 2;
	
}
int main(){
	thread t1(exampleFunction1); /* creating a thread for exampleFunction1 */

	int num = 100;
	thread t2(exampleFunctionWithParamater, ref(num)); /* creating a thread for exampleFunctionWithParamater */

	// waiting for all the threads terminate
	t1.join();
	t2.join();

	cout << num << endl;
}