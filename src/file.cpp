/*
* file.c
*
*  Created on: Feb 2, 2023
*      Author: piotrek
*/

#include <iostream>
#include <fstream>
#include <string>
#include "file.h"


File :: File(std::string name) : _name{name}
{
    _len = 0;
}

int File :: read_data()
{
    int len = 0;

    std::ifstream file(_name + "_input.txt");

    if (file.is_open()) {
        while(!file.eof() && len < FILE_MAX_LEN) {
            std::string line;
            getline(file, line);

            _rx_array[len++] = stoi(line);

        }
    } else {
        return -1;
    }

    _len = len;

    return 0;
}

int *File :: get_rx_array()
{
    return _rx_array;
}

int File :: get_len()
{
	return _len;
}

void File :: set_tx_array(int array[])
{
    memcpy(_tx_array, array, _len * sizeof(int));
}

int File :: write_data()
{
	std::ofstream file(_name + "_output.txt");

	if (file.is_open())
	{
		for (int line = 0; line < _len; line++) {
			file << _tx_array[line] << std::endl;
		}
		return 0;

	} else {
		return -1;
	}
}





