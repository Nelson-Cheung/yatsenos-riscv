#ifndef QUEUE_H
#define QUEUE_H

template <typename T>
class Queue
{
private:
    T *q;
    unsigned long length;
    unsigned long head, end;
    unsigned long counter;

public:
    Queue(T *q, unsigned long length);
    bool push(const T &item);
    bool pop(T &item);
    bool pop();
    bool empty();
};

template <typename T>
Queue<T>::Queue(T *q, unsigned long length)
{
    this->q = q;
    this->length = length;
    this->head = this->end = 0;
    this->counter = 0;
}

template <typename T>
bool Queue<T>::push(const T &item)
{
    if (counter == length)
    {
        return false;
    }

    q[end] = item;
    end = (end + 1) % length;
    ++counter;

    return true;
}

template <typename T>
bool Queue<T>::pop(T &item)
{
    if (empty())
    {
        return false;
    }

    item = q[head];
    return pop();
}

template <typename T>
bool Queue<T>::pop()
{
    if (empty())
    {
        return false;
    }

    head = (head + 1) % length;
    --counter;
    return true;
}

template <typename T>
bool Queue<T>::empty() {
    return !counter;
}

#endif