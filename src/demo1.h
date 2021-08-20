// Author: Dennis Yakovlev
#pragma once
#include <file_wrapper.h>
#include <fstream>
#include <ios>
#include <iostream>
#include <tuple_out.h>

// write integer to file
void write_file_1() {

    std::ofstream outf("test.bin", std::ios::binary); // open in binary
    if (!outf) {
        std::cerr << "cannot open writing file" << std::endl;
    }

    int a = 25; // value to write out

    auto res_tuple = write_block(&outf, &a); // write

    outf.close();
    if (outf.fail()) {
        std::cerr << "cannot close writing file" << std::endl;
    }

    std::cout << "Wrote:\n";
    print_tuple(std::cout, &res_tuple); // print result

}

// read integer from file
void read_file_1() {

    std::ifstream inf("test.bin", std::ios::binary); // open in binary
    if (!inf) {
        std::cerr << "cannot open reading file" << std::endl;
    }

    int a; // value to read into

    auto res_tuple = read_block(&inf, &a); // read

    inf.close();
    if (inf.fail()) {
        std::cerr << "cannot close reading file" << std::endl;
    }

    std::cout << "Read:\nvalue: " << a; // print result
    // OR
    // std::cout << "Read:\n";
    // print_tuple(std::cout, &res_tuple); // print result

}