/*
 * ArrayStack.h
 *
 *  Created on: 2011-11-23
 *      Author: morin
 *  Corregido: validación de índices, remove seguro con pila vacía,
 *             paso por referencia constante, const-correctness.
 */

#ifndef ARRAYSTACK_H_
#define ARRAYSTACK_H_

#include <algorithm>
#include <cassert>
#include <utility>

#include "array.h"

namespace ods {

template<class T>
class DualArrayDeque;

template<class T>
class ArrayStack {
protected:
    friend class DualArrayDeque<T>;
    array<T> a;
    int n;
    virtual void resize();
public:
    ArrayStack();
    virtual ~ArrayStack();

    int size() const;
    T get(int i) const;
    T set(int i, const T &x);

    virtual void add(int i, const T &x);
    virtual void add(const T &x) { add(size(), x); }
    virtual T remove(int i);
    virtual void clear();
};

template<class T> inline
int ArrayStack<T>::size() const {
    return n;
}

template<class T> inline
T ArrayStack<T>::get(int i) const {
    assert(0 <= i && i < n);
    return a[i];
}

template<class T> inline
T ArrayStack<T>::set(int i, const T &x) {
    assert(0 <= i && i < n);
    T y = a[i];
    a[i] = x;
    return y;
}

template<class T>
ArrayStack<T>::ArrayStack() : a(1), n(0) {}

template<class T>
ArrayStack<T>::~ArrayStack() {}

template<class T>
void ArrayStack<T>::clear() {
    n = 0;
    array<T> b(1);
    a = std::move(b);
}

template<class T>
void ArrayStack<T>::resize() {
    array<T> b(std::max(2 * n, 1));
    for (int i = 0; i < n; i++)
        b[i] = a[i];
    a = std::move(b);
}

template<class T>
void ArrayStack<T>::add(int i, const T &x) {
    assert(0 <= i && i <= n);       // permite insertar al final
    if (n + 1 > a.length) resize();
    for (int j = n; j > i; j--)
        a[j] = a[j - 1];
    a[i] = x;
    n++;
}

template<class T>
T ArrayStack<T>::remove(int i) {
    assert(0 <= i && i < n);        // falla con pila vacía
    T x = a[i];
    for (int j = i; j < n - 1; j++)
        a[j] = a[j + 1];
    n--;
    if (a.length >= 3 * n) resize();
    return x;
}

} /* namespace ods */

#endif /* ARRAYSTACK_H_ */