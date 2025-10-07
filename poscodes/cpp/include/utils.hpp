#pragma once
#include <string>
#include <cstddef>
#include "poscode.hpp"


void quick_sort(Poscode *A, std::size_t n);
void merge_sort(Poscode *A, std::size_t n);
void radix_sort(Poscode *A, std::size_t n);

Poscode* readCodes(const std::string &strfile, std::size_t n);
void deleteCodes(Poscode *codes);



