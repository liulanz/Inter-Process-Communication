/* 
  This program implements exceptions with lock_guard 
  Throws an exception when it's not even
*/
#include <iostream>       
#include <thread>         
#include <mutex>          
#include <stdexcept>      /* logic_error */
using namespace std;

mutex mtx;

void print_even (int x) {
    if (x%2 == 0) 
        cout << x << " is even" << endl;
    else 
        throw (logic_error("not even"));
}

void print_thread_id (int id) {
    try {
        lock_guard<mutex> lck (mtx);
        print_even(id);
    }
    catch (logic_error&) {
        cout << "[exception caught]" << endl;
    }
}

int main ()
{
    thread threads[10];

    for (int i=0; i<10; ++i){
        threads[i] = std::thread(print_thread_id,i+1);
    }

    for (auto& th : threads) {
        th.join();
    }

    return 0;
}