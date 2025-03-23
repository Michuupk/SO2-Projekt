#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <random>

using namespace std;

const int N = 5;
atomic<bool> forks[N]; // true - zajęty, false - wolny

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<int> dist(2, 4);

void philosopher1(int id)
{
    int left = id;
    int right = (id + 1) % N;

    while (true)
    {

        forks[left] = true;

        if (!forks[right])
        {
            forks[right] = true;

            cout << "Filozof " << id << " je." << endl;
            this_thread::sleep_for(std::chrono::seconds(dist(gen)));

            // Odłożenie
            forks[left] = false;
            forks[right] = false;

            cout << "Filozof " << id << " mysli." << endl;
            ;
            this_thread::sleep_for(std::chrono::seconds(dist(gen)));
        }
        if (forks[right])
        {
            //cout << "Filozof " << id << " czeka na prawa." << endl;
        }
    }
}

void philosopher2(int id)
{
    int left = id;
    int right = (id + 1) % N;

    while (true)
    {

        forks[left] = true;

        if (!forks[right])
        {
            forks[right] = true;

            cout << "Filozof " << id << " je." << endl;
            this_thread::sleep_for(std::chrono::seconds(dist(gen)));

            // Odłożenie
            forks[left] = false;
            forks[right] = false;

            cout << "Filozof " << id << " mysli." << endl;
            ;
            this_thread::sleep_for(std::chrono::seconds(dist(gen)));
        }
        if (forks[right])
        {
            cout << "Filozof " << id << " czeka na prawa." << endl;
        }
    }
}

int main()
{

    thread philosophers[N];
    int what;

    cout << "Wybierz strategie: " << endl;
    cout << "1. Z mozliwoscia zakleszczenia." << endl;
    cout << "2. Z zagladzaniem." << endl;
    cout << "3. Optymalne." << endl;
    cin >> what;

    switch (what)
    {
    case 1:
        for (int i = 0; i < N; ++i)
        {
            forks[i] = false;
            philosophers[i] = thread(philosopher1, i);
        }
        for (auto &p : philosophers)
        {
            p.join();
        }
        break;
    case 2:
        for (int i = 0; i < N; ++i)
        {
            forks[i] = false;
            philosophers[i] = thread(philosopher2, i);
        }
        for (auto &p : philosophers)
        {
            p.join();
        }
        break;

    default:
        cout << "Nie ma takiej opcji." << endl;
        break;
    }
    return 0;
}
