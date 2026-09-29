/*
 * array.h
 *
 *  Created on: 2011-11-24
 *      Author: morin
 *  Corregido: manejo correcto de copia/movimiento, copyOfRange y const-correctness.
 */

#ifndef ARRAY_H_
#define ARRAY_H_

#include <algorithm> // para std::fill, std::copy, std::swap, std::max
#include <cassert> // para assert.
#include <utility> // para std::move.

/*
    La plantilla de clase `ods::array` proporciona una implementación de matriz dinámica que gestiona 
    la memoria automáticamente, admitiendo la copia profunda y la semántica de movimiento conforme a 
    la «Regla de los cinco». Incluye métodos para rellenar la matriz, acceder a los elementos, 
    intercambiarlos, invertir el orden de la matriz y copiar un rango de elementos desde otra matriz.
*/
namespace ods {

/**
 * Arreglo dinámico simple. Sustituye al arreglo crudo con gestión de memoria.
 *
 * Reglas de copia/movimiento correctas (Rule of Five):
 *   - Copia: copia profunda.
 *   - Movimiento: roba el buffer.
 */
template<class T>
class array {
protected:
    T *a;
public:
    int length; // The length member of the ods::array<T> template class represents the number of elements in the array. It is defined as an integer, allowing users to easily access the size of the array instance.

    explicit array(int len);
    array(int len, const T &init);
    array(const array<T> &b);              // copia
    array(array<T> &&b) noexcept;          // movimiento
    virtual ~array();

    array<T>& operator=(const array<T> &b);
    array<T>& operator=(array<T> &&b) noexcept;

    void fill(const T &x);

    T& operator[](int i) {
        assert(i >= 0 && i < length);
        return a[i];
    }
    const T& operator[](int i) const {
        assert(i >= 0 && i < length);
        return a[i];
    }

    T* operator+(int i) { return &a[i]; }
    const T* operator+(int i) const { return &a[i]; }

    void swap(int i, int j) {
        std::swap(a[i], a[j]);
    }

    static void copyOfRange(array<T> &a0, const array<T> &a, int i, int j);
    virtual void reverse();
};

template<class T>
array<T>::array(int len) : a(new T[len]), length(len) {}

template<class T>
array<T>::array(int len, const T &init) : a(new T[len]), length(len) {
    std::fill(a, a + length, init);
}

template<class T>
array<T>::array(const array<T> &b) : a(new T[b.length]), length(b.length) {
    std::copy(b.a, b.a + b.length, a);
}

template<class T>
array<T>::array(array<T> &&b) noexcept : a(b.a), length(b.length) {
    b.a = nullptr;
    b.length = 0;
}

template<class T>
array<T>::~array() {
    delete[] a;      // delete[] sobre nullptr es seguro
}

template<class T>
array<T>& array<T>::operator=(const array<T> &b) {
    if (this != &b) {
        T *nuevo = new T[b.length];
        std::copy(b.a, b.a + b.length, nuevo);
        delete[] a;
        a = nuevo;
        length = b.length;
    }
    return *this;
}

template<class T>
array<T>& array<T>::operator=(array<T> &&b) noexcept {
    if (this != &b) {
        delete[] a;
        a = b.a;
        length = b.length;
        b.a = nullptr;
        b.length = 0;
    }
    return *this;
}

template<class T>
void array<T>::reverse() {
    for (int i = 0; i < length / 2; i++)
        swap(i, length - i - 1);
}

template<class T>
void array<T>::copyOfRange(array<T> &a0, const array<T> &a, int i, int j) {
    assert(0 <= i && i <= j && j <= a.length);
    array<T> b(j - i);
    std::copy(a.a + i, a.a + j, b.a);
    a0 = std::move(b);
}

template<class T>
void array<T>::fill(const T &x) {
    std::fill(a, a + length, x);
}

} /* namespace ods */

#endif /* ARRAY_H_ */