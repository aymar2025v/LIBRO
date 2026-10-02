/*
 * RootishArrayStack.h
 *
 *  Created on: 2011-11-23
 *      Author: morin
 *  (versión mejorada para el proyecto RootishArrayStack)
 */

#ifndef ROOTISHARRAYSTACK_H_
#define ROOTISHARRAYSTACK_H_

#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include "ArrayStack.h"

namespace ods {

template <class T>
class RootishArrayStack {
protected:
    ArrayStack<T*> blocks;   // blocks.get(b) es un T* con capacidad (b+1)
    int n;                   // número de elementos reales almacenados

    // Devuelve el bloque que contiene el índice i.
    // Los bloques tienen tamaños 1, 2, 3, ...  (bloque b -> b+1 slots)
    // El índice inicial del bloque b es b*(b+1)/2.
    int i2b(int i) const;

    void grow();
    void shrink();

public:
    RootishArrayStack();
    virtual ~RootishArrayStack();

    // Prohibimos copia: ArrayStack<T*> no es copiable.
    RootishArrayStack(const RootishArrayStack&) = delete;
    RootishArrayStack& operator=(const RootishArrayStack&) = delete;

    int size() const { return n; }
    bool empty() const { return n == 0; }

    T get(int i) const;
    T set(int i, T x);

    virtual void add(int i, T x);
    virtual void add(T x) { add(size(), x); }   // push
    virtual T remove(int i);
    virtual void clear();

    // ---- Auxiliares para el menú / depuración ----
    int numBlocks() const { return blocks.size(); }
    int blockCapacity(int b) const { return b + 1; }
    void printStructure() const;
};

// ---------- Implementaciones inline ----------

template <class T>
inline T RootishArrayStack<T>::get(int i) const {
    assert(i >= 0 && i < n);
    int b = i2b(i);
    int j = i - b * (b + 1) / 2;
    return blocks.get(b)[j];
}

template <class T>
inline T RootishArrayStack<T>::set(int i, T x) {
    assert(i >= 0 && i < n);
    int b = i2b(i);
    int j = i - b * (b + 1) / 2;
    T y = blocks.get(b)[j];
    blocks.get(b)[j] = x;
    return y;
}

template <class T>
inline int RootishArrayStack<T>::i2b(int i) const {
    assert(i >= 0);
    // Resolvemos b(b+1)/2 <= i  =>  b = ceil((-3 + sqrt(9 + 8i)) / 2)
    double db = (-3.0 + std::sqrt(9.0 + 8.0 * i)) / 2.0;
    int b = (int)std::ceil(db);
    if (b < 0) b = 0;
    return b;
}

// ---------- Ciclo de vida ----------

template <class T>
RootishArrayStack<T>::RootishArrayStack() {
    n = 0;
}

template <class T>
RootishArrayStack<T>::~RootishArrayStack() {
    clear();
}

// ---------- Operaciones ----------

template <class T>
void RootishArrayStack<T>::add(int i, T x) {
    assert(i >= 0 && i <= n);
    int r = blocks.size();
    if (r * (r + 1) / 2 < n + 1)
        grow();
    n++;
    for (int j = n - 1; j > i; j--)
        set(j, get(j - 1));
    set(i, x);
}

template <class T>
T RootishArrayStack<T>::remove(int i) {
    assert(i >= 0 && i < n);
    T x = get(i);
    for (int j = i; j < n - 1; j++)
        set(j, get(j + 1));
    n--;
    int r = blocks.size();
    // Si con r-1 bloques todavía caben n elementos, sobra el último bloque.
    if (r > 0 && (r - 2) * (r - 1) / 2 >= n)
        shrink();
    return x;
}

template <class T>
void RootishArrayStack<T>::grow() {
    // El nuevo bloque b tiene capacidad b+1 (donde b = size actual)
    int b = blocks.size();
    blocks.add(new T[b + 1]);
}

template <class T>
void RootishArrayStack<T>::shrink() {
    int r = blocks.size();
    while (r > 0 && (r - 2) * (r - 1) / 2 >= n) {
        delete[] blocks.remove(blocks.size() - 1);
        r--;
    }
}

template <class T>
void RootishArrayStack<T>::clear() {
    while (blocks.size() > 0) {
        delete[] blocks.remove(blocks.size() - 1);
    }
    n = 0;
}

// ---------- Utilidad para el menú ----------

template <class T>
void RootishArrayStack<T>::printStructure() const {
    int r = blocks.size();
    int totalCap = r * (r + 1) / 2;
    std::cout << "  Bloques: " << r
              << "  |  Capacidad total: " << totalCap
              << "  |  Elementos: " << n << "\n";
    std::cout << "  " << std::left
              << std::setw(10) << "Bloque"
              << std::setw(24) << "Direccion"
              << "Contenido\n";
    std::cout << "  " << std::string(58, '-') << "\n";
    for (int b = 0; b < r; b++) {
        T* blk = blocks.get(b);
        int cap = b + 1;
        std::cout << "  " << std::left
                  << std::setw(10) << b
                  << std::setw(24) << static_cast<const void*>(blk);
        for (int j = 0; j < cap; j++) {
            int idx = b * (b + 1) / 2 + j;
            if (j > 0) std::cout << " ";
            if (idx < n) std::cout << blk[j];
            else         std::cout << "_";
        }
        std::cout << "\n";
    }
}

} /* namespace ods */

#endif /* ROOTISHARRAYSTACK_H_ */