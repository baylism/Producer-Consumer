/******************************************************************
 * Header file for the helper functions. This file includes the
 * required header files, as well as the function signatures and
 * the semaphore values (which are to be changed as needed).
 ******************************************************************/


# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/shm.h>
# include <sys/sem.h>
# include <sys/time.h>
# include <math.h>
# include <errno.h>
# include <string.h>
# include <pthread.h>
# include <ctype.h>
# include <iostream>
using namespace std;

# define SEM_KEY 0x45 // Change this number as needed
# define NUM_SEMS 3
# define ITEMS_SEM 0  // 0
# define SPACE_SEM 1  // queue_size
# define MUTEX_SEM 2  // 1


/* --- Forward declarations --- */
class CQueue;

/* --- Global variables --- */

extern int semarray_id;
extern int jobs_per_producer;
extern CQueue* cq_ptr;

/* --------------- Provided code --------------- */

union semun_1 {
    int val;               /* used for SETVAL only */
    struct semid_ds *buf;  /* used for IPC_STAT and IPC_SET */
    ushort *array;         /* used for GETALL and SETALL */
};

int check_arg (char *);
int sem_create (key_t, int);
int sem_init (int, int, int);
void sem_wait (int, short unsigned int);
void sem_signal (int, short unsigned int);
int sem_close (int);

/* --------------- Added by Max --------------- */

/* --- Job handling --- */
int random_to(int max);
int sem_timed_wait(int id, short unsigned int num, struct timespec *timeout);
int sem_wait_timeout(int id, short unsigned int num, int timeout);

/* --- Circular queue --- */
class CQueue {
private:
  int *cqueue;
  int front;
  int rear;
  int queue_size;
public:
  CQueue(int queue_size);
  ~CQueue();
  bool queue_full();
  bool queue_empty();
  int enqueue(int duration);
  int dequeue(int &duration);
  void print_cqueue();
};

