#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "MergeSort.h"
#include "Classic_MergeSort.h"

/* increasing the array length will allow the program to handle larger array sizes
 *  when increasing the array length consider increasing the SUB_ARRAY_LENGTH in MergeSort.h also to allot each child process
 *  to handle more of the workload in sorting the array otherwise will run out of resources
 *  For example when ARR_LENGTH = 299000   SUB_ARRAY_LENGTH = 10000 */

//length of the array to sort
#define ARR_LENGTH 1000

long getMicroTime(){
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	return (currentTime.tv_sec * (int)1e6 + currentTime.tv_usec);
}

int main() {
    /*CREATE SHARED MEMORY*/
    //shared memory ID
    int shmid;
    
    //key is set to private for shared memory
    key_t key = IPC_PRIVATE;
    int *shm_array;
    
    //length of array to sort
    size_t SHM_SIZE = sizeof(int) * ARR_LENGTH;
    
    //create segment
    if((shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666)) < 0) {
        perror("shmget creation");
        _exit(1);
    }
    
    //attach segment to data space
    if((shm_array = shmat(shmid, NULL, 0)) == (int *) -1) {
        perror("shmat, can't attach segment to data space");
        _exit(1);
    }
    
    //needed to initialize the seed value in order to use the random number generator function
    srand(time(NULL));
    
    //generates random int values to store in array
    importArrData(shm_array, ARR_LENGTH);
    
    //performs deep copy of the original array in order to be able to sort it twice
    int *temp = deep_copy(shm_array, ARR_LENGTH);
    printf("*************************************************\n");
    printf("Array size: %d\n", ARR_LENGTH);
    
    /*******************NOT using child processes to mergesort**************************/
    //uses clock to start and stop in order to count and evaluate how many seconds the mergesort
    //operation takes
    printf("************No process creation******************\n");
    long begin = getMicroTime();
	classic_mergeSort(temp, 0, ARR_LENGTH-1);
	long end = getMicroTime();
	float elap = (end - begin)/1e6;
    verifySorting(temp, ARR_LENGTH);
    printf("Time spent: %f seconds\n", elap);
    // free the memory used by the regular mergesort
    free(temp);
    
    /*******************using child processes to mergesort**************************/
    //uses clock to start and stop in order to count and evaluate how many seconds the mergesort
    //operation takes
    printf("************With process creation*****************\n");
    begin = getMicroTime();
    mergeSort(shm_array, 0, ARR_LENGTH-1);
	end = getMicroTime();
	elap = (end - begin)/1e6;
    verifySorting(shm_array, ARR_LENGTH);
    printf("Time spent: %f seconds\n", elap);
    
    //detach shared memory
    if(shmdt(shm_array) == -1) {
        perror("shmdt detaching shared memory");
        _exit(1);
    }
    
    //delete shared memory
    if(shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl cant delete shared memory segment");
        _exit(1);
    }
    
    return 0;
}

