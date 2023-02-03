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

#define MAX_N_THREAD 30

static auto run_and_measure_time(int* array, int len, int n_threads)
{
	auto start = high_resolution_clock::now();
	mergeSort(array, len, n_threads);
	auto stop = high_resolution_clock::now();

	auto duration = duration_cast<milliseconds>(stop - start);

	return duration.count();
}

int main()
{
	//long long int time[MAX_N_THREAD];

	File myFile("merge_sort");
	int *array;
	int len;
	for (int i = 1; i <= MAX_N_THREAD; i++) {
		if (myFile.read_data() != 0) {
			cout << "File merge_sort_input.txt not found!" << endl;
			return -1;
		}

		array = myFile.get_rx_array();
		len = myFile.get_len();

		auto time = run_and_measure_time(array, len, i);

		cout << "time[" << i << "] = " << time << " ms." << endl;
	}

	mergeSort(array, len, 1);

	myFile.set_tx_array(array);

	myFile.write_data();

	return 0;

}


