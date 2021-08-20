// Author: Dennis Yakovlev
#pragma once
#include <algorithm>
#include <fstream>
#include <ios>
#include <iterator>
#include <ostream>
#include <stddef.h>
#include <tuple>
#include <type_traits>
#include <utility>

// Writable is something that can be written to binary file
struct Writable {

    Writable(std::streamsize sz, std::streamsize sz_tot) : size(sz), size_total(sz_tot) {}

    std::streamsize size;
    std::streamsize size_total;

};

// native types to be written to file
template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
struct NativeWritable : public Writable {

    using ty = T;

    NativeWritable(T* native) : var(native), Writable::Writable(sizeof(T), sizeof(T)) {}

    T* var;

};

// a container of native types
// assume container has size member function
template<typename Cont, typename = std::enable_if_t<std::is_arithmetic_v<typename Cont::value_type>>>
struct ContainerWritable : public Writable {

    using ty = Cont;

    ContainerWritable(Cont* cont) : Writable::Writable(static_cast<std::streamsize>(sizeof(typename Cont::value_type)), 
                                                       static_cast<std::streamsize>(sizeof(typename Cont::value_type) * cont->size())), 
                                                       var(cont) {}

    Cont* var;

};

// these should check for the member AND that it returns the correct type
template<typename T, typename = void>
struct _has_size_member : std::false_type {}; // whether type has <size> member function

template<typename T>
struct _has_size_member<T, std::void_t<std::is_member_function_pointer<decltype(&T::size)>>> : std::true_type {}; // whether type has <size> member function

template<typename T> constexpr bool _has_size_member_v = _has_size_member<T>::value; // whether type has <size> member function value

template<typename T, typename = void>
struct _has_cbegin_member : std::false_type {}; // whether type has <cbegin> member function

template<typename T>
struct _has_cbegin_member<T, std::void_t<std::is_member_function_pointer<decltype(&T::cbegin)>>> : std::true_type {}; // whether type has <cbegin> member function

template<typename T> constexpr bool _has_cbegin_member_v = _has_cbegin_member<T>::value; // whether type has <cbegin> member function value

template<typename T, typename = void>
struct _has_cend_member : std::false_type {};

template<typename T>
struct _has_cend_member<T, std::void_t<std::is_member_function_pointer<decltype(&T::cend)>>> : std::true_type {};

template<typename T> constexpr bool _has_cend_member_v = _has_cend_member<T>::value;

template<typename T, typename = std::enable_if_t<_has_size_member_v<T> &&
                                      _has_cbegin_member_v<T> &&
                                      _has_cend_member_v<T>>>
struct _is_container {}; // whether type is valid container
                         // Note: doesnt actually check ALL requirments

template<typename T> constexpr bool _is_container_v = _has_size_member_v<T> && _has_cbegin_member_v<T> && _has_cend_member_v<T>;

template<typename T, typename = void>
struct _is_nativewritable : std::false_type {}; // whether type is NativeWritable

template<typename T>
struct _is_nativewritable<T, std::void_t<std::enable_if_t<std::is_same_v<T, NativeWritable<typename T::ty>>>>> : std::true_type {}; // whether type is NativeWritable

template<typename T> constexpr bool _is_nw_v = _is_nativewritable<T>::value; // whether type is NativeWritable value

template<typename T, typename = void>
struct _is_containerwritable : std::false_type {}; // whether type is ContainerWritable

template<typename T>
struct _is_containerwritable<T, std::void_t<std::enable_if_t<std::is_same_v<T, ContainerWritable<typename T::ty>>>>> : std::true_type {}; // whether type is ContainerWritable

template<typename T> constexpr bool _is_cw_v = _is_containerwritable<T>::value; // whether type is ContainerWritable value

template<typename T, typename U, typename... Args>
struct _all_base { // whether all of U and Args are a base of T
    template<typename = std::enable_if_t<std::is_base_of_v<T, U>>>
    static void check() {
        _all_base<T, Args...>::check();
    }
};

template<typename T, typename U>
struct _all_base<T, U> { // base case
    template<typename = std::enable_if_t<std::is_base_of_v<T, U>>>
    static void check() {}
};

template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<std::remove_pointer_t<T>>>>
static NativeWritable<std::remove_pointer_t<T>> _get_writable(T var) {

    return NativeWritable(var);
    
}

template<typename T, typename = std::enable_if_t<_is_container_v<std::remove_pointer_t<T>>>>
static ContainerWritable<std::remove_pointer_t<T>> _get_writable(T var) {

    return ContainerWritable(var);

}

template<typename T>
auto get_args_tuple(T arg) {

    return std::tuple(_get_writable(arg));

}

template<typename T, typename... Args>
auto get_args_tuple(T arg, Args... args) {

    return std::tuple_cat(std::tuple(_get_writable(arg)), get_args_tuple(args...));

}

template<typename T, std::size_t N, typename = void>
struct _block {};

template<typename T, std::size_t N>
struct _block<T, N, std::enable_if_t<_is_nw_v<std::tuple_element_t<N - 1, T>>>> {

    static void write(std::ofstream* outf, T* tupe) {

        _block<T, N - 1>::write(outf, tupe);

        auto arg = std::get<N - 1>(*tupe);
        outf->write(reinterpret_cast<char*>(&*arg.var), arg.size);

    }

    static void read(std::ifstream* inf, T* tupe) {

        _block<T, N - 1>::read(inf, tupe);

        auto arg = std::get<N - 1>(*tupe);
        inf->read(reinterpret_cast<char*>(&*arg.var), arg.size);

    }

};

template<typename T, std::size_t N>
struct _block<T, N, std::enable_if_t<_is_cw_v<std::tuple_element_t<N - 1, T>>>> {

    static void write(std::ofstream* outf, T* tupe) {

        _block<T, N - 1>::write(outf, tupe);

        const auto arg = std::get<N - 1>(*tupe);
        for (auto iter = arg.var->cbegin(); iter != arg.var->cend(); ++iter) {
            outf->write(reinterpret_cast<const char*>(&*iter), arg.size);
        }

    }

    static void read(std::ifstream* inf, T* tupe) {

        _block<T, N - 1>::read(inf, tupe);

        auto arg = std::get<N - 1>(*tupe);
        for (auto iter = arg.var->begin(); iter != arg.var->end(); ++iter) {
            inf->read(reinterpret_cast<char*>(&*iter), arg.size);
        }

    }

};

template<typename T>
struct _block<T, 1, std::enable_if_t<_is_nw_v<std::tuple_element_t<0, T>>>> {

    static void write(std::ofstream* outf, T* tupe) {

        auto arg = std::get<0>(*tupe);
        outf->write(reinterpret_cast<char*>(&*arg.var), arg.size);

    }

    static void read(std::ifstream* inf, T* tupe) {

        auto arg = std::get<0>(*tupe);
        inf->read(reinterpret_cast<char*>(&*arg.var), arg.size);

    }

};

template<typename T>
struct _block<T, 1, std::enable_if_t<_is_cw_v<std::tuple_element_t<0, T>>>> {

    static void write(std::ofstream* outf, T* tupe) {

        const auto arg = std::get<0>(*tupe);
        for (auto iter = arg.var->cbegin(); iter != arg.var->cend(); ++iter) {
            outf->write(reinterpret_cast<const char*>(&*iter), arg.size);
        }

    }

    static void read(std::ifstream* inf, T* tupe) {

        auto arg = std::get<0>(*tupe);
        for (auto iter = arg.var->begin(); iter != arg.var->end(); ++iter) {
            inf->read(reinterpret_cast<char*>(&*iter), arg.size);
        }

    }

};

// a block is chunk of data read to be written/ read to/ from a file
// function will only works if args is made purely of arithmetic types
// and containers meeting c++17 requirements and contain only
// arithmetic types
template<typename... Args>
auto write_block(std::ofstream* outf, Args... args) {

    auto info_tuple = get_args_tuple(args...);

    using tuple_ty = decltype(info_tuple);

    _block<tuple_ty, std::tuple_size_v<tuple_ty>>::write(outf, &info_tuple);

    return info_tuple;

}

// a block is chunk of data read to be written/ read to/ from a file
// function will only works if args is made purely of arithmetic types
// and containers meeting c++17 requirements and contain only
// arithmetic types
template<typename... Args>
auto read_block(std::ifstream* inf, Args... args) {

    auto info_tuple = get_args_tuple(args...);

    using tuple_ty = decltype(info_tuple);

    _block<tuple_ty, std::tuple_size_v<tuple_ty>>::read(inf, &info_tuple);

    return info_tuple;

}

template<typename T, std::size_t N>
struct _block_size {

    static std::streamsize func(T* tupe) {

        return std::get<N - 1>(*tupe).size_total + _block_size<T, N - 1>::func(tupe);

    }

};

template<typename T>
struct _block_size<T, 1> {

    static std::streamsize func(T* tupe) {

        return std::get<0>(*tupe).size_total;

    }

};

// get total byte size of block
template<typename T, typename = _all_base<Writable, T>>
std::streamsize block_size_total(T* tupe) {

    return _block_size<T, std::tuple_size_v<T>>::func(tupe);

}