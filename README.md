# Binary File Wrapper

## Contents

* [Aim](#Project-aim)
* [Usage](#Usage)
  * [wrapper](#file_wrapper.h)
  * [print](#tuple_out.h)

## Project aim

C++ interface to binary files to simplify usage and make I/O errors harder to make.

## Usage

Copy paste files as needed into project and compile with project. See demos for examples.

### file_wrapper.h

```get_args_tuple``` gets formatted form of arguements to be used in I/O.  
<strong>Usage: </strong> Pass pointers to any information to be formatted. Returns tuple
containing formatted arguements.

```write_block``` writes to ofstream.  
<strong>Usage: </strong> Pass pointer to ofstream opened in binary mode, then pointers to any information to output. Returns tuple obtained with ```get_args_tuple```.

```read_block``` reads to ifstream.  
<strong>Usage: </strong> Pass pointer to ifstream opened in binary mode, then pointers to any information to read into. Space in containers must be allocated before using. Returns tuple obtained with ```get_args_tuple```.

```block_size_total``` gives total size in bytes of a block.  
<strong>Usage: </strong> Pass pointer to tuple returned from ```get_args_tuple```.

### tuple_out.h

```print_tuple``` outputs tuple to standard output.  
<strong>Usage: </strong> Pass standard output stream, then pointer to tuple to print. Can print result of functions from ```file_wrapper.h```.
