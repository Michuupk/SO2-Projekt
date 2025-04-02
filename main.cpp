#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <random>
#include <ncurses.h>
#include <mutex>

using namespace std;

const int N = 5;
atomic<bool> forks[N];
atomic<bool> stop_threads(false);
    
mutex ncurses_mutex;

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<int> dist1(1, 3);

random_device rd2;
mt19937 gen2(rd2());
uniform_int_distribution<int> dist2(5, 7);

const char *Names[] = {"Yoda", "Sheldon", "Hagrid", "Peter", "Dr House"};

void displayState(WINDOW *win, const char *name, int color_pair, int id)
{
    lock_guard<mutex> lock(ncurses_mutex);
    wattron(win, COLOR_PAIR(color_pair));
    mvwprintw(win, id, 1, "%s", name);
    wattroff(win, COLOR_PAIR(color_pair));
    wrefresh(win);
}
void philosopher1(int id)
{
    int left = id;
    int right = (id + 1) % N;

    while (!stop_threads)
    {
        // Think
        displayState(stdscr, Names[id], 1, id);
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
                }
                if (forks[right])
                {
                    forks[right] = false;
                    got_right = true;
                }
            }

            if (got_left && got_right)
            {
                // Eat
                displayState(stdscr, Names[id], 3, id);
                this_thread::sleep_for(chrono::seconds(dist2(gen)));

                // Put down
                lock_guard<mutex> lock(ncurses_mutex);
                forks[left] = true;
                forks[right] = true;
                break;
            }
            else
            {
                // Wait
                displayState(stdscr, Names[id], 2, id);
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
        displayState(stdscr, Names[id], 1, id);
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
                }
                if (forks[right])
                {
                    forks[right] = false;
                    got_right = true;
                }
            }

            if (got_left && got_right)
            {
                // Eat
                displayState(stdscr, Names[id], 3, id);
                this_thread::sleep_for(chrono::seconds(dist2(gen)));

                // Put down
                lock_guard<mutex> lock(ncurses_mutex);
                forks[left] = true;
                forks[right] = true;
                break;
            }
            else
            {
                // Wait
                displayState(stdscr, Names[id], 2, id);
                this_thread::sleep_for(chrono::milliseconds(10));
                if (id == 0)
                {
                    {
                        lock_guard<mutex> lock(ncurses_mutex);
                        got_left = false;
                        forks[left] = true;
                    }
                }
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
        displayState(stdscr, Names[id], 1, id);
        this_thread::sleep_for(chrono::seconds(dist1(gen)));

        bool got_left = false;
        bool got_right = false;

        // Pick up
        while (!stop_threads)
        {
            if (id % 2 == 0) // Even philosopher
            {
                {
                    lock_guard<mutex> lock(ncurses_mutex);
                    if (forks[left])
                    {
                        forks[left] = false;
                        got_left = true;
                    }
                    if (forks[right] && got_left)
                    {
                        forks[right] = false;
                        got_right = true;
                    }
                }
            }
            else // Odd philosopher
            {
                {
                    lock_guard<mutex> lock(ncurses_mutex);
                    if (forks[right])
                    {
                        forks[right] = false;
                        got_right = true;
                    }
                    if (forks[left] && got_right)
                    {
                        forks[left] = false;
                        got_left = true;
                    }
                }
            }

            if (got_left && got_right)
            {
                // Eating
                displayState(stdscr, Names[id], 3, id);
                this_thread::sleep_for(chrono::seconds(dist2(gen)));

                // Put down
                lock_guard<mutex> lock(ncurses_mutex);
                forks[left] = true;
                forks[right] = true;
                got_left = false;
                got_right = false;
                break;
            }
            else
            {
                // Waiting
                displayState(stdscr, Names[id], 2, id);
                if (got_left)
                    got_left = false;
                    forks[left] = true;
                if (got_right)
                    got_right = false;
                    forks[right] = true;
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        }
    }
}

int main()
{
    thread philosophers[N];

    initscr();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);    // thinking
    init_pair(2, COLOR_YELLOW, COLOR_BLACK); // waiting
    init_pair(3, COLOR_GREEN, COLOR_BLACK);  // eating

    printw("Wybierz strategie: \n");
    printw("1. Z mozliwoscia zakleszczenia.\n");
    printw("2. Z zagladzaniem.\n");
    printw("3. Optymalne.\n");
    refresh();

    int what;
    scanw("%d", &what);

    clear();
    refresh();

    for (int i = 0; i < N; ++i)
        forks[i] = true;

    switch (what)
    {
        case 1:
            for (int i = 0; i < N; ++i)
                philosophers[i] = thread(philosopher1, i);

            getch();

            break;
            
        case 2:
            for (int i = 0; i < N; ++i)
                philosophers[i] = thread(philosopher2, i);
            getch();

            break;
        
        case 3:
            for (int i = 0; i < N; ++i)
                philosophers[i] = thread(philosopher3, i);
            getch();

            break;

        default:
            break;
    }

    stop_threads = true;

    for (auto &p : philosophers)
        if (p.joinable())
            p.join();

    endwin();

    return 0;
}
