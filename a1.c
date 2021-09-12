#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/*
    1.Text parser
    2.Priority queue (scheduling algorithm)
    3.Mini simulator, keep track of time. Step 1, step 2, step 3, what happens...
*/
typedef struct {
    char* userName;
    char* processId;
    int arrival;
    int duration;
}Job;

/***********************************************
 * Creates (initializes) and returns a new Job 
 * 
 */
Job createJob(char* userName, char* processId, int arrival, int duration) {
    Job job;
    job.userName = userName;
    job.processId = processId;
    job.arrival = arrival;
    job.duration = duration;
    return job;
}


typedef struct {
    char* userName;
    int finishTime;
}User;

User createUser(char* userName, int finishTime) {
    User user;
    user.userName = userName;
    user.finishTime = finishTime;
    //printf("User created %s\n", userName);
    return user;
}

bool checkIfUserDuplicates(User* userArray, int userArrayLength, char* userName) {
    int userArrayIndex = 0; // This index is for to iterate through the userArray
    while(userArrayIndex != userArrayLength) {
        // If they have the same userName, they we shouldn't add new user
        if (strcmp(userArray[userArrayIndex].userName, userName) == 0) {
            return true;
        }
        userArrayIndex++;
    }
    return false;
}

void updateUserFinishTime(User* userArray, char* userName, int userArrayLength, int finishTime) {
    int userArrayIndex = 0; // This index is for to iterate through the userArray
    //printf("updated finish time %d\n", finishTime);
    while(userArrayIndex != userArrayLength) {
        if (strcmp(userArray[userArrayIndex].userName, userName) == 0) {
            userArray[userArrayIndex].finishTime = finishTime;
            break;
        }
        userArrayIndex++;
    }
}

void printUserSummary(User* userArray, int userArrayLength) {
    printf("Summary\n");
    int userArrayIndex = 0; // This index is for to iterate through the userArray
    while(userArrayIndex != userArrayLength) {
        printf("%s\t%d\n",userArray[userArrayIndex].userName, userArray[userArrayIndex].finishTime);
        userArrayIndex++;
    }
}

char* userName;
char* processId;
int arrival;
int duration;
Job* jobArray;
User* userArray;
int jobArrayIndex = 0; // This index is used for tracking the Job index
int userArrayIndex = 0;

int totalProcessTime = 0; // Should be 15, added the duration for all jobs.
struct node *head; // Pointer to the head of the priority queue(linked list)

// A Linked List
struct node
{
    Job job;
    int priority;
    struct node *next;
};

/***********************************************
 * returns a new Node struct
 * expect the value to set the Node to
 */
struct node* createJobQueue(Job job) {
    struct node *newJobQueue = (struct node*)malloc(sizeof(struct node));
    newJobQueue -> job = job;
    newJobQueue -> priority = 0; // this is the first queue, so set the priority to 0 (default highest priority)
    newJobQueue -> next = NULL;

    return newJobQueue; // node created and return it
}

/***********************************************
 * returns a new Node struct
 * expect the value to set the Node to
 */
void insertJobQueue(struct node **head, Job job) {
    struct node *newJobQueue = createJobQueue(job);

    // need to travel the list
    struct node *jobQueueInProcess = (*head);
    // First check if head's arrival time equals to the new job's arrival time, then compare their duration
    if (jobQueueInProcess -> job.arrival == job.arrival) {
        if (jobQueueInProcess -> job.duration > job.duration) {
            newJobQueue -> next = jobQueueInProcess; // jobInProcess has lower priority than newJob
            (*head) = newJobQueue; // then newJob should be the head (aka. jobInProcess), since it has the highest priority
        }
        else {
            //Else, the new job should have a lower priority
            jobQueueInProcess -> next = newJobQueue;
        }
        
    }
    else {
        if (jobQueueInProcess->job.duration > job.duration) {
            newJobQueue -> next = jobQueueInProcess;
            (*head) = newJobQueue;
        }
        else if (jobQueueInProcess->job.duration == job.duration) {
            // FCFS then
            if (jobQueueInProcess->job.arrival < newJobQueue->job.arrival) {
                jobQueueInProcess -> next = newJobQueue;
            }
        }
        else {
            while (jobQueueInProcess -> next != NULL && jobQueueInProcess->next->job.duration < job.duration) {
                jobQueueInProcess = jobQueueInProcess -> next; // iterate the linked list to find where the newJobQueue should go
            }
            newJobQueue -> next = jobQueueInProcess -> next;
            jobQueueInProcess -> next = newJobQueue;
        }
    }
}

void delete_highest_priority(struct node** head) {
    if((*head) == NULL){
        printf("The priority queue is empty");
    }
    else {
        struct node *finishedJobQueue = (*head);
        (*head) = (*head)->next;
        free(finishedJobQueue); // delete head
    }
}

// Time simulator
void scheduleProcess(struct node **head, Job* jobArray, int totalProcessTime) {

    int time = jobArray[0].arrival; // Timer. (Will start from 2)

    bool job_queue_created = false; // switch for creating a new node.

    printf("%s\n","Time\tJob");
    while (totalProcessTime >= 0) {

        if (jobArray[0].arrival == time) { // if the job's arrival time is at this time, do a pre-emption instead of increasing the time.
            if(!job_queue_created) { // this is ONLY for enqueuing the first node(job)
                (*head) = createJobQueue(jobArray[0]); // initialize the queue
                jobArray = (Job*)jobArray+1; // Increment the pointer to point to the next element of the job array
                job_queue_created = !job_queue_created;
            }
            else {
                // If the current job is done, deque it
                if((*head)->job.duration == 0) {
                    //printf("%s finished at %d\n", userArray[userArrayIndex].userName, userArray[userArrayIndex].finishTime);
                    updateUserFinishTime(userArray, (*head)->job.userName, userArrayIndex, time); // Update user's finish time
                    delete_highest_priority(head);
                }
                
                insertJobQueue(head, jobArray[0]);
                jobArray = (Job*)jobArray + 1; // Increment the pointer to point to the next element of the job array
            }
            
        }
        else {
            // If the current job is done, deque it (this job is being processed in the last time)
            if((*head)->job.duration == 0) {
                //printf("%s finished at %d\n", userArray[userArrayIndex].userName, userArray[userArrayIndex].finishTime);
                updateUserFinishTime(userArray, (*head)->job.userName, userArrayIndex, time); // Update user's finish time
                delete_highest_priority(head);

                if (totalProcessTime == 0){
                    // If the process time is end, then break now! To prevent segfault
                    break;
                }
            }

            // If the current job is not done, keep doing it in the next time
            // Or could say, start the next job in this time
            (*head)->job.duration = (*head)->job.duration-1;
            
            printf("%d\t%s\n", time, (*head)->job.processId);
            time++;
            totalProcessTime--;
        }
    }
    printf("%d\t%s\n\n", time, "IDLE");
}

void parseTextInput() {
    char line[64]; // Input file lines to read

    bool ifUserDuplicates = false;

    // Skip the header
    fgets(line, sizeof(line), stdin);

    while (fgets(line, sizeof(line), stdin) != NULL) {
        userName = (char*)malloc(sizeof(char)*10);
        processId = (char*)malloc(sizeof(char)*20);
        sscanf(line, "%s %s %d %d", userName, processId, &arrival, &duration);

        Job job = createJob(userName, processId, arrival, duration);
        jobArray[jobArrayIndex] = job; // Add each job to the job array

        if(userArrayIndex != 0) { // Do not check duplicates if the userArray is null
            ifUserDuplicates = checkIfUserDuplicates(userArray, userArrayIndex, userName);
        }
        if(!ifUserDuplicates){
            User user = createUser(userName, 0); // Set the finishTime to 0 for now, will update them in simulator
            userArray[userArrayIndex] = user; // Add each user to the user array
            userArrayIndex++;
        }

        jobArrayIndex++;
        
        totalProcessTime += duration;
    }
}

int main(int argc, char **argv) {
    jobArray = (Job*)malloc(100*sizeof(Job));
    userArray = (User*)malloc(100*sizeof(User));

    parseTextInput();
    scheduleProcess(&head, jobArray, totalProcessTime);
    printUserSummary(userArray, userArrayIndex);

    free(userName);
    free(processId);

    return EXIT_SUCCESS;
}