#include "osapi.h"
#include <iostream>
#include <vector>
#include <cmath>
#include "merge_sort.h"
using namespace std;

#define ALMOST_INFINITE 200000

static void merge(int *A, int p, int q, int r) {

	//Lengths of subarrays
	int len1 = q - p;
	int len2 = r - q + 1;

	//Subarrays, copy of A needs to done before merging
	int *A1 = new int[len1];
	int *A2 = new int[len2];

	for (int i = 0; i < len1; i++) {
		A1[i] = A[p + i];
	}

	for (int i = 0; i < len2; i++) {
		A2[i] = A[q + i];
	}

	//Pointers for merging data
	int ptr = p;
	int ptr1 = 0;
	int ptr2 = 0;


	while(ptr1 < len1 && ptr2 < len2) {
		if (A1[ptr1] < A2[ptr2]) {
			A[ptr++] = A1[ptr1++];
		} else {
			A[ptr++] = A2[ptr2++];
		}
	}

	while (ptr1 < len1) {
		A[ptr++] = A1[ptr1++];
	}

	while (ptr2 < len2) {
		A[ptr++] = A2[ptr2++];
	}

	delete [] A1;
	delete [] A2;
}

void mergeSortSingleThread(int *A, int p, int r)
{
	//Finish condition (array size is 1)
	if (p == r) return;

	int q = (p+r) / 2 + 1;
	mergeSortSingleThread(A, p, q-1);
	mergeSortSingleThread(A, q, r);

	merge(A, p, q, r);
}

class SortingThread : public osapi::Thread
{
	int *_A;
	int _len;

public:
    SortingThread(int *A, int len) : Thread(0, 256, osapi::JOINABLE), _A{A}, _len{len}  {;}
private:
    virtual void job() {
    	mergeSortSingleThread(_A, 0, _len-1);
    }
};

void mergeSort(int *A, int len, int n_threads)
{
	vector<SortingThread> threads;
	vector<int> chunks;

	//Calculate and allocate range for each thread
	float each_thread_portion = (float)len / n_threads;

	for (int i = 0; i < n_threads; i++) {
		int start_idx = (int)(each_thread_portion * i);
		int stop_idx = (int)(each_thread_portion * (i+1));

		int chunk_len = stop_idx - start_idx;

		threads.push_back(SortingThread(&A[start_idx],chunk_len));
		chunks.push_back(chunk_len);
	}

	//Run merge sort
	for (int i = 0; i < n_threads; i++){
		threads[i].run();
	}

	for (int i = 0; i < n_threads; i++) {
		threads[i].join(ALMOST_INFINITE);
	}

	//Merge thread portions one after another
	int current_idx = 0;
	for (int thread = 0; thread < n_threads-1; thread++) {
		current_idx += chunks[thread];
		merge(A, 0, current_idx, current_idx+chunks[thread+1]-1);
	}

}

