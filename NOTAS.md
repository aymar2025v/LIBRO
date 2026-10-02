## INLINE EN C++

En C++, la palabra clave inline es una sugerencia que se le da al compilador para optimizar el rendimiento del programa. Le pide que, en lugar de hacer una "llamada" tradicional a la función, copie y pegue el código de la función directamente en el lugar donde se la invoca.
Para entenderlo de forma sencilla, imagina que estás leyendo un libro técnico y encuentras un asterisco que te manda a ver una nota al pie de página al final del libro. Tienes que pausar tu lectura, ir al final, leer la nota y regresar a donde estabas. Eso es una llamada a función normal. inline equivale a imprimir esa nota directamente dentro del párrafo principal para que no tengas que saltar a ningún lado.

```cpp
#ifndef ROOTISHARRAYSTACK_H_
#define ROOTISHARRAYSTACK_H_

#include <cmath>
#include "ArrayStack.h"

namespace ods {

template<class T>
class RootishArrayStack {
protected:
    ArrayStack<T*> blocks;

    // Número de elementos reales almacenados en toda la estructura.
    // No es el número de bloques; es la cantidad de datos que el usuario ha insertado.
    int n;

    int i2b(int i);
    void grow();
    void shrink();
public:
    RootishArrayStack();
    virtual ~RootishArrayStack();
    int size();
    T get(int i);
    T set(int i, T x);
    virtual void add(int i, T x);
    virtual T remove(int i);
    virtual void clear();
};

template<class T> inline
int RootishArrayStack<T>::size(){
    return n;
}

template<class T> inline
T RootishArrayStack<T>::get(int i) {
    int b = i2b(i);
    int j = i - b*(b+1)/2;
    return blocks.get(b)[j];
}

template<class T> inline
T RootishArrayStack<T>::set(int i, T x) {
    int b = i2b(i);
    int j = i - b*(b+1)/2;
    T y = blocks.get(b)[j];
    blocks.get(b)[j] = x;
    return y;
}

template<class T> inline
int RootishArrayStack<T>::i2b(int i) {
    double db = (-3.0 + std::sqrt(9.0 + 8.0*i)) / 2.0;
    int b = (int)std::ceil(db);
    return b;
}

template<class T>
RootishArrayStack<T>::RootishArrayStack() {
    n = 0;
}

template<class T>
RootishArrayStack<T>::~RootishArrayStack() {
    clear();
}

template<class T>
void RootishArrayStack<T>::add(int i, T x) {
    int r = blocks.size();
    if (r*(r+1)/2 < n + 1) grow();
    n++;
    for (int j = n-1; j > i; j--)
        set(j, get(j-1));
    set(i, x);
}

template<class T>
T RootishArrayStack<T>::remove(int i) {
    T x = get(i);
    for (int j = i; j < n-1; j++)
        set(j, get(j+1));
    n--;
    int r = blocks.size();
    if ((r-2)*(r-1)/2 >= n) shrink();
    return x;
}

template<class T>
void RootishArrayStack<T>::grow() {
    blocks.add(blocks.size(), new T[blocks.size()+1]);
}

template<class T>
void RootishArrayStack<T>::shrink() {
    int r = blocks.size();
    while (r > 0 && (r-2)*(r-1)/2 >= n) {
        delete [] blocks.remove(blocks.size()-1);
        r--;
    }
}

template<class T>
void RootishArrayStack<T>::clear() {
    while (blocks.size() > 0) {
        delete [] blocks.remove(blocks.size()-1);
    }
    n = 0;
}

} /* namespace ods */
#endif /* ROOTISHARRAYSTACK_H_ */
```

```cpp
ods::RootishArrayStack<int> s;
```
