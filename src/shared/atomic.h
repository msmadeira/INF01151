#ifndef ATOMIC_H
#define ATOMIC_H

#include <vector>
#include <semaphore.h>

template <typename A>
class AtomicMultipleReadersVar
{
private:
    A val;
    sem_t reader_queue;
    sem_t writer_queue;
    int reader_count = 0;

public:
    AtomicMultipleReadersVar(A init_val);
    A read();
    void write(A new_val);
    A read_and_replace(A new_val);
    void lock();
    void unlock();
    A locked_read();
    void locked_write(A new_val);
};

template <typename A>
class AtomicVar
{
private:
    A val;
    sem_t semaphore;

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

/* Template implementation */

// AtomicMultipleReadersVar

template <typename A>
AtomicMultipleReadersVar<A>::AtomicMultipleReadersVar(A init_val) : val(init_val)
{
    int returned_value = sem_init(&(this->reader_queue), 0, 1);
    if (returned_value != 0)
    {
        printf("Initialization of AtomicMultipleReadersVar's reader_queue semaphore failed.\n");
    }
    returned_value = sem_init(&(this->writer_queue), 0, 1);
    if (returned_value != 0)
    {
        printf("Initialization of AtomicMultipleReadersVar's writer_queue semaphore failed.\n");
    }
}

template <typename A>
A AtomicMultipleReadersVar<A>::read()
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
void AtomicMultipleReadersVar<A>::write(A new_val)
{
#ifdef DEBUG
    printf("AtomicMultipleReadersVar<A>::write() pre-sem_wait\n");
#endif
    sem_wait(&(this->writer_queue));
#ifdef DEBUG
    printf("AtomicMultipleReadersVar<A>::write() pos-sem_wait\n");
#endif

    val = new_val;

#ifdef DEBUG
    printf("AtomicMultipleReadersVar<A>::write() pre-sem_post\n");
#endif
    sem_post(&(this->writer_queue));
#ifdef DEBUG
    printf("AtomicMultipleReadersVar<A>::write() pos-sem_post\n");
#endif
}

template <typename A>
A AtomicMultipleReadersVar<A>::read_and_replace(A new_val)
{
    sem_wait(&(this->writer_queue));

    A copied_val = val;
    val = new_val;

    sem_post(&(this->writer_queue));

    return copied_val;
}

template <typename A>
void AtomicMultipleReadersVar<A>::lock()
{
    sem_wait(&(this->writer_queue));
}

template <typename A>
void AtomicMultipleReadersVar<A>::unlock()
{
    sem_post(&(this->writer_queue));
}

template <typename A>
A AtomicMultipleReadersVar<A>::locked_read()
{
    return val;
}

template <typename A>
void AtomicMultipleReadersVar<A>::locked_write(A new_val)
{
    val = new_val;
}

// AtomicVar

template <typename A>
AtomicVar<A>::AtomicVar(A init_val) : val(init_val)
{
    int returned_value = sem_init(&(this->semaphore), 0, 1);
    if (returned_value != 0)
    {
        printf("Initialization of AtomicVar's semaphore failed.\n");
    }
}

template <typename A>
A AtomicVar<A>::read()
{
    sem_wait(&(this->semaphore));

    A copied_val = val;

    sem_post(&(this->semaphore));

    return copied_val;
}

template <typename A>
void AtomicVar<A>::write(A new_val)
{
    sem_wait(&(this->semaphore));

    val = new_val;

    sem_post(&(this->semaphore));
}

template <typename A>
A AtomicVar<A>::read_and_replace(A new_val)
{
    sem_wait(&(this->semaphore));

    A copied_val = val;
    val = new_val;

    sem_post(&(this->semaphore));

    return copied_val;
}

template <typename A>
void AtomicVar<A>::lock()
{
    sem_wait(&(this->semaphore));
}

template <typename A>
void AtomicVar<A>::unlock()
{
    sem_post(&(this->semaphore));
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

// AtomicVecQueue

template <typename A>
AtomicVecQueue<A>::AtomicVecQueue()
{
    int returned_value = sem_init(&(this->semaphore), 0, 1);
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
    std::vector<A> drained_results(this->vec_queue);
    this->vec_queue.clear();
    sem_post(&(this->semaphore));
    return drained_results;
}

#endif
