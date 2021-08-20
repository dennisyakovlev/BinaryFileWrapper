// Author: Dennis Yakovlev
#pragma once
#include <array>
#include <file_wrapper.h>
#include <fstream>
#include <ios>
#include <iostream>
#include <list>
#include <tuple_out.h>
#include <vector>

// write containers to file
void write_file_2() {

    std::ofstream outf("test.bin", std::ios::binary); // open in binary
    if (!outf) {
        std::cerr << "cannot open writing file" << std::endl;
    }

    std::vector<char> vec{'a', 'b', 'c', 'd', 'e'}; // value to write out
    std::list<double> lis{1,2,3}; // value to write out
    std::array<long long, 4> arr{10,20,30,40}; // value to write out

    auto res_tuple = write_block(&outf, &vec, &lis, &arr); // write

    outf.close();
    if (outf.fail()) {
        std::cerr << "cannot close writing file" << std::endl;
    }

    std::cout << "Wrote:\n";
    print_tuple(std::cout, &res_tuple); // print result

}

// read containers to file
void read_file_2() {

    std::ifstream inf("test.bin", std::ios::binary); // open in binary
    if (!inf) {
        std::cerr << "cannot open reading file" << std::endl;
    }

    // allocate size in container beforehand
    std::vector<char> vec(5); // value to read into
    std::list<double> lis(3); // value to read into
    std::array<long long, 4> arr; // value to read into

    auto res_tuple = read_block(&inf, &vec, &lis, &arr); // read

    inf.close();
    if (inf.fail()) {
        std::cerr << "cannot close reading file" << std::endl;
    }

    std::cout << "Read:\n";
    print_tuple(std::cout, &res_tuple); // print result

}