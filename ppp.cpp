// array_stack_demo.cpp
// Compilar con: g++ -std=c++11 -o demo array_stack_demo.cpp

#include <iostream>
#include <algorithm>
#include <cassert>
#include <cstdlib>

namespace ods {

using std::cout;
using std::endl;

// ============================================================
// Clase array<T>
// ============================================================
template<class T>
class array {
protected:
    T *a;
public:
    int length;

    array(int len) {
        length = len;
        a = new T[length];
    }

    array(int len, T init) {
        length = len;
        a = new T[length];
        for (int i = 0; i < length; i++)
            a[i] = init;
    }

    virtual ~array() {
        if (a != NULL) delete[] a;
    }

    array<T>& operator=(array<T> &b) {
        if (a != NULL) delete[] a;
        a = b.a;
        b.a = NULL;
        length = b.length;
        return *this;
    }

    T& operator[](int i) {
        assert(i >= 0 && i < length);
        return a[i];
    }

    T* operator+(int i) {
        return &a[i];
    }

    void swap(int i, int j) {
        T x = a[i];
        a[i] = a[j];
        a[j] = x;
    }

    void fill(T x) {
        std::fill(a, a + length, x);
    }

    void reverse() {
        for (int i = 0; i < length / 2; i++)
            swap(i, length - i - 1);
    }
};

// ============================================================
// utilidades
// ============================================================
template<class T> inline
T my_min(T a, T b) { return (a < b) ? a : b; }

template<class T> inline
T my_max(T a, T b) { return (a > b) ? a : b; }

// ============================================================
// Clase ArrayStack<T>
// ============================================================
template<class T>
class ArrayStack {
protected:
    array<T> a;
    int n;
    virtual void resize();
public:
    ArrayStack();
    virtual ~ArrayStack();
    int size();
    T get(int i);
    T set(int i, T x);
    virtual void add(int i, T x);
    virtual void add(T x) { add(size(), x); }
    virtual T remove(int i);
    virtual void clear();
    void print();
};

template<class T>
ArrayStack<T>::ArrayStack() : a(1) {
    n = 0;
}

template<class T>
ArrayStack<T>::~ArrayStack() {
}

template<class T> inline
int ArrayStack<T>::size() {
    return n;
}

template<class T> inline
T ArrayStack<T>::get(int i) {
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

template<class T>
void ArrayStack<T>::resize() {
    cout << "  [resize] n=" << n
         << "  capacidad vieja=" << a.length
         << "  capacidad nueva=" << my_max(2 * n, 1) << endl;

    array<T> b(my_max(2 * n, 1));
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
    if (a.length >= 3 * n) resize();
    return x;
}

template<class T>
void ArrayStack<T>::print() {
    cout << "n=" << n << "  capacidad=" << a.length << "  [";
    for (int i = 0; i < n; i++) {
        cout << a[i];
        if (i < n - 1) cout << ", ";
    }
    cout << "]" << endl;
}

} // namespace ods

// ============================================================
// main de prueba
// ============================================================
int main() {
    using namespace ods;
    using std::cout;
    using std::endl;

    ArrayStack<int> pila;

    cout << "--- Insertando 10,20,30,40,50 con add(x) ---" << endl;
    pila.add(10);
    pila.add(20);
    pila.add(30);
    pila.add(40);
    pila.add(50);
    pila.print();

    cout << endl;
    cout << "--- remove(2)  ->  elimina el 30 ---" << endl;
    int eliminado = pila.remove(2);
    cout << "Eliminado: " << eliminado << endl;
    pila.print();
    cout << "Ojo: el dato viejo 50 sigue en memoria en a[4], "
            "pero n=4, así que NO es parte de la pila." << endl;

    cout << endl;
    cout << "--- add(99) -> sobrescribe la basura en a[4] ---" << endl;
    pila.add(99);
    pila.print();

    cout << endl;
    cout << "--- remove(3)  ->  elimina el 50 valido ---" << endl;
    eliminado = pila.remove(3);
    cout << "Eliminado: " << eliminado << endl;
    pila.print();

    cout << endl;
    cout << "--- Como pila LIFO: push(7), push(8), pop() ---" << endl;
    pila.add(7);
    pila.add(8);
    pila.print();
    int tope = pila.remove(pila.size() - 1);
    cout << "Pop: " << tope << endl;
    pila.print();

    cout << endl;
    cout << "--- Forzando resize por eliminaciones ---" << endl;
    pila.remove(0);
    pila.remove(0);
    pila.remove(0);
    pila.print();

    cout << endl;
    cout << "--- clear() ---" << endl;
    pila.clear();
    pila.print();

    return 0;
}