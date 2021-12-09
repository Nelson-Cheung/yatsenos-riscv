#include "queue.h"
#include <iostream>
using namespace std;

int main()
{
    unsigned long a[6];
    Queue<unsigned long> q(a, sizeof(a) / sizeof(unsigned long));

    // cout << q.empty() << endl;s

    cout << q.push(5) << endl;
    cout << q.push(53) << endl;
    cout << q.push(125) << endl;
    cout << q.push(2455) << endl;
    cout << q.push(4365) << endl;
    cout << q.push(5104124) << endl;
    cout << q.push(32452) << endl;

    unsigned long item;

    cout << "--------------" << endl;

    cout << q.pop(item) << endl;
    cout << item << endl;
    cout << q.pop() << endl;
    cout << q.pop() << endl;
    cout << q.pop(item) << endl;
    cout << item << endl;
    cout << q.pop(item) << endl;
    cout << item << endl;
    cout << q.pop() << endl;
    cout << q.pop() << endl;
    cout << q.empty() << endl;

    cout << "--------------" << endl;

    cout << q.push(5) << endl;
    cout << q.push(53) << endl;
    cout << q.push(125) << endl;
    cout << q.push(2455) << endl;
    cout << q.push(4365) << endl;
    cout << q.push(5104124) << endl;
    cout << q.push(32452) << endl;

    cout << "--------------" << endl;

    cout << q.pop(item) << endl;
    cout << item << endl;
    cout << q.pop() << endl;
    cout << q.pop() << endl;
    cout << q.pop(item) << endl;
    cout << item << endl;
    cout << q.pop(item) << endl;
    cout << item << endl;
    cout << q.pop() << endl;
    cout << q.pop() << endl;
    cout << q.empty() << endl;
}