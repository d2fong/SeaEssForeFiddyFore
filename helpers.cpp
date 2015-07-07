//
// Created by Dylan Fong on 2015-07-06.
//

#include "source_file.h"


//Get the size an argTypes array from an rpcCall
int get_int_array_length(int* intArray) {
    int i = 0;
    while(intArray[i] != 0) {
        i++;
    }
    //We dont count the zero we find as another argType
    return i - 1;
}