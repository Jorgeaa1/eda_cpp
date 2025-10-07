#include "utils.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

// ---------- Comparador ----------
static inline bool pos_less(const Poscode& a, const Poscode& b) {
    return a.getData() < b.getData();
}

// ================== QUICK ==================
void quick_sort(Poscode *A, std::size_t n) {
    if (!A || n <= 1) return;
    std::vector<Poscode> v(A, A + n);
    std::sort(v.begin(), v.end(),
              [](const Poscode& x, const Poscode& y){ return pos_less(x, y); });
    for (std::size_t i = 0; i < n; ++i) A[i] = v[i];
}

// ================== MERGE ==================
static void merge_range(std::vector<Poscode>& v, std::vector<Poscode>& tmp,
                        std::size_t l, std::size_t m, std::size_t r) {
    std::size_t i = l, j = m, k = l;
    while (i < m && j < r) {
        if (pos_less(v[i], v[j])) tmp[k++] = v[i++];
        else                      tmp[k++] = v[j++];
    }
    while (i < m) tmp[k++] = v[i++];
    while (j < r) tmp[k++] = v[j++];
    for (std::size_t t = l; t < r; ++t) v[t] = tmp[t];
}

static void mergesort_rec(std::vector<Poscode>& v, std::vector<Poscode>& tmp,
                          std::size_t l, std::size_t r) {
    if (r - l <= 32) {
        std::sort(v.begin() + l, v.begin() + r,
                  [](const Poscode& x, const Poscode& y){ return pos_less(x, y); });
        return;
    }
    std::size_t m = l + (r - l) / 2;
    mergesort_rec(v, tmp, l, m);
    mergesort_rec(v, tmp, m, r);
    if (!pos_less(v[m], v[m - 1])) return; // ya ordenado
    merge_range(v, tmp, l, m, r);
}

void merge_sort(Poscode *A, std::size_t n) {
    if (!A || n <= 1) return;
    std::vector<Poscode> v(A, A + n);
    std::vector<Poscode> tmp(n);
    mergesort_rec(v, tmp, 0, n);
    for (std::size_t i = 0; i < n; ++i) A[i] = v[i];
}

// ======= RADIX (LSD) + COUNTING estable con listas enlazadas =======
static inline int bucket_of(const Poscode& code, std::size_t p) {
    char c = code.getValue(p);
    if (p <= 3) { // dígitos
        return int(c - '0');
    } else {      // letras
        if (c >= 'a' && c <= 'z') c = char(c - 'a' + 'A');
        return int(c - 'A');
    }
}

struct Node {
    int idx;
    Node* next;
    explicit Node(int i): idx(i), next(nullptr) {}
};

static void counting_by_column(const std::vector<Poscode*>& ref,
                               std::vector<int>& idx,
                               std::size_t p, int M) {
    std::vector<Node*> head(M, nullptr), tail(M, nullptr);

    // Distribución estable
    for (int id : idx) {
        int b = bucket_of(*ref[id], p);
        Node* node = new Node(id);
        if (!head[b]) head[b] = tail[b] = node;
        else { tail[b]->next = node; tail[b] = node; }
    }

    // Recolección
    std::vector<int> out; out.reserve(idx.size());
    for (int b = 0; b < M; ++b) {
        Node* cur = head[b];
        while (cur) {
            out.push_back(cur->idx);
            Node* tmp = cur; cur = cur->next; delete tmp;
        }
    }
    idx.swap(out);
}

void radix_sort(Poscode *A, std::size_t n) {
    if (!A || n <= 1) return;

    std::vector<Poscode*> ref(n);
    for (std::size_t i = 0; i < n; ++i) ref[i] = &A[i];
    std::vector<int> idx(n); std::iota(idx.begin(), idx.end(), 0);

    // Columnas: 5,4 (letras M=26) y 3..0 (dígitos M=10)
    counting_by_column(ref, idx, 5, 26);
    counting_by_column(ref, idx, 4, 26);
    counting_by_column(ref, idx, 3, 10);
    counting_by_column(ref, idx, 2, 10);
    counting_by_column(ref, idx, 1, 10);
    counting_by_column(ref, idx, 0, 10);

    std::vector<Poscode> out; out.reserve(n);
    for (std::size_t k = 0; k < n; ++k) out.push_back(A[idx[k]]);
    for (std::size_t i = 0; i < n; ++i) A[i] = out[i];
}

// ================== I/O ==================
Poscode* readCodes(const std::string &strfile, std::size_t n) {
    Poscode *codes = new Poscode[n];
    std::ifstream inputFile(strfile);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open the file!\n";
        delete[] codes;
        return nullptr;
    }
    std::string line;
    for (std::size_t i = 0; i < n; i++) {
        if (!std::getline(inputFile, line)) line = "";
        codes[i] = Poscode(line);
    }
    inputFile.close();
    return codes;
}

void deleteCodes(Poscode *codes) {
    if (codes) delete[] codes;
}
