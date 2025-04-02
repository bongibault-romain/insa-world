#include "insaworld.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>

pthread_barrier_t barrier;
pthread_mutex_t mutex;

struct thread_args
{
  struct timespec period;
};

#define TASK_LOAD 100000

void task()
{
  for (int i = 0; i < TASK_LOAD; i++)
  {
    for (int j = 0; j < 10000; j++)
    {
    }
  }
}

void task_with_mutex()
{
  pthread_mutex_lock(&mutex);
  for (int i = 0; i < TASK_LOAD; i++)
  {
    for (int j = 0; j < 6000; j++)
    {
    }
  }
  pthread_mutex_unlock(&mutex);
}

void timespec_add(struct timespec *ts1, struct timespec *ts2)
{
  ts1->tv_sec += ts2->tv_sec;
  ts1->tv_nsec += ts2->tv_nsec;

  if (ts1->tv_nsec >= 1000000000)
  {
    ts1->tv_sec++;
    ts1->tv_nsec -= 1000000000;
  }
}

void sleep_until_next_activation(struct timespec *activation)
{
  int err;
  do
  {
    err = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, activation, NULL);
  } while (err != 0 && errno != EINTR);
}

void *thread_function(void *arg)
{
  pthread_barrier_wait(&barrier);

  int pperiod = *(int *)arg;
  // struct timespec *pperiod = (struct timespec *)arg; // 1 seconds
  // struct timespec period = *pperiod; // 1 seconds
  struct timespec next_activation;
  struct timespec period = {pperiod, 0}; // 1 seconds

  clock_gettime(CLOCK_MONOTONIC, &next_activation);

  printf("Next activation: %ld.%09ld seconds\n", next_activation.tv_sec, next_activation.tv_nsec);

  while (1)
  {
    sleep_until_next_activation(&next_activation);

    if (pperiod == 2)
    {
      task();
    }
    else
    {
      task_with_mutex();
    }

    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double elapsed = (end_time.tv_sec - next_activation.tv_sec) +
                     (end_time.tv_nsec - next_activation.tv_nsec) / 1e9;
    printf("[%d] Task executed in %f seconds\n", pperiod, elapsed);

    timespec_add(&next_activation, &period);
  }

  return NULL;
}

int main()
{
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);

  pthread_mutex_init(&mutex, &attr);

  pthread_t thread1, thread2, thread3;

  int period1 = 1;
  int period2 = 2;
  int period3 = 3;

  pthread_attr_t attr1;
  pthread_attr_t attr2;
  pthread_attr_t attr3;

  pthread_attr_init(&attr1);
  pthread_attr_init(&attr2);
  pthread_attr_init(&attr3);

  struct sched_param sch1;
  struct sched_param sch2;
  struct sched_param sch3;
  sch1.sched_priority = 20;
  sch2.sched_priority = 10;
  sch3.sched_priority = 0;

  pthread_attr_setschedpolicy(&attr1, SCHED_RR);
  pthread_attr_setschedpolicy(&attr2, SCHED_RR);
  pthread_attr_setschedpolicy(&attr3, SCHED_RR);
  pthread_attr_setschedparam(&attr1, &sch1);
  pthread_attr_setschedparam(&attr2, &sch2);
  pthread_attr_setschedparam(&attr3, &sch3);

  pthread_barrier_init(&barrier, NULL, 3);

  int err = pthread_create(&thread1, NULL, thread_function, &period1);

  if (err != 0)
  {
    fprintf(stderr, "Error creating thread 1: %d\n", err);
    return EXIT_FAILURE;
  }

  err = pthread_create(&thread2, NULL, thread_function, &period2);

  if (err != 0)
  {
    fprintf(stderr, "Error creating thread 2: %d\n", err);
    return EXIT_FAILURE;
  }

  err = pthread_create(&thread3, NULL, thread_function, &period3);

  if (err != 0)
  {
    fprintf(stderr, "Error creating thread 3: %d\n", err);
    return EXIT_FAILURE;
  }

  pthread_barrier_destroy(&barrier);

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  // thread_function(&period);
}
