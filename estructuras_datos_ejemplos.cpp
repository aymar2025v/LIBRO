// =====================================================================
//  Open Data Structures (C++) - Pat Morin
//  Ejemplos completos de las 9 estructuras de los capitulos 2 y 3
//
//  Compilar:  g++ -std=c++17 -O2 -o ejemplos estructuras_datos_ejemplos.cpp
//  Ejecutar:  ./ejemplos
//
//  Nota: para que los ejemplos se lean facil, los elementos son 'char'
//  (typedef T). En el libro las clases son plantillas (template<class T>).
//  La logica es identica.
// =====================================================================
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <string>
using namespace std;

typedef char T;

void titulo(const string& s) {
    cout << "\n==================================================\n"
         << s
         << "\n==================================================\n";
}

// Imprime un arreglo circular: '.' = casilla vacia, y marca donde esta j
void mostrarCircular(const char* nombre, T* a, int len, int j, int n) {
    cout << "  " << nombre << " = [";
    for (int k = 0; k < len; k++) {
        bool ocupado = ((k - j + len) % len) < n;
        cout << (ocupado ? a[k] : '.');
        if (k < len - 1) cout << ' ';
    }
    cout << "]  j=" << j << " n=" << n << " len=" << len << "\n";
}

// =====================================================================
// 1. ArrayStack
//    List sobre un arreglo 'a'. n = cuantos elementos hay, len = capacidad.
//    get/set: O(1).  add/remove: O(1 + n - i) porque hay que desplazar.
// =====================================================================
struct ArrayStack {
    T* a;
    int len;   // capacidad del arreglo
    int n;     // elementos usados

    ArrayStack() : a(new T[1]), len(1), n(0) {}
    ~ArrayStack() { delete[] a; }
    ArrayStack(const ArrayStack&) = delete;
    ArrayStack& operator=(const ArrayStack&) = delete;

    int size() const { return n; }
    T get(int i) const { return a[i]; }
    T set(int i, T x) { T y = a[i]; a[i] = x; return y; }

    // Crea un arreglo nuevo de tamano max(2n, 1) y copia todo.
    void resize() {
        int nl = max(2 * n, 1);
        T* b = new T[nl];
        for (int k = 0; k < n; k++) b[k] = a[k];
        delete[] a;
        a = b;
        len = nl;
    }

    void add(int i, T x) {
        if (n + 1 > len) resize();                 // lleno: duplicar
        for (int j = n; j > i; j--) a[j] = a[j - 1]; // desplazar a la derecha
        a[i] = x;
        n++;
    }

    T remove(int i) {
        T x = a[i];
        for (int j = i; j < n - 1; j++) a[j] = a[j + 1]; // desplazar a la izquierda
        n--;
        if (len >= 3 * n) resize();                // muy vacio: reducir
        return x;
    }
};

void mostrar(const ArrayStack& s, const char* nombre) {
    cout << "  " << nombre << ": a = [";
    for (int k = 0; k < s.len; k++) {
        cout << (k < s.n ? s.a[k] : '.');
        if (k < s.len - 1) cout << ' ';
    }
    cout << "]  n=" << s.n << " len=" << s.len << "\n";
}

void demoArrayStack() {
    titulo("1. ArrayStack");
    ArrayStack s;
    cout << "Agregamos a, b, c, d al final (O(1) amortizado). Observa como\n"
            "la capacidad se duplica cuando se llena (1 -> 2 -> 4):\n";
    for (T c : {'a', 'b', 'c', 'd'}) {
        s.add(s.size(), c);
        cout << " add(" << s.size() - 1 << ",'" << c << "')";
        mostrar(s, "");
    }
    cout << "\nadd(2,'x'): inserta en el medio y desplaza c y d a la derecha.\n"
            "Como n=4 == len=4, primero hace resize() a capacidad 8:\n";
    s.add(2, 'x');
    mostrar(s, "resultado");

    cout << "\nget(2) = " << s.get(2) << "  (acceso directo por indice, O(1))\n";
    cout << "set(0,'Z') devuelve el valor anterior: " << s.set(0, 'Z') << "\n";
    mostrar(s, "resultado");

    cout << "\nremove(1) quita '" << s.remove(1) << "' y desplaza a la izquierda:\n";
    mostrar(s, "resultado");

    cout << "\nQuitamos casi todo para ver la reduccion (len >= 3n => resize):\n";
    while (s.size() > 1) {
        T q = s.remove(s.size() - 1);
        cout << " remove ultimo '" << q << "'";
        mostrar(s, "");
    }
}

// =====================================================================
// 2. FastArrayStack
//    Igual que ArrayStack, pero desplaza con std::copy / copy_backward
//    (copia de bloques) en lugar de un bucle a mano. Mismo O(), menor
//    constante en la practica.
// =====================================================================
struct FastArrayStack : ArrayStack {
    void resize() {
        int nl = max(2 * n, 1);
        T* b = new T[nl];
        copy(a, a + n, b);
        delete[] a;
        a = b;
        len = nl;
    }
    void add(int i, T x) {
        if (n + 1 > len) resize();
        copy_backward(a + i, a + n, a + n + 1);   // mueve [i, n) una posicion a la derecha
        a[i] = x;
        n++;
    }
    T remove(int i) {
        T x = a[i];
        copy(a + i + 1, a + n, a + i);            // mueve (i, n) una posicion a la izquierda
        n--;
        if (len >= 3 * n) resize();
        return x;
    }
};

void demoFastArrayStack() {
    titulo("2. FastArrayStack");
    FastArrayStack s;
    for (T c : {'a', 'b', 'c', 'd'}) s.add(s.size(), c);
    mostrar(s, "inicial ");
    s.add(1, 'x');
    mostrar(s, "add(1,'x')");
    s.remove(3);
    mostrar(s, "remove(3)");

    // Comparacion de tiempos: insertar siempre al inicio (peor caso, O(n) c/u)
    const int N = 30000;
    auto t0 = chrono::steady_clock::now();
    { ArrayStack lenta; for (int k = 0; k < N; k++) lenta.add(0, 'x'); }
    auto t1 = chrono::steady_clock::now();
    { FastArrayStack rapida; for (int k = 0; k < N; k++) rapida.add(0, 'x'); }
    auto t2 = chrono::steady_clock::now();
    auto ms = [](auto a, auto b) { return chrono::duration<double, milli>(b - a).count(); };
    cout << "\nInsertar " << N << " elementos al inicio:\n"
         << "  ArrayStack     (bucle): " << ms(t0, t1) << " ms\n"
         << "  FastArrayStack (copy) : " << ms(t1, t2) << " ms\n"
         << "(Ambas son O(n) por insercion: el O() no cambia. Lo que baja es la constante,\n"
            " porque copy/copy_backward mueven bloques de memoria de una vez. Los tiempos\n"
            " exactos dependen de tu maquina y del compilador.)\n";
}

// =====================================================================
// 3. ArrayQueue
//    Cola FIFO con arreglo circular. j = indice del primer elemento.
//    El siguiente hueco libre es (j + n) % len. add/remove: O(1) amort.
// =====================================================================
struct ArrayQueue {
    T* a;
    int len, j, n;

    ArrayQueue() : a(new T[1]), len(1), j(0), n(0) {}
    ~ArrayQueue() { delete[] a; }
    ArrayQueue(const ArrayQueue&) = delete;
    ArrayQueue& operator=(const ArrayQueue&) = delete;

    // Al redimensionar se "endereza" el arreglo: el frente vuelve a la posicion 0
    void resize() {
        int nl = max(2 * n, 1);
        T* b = new T[nl];
        for (int k = 0; k < n; k++) b[k] = a[(j + k) % len];
        delete[] a;
        a = b;
        len = nl;
        j = 0;
    }
    void add(T x) {
        if (n + 1 > len) resize();
        a[(j + n) % len] = x;   // <-- aritmetica modular: "da la vuelta"
        n++;
    }
    T remove() {
        T x = a[j];
        j = (j + 1) % len;      // solo avanza el frente, no se mueve nada
        n--;
        if (len >= 3 * n) resize();
        return x;
    }
};

void demoArrayQueue() {
    titulo("3. ArrayQueue (arreglo circular)");
    ArrayQueue q;
    for (T c : {'a', 'b', 'c'}) {
        q.add(c);
        cout << " add('" << c << "')";
        mostrarCircular("", q.a, q.len, q.j, q.n);
    }
    cout << "\nremove() saca del frente ('" << q.remove() << "'); j avanza a 1, no se desplaza nada:\n";
    mostrarCircular("      ", q.a, q.len, q.j, q.n);

    q.add('d');
    cout << "\nadd('d') va a la casilla (j+n)%len = 3:\n";
    mostrarCircular("      ", q.a, q.len, q.j, q.n);

    q.add('e');
    cout << "\nadd('e'): (j+n)%len = (1+3)%4 = 0  --> el arreglo DA LA VUELTA:\n";
    mostrarCircular("      ", q.a, q.len, q.j, q.n);

    q.add('f');
    cout << "\nadd('f'): esta lleno, resize() lo endereza y duplica la capacidad:\n";
    mostrarCircular("      ", q.a, q.len, q.j, q.n);

    cout << "\nVaciamos la cola (orden FIFO): ";
    while (q.n > 0) cout << q.remove() << ' ';
    cout << "\n";
}

// =====================================================================
// 4. ArrayDeque
//    List completa sobre arreglo circular. Al insertar/borrar en i,
//    desplaza el lado mas corto: O(1 + min{i, n-i}).
// =====================================================================
struct ArrayDeque {
    T* a;
    int len, j, n;

    ArrayDeque() : a(new T[1]), len(1), j(0), n(0) {}
    ~ArrayDeque() { delete[] a; }
    ArrayDeque(const ArrayDeque&) = delete;
    ArrayDeque& operator=(const ArrayDeque&) = delete;

    T get(int i) const { return a[(j + i) % len]; }
    T set(int i, T x) { T y = a[(j + i) % len]; a[(j + i) % len] = x; return y; }

    void resize() {
        int nl = max(2 * n, 1);
        T* b = new T[nl];
        for (int k = 0; k < n; k++) b[k] = a[(j + k) % len];
        delete[] a;
        a = b;
        len = nl;
        j = 0;
    }

    void add(int i, T x) {
        if (n + 1 > len) resize();
        if (i < n / 2) {
            // Mitad izquierda: retrocedemos j y desplazamos los i primeros a la izquierda
            j = (j == 0) ? len - 1 : j - 1;
            for (int k = 0; k <= i - 1; k++) a[(j + k) % len] = a[(j + k + 1) % len];
        } else {
            // Mitad derecha: desplazamos a la derecha los elementos i..n-1
            for (int k = n; k > i; k--) a[(j + k) % len] = a[(j + k - 1) % len];
        }
        a[(j + i) % len] = x;
        n++;
    }

    T remove(int i) {
        T x = a[(j + i) % len];
        if (i < n / 2) {
            // Mitad izquierda: corremos los anteriores a la derecha y avanzamos j
            for (int k = i; k > 0; k--) a[(j + k) % len] = a[(j + k - 1) % len];
            j = (j + 1) % len;
        } else {
            // Mitad derecha: corremos los posteriores a la izquierda
            for (int k = i; k < n - 1; k++) a[(j + k) % len] = a[(j + k + 1) % len];
        }
        n--;
        if (len >= 3 * n) resize();
        return x;
    }
};

void demoArrayDeque() {
    titulo("4. ArrayDeque");
    ArrayDeque d;
    for (T c : {'a', 'b', 'c', 'd', 'e'}) d.add(d.n, c);
    mostrarCircular("inicial          ", d.a, d.len, d.j, d.n);

    d.add(0, 'X');
    cout << "add(0,'X'): i=0 < n/2, asi que NO desplaza nada: solo retrocede j\n"
            "(j pasa de 0 a len-1 y 'X' cae al final fisico del arreglo):\n";
    mostrarCircular("despues          ", d.a, d.len, d.j, d.n);

    d.add(3, 'Y');
    cout << "\nadd(3,'Y'): i=3 en el lado derecho (n=6), desplaza solo los que quedan a la derecha:\n";
    mostrarCircular("despues          ", d.a, d.len, d.j, d.n);

    cout << "\nLista logica: ";
    for (int k = 0; k < d.n; k++) cout << d.get(k) << ' ';
    cout << "\n";

    cout << "\nremove(0) = '" << d.remove(0) << "' (solo avanza j):\n";
    mostrarCircular("despues          ", d.a, d.len, d.j, d.n);
    cout << "remove(n-1) = '" << d.remove(d.n - 1) << "' (solo baja n):\n";
    mostrarCircular("despues          ", d.a, d.len, d.j, d.n);
}

// =====================================================================
// 5. DualArrayDeque
//    Dos ArrayStack espalda con espalda. 'front' guarda la primera parte
//    de la lista AL REVES; 'back' guarda el resto en orden normal.
//    Rebalancea cuando una pila tiene mas del triple que la otra.
// =====================================================================
struct DualArrayDeque {
    ArrayStack front, back;
    int rebalanceos = 0;

    int size() const { return front.n + back.n; }

    T get(int i) const {
        if (i < front.n) return front.get(front.n - i - 1);   // 'front' esta invertida
        return back.get(i - front.n);
    }
    T set(int i, T x) {
        if (i < front.n) return front.set(front.n - i - 1, x);
        return back.set(i - front.n, x);
    }

    void add(int i, T x) {
        if (i < front.n) front.add(front.n - i, x);
        else             back.add(i - front.n, x);
        balance();
    }
    T remove(int i) {
        T x;
        if (i < front.n) x = front.remove(front.n - i - 1);
        else             x = back.remove(i - front.n);
        balance();
        return x;
    }

    void balance() {
        int n = size();
        if (3 * front.n < back.n || 3 * back.n < front.n) {
            int nf = n / 2, nb = n - nf;
            if (nf == front.n) return;            // ya esta repartido igual, nada que hacer
            T* af = new T[max(2 * nf, 1)];
            T* ab = new T[max(2 * nb, 1)];
            for (int i = 0; i < nf; i++) af[nf - i - 1] = get(i);  // front invertida
            for (int i = 0; i < nb; i++) ab[i] = get(nf + i);
            delete[] front.a; delete[] back.a;
            front.a = af; front.len = max(2 * nf, 1); front.n = nf;
            back.a = ab;  back.len = max(2 * nb, 1);  back.n = nb;
            rebalanceos++;
            cout << "    [balance] repartidos: front=" << nf << ", back=" << nb << "\n";
        }
    }
};

void mostrarDual(const DualArrayDeque& d) {
    cout << "  front (invertida) = [";
    for (int k = 0; k < d.front.n; k++) cout << d.front.a[k] << (k < d.front.n - 1 ? " " : "");
    cout << "]   back = [";
    for (int k = 0; k < d.back.n; k++) cout << d.back.a[k] << (k < d.back.n - 1 ? " " : "");
    cout << "]   => lista: ";
    for (int k = 0; k < d.size(); k++) cout << d.get(k) << ' ';
    cout << "\n";
}

void demoDualArrayDeque() {
    titulo("5. DualArrayDeque");
    DualArrayDeque d;
    cout << "Agregamos a..f al final. Todo cae en 'back' y se rebalancea cuando hace falta:\n";
    for (T c : {'a', 'b', 'c', 'd', 'e', 'f'}) {
        d.add(d.size(), c);
        cout << " add('" << c << "')";
        mostrarDual(d);
    }

    cout << "\nadd(0,'X'): agregar al INICIO es un push en 'front' (O(1)):\n";
    d.add(0, 'X');
    mostrarDual(d);

    cout << "\nQuitamos 4 elementos del final; 'back' se queda casi vacia y salta el rebalanceo:\n";
    for (int k = 0; k < 4; k++) {
        cout << " remove ultimo:\n";
        T q = d.remove(d.size() - 1);
        cout << "  se quito '" << q << "'\n";
        mostrarDual(d);
    }
    cout << "\nTotal de rebalanceos: " << d.rebalanceos << "\n";
}

// =====================================================================
// 6. RootishArrayStack
//    Bloques de tamano creciente 1, 2, 3, ... (r bloques => r(r+1)/2 lugares).
//    Desperdicio de espacio: solo O(sqrt(n)).
//    El indice i se convierte en (bloque b, posicion j).
// =====================================================================
struct RootishArrayStack {
    vector<T*> blocks;
    int n = 0;

    ~RootishArrayStack() { for (T* p : blocks) delete[] p; }

    // Formula del libro: b = ceil( (-3 + sqrt(9 + 8i)) / 2 )
    static int i2b(int i) {
        double db = (-3.0 + sqrt(9.0 + 8.0 * i)) / 2.0;
        return (int)ceil(db);
    }

    T get(int i) const {
        int b = i2b(i);
        int j = i - b * (b + 1) / 2;
        return blocks[b][j];
    }
    void set(int i, T x) {
        int b = i2b(i);
        int j = i - b * (b + 1) / 2;
        blocks[b][j] = x;
    }

    void grow() { blocks.push_back(new T[blocks.size() + 1]); }   // el bloque nuevo tiene tamano r+1

    void shrink() {
        int r = blocks.size();
        while (r > 0 && (r - 2) * (r - 1) / 2 >= n) {
            delete[] blocks.back();
            blocks.pop_back();
            r--;
        }
    }

    void add(int i, T x) {
        int r = blocks.size();
        if (r * (r + 1) / 2 < n + 1) grow();     // no hay lugar: agregar UN bloque (sin copiar nada)
        n++;
        for (int j = n - 1; j > i; j--) set(j, get(j - 1));
        set(i, x);
    }

    T remove(int i) {
        T x = get(i);
        for (int j = i; j < n - 1; j++) set(j, get(j + 1));
        n--;
        int r = blocks.size();
        if ((r - 2) * (r - 1) / 2 >= n) shrink();
        return x;
    }
};

void mostrarRootish(const RootishArrayStack& s) {
    cout << "  bloques: ";
    for (size_t b = 0; b < s.blocks.size(); b++) {
        cout << "[";
        int inicio = b * (b + 1) / 2;
        for (size_t k = 0; k <= b; k++) {
            int idx = inicio + k;
            cout << (idx < s.n ? s.blocks[b][k] : '.') << (k < b ? " " : "");
        }
        cout << "] ";
    }
    cout << "  n=" << s.n << "\n";
}

void demoRootish() {
    titulo("6. RootishArrayStack");
    cout << "Conversion indice -> (bloque, posicion):\n";
    for (int i = 0; i < 10; i++) {
        int b = RootishArrayStack::i2b(i);
        int j = i - b * (b + 1) / 2;
        cout << "  i=" << i << " -> bloque " << b << ", posicion " << j << "\n";
    }

    RootishArrayStack s;
    cout << "\nAgregamos a..f. Cada vez que falta espacio se anade UN bloque nuevo (tamano 1,2,3...):\n";
    for (T c : {'a', 'b', 'c', 'd', 'e', 'f'}) {
        s.add(s.n, c);
        cout << " add('" << c << "')";
        mostrarRootish(s);
    }
    cout << "\nget(4) = '" << s.get(4) << "'  (bloque 2, posicion 1)\n";

    s.add(2, 'X');
    cout << "\nadd(2,'X') desplaza c..f una posicion (cruza bloques) y anade el bloque 3:\n";
    mostrarRootish(s);

    cout << "\nQuitamos elementos del final; se liberan bloques sobrantes (shrink):\n";
    while (s.n > 2) {
        T q = s.remove(s.n - 1);
        cout << " remove '" << q << "'";
        mostrarRootish(s);
    }
}

// =====================================================================
// 7. SLList (lista simplemente enlazada)
//    head y tail. push/pop en la cabeza, add en la cola. Todo O(1) real.
// =====================================================================
struct SLList {
    struct Node { T x; Node* next; };
    Node* head = nullptr;
    Node* tail = nullptr;
    int n = 0;

    ~SLList() { while (n > 0) pop(); }

    // Pila: insertar en la cabeza
    void push(T x) {
        Node* u = new Node{x, head};
        head = u;
        if (n == 0) tail = u;
        n++;
    }
    // Pila y cola: quitar de la cabeza
    T pop() {
        T x = head->x;
        Node* u = head;
        head = head->next;
        delete u;
        if (--n == 0) tail = nullptr;
        return x;
    }
    // Cola: insertar al final usando tail
    void add(T x) {
        Node* u = new Node{x, nullptr};
        if (n == 0) head = u; else tail->next = u;
        tail = u;
        n++;
    }
    T remove() { return pop(); }
};

void mostrarSL(const SLList& l) {
    cout << "  head -> ";
    for (SLList::Node* p = l.head; p; p = p->next) cout << p->x << " -> ";
    cout << "null   (tail=" << (l.tail ? l.tail->x : '-') << ", n=" << l.n << ")\n";
}

void demoSLList() {
    titulo("7. SLList");
    cout << "Usada como PILA (LIFO): push y pop en la cabeza\n";
    SLList pila;
    for (T c : {'a', 'b', 'c'}) { pila.push(c); cout << " push('" << c << "')"; mostrarSL(pila); }
    cout << " pop() = '" << pila.pop() << "'"; mostrarSL(pila);
    cout << " pop() = '" << pila.pop() << "'"; mostrarSL(pila);

    cout << "\nUsada como COLA (FIFO): add en la cola, remove en la cabeza\n";
    SLList cola;
    for (T c : {'1', '2', '3'}) { cola.add(c); cout << " add('" << c << "')"; mostrarSL(cola); }
    cout << " remove() = '" << cola.remove() << "'  (el primero en llegar)"; mostrarSL(cola);
    cout << " add('4')"; cola.add('4'); mostrarSL(cola);

    cout << "\nPor que no sirve como deque: para quitar el ULTIMO nodo hay que\n"
            "recorrer toda la lista para hallar el penultimo (no hay puntero 'prev'): O(n).\n";
}

// =====================================================================
// 8. DLList (lista doblemente enlazada con nodo ficticio)
//    El nodo 'dummy' cierra la lista en circulo: dummy.next = primero,
//    dummy.prev = ultimo. Asi no hay casos especiales para lista vacia.
// =====================================================================
struct DLList {
    struct Node { T x; Node* prev; Node* next; };
    Node dummy;
    int n;

    DLList() : n(0) { dummy.prev = dummy.next = &dummy; }
    ~DLList() { while (n > 0) removeNode(dummy.next); }
    DLList(const DLList&) = delete;
    DLList& operator=(const DLList&) = delete;

    // Recorre desde el extremo mas cercano: O(1 + min{i, n-i})
    Node* getNode(int i) {
        Node* p;
        if (i < n / 2) {
            p = dummy.next;
            for (int k = 0; k < i; k++) p = p->next;
        } else {
            p = &dummy;
            for (int k = n; k > i; k--) p = p->prev;   // getNode(n) devuelve dummy
        }
        return p;
    }
    T get(int i) { return getNode(i)->x; }

    // Inserta un nodo nuevo justo ANTES de w: O(1)
    Node* addBefore(Node* w, T x) {
        Node* u = new Node;
        u->x = x;
        u->prev = w->prev;
        u->next = w;
        u->next->prev = u;
        u->prev->next = u;
        n++;
        return u;
    }
    void add(int i, T x) { addBefore(getNode(i), x); }

    // Elimina el nodo w: O(1)
    void removeNode(Node* w) {
        w->prev->next = w->next;
        w->next->prev = w->prev;
        delete w;
        n--;
    }
    T remove(int i) {
        Node* w = getNode(i);
        T x = w->x;
        removeNode(w);
        return x;
    }
};

void mostrarDL(DLList& l) {
    cout << "  adelante: ";
    for (DLList::Node* p = l.dummy.next; p != &l.dummy; p = p->next) cout << p->x << ' ';
    cout << "  |  atras: ";
    for (DLList::Node* p = l.dummy.prev; p != &l.dummy; p = p->prev) cout << p->x << ' ';
    cout << "  (n=" << l.n << ")\n";
}

void demoDLList() {
    titulo("8. DLList");
    DLList l;
    cout << "Lista vacia: dummy apunta a si mismo en ambos sentidos.\n";
    for (T c : {'a', 'b', 'c', 'd'}) l.add(l.n, c);   // add(n) = addBefore(dummy) = agregar al final
    mostrarDL(l);

    l.add(2, 'X');
    cout << "\nadd(2,'X'): getNode(2) ubica el nodo 'c', addBefore inserta 'X' antes de el:\n";
    mostrarDL(l);

    cout << "\nget(4) = '" << l.get(4) << "'  (i=4 >= n/2, asi que recorre DESDE ATRAS usando prev)\n";

    cout << "\nremove(0) = '" << l.remove(0) << "'  (sin casos especiales gracias al dummy):\n";
    mostrarDL(l);

    // Ventaja clave: si ya tienes el nodo, borrar/insertar es O(1) sin recorrer
    DLList::Node* cursor = l.getNode(1);
    cout << "\nTengo un 'cursor' apuntando al nodo '" << cursor->x << "'. Insertar antes y borrarlo es O(1):\n";
    l.addBefore(cursor, 'Q');
    mostrarDL(l);
    l.removeNode(cursor);
    mostrarDL(l);
}

// =====================================================================
// 9. SEList (space-efficient list)
//    Una DLList donde cada nodo guarda un BLOQUE (un pequeno deque
//    circular con capacidad b+1). Invariante: cada bloque tiene entre
//    b-1 y b+1 elementos (salvo el ultimo, que puede tener menos).
// =====================================================================
struct SEList {
    int b;

    // Deque circular de capacidad fija b+1 (dentro de cada nodo)
    struct BDeque {
        T* a;
        int len, j, n;
        BDeque(int b) : a(new T[b + 1]), len(b + 1), j(0), n(0) {}
        ~BDeque() { delete[] a; }
        BDeque(const BDeque&) = delete;
        BDeque& operator=(const BDeque&) = delete;

        T get(int i) const { return a[(j + i) % len]; }
        void add(int i, T x) {           // supone n < len
            if (i < n / 2) {
                j = (j == 0) ? len - 1 : j - 1;
                for (int k = 0; k <= i - 1; k++) a[(j + k) % len] = a[(j + k + 1) % len];
            } else {
                for (int k = n; k > i; k--) a[(j + k) % len] = a[(j + k - 1) % len];
            }
            a[(j + i) % len] = x;
            n++;
        }
        void add(T x) { add(n, x); }
        T remove(int i) {
            T x = a[(j + i) % len];
            if (i < n / 2) {
                for (int k = i; k > 0; k--) a[(j + k) % len] = a[(j + k - 1) % len];
                j = (j + 1) % len;
            } else {
                for (int k = i; k < n - 1; k++) a[(j + k) % len] = a[(j + k + 1) % len];
            }
            n--;
            return x;
        }
    };

    struct Node {
        BDeque d;
        Node *prev, *next;
        Node(int b) : d(b), prev(nullptr), next(nullptr) {}
    };
    struct Location { Node* u; int j; };

    Node dummy;
    int n;

    SEList(int b) : b(b), dummy(b), n(0) { dummy.next = dummy.prev = &dummy; }
    ~SEList() { while (dummy.next != &dummy) removeNode(dummy.next); }
    SEList(const SEList&) = delete;
    SEList& operator=(const SEList&) = delete;

    Node* addBefore(Node* w) {
        Node* u = new Node(b);
        u->prev = w->prev;
        u->next = w;
        u->next->prev = u;
        u->prev->next = u;
        return u;
    }
    void removeNode(Node* w) {
        w->prev->next = w->next;
        w->next->prev = w->prev;
        delete w;
    }

    // Traduce el indice de la lista (i) a (nodo u, posicion j dentro de su bloque)
    Location getLocation(int i) {
        if (i < n / 2) {
            Node* u = dummy.next;
            while (i >= u->d.n) { i -= u->d.n; u = u->next; }
            return {u, i};
        } else {
            Node* u = &dummy;
            int idx = n;
            while (i < idx) { u = u->prev; idx -= u->d.n; }
            return {u, i - idx};
        }
    }

    T get(int i) {
        Location l = getLocation(i);
        return l.u->d.get(l.j);
    }

    // Agregar al final
    void add(T x) {
        Node* last = dummy.prev;
        if (last == &dummy || last->d.n == b + 1) last = addBefore(&dummy);
        last->d.add(x);
        n++;
    }

    // Reparte los elementos de b bloques llenos (desde u) entre b+1 bloques
    void spread(Node* u) {
        Node* w = u;
        for (int j = 0; j < b; j++) w = w->next;
        w = addBefore(w);                       // nodo nuevo al final del tramo
        cout << "    [spread] b bloques llenos: se crea un bloque nuevo y se reparte\n";
        while (w != u) {
            while (w->d.n < b) w->d.add(0, w->prev->d.remove(w->prev->d.n - 1));
            w = w->prev;
        }
    }

    // Junta los elementos de b bloques con b-1 elementos en b-1 bloques
    void gather(Node* u) {
        Node* w = u;
        for (int j = 0; j < b - 1; j++) {
            while (w->d.n < b) w->d.add(w->next->d.remove(0));
            w = w->next;
        }
        cout << "    [gather] b bloques con b-1 elementos: se juntan y se elimina un bloque\n";
        removeNode(w);
    }

    void add(int i, T x) {
        if (i == n) { add(x); return; }
        Location l = getLocation(i);
        Node* u = l.u;
        int r = 0;
        while (r < b && u != &dummy && u->d.n == b + 1) { u = u->next; r++; }
        if (r == b) {                     // b bloques consecutivos llenos
            spread(l.u);
            u = l.u;
        }
        if (u == &dummy) u = addBefore(u); // nos salimos por el final: bloque nuevo
        while (u != l.u) {                 // corremos UN elemento por bloque, hacia atras
            u->d.add(0, u->prev->d.remove(u->prev->d.n - 1));
            u = u->prev;
        }
        u->d.add(l.j, x);
        n++;
    }

    T remove(int i) {
        Location l = getLocation(i);
        T y = l.u->d.get(l.j);
        Node* u = l.u;
        int r = 0;
        while (r < b && u != &dummy && u->d.n == b - 1) { u = u->next; r++; }
        if (r == b) gather(l.u);           // b bloques consecutivos con b-1 elementos
        u = l.u;
        u->d.remove(l.j);
        while (u->d.n < b - 1 && u->next != &dummy) {  // rellenar tomando del siguiente
            u->d.add(u->next->d.remove(0));
            u = u->next;
        }
        if (u->d.n == 0) removeNode(u);
        n--;
        return y;
    }
};

void mostrarSE(SEList& l) {
    cout << "  ";
    for (SEList::Node* p = l.dummy.next; p != &l.dummy; p = p->next) {
        cout << "[";
        for (int k = 0; k < p->d.n; k++) cout << p->d.get(k) << (k < p->d.n - 1 ? " " : "");
        cout << "] ";
    }
    cout << "  (n=" << l.n << ", b=" << l.b << ")\n";
}

void demoSEList() {
    titulo("9. SEList (b = 2: cada bloque tiene entre 1 y 3 elementos)");
    SEList l(2);
    for (T c : {'a', 'b', 'c', 'd', 'e', 'f'}) l.add(c);
    cout << "Agregar al final llena cada bloque hasta b+1 = 3 elementos:\n";
    mostrarSE(l);

    cout << "\nget(4) = '" << l.get(4) << "'  (getLocation salta bloques enteros, no elemento por elemento)\n";

    cout << "\nadd(1,'X'): el bloque 0 y el 1 estan llenos (r llega a b) => se dispara spread:\n";
    l.add(1, 'X');
    mostrarSE(l);

    cout << "\nremove(3) = '" << l.remove(3) << "':\n";
    mostrarSE(l);

    cout << "\nremove(3) = '" << l.remove(3) << "'  (el bloque queda vacio y toma un elemento del siguiente):\n";
    mostrarSE(l);

    cout << "\nremove(3): ahora hay b bloques seguidos con b-1 elementos => se dispara gather:\n";
    T q = l.remove(3);
    cout << "  quitado '" << q << "'\n";
    mostrarSE(l);
}

// =====================================================================
int main() {
    demoArrayStack();
    demoFastArrayStack();
    demoArrayQueue();
    demoArrayDeque();
    demoDualArrayDeque();
    demoRootish();
    demoSLList();
    demoDLList();
    demoSEList();
    cout << "\nFin de los ejemplos.\n";
    return 0;
}
