#ifndef ATOMIC_H
#define ATOMIC_H

#include <vector>
#include <semaphore.h>

template <typename A>
class AtomicVar
{
private:
    A val;
    sem_t reader_queue;
    sem_t writer_queue;
    int reader_count = 0;

public:
    AtomicVar(A init_val);
    A read();
    void write(A new_val);
    A read_and_replace(A new_val);
    void lock();
    void unlock();
    A locked_read();
    void locked_write(A new_val);
};

template <typename A>
class AtomicVecQueue
{
private:
    std::vector<A> vec_queue;
    sem_t semaphore;

public:
    AtomicVecQueue();
    void push(A new_val);
    void extend(std::vector<A> new_val);
    std::vector<A> drain();
};

// Template implementation

template <typename A>
AtomicVar<A>::AtomicVar(A init_val) : val(init_val)
{
    int returned_value = sem_init(&(this->reader_queue), 1, 1);
    if (returned_value != 0)
    {
        printf("Initialization of AtomicVar's reader_queue semaphore failed.\n");
    }
    returned_value = sem_init(&(this->writer_queue), 1, 1);
    if (returned_value != 0)
    {
        printf("Initialization of AtomicVar's writer_queue semaphore failed.\n");
    }
}

template <typename A>
A AtomicVar<A>::read()
{
    sem_wait(&(this->reader_queue));
    reader_count++;
    if (reader_count == 1)
    {
        sem_wait(&(this->writer_queue));
    }
    sem_post(&(this->reader_queue));

    A copied_val = val;

    sem_wait(&(this->reader_queue));
    reader_count--;
    if (reader_count == 0)
    {
        sem_post(&(this->writer_queue));
    }
    sem_post(&(this->reader_queue));

    return copied_val;
}

template <typename A>
void AtomicVar<A>::write(A new_val)
{
    sem_wait(&(this->writer_queue));

    val = new_val;

    sem_post(&(this->writer_queue));
}

template <typename A>
A AtomicVar<A>::read_and_replace(A new_val)
{
    sem_wait(&(this->writer_queue));

    A copied_val = val;
    val = new_val;

    sem_post(&(this->writer_queue));

    return copied_val;
}

template <typename A>
void AtomicVar<A>::lock()
{
    sem_wait(&(this->writer_queue));
}

template <typename A>
void AtomicVar<A>::unlock()
{
    sem_post(&(this->writer_queue));
}

template <typename A>
A AtomicVar<A>::locked_read()
{
    return val;
}

template <typename A>
void AtomicVar<A>::locked_write(A new_val)
{
    val = new_val;
}

template <typename A>
AtomicVecQueue<A>::AtomicVecQueue()
{
    int returned_value = sem_init(&(this->semaphore), 1, 1);
    if (returned_value != 0)
    {
        printf("Initialization of AtomicVecQueue's semaphore failed.\n");
    }
}

template <typename A>
void AtomicVecQueue<A>::push(A new_val)
{
    sem_wait(&(this->semaphore));
    vec_queue.push_back(new_val);
    sem_post(&(this->semaphore));
}

template <typename A>
void AtomicVecQueue<A>::extend(std::vector<A> new_vals)
{
    sem_wait(&(this->semaphore));
    vec_queue.reserve(vec_queue.size() + new_vals.size());
    vec_queue.insert(vec_queue.end(), new_vals.begin(), new_vals.end());
    sem_post(&(this->semaphore));
}

template <typename A>
std::vector<A> AtomicVecQueue<A>::drain()
{
    sem_wait(&(this->semaphore));
    std::vector<A> drained_results(vec_queue);
    sem_post(&(this->semaphore));
    return drained_results;
}

#endif
