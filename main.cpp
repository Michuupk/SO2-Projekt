#include <iostream>
#include <iomanip>
#include <thread>
#include <atomic>
#include <chrono>
#include <random>
#include <mutex>

using namespace std;

const int N = 5;
atomic<bool> forks[N];
atomic<bool> stop_threads(false);

enum PhilosopherState
{
    THINKING,
    WAITING,
    EATING
};
vector<PhilosopherState> philosopher_states(N, THINKING);
vector<bool> has_left_fork(N, false);
vector<bool> has_right_fork(N, false);
vector<int> eat_count(N, 0);

mutex state_mutex;
mutex ncurses_mutex;

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<int> dist1(1, 2);

random_device rd2;
mt19937 gen2(rd2());
uniform_int_distribution<int> dist2(3, 4);

const char *Names[] = {"Yoda", "Sheldon", "Hagrid", "Peter", "Dr House"};

void getState()
{
    lock_guard<mutex> lock(ncurses_mutex);

    for (int i = 0; i < N; i++)
    {
        string state_str;
        switch (philosopher_states[i])
        {
        case THINKING:
            state_str = "Thinking";
            break;
        case WAITING:
            state_str = "Waiting";
            break;
        case EATING:
            state_str = "Eating";
            break;
        }

        string forks_info = "";
        forks_info += has_left_fork[i] ? "L" : "-";
        forks_info += has_right_fork[i] ? "P" : "-";

        cout << "[" << setw(9) << Names[i] << " -> " << setw(9) << state_str << "; Forks:" << forks_info << "; Meals:" << setw(3) << right << eat_count[i] << "] " << endl;
    }

    cout << endl;
}

void philosopher1(int id)
{
    int left = id;
    int right = (id + 1) % N;

    while (!stop_threads)
    {
        // Think
        {
            lock_guard<mutex> lock(state_mutex);
            philosopher_states[id] = THINKING;
            has_left_fork[id] = false;
            has_right_fork[id] = false;
        }

        this_thread::sleep_for(chrono::seconds(dist1(gen)));
        bool got_left = false;
        bool got_right = false;

        // Pick up
        while (!stop_threads)
        {
            {
                lock_guard<mutex> lock(ncurses_mutex);
                if (forks[left])
                {
                    forks[left] = false;
                    got_left = true;

                    lock_guard<mutex> state_lock(state_mutex);
                    has_left_fork[id] = true;
                    philosopher_states[id] = WAITING;
                }
                if (forks[right])
                {
                    forks[right] = false;
                    got_right = true;

                    lock_guard<mutex> state_lock(state_mutex);
                    has_right_fork[id] = true;
                    philosopher_states[id] = WAITING;
                }
            }

            if (got_left && got_right)
            {
                // Eat
                {
                    lock_guard<mutex> lock(state_mutex);
                    philosopher_states[id] = EATING;
                }

                this_thread::sleep_for(chrono::seconds(dist1(gen)));

                {
                    lock_guard<mutex> lock(state_mutex);
                    eat_count[id]++;
                }

                // Put down
                {
                    lock_guard<mutex> lock(ncurses_mutex);
                    forks[left] = true;
                    forks[right] = true;
                }

                // Update for display
                {
                    lock_guard<mutex> lock(state_mutex);
                    has_left_fork[id] = false;
                    has_right_fork[id] = false;
                }

                break;
            }
            else
            {
                // Wait
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        }
    }
}

void philosopher2(int id)
{
    int left = id;
    int right = (id + 1) % N;

    while (!stop_threads)
    {
        // Think
        {
            lock_guard<mutex> lock(state_mutex);
            philosopher_states[id] = THINKING;
            has_left_fork[id] = false;
            has_right_fork[id] = false;
        }

        if (id % 2 == 0)
            this_thread::sleep_for(chrono::seconds(dist1(gen)));
        else
            this_thread::sleep_for(chrono::seconds(dist2(gen2)));
        bool got_left = false;
        bool got_right = false;

        // Pick up
        while (!stop_threads)
        {
            {
                lock_guard<mutex> lock(ncurses_mutex);
                if (forks[left])
                {
                    forks[left] = false;
                    got_left = true;

                    lock_guard<mutex> state_lock(state_mutex);
                    has_left_fork[id] = true;
                    philosopher_states[id] = WAITING;
                }
                if (forks[right])
                {
                    forks[right] = false;
                    got_right = true;

                    lock_guard<mutex> state_lock(state_mutex);
                    has_right_fork[id] = true;
                    philosopher_states[id] = WAITING;
                }
                else if (forks[left] && id == 0)
                {
                    forks[left] = true;
                    got_left = false;

                    lock_guard<mutex> state_lock(state_mutex);
                    has_left_fork[id] = false;
                    break;
                }
            }

            if (got_left && got_right)
            {
                // Eat
                {
                    lock_guard<mutex> lock(state_mutex);
                    philosopher_states[id] = EATING;
                }

                if (id % 2 == 0)
                    this_thread::sleep_for(chrono::seconds(dist1(gen)));
                else
                    this_thread::sleep_for(chrono::seconds(dist2(gen2)));

                {
                    lock_guard<mutex> lock(state_mutex);
                    eat_count[id]++;
                }

                // Put down
                {
                    lock_guard<mutex> lock(ncurses_mutex);
                    forks[left] = true;
                    forks[right] = true;
                }

                // Update for display
                {
                    lock_guard<mutex> lock(state_mutex);
                    has_left_fork[id] = false;
                    has_right_fork[id] = false;
                }

                break;
            }
            else
            {
                // Wait
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        }
    }
}

void philosopher3(int id)
{
    int left = id;
    int right = (id + 1) % N;

    while (!stop_threads)
    {
        // Think
        {
            lock_guard<mutex> lock(state_mutex);
            philosopher_states[id] = THINKING;
            has_left_fork[id] = false;
            has_right_fork[id] = false;
        }

        this_thread::sleep_for(chrono::seconds(dist1(gen)));
        bool got_left = false;
        bool got_right = false;

        // Pick up
        while (!stop_threads)
        {
            // Problem: Trzymasz ncurses_mutex przez cały czas pętli wewnętrznej
            // To może blokować inne wątki na długi czas
            // Rozwiązanie: Zwalniaj mutex po każdej próbie podniesienia widelca

            if (id % 2 == 0) // Parzyści filozofowie najpierw lewą pałeczkę
            {
                if (!got_left)
                {
                    lock_guard<mutex> lock(ncurses_mutex);
                    if (forks[left])
                    {
                        forks[left] = false;
                        got_left = true;

                        lock_guard<mutex> state_lock(state_mutex);
                        has_left_fork[id] = true;
                        philosopher_states[id] = WAITING;
                    }
                }
                else if (!got_right)
                {
                    lock_guard<mutex> lock(ncurses_mutex);
                    if (forks[right])
                    {
                        forks[right] = false;
                        got_right = true;

                        lock_guard<mutex> state_lock(state_mutex);
                        has_right_fork[id] = true;
                    }
                }
            }
            else // Nieparzyści filozofowie najpierw prawą pałeczkę
            {
                if (!got_right)
                {
                    lock_guard<mutex> lock(ncurses_mutex);
                    if (forks[right])
                    {
                        forks[right] = false;
                        got_right = true;

                        lock_guard<mutex> state_lock(state_mutex);
                        has_right_fork[id] = true;
                        philosopher_states[id] = WAITING;
                    }
                }
                else if (!got_left)
                {
                    lock_guard<mutex> lock(ncurses_mutex);
                    if (forks[left])
                    {
                        forks[left] = false;
                        got_left = true;

                        lock_guard<mutex> state_lock(state_mutex);
                        has_left_fork[id] = true;
                    }
                }
            }

            // Jeśli filozof ma obie pałeczki, może jeść
            if (got_left && got_right)
            {
                // Eat
                {
                    lock_guard<mutex> lock(state_mutex);
                    philosopher_states[id] = EATING;
                }

                this_thread::sleep_for(chrono::seconds(dist1(gen)));

                // Zwiększ licznik posiłków
                {
                    lock_guard<mutex> lock(state_mutex);
                    eat_count[id]++;
                }

                // Put down
                {
                    lock_guard<mutex> lock(ncurses_mutex);
                    forks[left] = true;
                    forks[right] = true;
                }

                // Update for display
                {
                    lock_guard<mutex> lock(state_mutex);
                    has_left_fork[id] = false;
                    has_right_fork[id] = false;
                }

                break;
            }
            else
            {
                // Wait
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        }
    }
}

int main()
{
    thread philosophers[N];

    cout << "Wybierz strategie: \n";
    cout << "1. Z mozliwoscia zakleszczenia.\n";
    cout << "2. Z zagladzaniem.\n";
    cout << "3. Optymalne.\n";

    int what;
    cin >> what;

    for (int i = 0; i < N; ++i)
        forks[i] = true;

    switch (what)
    {
    case 1:
        for (int i = 0; i < N; ++i)
            philosophers[i] = thread(philosopher1, i);
        cin.ignore();
        break;

    case 2:
        for (int i = 0; i < N; ++i)
            philosophers[i] = thread(philosopher2, i);
        cin.ignore();
        break;

    case 3:
        for (int i = 0; i < N; ++i)
            philosophers[i] = thread(philosopher3, i);
        cin.ignore();
        break;

    default:
        break;
    }

    while (!stop_threads)
    {
        getState();
        this_thread::sleep_for(chrono::seconds(2));
    }

    stop_threads = true;

    for (auto &p : philosophers)
        if (p.joinable())
            p.join();

    return 0;
}
