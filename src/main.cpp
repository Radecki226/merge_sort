/*
 * main.cpp
 *
 *  Created on: Feb 2, 2023
 *      Author: piotrek
 */
#include "file.h"
#include "merge_sort.h"
#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;
using namespace std::chrono;

#define MAX_N_THREAD 10

#define N_REPETITIONS 3

File myFile("merge_sort");

static auto runAndMeasureTime(int* array, int len, int n_threads)
{
	auto start = high_resolution_clock::now();
	mergeSort(array, len, n_threads);
	auto stop = high_resolution_clock::now();

	auto duration = duration_cast<milliseconds>(stop - start);

	return duration.count();
}

static auto runAndMeasureTimeSingleThread(int *array, int len)
{
	auto start = high_resolution_clock::now();
	mergeSortSingleThread(array, 0, len-1);
	auto stop = high_resolution_clock::now();

	auto duration = duration_cast<milliseconds>(stop - start);

	return duration.count();
}

int main()
{
	cout << "Merge sort test" << endl;
	//File myFile("merge_sort");
	int *array;
	int len;
	auto time = 0;

	//Check singlethread version
	for (int j = 0; j < N_REPETITIONS; j++) {
		if (myFile.read_data() != 0) {
			cout << "File merge_sort_input.txt not found!" << endl;
			return -1;
		}
		array = myFile.get_rx_array();
		len = myFile.get_len();

		time += runAndMeasureTimeSingleThread(array, len);

	}

	//average
	time /= N_REPETITIONS;
	cout << "Classic Merge Sort Time = " << time << " ms." << endl;


	//Check multithread version
	for (int i = 1; i <= MAX_N_THREAD; i++) {

		time = 0;

		//Repeat N_REPETITIONS times
		for (int j = 0; j < N_REPETITIONS; j++) {
			if (myFile.read_data() != 0) {
				cout << "File merge_sort_input.txt not found!" << endl;
				return -1;
			}
			array = myFile.get_rx_array();
			len = myFile.get_len();

			time += runAndMeasureTime(array, len, i);

		}

		//average
		time /= N_REPETITIONS;
		cout << "time[" << i << "] = " << time << " ms." << endl;

	}

	mergeSort(array, len, 1);

	myFile.set_tx_array(array);

	myFile.write_data();

	return 0;

}


