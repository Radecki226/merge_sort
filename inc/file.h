#ifndef _FILE_H_
#define _FILE_H_

#define FILE_MAX_LEN 10000000

#include <string>
class File
{
        std::string _name;
        int _rx_array[FILE_MAX_LEN];
        int _tx_array[FILE_MAX_LEN];
        int _len;
public:
        File(std::string name);

        int read_data();

        int* get_rx_array();

        int get_len();

        void set_tx_array(int array[]);

        int write_data();

};


#endif //_FILE_H_
