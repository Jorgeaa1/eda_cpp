#include "poscode.hpp"
#include "utils.hpp"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

static void print_first(const char* title, Poscode* A, std::size_t n, std::size_t k=10) {
    std::cout << "\n" << title << " — primeros " << k << ":\n";
    for (std::size_t i = 0; i < k && i < n; ++i) {
        std::cout << A[i].getData() << "\n";
    }
}

static bool is_sorted(Poscode* A, std::size_t n) {
    for (std::size_t i = 1; i < n; ++i) {
        if (A[i].getData() < A[i-1].getData()) return false;
    }
    return true;
}


typedef void (*SorterFn)(Poscode*, std::size_t);

static void bench(const std::string& name, SorterFn sorter,
                  Poscode* data, std::size_t n, int runs) {
    double total_ms = 0.0;
    std::vector<Poscode> tmp; tmp.reserve(n);

    for (int r = 0; r < runs; ++r) {
        tmp.assign(data, data + n);
        const auto t0 = std::chrono::high_resolution_clock::now();
        sorter(&tmp[0], n);
        const auto t1 = std::chrono::high_resolution_clock::now();
        const double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
        total_ms += ms;
        if (r == 0) {
            std::cout << "\n" << name << " (run 1): " << ms << " ms\n";
            print_first(name.c_str(), &tmp[0], n, 10);
            std::cout << "verificación: " << (is_sorted(&tmp[0], n) ? "OK" : "FAIL") << "\n";
        }
    }
    std::cout << name << " — promedio (" << runs << " corridas): " << (total_ms / runs) << " ms\n";
}

int main(int argc, char** argv) {
    
    if (argc < 3) {
        std::cerr << "Uso: " << argv[0]
                  << "<ruta_archivo> <n> [--algo radix|quick|merge|all] [--runs R]\n";
        return 1;
    }

    std::string path = argv[1];
    std::size_t n = std::strtoull(argv[2], nullptr, 10);
    std::string which = "all";
    int runs = 1;

    for (int i = 3; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--algo" && i + 1 < argc) which = argv[++i];
        else if (a == "--runs" && i + 1 < argc) runs = std::atoi(argv[++i]);
    }

    Poscode* data = readCodes(path, n);
    if (!data) {
        std::cerr << "No se pudo leer el archivo.\n";
        return 2;
    }

    print_first("Original", data, n, 10);

    if (which == "radix" || which == "all")  bench("RadixSort", radix_sort, data, n, runs);
    if (which == "quick" || which == "all")  bench("QuickSort", quick_sort, data, n, runs);
    if (which == "merge" || which == "all")  bench("MergeSort", merge_sort, data, n, runs);

    deleteCodes(data);
    return 0;