#include "insaworld.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>

#define TASK_LOAD 10000

void task() {
  for (int i = 0; i < TASK_LOAD; i++)
  {
    for ( int j =0; j < 10000; j ++) {
      /* do nothing , keep counting */
    }
  }
}

void timespec_add(struct timespec *ts1, struct timespec *ts2) {
  ts1->tv_sec += ts2->tv_sec;
  ts1->tv_nsec += ts2->tv_nsec;

  if (ts1->tv_nsec >= 1000000000) {
    ts1->tv_sec++;
    ts1->tv_nsec -= 1000000000;
  }
}

void sleep_until_next_activation(struct timespec *activation) {
  int err;
  do {
    err = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, activation, NULL);
  } while (err != 0 && errno != EINTR);
} 

void *thread_function(void *arg) {
  struct timespec *pperiod = (struct timespec *)arg; // 1 seconds
  struct timespec period = *pperiod; // 1 seconds
  struct timespec next_activation;

  printf("Thread %ld started with period %ld.%09ld seconds\n", pthread_self(), period.tv_sec, period.tv_nsec);

  clock_gettime(CLOCK_MONOTONIC, &next_activation);

  printf("Next activation: %ld.%09ld seconds\n", next_activation.tv_sec, next_activation.tv_nsec);

  while (1) {
    sleep_until_next_activation(&next_activation);

    task();
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double elapsed = (end_time.tv_sec - next_activation.tv_sec) + 
                     (end_time.tv_nsec - next_activation.tv_nsec) / 1e9;
    printf("[%ld] Task executed in %f seconds\n", pthread_self(), elapsed);

    timespec_add(&next_activation, &period) ;
  }

  return NULL;
}


int main(int argc, char *argv[]) {
  // pthread_t thread1,thread2;
  struct timespec period = {1,0}; // 1 seconds

  // period1.tv_nsec = 0;
  // struct timespec period2 = {2,0}; // 2 seconds
  // pthread_create(&thread1, &period1, thread_function, NULL);
  // pthread_create(&thread2, &period2, thread_function, NULL);


  thread_function(&period);
}
