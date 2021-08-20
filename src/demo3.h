// Author: Dennis Yakovlev
#pragma once
#include <climits>
#include <file_wrapper.h>
#include <fstream>
#include <ios>
#include <iostream>
#include <tuple_out.h>
#include <vector>

// write integers and container to file
void write_file_3() {

    std::ofstream outf("test.bin", std::ios::binary); // open in binary

    unsigned long long a = ULLONG_MAX;
    std::vector<double> vec(10, 45.234); // value to write out
    int b = INT_MIN;

    auto res_tuple = write_block(&outf, &a, &vec, &b); // write

    std::cout << "Wrote:\n";
    
    print_tuple(std::cout, &res_tuple); // print result

}

// read integers and container from file
void read_file_3() {

    std::ifstream inf("test.bin", std::ios::binary); // open in binary

    unsigned long long a;
    // allocate size in container beforehand
    std::vector<double> vec(10); // value to read into
    int b;

    auto res_tuple = read_block(&inf, &a, &vec, &b); // read

    std::cout << "Read:\n";
    print_tuple(std::cout, &res_tuple); // print result

}