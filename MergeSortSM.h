/* Merge sort that utilizes process creation */
#ifndef MergeSort_h
#define MergeSort_h

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARRAY_NUM 500 //max integer generated in random array
#define SUB_ARRAY_LENGTH 10000 //length of array that one child process will handle

//creates array of integers that need to be sorted
//commented out loop creates array counting backwards where a[0] = length
//then a[1] = length - 1 ...etc, for loop chooses random int between 0 and 50 
//and stores in array up to specified length
void importArrData(int a[], int len) {
	for (int i = 0; i < len; i++) {
		a[i] = rand() % (MAX_ARRAY_NUM + 1 - 0) + 0;
	} 
	return;
}

//subarrays merged together
void merge(int a[], int left1, int mid, int right1) {
    int counter = right1-left1+1;
    int sorted[counter];
    int i = left1;
    int j = mid+1;
    int k = 0;
    //continue loop while the counter variables i and j are
	//less than the subarray bounds
    while(i <= mid && j <= right1) {
		//if the first elem of sub array is less than
		//first elem of 2nd sub array, switch
        if (a[i] < a[j]) {
            sorted[k++] = a[i++];
        }
		//visa versa for this where the values swap
        else if (a[j] < a[i]) {
            sorted[k++] = a[j++];
        }
		//if they are equal, then place both values
		//next to each other
        else if (a[i] == a[j]) {
            sorted[k++] = a[i++];
            sorted[k++] = a[j++];
        }
    }
    while (i <= mid) {
        sorted[k++] = a[i++];
    }
    while (j <= right1) {
        sorted[k++] = a[j++];
    }
    //int arrayCount = left1;
	
	//copy entire sorted array into a at left index
    i = 0;
    while (i < counter) {
        a[left1] = sorted[i];
        i++;
        left1++;
    }
}
//used only when elements are less than 5 in considered array
void insertionSort(int a[], int elems) {
    int key;
    int j;
    for (int i = 1; i < elems; i++) {
        key = a[i];
        j = i -1;
        //elements greater than key are moved to one position
        //ahead of their current position
        while (j >= 0 && a[j] > key) {
            a[j + 1] = a[j];
            j = j - 1;
        }
        a[j + 1] = key;
    }
}

//recursive function
//a is unsorted array, left is index of current left array
//right is index of current right array 
void mergeSort(int a[], int left, int right) {
	//length of current sub array is calculated every recursion
    int arrLen =(right-left+1);
    
    //insertion sort for arrays less than 6 elements
    if (arrLen < SUB_ARRAY_LENGTH) {
        insertionSort(a+left, arrLen);
        return;
    }
    
    //create left and right process
    pid_t lpid, rpid;
    
    lpid = fork();
    
    //left process not created catch
    if (lpid < 0) {
        perror("Left child process not created.\n");
        _exit(-1);
    }
    else if (lpid == 0) {
        //recursively call mergeSort again with half the array length
        mergeSort(a, left, left+arrLen/2-1);
        //need to exit if recursive call or it will get stuck
        _exit(0);
    }
    else {
        rpid = fork();
        
        //right process not created catch
        if (rpid < 0) {
            perror("Right child process not created.\n");
            _exit(-1);
        }
        else if (rpid == 0) {
            //recursively call mergerSort again if fork successful with half the length
            mergeSort(a, left+arrLen/2, right);
            _exit(0);
        }
    }
    
    int status;
    //wait for child process to finish if mergeSort process is completed
    waitpid(lpid, &status, 0);
    waitpid(rpid, &status, 0);
    
    //merge sorted sub arrays back together sorted
    merge(a, left, left+arrLen/2-1, right);
}

void verifySorting(int a[], int len) {
    if (len == 1) {
        printf("Successful sorting\n");
		//if successful, return out of function
		return;
    }
    int i = 1;
    while (i < len) {
        if (a[i] < a[i-1]) {
            printf("Failed sorting\n");
			return;
        }
        i++;
    }
    printf("Successful sorting\n");
	return;
}

//prints current array of parameter provided
void printArray(int a[], int length) {
    printf("{%d", a[0]);
    for(int i = 1; i < length; i++) {
        printf(", %d", a[i]);
    }
    printf("}\n");
}


#endif /* MergeSort_h */
