/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/

#include "helper.h"
#include <ctime>
#include <vector>

void *producer (void *id);
void *consumer (void *id);

int jobs_per_producer;
int semarray_id;
CQueue *cq_ptr;

/* Usage: queue size, jobs_per_producer, num_producers, num_consumers */
int main (int argc, char **argv)
{
  int queue_size;
  int num_producers;
  int num_consumers;
  key_t key = SEM_KEY;

  if (argc != 5) {
    cout << "Incorrect number of paraters." << endl;
    return 0;
  }

  try {
    queue_size = check_arg(argv[1]);
    jobs_per_producer = check_arg(argv[2]);
    num_producers = check_arg(argv[3]);
    num_consumers = check_arg(argv[4]);
  } catch (int error) {
    cout << "Input error. Please enter four numeric characters." << endl;
    return 0;
  }

  /* Create array of required number of semaphores */
  semarray_id = sem_create(key, NUM_SEMS);

  /* Initialise semaphores */
  if (sem_init(semarray_id, ITEMS_SEM, 0) != 0) {
    cout << "Error initialising ITEMS_SEM" << endl;
    return 0;
  }

  if (sem_init(semarray_id, SPACE_SEM, queue_size) != 0) {
    cout << "Error initialising SPACE_SEM" << endl;
    return 0;
  }

  if (sem_init(semarray_id, MUTEX_SEM, 1) != 0) {
    cout << "Error initialising MUTEX_SEM" << endl;
    return 0;
  }

  /* Create jobs queue */
  CQueue cq(queue_size);
  cq_ptr = &cq;

  /* Add thread ids returned by pthread_create to array */
  vector<pthread_t> producers(num_producers);
  vector<pthread_t> consumers(num_consumers);

  int p_counts[num_producers];
  int c_counts[num_consumers];

  /* Create producers */
  for (int i = 0; i < num_producers; i++) {
    p_counts[i] = i + 1;

    if (pthread_create(&producers[i], NULL, producer, (void *) &p_counts[i]) != 0) {
      cout << "Error creating producer thread " << i << endl;
    }
  }

  /* Create consumers */
  for (int i = 0; i < num_consumers; i++) {
    c_counts[i] = i + 1;

    if (pthread_create(&consumers[i], NULL, consumer, (void *) &c_counts[i]) != 0) {
      cout << "Error creating consumer thread " << i << endl;
    }
  }

  /* Wait for threads to finish and clean up */
  for (int i = 0; i < num_producers; i++) {
    pthread_join (producers[i], NULL);
  }

  for (int i = 0; i < num_consumers; i++) {
    pthread_join (consumers[i], NULL);
  }

  /* Delete semaphor array */
  sem_close(semarray_id);

  return 0;
}

void *producer(void *parameters)
{
  int job_id;
  int duration;
  int* param = (int *) parameters;

  for (int i = 0; i < jobs_per_producer; i++) {
    sleep(random_to(5));

    if (sem_wait_timeout(semarray_id, SPACE_SEM, 20) != 0){
      if (errno == EAGAIN) {
        printf("Producer (%d): No more jobs to generate\n", *param);
        pthread_exit(0);
      }
    }

    // down on mutex
    sem_wait(semarray_id, MUTEX_SEM);

    duration = random_to(10);
    job_id = cq_ptr->enqueue(duration);
    printf("Producer (%d): Job id %d duration %d\n", *param, job_id, duration);

    // up mutex
    sem_signal(semarray_id, MUTEX_SEM);

    // up item
    sem_signal(semarray_id, ITEMS_SEM);
  }

  printf("Producer (%d): No more jobs to generate\n", *param);
  pthread_exit(0);
}

void *consumer (void *parameters)
{
  int job_id;
  int duration;
  int sem_wait_result;

  int *param = (int *) parameters;

  while (true) {
    sem_wait_result = sem_wait_timeout(semarray_id, ITEMS_SEM, 20);
    if (sem_wait_result != 0 ) {
      if (errno == EAGAIN) {
        printf("Consumer (%d): No more jobs left.\n", *param);
        pthread_exit (0);
      }
    }

    // down mutex
    sem_wait(semarray_id, MUTEX_SEM);

    // fetch item
    job_id = cq_ptr->dequeue(duration);

    // up mutex
    sem_signal(semarray_id, MUTEX_SEM);

    // up space
    sem_signal(semarray_id, SPACE_SEM);

    printf("Consumer (%d): Job id %d executing sleep duration %d\n", *param, job_id, duration);
    sleep(duration);
    printf("Consumer (%d): Job id %d completed %d\n", *param, job_id, duration);
    }

  pthread_exit (0);
}
