/******************************************************************
 * The helper file that contains the following helper functions:
 * check_arg - Checks if command line input is a number and returns it
 * sem_create - Create number of sempahores required in a semaphore array
 * sem_init - Initialise particular semaphore in semaphore array
 * sem_wait - Waits on a semaphore (akin to down ()) in the semaphore array
 * sem_signal - Signals a semaphore (akin to up ()) in the semaphore array
 * sem_close - Destroy the semaphore array
 ******************************************************************/

# include "helper.h"

/* Checks if command line input is a number and returns it */
int check_arg (char *buffer)
{
  int i, num = 0, temp = 0;
  if (strlen (buffer) == 0)
    throw -1;
  for (i=0; i < (int) strlen (buffer); i++)
  {
    temp = 0 + buffer[i];
    if (temp > 57 || temp < 48)
      throw -1;
    num += pow (10, strlen (buffer)-i-1) * (buffer[i] - 48);
  }
  return num;
}

/* Create number of sempahores required in a semaphore array */
int sem_create (key_t key, int num)
{
  int id;
  if ((id = semget (key, num,  0666 | IPC_CREAT | IPC_EXCL)) < 0)
    return -1;
  return id;
}

/* Initialise particular semaphore in semaphore array */
int sem_init (int id, int num, int value)
{
  union semun_1 semctl_arg;
  semctl_arg.val = value;
  if (semctl (id, num, SETVAL, semctl_arg) < 0)
    return -1;
  return 0;
}

/* Waits on a semaphore (akin to down ()) in the semaphore array */
void sem_wait (int id, short unsigned int num)
{
  struct sembuf op[] = {
    {num, -1, SEM_UNDO}
  };
  semop (id, op, 1);
}

/* Signals a semaphore (akin to up ()) in the semaphore array */
void sem_signal (int id, short unsigned int num)
{
  struct sembuf op[] = {
    {num, 1, SEM_UNDO}
  };
  semop (id, op, 1);
}

/* Destroy the semaphore array */
int sem_close (int id)
{
  if (semctl (id, 0, IPC_RMID, 0) < 0)
    return -1;
  return 0;
}


/* --------------- Added by Max --------------- */

/* --- Job handling --- */
int random_to(int max) {
  return rand() % max + 1;
}

/* Timed wait on a semaphore (akin to down ()) in the semaphore array */
int sem_wait_timeout(int id, short unsigned int num, int timeout)
{
   struct sembuf op[] = {
     {num, -1, SEM_UNDO}
   };
   struct timespec tout[] = {
     {timeout, 0}
   };
   return semtimedop (id, op, 1, tout);
}

/* --- Circular queue --- */
/* Constructor */
CQueue::CQueue(int queue_size):cqueue(new int[queue_size]), front(-1), rear(-1), queue_size(queue_size){}

/* Destructor */
CQueue::~CQueue() {
  delete[] cqueue;
}

/* Return true if the queue is full */
bool CQueue::queue_full() {
  return ((front == rear + 1) || (front == 0 && rear == queue_size - 1));
}

/* Return true if the queue is empty */
bool CQueue::queue_empty() {
  return (front == -1);
}

/* Add an item to the queue and return its location */
int CQueue::enqueue(int duration) {
  if(queue_full()) {
    cout << "Queue is full, can't enqueue. " << endl;
    return -1;
  }
  else {
    if (front == -1)                        // adding first element to the list
      front = 0;
    rear = (rear + 1) % queue_size;         // move rear to next position
    cqueue[rear] = duration;                // add element in next position

    return rear;
  }
}

/* Remove an item to the queue into duration and return its location */
int CQueue::dequeue(int& duration) {
  int front_temp;
  if(queue_empty()) {
    cout << "Queue is empty, can't dequeue. " << endl;
    return -1;
  }
  else {
    duration = cqueue[front];               // take element at front
    front_temp = front;

    if (front == rear) {                    // queue is now empty
      front = -1;
      rear = -1;
    }
    else                                    // move front to next position
      front = (front + 1) % queue_size;

    return front_temp;
  }
}
