/*
 * array.h
 *
 *  Created on: 2011-11-24
 *      Author: morin
 *  (versión mejorada para el proyecto RootishArrayStack)
 */

#ifndef ARRAY_H_
#define ARRAY_H_

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cassert>

namespace ods {

/**
 * A simple array class that simulates Java's arrays implementation - kind of
 *
 * NOTA: El operator= transfiere la propiedad del puntero interno
 * (el array de la derecha queda con a = NULL). Esto es intencional
 * para que ArrayStack::resize() y clear() funcionen eficientemente.
 */
template<class T>
class array {
protected:
    T *a;
public:
    int length;

    array();
    array(int len);
    array(int len, T init);
    void fill(T x);
    virtual ~array();

    // Transfiere la propiedad del array interno de b a *this.
    array<T>& operator=(array<T> &b) {
        if (this != &b) {
            if (a != NULL) delete[] a;
            a = b.a;
            length = b.length;
            b.a = NULL;
            b.length = 0;
        }
        return *this;
    }

    T& operator[](int i) {
        assert(i >= 0 && i < length);
        return a[i];
    }

    const T& operator[](int i) const {
        assert(i >= 0 && i < length);
        return a[i];
    }

    T* operator+(int i) {
        return &a[i];
    }

    void swap(int i, int j) {
        std::swap(a[i], a[j]);
    }

    // Copia el rango [i, j) del array 'a' dentro de 'a0'.
    static void copyOfRange(array<T> &a0, array<T> &a, int i, int j);

    virtual void reverse();
};

template<class T>
array<T>::array() {
    length = 0;
    a = NULL;
}

template<class T>
array<T>::array(int len) {
    length = len;
    a = (len > 0) ? new T[len] : NULL;
}

template<class T>
array<T>::array(int len, T init) {
    length = len;
    a = (len > 0) ? new T[len] : NULL;
    for (int i = 0; i < length; i++)
        a[i] = init;
}

template<class T>
array<T>::~array() {
    if (a != NULL) delete[] a;
}

template<class T>
void array<T>::reverse() {
    for (int i = 0; i < length / 2; i++) {
        swap(i, length - i - 1);
    }
}

template<class T>
void array<T>::copyOfRange(array<T> &a0, array<T> &a, int i, int j) {
    assert(i >= 0 && j >= i && j <= a.length);
    array<T> b(j - i);
    std::copy(a.a + i, a.a + j, b.a);   // <-- corregido: antes era a.a
    a0 = b;
}

template<class T>
void array<T>::fill(T x) {
    std::fill(a, a + length, x);
}

} /* namespace ods */

#endif /* ARRAY_H_ */