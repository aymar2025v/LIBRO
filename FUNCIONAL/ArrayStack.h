/*
 * ArrayStack.h
 *
 *  Created on: 2011-11-23
 *      Author: morin
 *  (versión mejorada para el proyecto RootishArrayStack)
 */

#ifndef ARRAYSTACK_H_
#define ARRAYSTACK_H_

#include "array.h"
#include "utils.h"

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

    // Prohibimos copia: array<T> transfiere propiedad, copiar rompería invariantes.
    ArrayStack(const ArrayStack&) = delete;
    ArrayStack& operator=(const ArrayStack&) = delete;

    int size() const { return n; }
    bool empty() const { return n == 0; }

    T get(int i) const;
    T set(int i, T x);

    virtual void add(int i, T x);
    virtual void add(T x) { add(size(), x); }
    virtual T remove(int i);
    virtual void clear();

    void printAddresses() const {
        std::cout << "  [";
        for (int i = 0; i < n; i++) {
            std::cout << " [" << i << "]=" << static_cast<const void*>(a[i]);
            if (i + 1 < n) std::cout << ",";
        }
        std::cout << " ]\n";
    }
};

template<class T> inline
T ArrayStack<T>::get(int i) const {
    return a[i];
}

template<class T> inline
T ArrayStack<T>::set(int i, T x) {
    T y = a[i];
    a[i] = x;
    return y;
}

template<class T>
void ArrayStack<T>::clear() {
    n = 0;
    array<T> b(1);
    a = b;
}

template <class T>
ArrayStack<T>::ArrayStack() : a(1) {
    n = 0;
}

template<class T>
ArrayStack<T>::~ArrayStack() {
}

template<class T>
void ArrayStack<T>::resize() {
    array<T> b(ods::max(2 * n, 1));
    for (int i = 0; i < n; i++)
        b[i] = a[i];
    a = b;
}

template<class T>
void ArrayStack<T>::add(int i, T x) {
    if (n + 1 > a.length) resize();
    for (int j = n; j > i; j--)
        a[j] = a[j - 1];
    a[i] = x;
    n++;
}

template<class T>
T ArrayStack<T>::remove(int i) {
    T x = a[i];
    for (int j = i; j < n - 1; j++)
        a[j] = a[j + 1];
    n--;
    if (a.length >= 3 * n && a.length > 1) resize();
    return x;
}

} /* namespace ods */

#endif /* ARRAYSTACK_H_ */