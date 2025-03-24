#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>
#include <random>

using namespace std;

//Class semaphore (waiter) to synchronize the philosopers
class Semaphore {
public:
    //Construction of waiter
    explicit Semaphore(int count = 0)
        : count(count) {}
    //Function to notify semaphore (waiter) that philosopher is done with eating and fork is available
    void notify() {
        unique_lock<mutex> lock(fork);
        ++count;
        cv.notify_one();
    }
    //Function to inform semaphore (waiter) that philosopher can eat (is waiting)
    void wait() {
        unique_lock<mutex> lock(fork);
        cv.wait(lock, [&]{ return count > 0; });
        --count;
    }
private:
    mutex fork;
    condition_variable cv;
    int count;
};

mutex coutMutex;

void philosopher(int id, vector<mutex>& forks, Semaphore& waiter, int n) {
    //Random number generator
    mt19937 rng(random_device{}());
    uniform_int_distribution<> thinkTime(1000, 3000);
    uniform_int_distribution<> eatTime(1000, 2000);

    while (true) {
        //Creating a philosopher and making him think
        { 
            lock_guard<mutex> lock(coutMutex);
            cout << "Filozof " << id << " myśli." << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(thinkTime(rng)));

        //Changing state of philosopher to hungry
        { 
            lock_guard<mutex> lock(coutMutex);
            cout << "Filozof " << id << " jest głodny." << endl;
        }
        //Using wait function to informt waiter that philosopher is ready to eat
        waiter.wait();

        //Searching if fork is available (left and then right) and locking it
        //If fork is not available, philosopher is waiting for it
        forks[id].lock();
        forks[(id + 1) % n].lock();

        //Changing state of philosopher to eating
        {
            lock_guard<mutex> lock(coutMutex);
            cout << "Filozof " << id << " je." << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(eatTime(rng)));

        //After eating philosopher is puting donw the forks
        forks[id].unlock();
        forks[(id + 1) % n].unlock();
        //Notifying waiter that philosopher is done and fork is available
        waiter.notify();
    }
}

int main(int argc, char* argv[]) {
    int n = 100;
    //Checking if the program is able to run (minimum 2 philosophers)
    if (argc > 1) {
        try {
            if (n < 2) {
                cerr << "Liczba filozofów musi być większa lub równa 2!" << endl;
                return 1;
            }
        } catch (exception& e) {
            cerr << "Błąd konwersji argumentu: " << e.what() << endl;
            return 1;
        }
    }

    //Creating forks and philosophers
    vector<mutex> forks(n);
    Semaphore waiter(n - 1);
    vector<thread> philosophers;

    //Creating thread for each philosopher
    for (int i = 0; i < n; ++i) {
        philosophers.emplace_back(philosopher, i, ref(forks), ref(waiter), n);
    }
    //Joining main thread to philosopers to avid closing the program too soon
    for (auto& mainthread : philosophers) {
        mainthread.join();
    }
    return 0;
}
