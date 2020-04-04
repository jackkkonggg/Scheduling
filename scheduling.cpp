#include <iostream>
#include <queue>
#include <algorithm>
#include <vector>
#include <utility>

using namespace std;

#define vsii vector<pair<string, pair<int, int>>>
#define mp make_pair

void printProcesses(const vsii& p) {
    for(int i = 0; i < p.size(); ++i)
        cout << p[i].first << "\tA: " << p[i].second.first << "\tB: " << p[i].second.second << endl;
        return;
}

void printOrder(const vsii& p) {
    for(int i = 0; i < p.size(); ++i)
        printf("%s\tStart: %d\tEnd: %d\n", p[i].first.c_str(), p[i].second.first, p[i].second.second);
}

vsii FCFS(const vsii& p) {
    // Assume p is already sorted by arrival time.
    vsii order; // pair<int, int> will be start and stop time for each process string.
    int currTime = 0;
    for(int i = 0; i < p.size(); ++i) {
        int nextTime = currTime + p[i].second.second;
        order.push_back(mp(p[i].first, mp(currTime, nextTime)));
        currTime = nextTime;
    }
    printOrder(order);
    return order;
}

vsii SJF(const vsii& p) {
    vsii order;
    vector<pair<string, int>> q;
    int time = 0, idxNext = 0; // Index of next process.
    int numComplete = 0, compTime = 0; // Completion time.
    bool exe = false;
    while(numComplete != p.size()) {
        if(time == p[idxNext].second.first) { // If equal arrival time.
            q.push_back(mp(p[idxNext].first, p[idxNext].second.second));
            ++idxNext;
        }
        if(time == compTime) {
            exe = false;
            ++numComplete;
        }
        if(!q.empty() && !exe) {
            sort(q.begin(), q.end(), [](const pair<string, int> &left, const pair<string, int> &right) {
                return left.second < right.second;
            }); // Sort by burst time.
            compTime = time + q.front().second;
            order.push_back(mp(q.front().first, mp(time, compTime)));
            q.erase(q.begin()); // Remove job from vector.
            exe = true;
        }
        ++time;
    }
    printOrder(order);
    return order;
}

vsii SRTF(const vsii& p) {
    vsii order;
    vector<pair<string, int>> q; // Stores process name and remaining time left.
    int time = 0, idxNext = 0, numComplete = 0, joinTime = 0;
    pair<string, int> currProcess = mp("null", INT_MAX);
    bool exe = false;
    while(numComplete != p.size()) {
        if(time == p[idxNext].second.first) { // If equal arrival time.
            q.push_back(mp(p[idxNext].first, p[idxNext].second.second));
            ++idxNext;
        }
        if(currProcess.second == 0) {
            exe = false;
            order.push_back(mp(currProcess.first, mp(joinTime, time)));
            ++numComplete;
        }
        if(!q.empty()) {
            sort(q.begin(), q.end(), [](const pair<string, int> &left, const pair<string, int> &right) {
                return left.second < right.second;
            }); // Sort by burst time.
            if(!exe) { // No processes is executing.
                currProcess = q.front();
                exe = true;
                joinTime = time;
                q.erase(q.begin()); // Remove job.
            }
            if(exe) { // If there's a process executing.
                if(q.begin()->second < currProcess.second) { // Job in Q has shorter remaining time.
                    q.push_back(currProcess); // Add current job to Q.
                    order.push_back(mp(currProcess.first, mp(joinTime, time)));
                    currProcess = q.front();
                    joinTime = time;
                    q.erase(q.begin()); // Remove job.;

                }
            }
        }
        ++time; --currProcess.second;
    }
    printOrder(order);
    return order;
}

vsii RR(const vsii& p, const int& quantum) {
    vsii order;
    queue<pair<string, int>> q; // Stores process name and remaining time left;
    int time = 0, idxNext = 0, numComplete = 0, joinTime = 0, cutOffTime = 0;
    bool exe = false;
    pair<string, int> currProcess = mp("null", INT_MAX);
    while(numComplete != p.size()) {
        if(time == p[idxNext].second.first) { // If equal arrival time.
            q.push(mp(p[idxNext].first, p[idxNext].second.second));
            ++idxNext;
        }
        if(currProcess.second == 0) {
            exe = false;
            order.push_back(mp(currProcess.first, mp(joinTime, time)));
            ++numComplete;
            cutOffTime = time; // Bring in next job immediately.
        }
        if(!q.empty() && time == cutOffTime) { // Time slice is over.
            if(!exe) {
                currProcess = q.front();
                joinTime = 0;
                exe = true;
                q.pop(); // Remove from queue.
                cutOffTime += quantum;
            }
            else {
                q.push(currProcess); // Add the process back to Q.
                order.push_back(mp(currProcess.first, mp(joinTime, time)));
                currProcess = q.front();
                joinTime = time;
                q.pop(); // Remove from queue.
                cutOffTime += quantum;
            }
        }
        ++time; --currProcess.second;
    }
    printOrder(order);
    return order;
}

#define ppsii pair<pair<string, int>, int>

namespace MLFQVar {

}

vsii MLFQ(const vsii& p, const int& quantumFirst, const int& quantumSecond bool preemption = false) { // Non-preemptive
    vsii order;
    queue<ppsii> qFirst, qSecond, fcfs;
    int time = 0, joinTime = 0, cutOffTime = 0, idxNext = 0, numComplete = 0;
    ppsii currProcess = mp(mp("null", 1), INT_MAX), saveProcess = mp(mp("null", 1), INT_MAX);
    bool exe = false;
    while(numComplete != p.size()) {
        if(time == p[idxNext].second.first) { // If equal arrival time.
            qFirst.push(mp(mp(p[idxNext].first, 1), p[idxNext].second.second));
            ++idxNext;
        }
        if(currProcess.second == 0) { // When job finishes.
            exe = false;
            order.push_back(mp(currProcess.first.first, mp(joinTime, time)));
            ++numComplete;
            cutOffTime = time;
        }
        if(cutOffTime == time) { // When time slice is over.
            if(exe) {
                exe = false;
                if(currProcess.first.second == 1) {
                    qSecond.push(currProcess);
                    qSecond.back().first.second = 2;
                }
                if(currProcess.first.second == 2) {
                    fcfs.push(currProcess);
                    fcfs.back().first.second = 3;
                }
                order.push_back(mp(currProcess.first.first, mp(joinTime, time)));
            }
            // If queue has a job and nothing is executing, this has top priority.
            if(!qFirst.empty() && !exe) { // Note that this is non-preemptive.
                currProcess = qFirst.front();
                cutOffTime += quantumFirst;
                exe = true;
                joinTime = time;
                qFirst.pop();
            }
            if(!qSecond.empty() && !exe) {
                currProcess = qSecond.front();
                cutOffTime += quantumSecond;
                exe = true;
                joinTime = time;
                qSecond.pop();
            }
            if(!fcfs.empty() && !exe) {
                currProcess = fcfs.front();
                exe = true;
                joinTime = time;
                fcfs.pop();
            }
        }
        ++time; --currProcess.second;
    }
    printOrder(order);
    return order;
}
int main() {
    int n; // Number of processes
    cin >> n;
    vsii processes;
    for(int i = 0; i < n; ++i) {
        string name; int arrival, burst;
        cin >> name >> arrival >> burst;
        processes.push_back(mp(name, mp(arrival, burst)));
    }
    // Sort according to arrival time.
    sort(processes.begin(), processes.end(), [](const pair<string, pair<int, int>> &left,
                                            const pair<string, pair<int, int>> &right) {
        return left.second.first < right.second.first;
    });
    MLFQ(processes, 8, 16);
    return 0;
}
