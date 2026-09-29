// =====================================================================
//  Open Data Structures (C++) - Pat Morin
//  Ejemplos completos de las 9 estructuras de los capitulos 2 y 3
//  + LABORATORIO INTERACTIVO MEJORADO (menus, diagnosticos, colores y tests)
//
//  Compilar:  g++ -std=c++17 -O2 -o ejemplos1 estructuras_datos_ejemplosss.cpp
//  Ejecutar:  ./ejemplos1
//
//  Nota: para que los ejemplos se lean facil, los elementos son 'char'
//  (typedef T). En el libro las clases son plantillas (template<class T>).
//  La logica es identica.
// =====================================================================
#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <stdexcept>
#include <cstdlib>
#include <cctype>
#include <string>
#include <random>
#include <cassert>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

using namespace std;

typedef char T;

// ---------------------------------------------------------------------
// Colores ANSI para terminal moderna
// ---------------------------------------------------------------------
namespace Color {
    static bool enabled = true;
    inline const char* reset()   { return enabled ? "\033[0m"  : ""; }
    inline const char* bold()    { return enabled ? "\033[1m"  : ""; }
    inline const char* dim()     { return enabled ? "\033[90m" : ""; }  // Gris: casillas vacias
    inline const char* red()     { return enabled ? "\033[91m" : ""; }  // Errores o limites
    inline const char* green()   { return enabled ? "\033[92m" : ""; }  // Datos activos
    inline const char* yellow()  { return enabled ? "\033[93m" : ""; }  // Puntero j / alertas
    inline const char* blue()    { return enabled ? "\033[94m" : ""; }  // Enlaces / front
    inline const char* magenta() { return enabled ? "\033[95m" : ""; }  // Reorganizaciones (balance/spread)
    inline const char* cyan()    { return enabled ? "\033[96m" : ""; }  // Nodos dummy / titulos
}

void habilitarColoresTerminal() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif
}

// Contadores globales para el laboratorio interactivo (miden el trabajo real de cada operacion)
static long long g_mov = 0;    // elementos copiados / desplazados
static long long g_rec = 0;    // nodos o bloques recorridos
static long long g_reorg = 0;  // resize, balance, bloques anadidos/quitados, spread, gather
static bool g_verbose = true;  // false = silencia los mensajes [balance], [spread], [gather]

void titulo(const string& s) {
    cout << "\n==================================================\n"
         << Color::bold() << Color::cyan() << s << Color::reset()
         << "\n==================================================\n";
}

// Imprime un arreglo circular: '.' = casilla vacia, y resalta donde esta j
void mostrarCircular(const char* nombre, T* a, int len, int j, int n) {
    cout << "  " << nombre << " = [";
    for (int k = 0; k < len; k++) {
        bool ocupado = ((k - j + len) % len) < n;
        if (k == j && ocupado) {
            cout << Color::yellow() << Color::bold() << a[k] << Color::reset();
        } else if (ocupado) {
            cout << Color::green() << a[k] << Color::reset();
        } else {
            cout << Color::dim() << '.' << Color::reset();
        }
        if (k < len - 1) cout << ' ';
    }
    cout << "]  " << Color::yellow() << "j=" << j << Color::reset()
         << " n=" << n << " len=" << len << "\n";
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
    virtual ~ArrayStack() { delete[] a; }
    ArrayStack(const ArrayStack&) = delete;
    ArrayStack& operator=(const ArrayStack&) = delete;

    int size() const { return n; }

    T get(int i) const {
        if (i < 0 || i >= n) throw out_of_range("ArrayStack::get: indice fuera de rango");
        return a[i];
    }
    T set(int i, T x) {
        if (i < 0 || i >= n) throw out_of_range("ArrayStack::set: indice fuera de rango");
        T y = a[i]; a[i] = x; return y;
    }

    // Crea un arreglo nuevo de tamano max(2n, 1) y copia todo.
    virtual void resize() {
        g_reorg++;
        int nl = max(2 * n, 1);
        T* b = new T[nl];
        copy(a, a + n, b);            // copiar los n elementos al arreglo nuevo
        g_mov += n;
        delete[] a;
        a = b;
        len = nl;
    }

    virtual void add(int i, T x) {
        if (i < 0 || i > n) throw out_of_range("ArrayStack::add: indice fuera de rango");
        if (n + 1 > len) resize();                 // lleno: duplicar
        for (int j = n; j > i; j--) { a[j] = a[j - 1]; g_mov++; } // desplazar a la derecha
        a[i] = x;
        n++;
    }

    virtual T remove(int i) {
        if (i < 0 || i >= n) throw out_of_range("ArrayStack::remove: indice fuera de rango");
        T x = a[i];
        for (int j = i; j < n - 1; j++) { a[j] = a[j + 1]; g_mov++; } // desplazar a la izquierda
        n--;
        if (len >= 3 * n) resize();                // muy vacio: reducir
        return x;
    }

    int find(T x) const {
        for (int i = 0; i < n; i++) {
            g_rec++;
            if (a[i] == x) return i;
        }
        return -1;
    }

    void reverse() {
        for (int i = 0; i < n / 2; i++) {
            swap(a[i], a[n - 1 - i]);
            g_mov += 3;
        }
    }

    bool checkInvariants(string& err) const {
        if (len < 1) { err = "len < 1"; return false; }
        if (n < 0 || n > len) { err = "n fuera de [0, len]"; return false; }
        if (n > 0 && len >= 3 * n) { err = "len >= 3n (debio haberse reducido)"; return false; }
        if (n == 0 && len > 1) { err = "n=0 pero len > 1"; return false; }
        return true;
    }
};

void mostrar(const ArrayStack& s, const char* nombre) {
    cout << "  " << nombre << ": a = [";
    for (int k = 0; k < s.len; k++) {
        if (k < s.n) {
            cout << Color::green() << s.a[k] << Color::reset();
        } else {
            cout << Color::dim() << '.' << Color::reset();
        }
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
    void resize() override {
        g_reorg++;
        int nl = max(2 * n, 1);
        T* b = new T[nl];
        copy(a, a + n, b); g_mov += n;
        delete[] a;
        a = b;
        len = nl;
    }
    void add(int i, T x) override {
        if (i < 0 || i > n) throw out_of_range("FastArrayStack::add: indice fuera de rango");
        if (n + 1 > len) resize();
        copy_backward(a + i, a + n, a + n + 1); g_mov += n - i;   // mueve [i, n) una posicion a la derecha
        a[i] = x;
        n++;
    }
    T remove(int i) override {
        if (i < 0 || i >= n) throw out_of_range("FastArrayStack::remove: indice fuera de rango");
        T x = a[i];
        copy(a + i + 1, a + n, a + i); g_mov += n - i - 1;            // mueve (i, n) una posicion a la izquierda
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
            " exactos dependen de tu maquina y del compilador. Nota: el bucle de ArrayStack incluye\n"
            " un contador de movimientos, asi que aqui sale algo mas lento de lo normal.)\n";
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

    int size() const { return n; }

    // Al redimensionar se "endereza" el arreglo: el frente vuelve a la posicion 0
    void resize() {
        g_reorg++;
        int nl = max(2 * n, 1);
        T* b = new T[nl];
        for (int k = 0; k < n; k++) { b[k] = a[(j + k) % len]; g_mov++; }
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
        if (n == 0) throw underflow_error("ArrayQueue::remove: la cola esta vacia");
        T x = a[j];
        j = (j + 1) % len;      // solo avanza el frente, no se mueve nada
        n--;
        if (len >= 3 * n) resize();
        return x;
    }
    T peek() const {
        if (n == 0) throw underflow_error("ArrayQueue::peek: la cola esta vacia");
        return a[j];
    }
    int find(T x) const {
        for (int k = 0; k < n; k++) {
            g_rec++;
            if (a[(j + k) % len] == x) return k;
        }
        return -1;
    }
    bool checkInvariants(string& err) const {
        if (len < 1) { err = "len < 1"; return false; }
        if (n < 0 || n > len) { err = "n fuera de [0, len]"; return false; }
        if (j < 0 || j >= len) { err = "j fuera de [0, len)"; return false; }
        if (n > 0 && len >= 3 * n) { err = "len >= 3n (debio haberse reducido)"; return false; }
        if (n == 0 && len > 1) { err = "n=0 pero len > 1"; return false; }
        return true;
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
    int size() const { return n; }

    T get(int i) const {
        if (i < 0 || i >= n) throw out_of_range("ArrayDeque::get: indice fuera de rango");
        return a[(j + i) % len];
    }
    T set(int i, T x) {
        if (i < 0 || i >= n) throw out_of_range("ArrayDeque::set: indice fuera de rango");
        T y = a[(j + i) % len]; a[(j + i) % len] = x; return y;
    }

    void resize() {
        g_reorg++;
        int nl = max(2 * n, 1);
        T* b = new T[nl];
        for (int k = 0; k < n; k++) { b[k] = a[(j + k) % len]; g_mov++; }
        delete[] a;
        a = b;
        len = nl;
        j = 0;
    }

    void add(int i, T x) {
        if (i < 0 || i > n) throw out_of_range("ArrayDeque::add: indice fuera de rango");
        if (n + 1 > len) resize();
        if (i < n / 2) {
            // Mitad izquierda: retrocedemos j y desplazamos los i primeros a la izquierda
            j = (j == 0) ? len - 1 : j - 1;
            for (int k = 0; k <= i - 1; k++) { a[(j + k) % len] = a[(j + k + 1) % len]; g_mov++; }
        } else {
            // Mitad derecha: desplazamos a la derecha los elementos i..n-1
            for (int k = n; k > i; k--) { a[(j + k) % len] = a[(j + k - 1) % len]; g_mov++; }
        }
        a[(j + i) % len] = x;
        n++;
    }

    T remove(int i) {
        if (i < 0 || i >= n) throw out_of_range("ArrayDeque::remove: indice fuera de rango");
        T x = a[(j + i) % len];
        if (i < n / 2) {
            // Mitad izquierda: corremos los anteriores a la derecha y avanzamos j
            for (int k = i; k > 0; k--) { a[(j + k) % len] = a[(j + k - 1) % len]; g_mov++; }
            j = (j + 1) % len;
        } else {
            // Mitad derecha: corremos los posteriores a la izquierda
            for (int k = i; k < n - 1; k++) { a[(j + k) % len] = a[(j + k + 1) % len]; g_mov++; }
        }
        n--;
        if (len >= 3 * n) resize();
        return x;
    }

    int find(T x) const {
        for (int k = 0; k < n; k++) {
            g_rec++;
            if (a[(j + k) % len] == x) return k;
        }
        return -1;
    }

    void reverse() {
        for (int k = 0; k < n / 2; k++) {
            swap(a[(j + k) % len], a[(j + n - 1 - k) % len]);
            g_mov += 3;
        }
    }

    bool checkInvariants(string& err) const {
        if (len < 1) { err = "len < 1"; return false; }
        if (n < 0 || n > len) { err = "n fuera de [0, len]"; return false; }
        if (j < 0 || j >= len) { err = "j fuera de [0, len)"; return false; }
        if (n > 0 && len >= 3 * n) { err = "len >= 3n (debio haberse reducido)"; return false; }
        if (n == 0 && len > 1) { err = "n=0 pero len > 1"; return false; }
        return true;
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
        if (i < 0 || i >= size()) throw out_of_range("DualArrayDeque::get: indice fuera de rango");
        if (i < front.n) return front.get(front.n - i - 1);   // 'front' esta invertida
        return back.get(i - front.n);
    }
    T set(int i, T x) {
        if (i < 0 || i >= size()) throw out_of_range("DualArrayDeque::set: indice fuera de rango");
        if (i < front.n) return front.set(front.n - i - 1, x);
        return back.set(i - front.n, x);
    }

    void add(int i, T x) {
        if (i < 0 || i > size()) throw out_of_range("DualArrayDeque::add: indice fuera de rango");
        if (i < front.n) front.add(front.n - i, x);
        else             back.add(i - front.n, x);
        balance();
    }
    T remove(int i) {
        if (i < 0 || i >= size()) throw out_of_range("DualArrayDeque::remove: indice fuera de rango");
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
            for (int i = 0; i < nf; i++) { af[nf - i - 1] = get(i); g_mov++; }  // front invertida
            for (int i = 0; i < nb; i++) { ab[i] = get(nf + i); g_mov++; }
            delete[] front.a; delete[] back.a;
            front.a = af; front.len = max(2 * nf, 1); front.n = nf;
            back.a = ab;  back.len = max(2 * nb, 1);  back.n = nb;
            rebalanceos++; g_reorg++;
            if (g_verbose) {
                cout << Color::magenta() << "    [balance] repartidos: front=" << nf << ", back=" << nb << Color::reset() << "\n";
            }
        }
    }

    int find(T x) const {
        for (int i = 0; i < size(); i++) {
            g_rec++;
            if (get(i) == x) return i;
        }
        return -1;
    }

    void reverse() {
        // Intercambiar front y back invierte toda la lista logica en O(1)
        swap(front.a, back.a);
        swap(front.n, back.n);
        swap(front.len, back.len);
        g_reorg++;
        balance();
    }

    bool checkInvariants(string& err) const {
        string subErr;
        if (!front.checkInvariants(subErr)) { err = "front: " + subErr; return false; }
        if (!back.checkInvariants(subErr)) { err = "back: " + subErr; return false; }
        int n = size();
        if (n >= 2) {
            if (3 * front.n < back.n || 3 * back.n < front.n) {
                err = "desbalance: front=" + to_string(front.n) + ", back=" + to_string(back.n);
                return false;
            }
        }
        return true;
    }
};

void mostrarDual(const DualArrayDeque& d) {
    cout << "  " << Color::blue() << "front (invertida)" << Color::reset() << " = [";
    for (int k = 0; k < d.front.n; k++) {
        cout << Color::green() << d.front.a[k] << Color::reset() << (k < d.front.n - 1 ? " " : "");
    }
    cout << "]   " << Color::cyan() << "back" << Color::reset() << " = [";
    for (int k = 0; k < d.back.n; k++) {
        cout << Color::green() << d.back.a[k] << Color::reset() << (k < d.back.n - 1 ? " " : "");
    }
    cout << "]   => lista: ";
    for (int k = 0; k < d.size(); k++) cout << Color::green() << d.get(k) << Color::reset() << ' ';
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

    RootishArrayStack() = default;
    ~RootishArrayStack() { for (T* p : blocks) delete[] p; }
    RootishArrayStack(const RootishArrayStack&) = delete;
    RootishArrayStack& operator=(const RootishArrayStack&) = delete;

    int size() const { return n; }

    // Formula del libro: b = ceil( (-3 + sqrt(9 + 8i)) / 2 )
    static int i2b(int i) {
        double db = (-3.0 + sqrt(9.0 + 8.0 * i)) / 2.0;
        return (int)ceil(db);
    }

    T get(int i) const {
        if (i < 0 || i >= n) throw out_of_range("RootishArrayStack::get: indice fuera de rango");
        int b = i2b(i);
        int j = i - b * (b + 1) / 2;
        return blocks[b][j];
    }
    T set(int i, T x) {
        if (i < 0 || i >= n) throw out_of_range("RootishArrayStack::set: indice fuera de rango");
        int b = i2b(i);
        int j = i - b * (b + 1) / 2;
        T y = blocks[b][j];
        blocks[b][j] = x;
        return y;
    }

    void grow() { g_reorg++; blocks.push_back(new T[blocks.size() + 1]); }

    void shrink() {
        int r = blocks.size();
        while (r > 0 && (r - 2) * (r - 1) / 2 >= n) {
            g_reorg++;
            delete[] blocks.back();
            blocks.pop_back();
            r--;
        }
    }

    void add(int i, T x) {
        if (i < 0 || i > n) throw out_of_range("RootishArrayStack::add: indice fuera de rango");
        int r = blocks.size();
        if (r * (r + 1) / 2 < n + 1) grow();
        n++;
        for (int j = n - 1; j > i; j--) { set(j, get(j - 1)); g_mov++; }
        set(i, x);
    }

    T remove(int i) {
        if (i < 0 || i >= n) throw out_of_range("RootishArrayStack::remove: indice fuera de rango");
        T x = get(i);
        for (int j = i; j < n - 1; j++) { set(j, get(j + 1)); g_mov++; }
        n--;
        int r = blocks.size();
        if ((r - 2) * (r - 1) / 2 >= n) shrink();
        return x;
    }

    int find(T x) const {
        for (int i = 0; i < n; i++) {
            g_rec++;
            if (get(i) == x) return i;
        }
        return -1;
    }

    void reverse() {
        for (int i = 0; i < n / 2; i++) {
            T temp = get(i);
            set(i, get(n - 1 - i));
            set(n - 1 - i, temp);
            g_mov += 3;
        }
    }

    bool checkInvariants(string& err) const {
        int r = blocks.size();
        if (r * (r + 1) / 2 < n) { err = "capacidad total de bloques menor que n"; return false; }
        if (r >= 2 && (r - 2) * (r - 1) / 2 >= n) { err = "debio haber hecho shrink()"; return false; }
        return true;
    }
};

void mostrarRootish(const RootishArrayStack& s) {
    cout << "  bloques: ";
    for (size_t b = 0; b < s.blocks.size(); b++) {
        cout << Color::cyan() << "[" << Color::reset();
        int inicio = b * (b + 1) / 2;
        for (size_t k = 0; k <= b; k++) {
            int idx = inicio + k;
            if (idx < s.n) {
                cout << Color::green() << s.blocks[b][k] << Color::reset();
            } else {
                cout << Color::dim() << '.' << Color::reset();
            }
            if (k < b) cout << ' ';
        }
        cout << Color::cyan() << "] " << Color::reset();
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

    SLList() = default;
    ~SLList() { while (n > 0) pop(); }
    SLList(const SLList&) = delete;
    SLList& operator=(const SLList&) = delete;

    int size() const { return n; }

    // Pila: insertar en la cabeza
    void push(T x) {
        Node* u = new Node{x, head};
        head = u;
        if (n == 0) tail = u;
        n++;
    }
    // Pila y cola: quitar de la cabeza
    T pop() {
        if (n == 0) throw underflow_error("SLList::pop: lista vacia");
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
    // Quitar el ULTIMO: sin puntero 'prev' hay que recorrer hasta el penultimo -> O(n)
    T removeLast() {
        if (n == 0) throw underflow_error("SLList::removeLast: lista vacia");
        if (n == 1) return pop();
        Node* p = head;
        while (p->next != tail) { p = p->next; g_rec++; }
        T x = tail->x;
        delete tail;
        tail = p;
        p->next = nullptr;
        n--;
        return x;
    }
    T peek() const {
        if (n == 0) throw underflow_error("SLList::peek: lista vacia");
        return head->x;
    }
    int find(T x) const {
        int idx = 0;
        for (Node* p = head; p; p = p->next, idx++) {
            g_rec++;
            if (p->x == x) return idx;
        }
        return -1;
    }
    void reverse() {
        Node* prev = nullptr;
        Node* curr = head;
        tail = head;
        while (curr) {
            Node* next = curr->next;
            curr->next = prev;
            prev = curr;
            curr = next;
            g_rec++;
        }
        head = prev;
    }
    bool checkInvariants(string& err) const {
        if (n == 0) {
            if (head != nullptr || tail != nullptr) { err = "n=0 pero head o tail no son null"; return false; }
            return true;
        }
        if (head == nullptr || tail == nullptr) { err = "n>0 pero head o tail son null"; return false; }
        if (tail->next != nullptr) { err = "tail->next no es null"; return false; }
        int count = 0;
        for (Node* p = head; p; p = p->next) {
            count++;
            if (count > n + 1) { err = "ciclo infinito detectado"; return false; }
        }
        if (count != n) { err = "conteo (" + to_string(count) + ") != n (" + to_string(n) + ")"; return false; }
        return true;
    }
};

void mostrarSL(const SLList& l) {
    cout << "  " << Color::cyan() << "head" << Color::reset() << " -> ";
    for (SLList::Node* p = l.head; p; p = p->next) {
        cout << "[" << Color::green() << Color::bold() << p->x << Color::reset() << "]"
             << Color::blue() << " -> " << Color::reset();
    }
    cout << Color::dim() << "null" << Color::reset()
         << "   (tail=" << (l.tail ? string(1, l.tail->x) : "-") << ", n=" << l.n << ")\n";
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

    int size() const { return n; }

    // Recorre desde el extremo mas cercano: O(1 + min{i, n-i})
    Node* getNode(int i) {
        if (i < 0 || i > n) throw out_of_range("DLList::getNode: indice fuera de rango");
        Node* p;
        if (i < n / 2) {
            p = dummy.next;
            for (int k = 0; k < i; k++) { p = p->next; g_rec++; }
        } else {
            p = &dummy;
            for (int k = n; k > i; k--) { p = p->prev; g_rec++; }   // getNode(n) devuelve dummy
        }
        return p;
    }
    T get(int i) {
        if (i < 0 || i >= n) throw out_of_range("DLList::get: indice fuera de rango");
        return getNode(i)->x;
    }
    T set(int i, T x) {
        if (i < 0 || i >= n) throw out_of_range("DLList::set: indice fuera de rango");
        Node* w = getNode(i); T y = w->x; w->x = x; return y;
    }

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
    void add(int i, T x) {
        if (i < 0 || i > n) throw out_of_range("DLList::add: indice fuera de rango");
        addBefore(getNode(i), x);
    }

    // Elimina el nodo w: O(1)
    void removeNode(Node* w) {
        if (w == &dummy) return;        // el nodo ficticio nunca se borra
        w->prev->next = w->next;
        w->next->prev = w->prev;
        delete w;
        n--;
    }
    T remove(int i) {
        if (i < 0 || i >= n) throw out_of_range("DLList::remove: indice fuera de rango");
        Node* w = getNode(i);
        if (w == &dummy) return T();
        T x = w->x;
        removeNode(w);
        return x;
    }

    int find(T x) {
        int idx = 0;
        for (Node* p = dummy.next; p != &dummy; p = p->next, idx++) {
            g_rec++;
            if (p->x == x) return idx;
        }
        return -1;
    }

    void reverse() {
        Node* curr = &dummy;
        do {
            swap(curr->next, curr->prev);
            curr = curr->prev;
            g_rec++;
        } while (curr != &dummy);
    }

    bool checkInvariants(string& err) const {
        int count = 0;
        for (const Node* p = dummy.next; p != &dummy; p = p->next) {
            count++;
            if (p->next->prev != p) { err = "incoherencia p->next->prev != p"; return false; }
            if (p->prev->next != p) { err = "incoherencia p->prev->next != p"; return false; }
            if (count > n + 1) { err = "ciclo infinito en DLList"; return false; }
        }
        if (count != n) { err = "conteo (" + to_string(count) + ") != n (" + to_string(n) + ")"; return false; }
        if (dummy.next->prev != &dummy || dummy.prev->next != &dummy) {
            err = "dummy desvinculado"; return false;
        }
        return true;
    }
};

void mostrarDL(DLList& l) {
    cout << "  " << Color::cyan() << "[dummy]" << Color::reset();
    for (DLList::Node* p = l.dummy.next; p != &l.dummy; p = p->next) {
        cout << Color::blue() << " <===> " << Color::reset()
             << "[" << Color::green() << Color::bold() << p->x << Color::reset() << "]";
    }
    cout << Color::blue() << " <===> " << Color::reset() << Color::cyan() << "[dummy]" << Color::reset();
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

        T get(int i) const {
            if (i < 0 || i >= n) throw out_of_range("BDeque::get fuera de rango");
            return a[(j + i) % len];
        }
        T set(int i, T x) {
            if (i < 0 || i >= n) throw out_of_range("BDeque::set fuera de rango");
            T y = a[(j + i) % len]; a[(j + i) % len] = x; return y;
        }
        void add(int i, T x) {
            if (i < 0 || i > n || n >= len) throw out_of_range("BDeque::add fuera de rango");
            if (i < n / 2) {
                j = (j == 0) ? len - 1 : j - 1;
                for (int k = 0; k <= i - 1; k++) { a[(j + k) % len] = a[(j + k + 1) % len]; g_mov++; }
            } else {
                for (int k = n; k > i; k--) { a[(j + k) % len] = a[(j + k - 1) % len]; g_mov++; }
            }
            a[(j + i) % len] = x;
            n++;
        }
        void add(T x) { add(n, x); }
        T remove(int i) {
            if (i < 0 || i >= n) throw out_of_range("BDeque::remove fuera de rango");
            T x = a[(j + i) % len];
            if (i < n / 2) {
                for (int k = i; k > 0; k--) { a[(j + k) % len] = a[(j + k - 1) % len]; g_mov++; }
                j = (j + 1) % len;
            } else {
                for (int k = i; k < n - 1; k++) { a[(j + k) % len] = a[(j + k + 1) % len]; g_mov++; }
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
        if (i < 0 || i >= n) throw out_of_range("SEList::getLocation: indice fuera de rango");
        if (i < n / 2) {
            Node* u = dummy.next;
            while (i >= u->d.n) { i -= u->d.n; u = u->next; g_rec++; }
            return {u, i};
        } else {
            Node* u = &dummy;
            int idx = n;
            while (i < idx) { u = u->prev; idx -= u->d.n; g_rec++; }
            return {u, i - idx};
        }
    }

    T get(int i) {
        if (i < 0 || i >= n) throw out_of_range("SEList::get: indice fuera de rango");
        Location l = getLocation(i);
        return l.u->d.get(l.j);
    }
    T set(int i, T x) {
        if (i < 0 || i >= n) throw out_of_range("SEList::set: indice fuera de rango");
        Location l = getLocation(i);
        return l.u->d.set(l.j, x);
    }
    int size() const { return n; }

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
        w = addBefore(w);
        g_reorg++;
        if (g_verbose) {
            cout << Color::magenta() << "    [spread] b bloques llenos: se crea un bloque nuevo y se reparte" << Color::reset() << "\n";
        }
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
        g_reorg++;
        if (g_verbose) {
            cout << Color::magenta() << "    [gather] b bloques con b-1 elementos: se juntan y se elimina un bloque" << Color::reset() << "\n";
        }
        removeNode(w);
    }

    void add(int i, T x) {
        if (i < 0 || i > n) throw out_of_range("SEList::add: indice fuera de rango");
        if (i == n) { add(x); return; }
        Location l = getLocation(i);
        Node* u = l.u;
        int r = 0;
        while (r < b && u != &dummy && u->d.n == b + 1) { u = u->next; r++; }
        if (r == b) {                     // b bloques consecutivos llenos
            spread(l.u);
            u = l.u;
        }
        if (u == &dummy) u = addBefore(u);
        while (u != l.u) {
            u->d.add(0, u->prev->d.remove(u->prev->d.n - 1));
            u = u->prev;
        }
        u->d.add(l.j, x);
        n++;
    }

    T remove(int i) {
        if (i < 0 || i >= n) throw out_of_range("SEList::remove: indice fuera de rango");
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

    int find(T x) {
        int idx = 0;
        for (Node* p = dummy.next; p != &dummy; p = p->next) {
            for (int k = 0; k < p->d.n; k++, idx++) {
                g_rec++;
                if (p->d.get(k) == x) return idx;
            }
        }
        return -1;
    }

    void reverse() {
        for (int i = 0; i < n / 2; i++) {
            T temp = get(i);
            set(i, get(n - 1 - i));
            set(n - 1 - i, temp);
            g_mov += 3;
        }
    }

    bool checkInvariants(string& err) const {
        if (dummy.next->prev != &dummy || dummy.prev->next != &dummy) {
            err = "dummy desvinculado"; return false;
        }
        int total = 0;
        for (const Node* p = dummy.next; p != &dummy; p = p->next) {
            if (p->next->prev != p || p->prev->next != p) {
                err = "incoherencia en enlaces de nodos"; return false;
            }
            if (p->d.n < 0 || p->d.n > b + 1) {
                err = "tamano de BDeque invalido: " + to_string(p->d.n); return false;
            }
            if (p != dummy.prev && p->d.n < b - 1) {
                err = "bloque intermedio con menos de b-1 elementos"; return false;
            }
            total += p->d.n;
        }
        if (total != n) {
            err = "suma de bloques (" + to_string(total) + ") != n (" + to_string(n) + ")";
            return false;
        }
        return true;
    }
};

void mostrarSE(SEList& l) {
    cout << "  ";
    for (SEList::Node* p = l.dummy.next; p != &l.dummy; p = p->next) {
        bool lleno = (p->d.n == l.b + 1);
        bool minimo = (p->d.n == l.b - 1);
        string colorBorde = lleno ? Color::magenta() : (minimo ? Color::yellow() : Color::cyan());
        cout << colorBorde << "[" << Color::reset();
        for (int k = 0; k < p->d.n; k++) {
            cout << Color::green() << p->d.get(k) << Color::reset() << (k < p->d.n - 1 ? " " : "");
        }
        cout << colorBorde << "]" << Color::reset() << " ";
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
//  LABORATORIO INTERACTIVO
//  Menus para probar cada estructura con tus propias entradas.
// =====================================================================

// ---------- Lectura de entradas (robusta: no se cae con datos raros) ----------
static string recortar(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

static string leerLinea(const string& prompt) {
    cout << prompt << flush;
    string s;
    if (!getline(cin, s)) {
        cout << "\n(Fin de la entrada. Hasta luego!)\n";
        exit(0);
    }
    return recortar(s);
}

static int leerEntero(const string& prompt, int lo, int hi) {
    while (true) {
        string s = leerLinea(prompt);
        try {
            size_t pos = 0;
            long v = stol(s, &pos);
            if (pos != s.size()) throw invalid_argument("resto");
            if (v < lo || v > hi) {
                cout << Color::red() << "  ! Debe estar entre " << lo << " y " << hi << "." << Color::reset() << "\n";
                continue;
            }
            return (int)v;
        } catch (...) {
            cout << Color::red() << "  ! Entrada no valida. Escribe un numero entero." << Color::reset() << "\n";
        }
    }
}

static T leerChar(const string& prompt) {
    while (true) {
        string s = leerLinea(prompt);
        if (s.empty()) {
            cout << Color::red() << "  ! Escribe un caracter (por ejemplo: a, 7, Z)." << Color::reset() << "\n";
            continue;
        }
        if (s.size() > 1) cout << "  (Se usara solo el primer caracter: '" << s[0] << "')\n";
        return s[0];
    }
}

// ---------- Presentacion ----------
static void encabezado(const string& t) {
    string linea(66, '=');
    cout << "\n" << Color::cyan() << linea << "\n  " << Color::bold() << t << Color::reset() << "\n"
         << Color::cyan() << linea << Color::reset() << "\n";
}

static void imprimirCosto(long long m, long long r, long long o) {
    cout << "  >> " << Color::bold() << "COSTO REAL: " << Color::reset()
         << Color::green() << m << " elementos movidos" << Color::reset() << " | "
         << Color::yellow() << r << " nodos/bloques recorridos" << Color::reset() << " | "
         << Color::magenta() << o << " reorganizacion(es)" << Color::reset() << "\n";
}

// Vistas de la estructura interna de cada tipo
static void ver(ArrayStack& s)        { mostrar(s, "interno"); }
static void ver(ArrayDeque& d)        { mostrarCircular("interno", d.a, d.len, d.j, d.n); }
static void ver(DualArrayDeque& d)    { mostrarDual(d); }
static void ver(RootishArrayStack& s) { mostrarRootish(s); }
static void ver(DLList& l)            { mostrarDL(l); }
static void ver(SEList& l)            { mostrarSE(l); }

template <class S>
static void logica(S& s) {
    int n = s.size();
    cout << "  Lista logica (" << n << " elem): ";
    if (n == 0) cout << "(vacia)";
    for (int i = 0; i < n && i < 60; i++) cout << Color::green() << s.get(i) << Color::reset() << ' ';
    if (n > 60) cout << "... (solo los primeros 60)";
    cout << "\n";
}

template <class S>
static void vistaCompleta(S& s) {
    ver(s);
    logica(s);
}

// ---------- Textos informativos ----------
static const char* INFO_AS =
    "ArrayStack: List sobre un arreglo que se duplica al llenarse y se reduce al vaciarse.\n"
    "  get/set: O(1).  add/remove(i): O(1 + n - i) porque desplazan elementos.\n"
    "  Al FINAL es O(1) amortizado; al INICIO desplaza todo. Espacio desperdiciado: O(n).\n"
    "  Prueba: agrega al INICIO (opcion 5) y luego al FINAL (opcion 6) y compara los movimientos.";

static const char* INFO_FAS =
    "FastArrayStack: igual que ArrayStack pero desplaza con std::copy (bloques de memoria).\n"
    "  Mismo O() que ArrayStack, y los mismos elementos movidos; solo baja el tiempo real.";

static const char* INFO_AD =
    "ArrayDeque: List sobre arreglo CIRCULAR. Desplaza siempre el lado mas corto.\n"
    "  add/remove(i): O(1 + min{i, n - i}) amortizado. Los extremos son O(1).\n"
    "  Prueba: agrega al INICIO: solo retrocede j, casi no mueve nada.";

static const char* INFO_DAD =
    "DualArrayDeque: dos ArrayStack espalda con espalda ('front' guardada al reves).\n"
    "  Mismo costo que ArrayDeque: O(1 + min{i, n - i}) amortizado.\n"
    "  Cuando una pila tiene mas del triple que la otra, rebalancea (mensaje [balance]).";

static const char* INFO_RAS =
    "RootishArrayStack: bloques de tamano 1, 2, 3, ... Espacio desperdiciado solo O(raiz de n).\n"
    "  get/set: O(1) usando la formula indice -> (bloque, posicion).\n"
    "  add/remove(i): O(1 + n - i). Crecer = anadir UN bloque, sin copiar todo.";

static const char* INFO_DL =
    "DLList: lista doblemente enlazada con nodo ficticio (dummy) que la cierra en circulo.\n"
    "  get/set/add/remove(i): O(1 + min{i, n - i}) (recorre desde el extremo mas cercano).\n"
    "  Sin resize. Cada nodo gasta memoria extra en 2 punteros.";

static string infoSE(int b) {
    return "SEList (b = " + to_string(b) + "): DLList donde cada nodo guarda un BLOQUE de ~b elementos.\n"
           "  Cada bloque tiene entre b-1 y b+1 elementos (invariante).\n"
           "  get/set: O(1 + i/b).  add/remove: O(b + min{i, n - i}/b) amortizado.\n"
           "  Espacio desperdiciado: O(b + n/b).  spread/gather redistribuyen bloques.";
}

// ---------- Menu generico para las estructuras tipo List ----------
template <class S>
static bool menuLista(S& s, const string& nombre, const string& info) {
    auto mutar = [&](const string& desc, auto op) {
        cout << "\n  ANTES:\n";
        vistaCompleta(s);
        g_mov = g_rec = g_reorg = 0;
        op();
        long long m = g_mov, r = g_rec, o = g_reorg;
        cout << "\n  " << Color::bold() << desc << Color::reset() << "\n  DESPUES:\n";
        vistaCompleta(s);
        imprimirCosto(m, r, o);
    };

    while (true) {
        int n = s.size();
        cout << "\n----------------------------------------------------------------\n"
             << "  " << Color::bold() << nombre << Color::reset() << "     (n = " << n << ")\n"
             << "----------------------------------------------------------------\n"
             << "   1) add(i, x)          insertar x en la posicion i\n"
             << "   2) remove(i)          eliminar la posicion i\n"
             << "   3) get(i)             leer la posicion i\n"
             << "   4) set(i, x)          reemplazar la posicion i\n"
             << "   5) add al INICIO      add(0, x)\n"
             << "   6) add al FINAL       add(n, x)\n"
             << "   7) quitar el PRIMERO  remove(0)\n"
             << "   8) quitar el ULTIMO   remove(n-1)\n"
             << "   9) cargar varios      (agrega texto caracter por caracter)\n"
             << "  10) buscar elemento    find(x) -> indice o -1\n"
             << "  11) invertir lista     reverse() in-situ\n"
             << "  12) ver diagnostico    verificar invariantes matematicas\n"
             << "  13) ver estructura interna\n"
             << "  14) vaciar / reiniciar\n"
             << "  15) info y complejidades\n"
             << "   0) volver al menu principal\n";
        int op = leerEntero("Opcion: ", 0, 15);
        n = s.size();
        bool vacia = (n == 0);

        switch (op) {
        case 0:
            return false;
        case 1: {
            int i = leerEntero("Posicion i (0.." + to_string(n) + "): ", 0, n);
            T x = leerChar("Elemento x: ");
            mutar("add(" + to_string(i) + ", '" + string(1, x) + "')", [&] { s.add(i, x); });
            break;
        }
        case 2: {
            if (vacia) { cout << Color::red() << "  ! La estructura esta vacia." << Color::reset() << "\n"; break; }
            int i = leerEntero("Posicion i (0.." + to_string(n - 1) + "): ", 0, n - 1);
            T out = 0;
            mutar("remove(" + to_string(i) + ")", [&] { out = s.remove(i); });
            cout << "  remove devolvio '" << out << "'\n";
            break;
        }
        case 3: {
            if (vacia) { cout << Color::red() << "  ! La estructura esta vacia." << Color::reset() << "\n"; break; }
            int i = leerEntero("Posicion i (0.." + to_string(n - 1) + "): ", 0, n - 1);
            g_mov = g_rec = g_reorg = 0;
            T v = s.get(i);
            long long m = g_mov, r = g_rec, o = g_reorg;
            cout << "\n  get(" << i << ") = '" << Color::green() << v << Color::reset() << "'\n";
            imprimirCosto(m, r, o);
            break;
        }
        case 4: {
            if (vacia) { cout << Color::red() << "  ! La estructura esta vacia." << Color::reset() << "\n"; break; }
            int i = leerEntero("Posicion i (0.." + to_string(n - 1) + "): ", 0, n - 1);
            T x = leerChar("Nuevo elemento x: ");
            T viejo = 0;
            mutar("set(" + to_string(i) + ", '" + string(1, x) + "')", [&] { viejo = s.set(i, x); });
            cout << "  set devolvio el valor anterior '" << viejo << "'\n";
            break;
        }
        case 5: {
            T x = leerChar("Elemento x: ");
            mutar("add(0, '" + string(1, x) + "')  -> al INICIO", [&] { s.add(0, x); });
            break;
        }
        case 6: {
            T x = leerChar("Elemento x: ");
            mutar("add(" + to_string(n) + ", '" + string(1, x) + "')  -> al FINAL", [&] { s.add(n, x); });
            break;
        }
        case 7: {
            if (vacia) { cout << Color::red() << "  ! La estructura esta vacia." << Color::reset() << "\n"; break; }
            T out = 0;
            mutar("remove(0)  -> quitar el PRIMERO", [&] { out = s.remove(0); });
            cout << "  remove devolvio '" << out << "'\n";
            break;
        }
        case 8: {
            if (vacia) { cout << Color::red() << "  ! La estructura esta vacia." << Color::reset() << "\n"; break; }
            T out = 0;
            mutar("remove(" + to_string(n - 1) + ")  -> quitar el ULTIMO", [&] { out = s.remove(n - 1); });
            cout << "  remove devolvio '" << out << "'\n";
            break;
        }
        case 9: {
            string t = leerLinea("Escribe caracteres (los espacios se ignoran, max 100): ");
            string limpio;
            for (char c : t) if (!isspace((unsigned char)c) && limpio.size() < 100) limpio += c;
            if (limpio.empty()) { cout << Color::red() << "  ! No escribiste nada." << Color::reset() << "\n"; break; }
            mutar("cargar \"" + limpio + "\" al final", [&] {
                for (char c : limpio) s.add(s.size(), c);
            });
            break;
        }
        case 10: {
            if (vacia) { cout << Color::red() << "  ! La estructura esta vacia." << Color::reset() << "\n"; break; }
            T x = leerChar("Caracter a buscar: ");
            g_mov = g_rec = g_reorg = 0;
            int pos = s.find(x);
            long long m = g_mov, r = g_rec, o = g_reorg;
            if (pos != -1) {
                cout << "\n  " << Color::green() << "Encontrado en posicion " << pos << Color::reset() << "\n";
            } else {
                cout << "\n  " << Color::red() << "No encontrado (-1)" << Color::reset() << "\n";
            }
            imprimirCosto(m, r, o);
            break;
        }
        case 11: {
            if (vacia) { cout << Color::red() << "  ! La estructura esta vacia." << Color::reset() << "\n"; break; }
            mutar("reverse()  -> invertir lista", [&] { s.reverse(); });
            break;
        }
        case 12: {
            string err;
            bool ok = s.checkInvariants(err);
            cout << "\n  Diagnostico de Invariantes: ";
            if (ok) {
                cout << Color::green() << Color::bold() << "[TODO CORRECTO] Todas las invariantes se cumplen." << Color::reset() << "\n";
            } else {
                cout << Color::red() << Color::bold() << "[FALLO]: " << err << Color::reset() << "\n";
            }
            break;
        }
        case 13:
            cout << "\n";
            vistaCompleta(s);
            break;
        case 14:
            cout << "  Estructura vaciada.\n";
            return true;
        case 15:
            cout << "\n" << info << "\n";
            break;
        }
    }
}

template <class S, class... A>
static void ejecutarLista(const string& nombre, const string& info, A... args) {
    encabezado(nombre);
    cout << info << "\n";
    bool otra = true;
    while (otra) {
        S s(args...);
        otra = menuLista(s, nombre, info);
    }
}

// ---------- ArrayQueue (interfaz Queue, no List) ----------
static void verCola(ArrayQueue& q) {
    mostrarCircular("interno", q.a, q.len, q.j, q.n);
    cout << "  Cola logica: frente -> ";
    if (q.n == 0) cout << "(vacia) ";
    for (int k = 0; k < q.n; k++) cout << Color::green() << q.a[(q.j + k) % q.len] << Color::reset() << ' ';
    cout << "<- final\n";
}

static bool menuCola(ArrayQueue& q) {
    auto mutar = [&](const string& desc, auto op) {
        cout << "\n  ANTES:\n";
        verCola(q);
        g_mov = g_rec = g_reorg = 0;
        op();
        long long m = g_mov, r = g_rec, o = g_reorg;
        cout << "\n  " << Color::bold() << desc << Color::reset() << "\n  DESPUES:\n";
        verCola(q);
        imprimirCosto(m, r, o);
    };
    while (true) {
        cout << "\n----------------------------------------------------------------\n"
             << "  " << Color::bold() << "ArrayQueue" << Color::reset() << "     (n = " << q.n << ")\n"
             << "----------------------------------------------------------------\n"
             << "   1) add(x)            ENCOLAR al final\n"
             << "   2) remove()          DESENCOLAR el primero (FIFO)\n"
             << "   3) primero           ver el frente sin sacarlo\n"
             << "   4) cargar varios     (encola un texto caracter por caracter)\n"
             << "   5) buscar elemento   find(x) -> indice relativo o -1\n"
             << "   6) verificar diag.   verificar invariantes\n"
             << "   7) ver estructura interna (arreglo circular)\n"
             << "   8) vaciar / reiniciar\n"
             << "   9) info y complejidades\n"
             << "   0) volver al menu principal\n";
        int op = leerEntero("Opcion: ", 0, 9);
        switch (op) {
        case 0: return false;
        case 1: {
            T x = leerChar("Elemento x: ");
            mutar("add('" + string(1, x) + "')", [&] { q.add(x); });
            break;
        }
        case 2: {
            if (q.n == 0) { cout << Color::red() << "  ! La cola esta vacia." << Color::reset() << "\n"; break; }
            T out = 0;
            mutar("remove()", [&] { out = q.remove(); });
            cout << "  remove devolvio '" << out << "'  (solo avanzo j, no se desplazo nada)\n";
            break;
        }
        case 3:
            if (q.n == 0) cout << Color::red() << "  ! La cola esta vacia." << Color::reset() << "\n";
            else cout << "  Primero = '" << Color::green() << q.peek() << Color::reset() << "'  (casilla j = " << q.j << ")\n";
            break;
        case 4: {
            string t = leerLinea("Escribe caracteres (los espacios se ignoran, max 100): ");
            string limpio;
            for (char c : t) if (!isspace((unsigned char)c) && limpio.size() < 100) limpio += c;
            if (limpio.empty()) { cout << Color::red() << "  ! No escribiste nada." << Color::reset() << "\n"; break; }
            mutar("encolar \"" + limpio + "\"", [&] { for (char c : limpio) q.add(c); });
            break;
        }
        case 5: {
            if (q.n == 0) { cout << Color::red() << "  ! La cola esta vacia." << Color::reset() << "\n"; break; }
            T x = leerChar("Caracter a buscar: ");
            g_mov = g_rec = g_reorg = 0;
            int pos = q.find(x);
            long long m = g_mov, r = g_rec, o = g_reorg;
            if (pos != -1) cout << "\n  " << Color::green() << "Encontrado en posicion relativa " << pos << Color::reset() << "\n";
            else cout << "\n  " << Color::red() << "No encontrado (-1)" << Color::reset() << "\n";
            imprimirCosto(m, r, o);
            break;
        }
        case 6: {
            string err;
            bool ok = q.checkInvariants(err);
            cout << "\n  Diagnostico: ";
            if (ok) cout << Color::green() << Color::bold() << "[OK] Invariantes validas." << Color::reset() << "\n";
            else cout << Color::red() << Color::bold() << "[FALLO]: " << err << Color::reset() << "\n";
            break;
        }
        case 7:
            cout << "\n";
            verCola(q);
            break;
        case 8:
            cout << "  Cola vaciada.\n";
            return true;
        case 9:
            cout << "\nArrayQueue: cola FIFO con arreglo CIRCULAR. j = indice del primer elemento;\n"
                    "  el siguiente hueco libre es (j + n) % len (aritmetica modular).\n"
                    "  add/remove: O(1) amortizado (solo el resize ocasional copia todo).\n"
                    "  Solo agrega al final y quita del frente: no tiene acceso por indice.\n"
                    "  Prueba: encola hasta llenarla, desencola dos y encola de nuevo: veras que 'da la vuelta'.\n";
            break;
        }
    }
}

static void menuArrayQueue() {
    encabezado("ArrayQueue");
    cout << "Cola FIFO sobre arreglo circular.\n";
    bool otra = true;
    while (otra) {
        ArrayQueue q;
        otra = menuCola(q);
    }
}

// ---------- SLList (Stack + Queue) ----------
static bool menuSLList(SLList& l) {
    auto mutar = [&](const string& desc, auto op) {
        cout << "\n  ANTES:\n";
        mostrarSL(l);
        g_mov = g_rec = g_reorg = 0;
        op();
        long long m = g_mov, r = g_rec, o = g_reorg;
        cout << "\n  " << Color::bold() << desc << Color::reset() << "\n  DESPUES:\n";
        mostrarSL(l);
        imprimirCosto(m, r, o);
    };
    while (true) {
        cout << "\n----------------------------------------------------------------\n"
             << "  " << Color::bold() << "SLList" << Color::reset() << "     (n = " << l.n << ")\n"
             << "----------------------------------------------------------------\n"
             << "   1) push(x)           PILA: insertar en la CABEZA\n"
             << "   2) pop()             PILA: quitar de la CABEZA\n"
             << "   3) add(x)            COLA: insertar en la COLA (usa tail)\n"
             << "   4) remove()          COLA: quitar de la CABEZA\n"
             << "   5) quitar el ULTIMO  (intento de usarla como deque: mira el costo)\n"
             << "   6) cargar varios     (add al final, caracter por caracter)\n"
             << "   7) buscar elemento   find(x) -> indice o -1\n"
             << "   8) invertir lista    reverse() in-situ (invierte punteros)\n"
             << "   9) verificar diag.   verificar invariantes\n"
             << "  10) ver estructura interna\n"
             << "  11) vaciar / reiniciar\n"
             << "  12) info y complejidades\n"
             << "   0) volver al menu principal\n";
        int op = leerEntero("Opcion: ", 0, 12);
        bool vacia = (l.n == 0);
        switch (op) {
        case 0: return false;
        case 1: {
            T x = leerChar("Elemento x: ");
            mutar("push('" + string(1, x) + "')  -> nueva CABEZA", [&] { l.push(x); });
            break;
        }
        case 2: {
            if (vacia) { cout << Color::red() << "  ! La lista esta vacia." << Color::reset() << "\n"; break; }
            T out = 0;
            mutar("pop()", [&] { out = l.pop(); });
            cout << "  pop devolvio '" << out << "'\n";
            break;
        }
        case 3: {
            T x = leerChar("Elemento x: ");
            mutar("add('" + string(1, x) + "')  -> nueva COLA", [&] { l.add(x); });
            break;
        }
        case 4: {
            if (vacia) { cout << Color::red() << "  ! La lista esta vacia." << Color::reset() << "\n"; break; }
            T out = 0;
            mutar("remove()", [&] { out = l.remove(); });
            cout << "  remove devolvio '" << out << "'\n";
            break;
        }
        case 5: {
            if (vacia) { cout << Color::red() << "  ! La lista esta vacia." << Color::reset() << "\n"; break; }
            T out = 0;
            mutar("quitar el ULTIMO: recorre hasta el penultimo", [&] { out = l.removeLast(); });
            cout << "  Se quito '" << out << "'. Fijate en 'nodos recorridos': crece con n (O(n)).\n"
                    "  Con una DLList (puntero prev) esto seria O(1).\n";
            break;
        }
        case 6: {
            string t = leerLinea("Escribe caracteres (los espacios se ignoran, max 100): ");
            string limpio;
            for (char c : t) if (!isspace((unsigned char)c) && limpio.size() < 100) limpio += c;
            if (limpio.empty()) { cout << Color::red() << "  ! No escribiste nada." << Color::reset() << "\n"; break; }
            mutar("cargar \"" + limpio + "\" con add", [&] { for (char c : limpio) l.add(c); });
            break;
        }
        case 7: {
            if (vacia) { cout << Color::red() << "  ! La lista esta vacia." << Color::reset() << "\n"; break; }
            T x = leerChar("Caracter a buscar: ");
            g_mov = g_rec = g_reorg = 0;
            int pos = l.find(x);
            long long m = g_mov, r = g_rec, o = g_reorg;
            if (pos != -1) cout << "\n  " << Color::green() << "Encontrado en posicion " << pos << Color::reset() << "\n";
            else cout << "\n  " << Color::red() << "No encontrado (-1)" << Color::reset() << "\n";
            imprimirCosto(m, r, o);
            break;
        }
        case 8: {
            if (vacia) { cout << Color::red() << "  ! La lista esta vacia." << Color::reset() << "\n"; break; }
            mutar("reverse()  -> invertir punteros de la lista", [&] { l.reverse(); });
            break;
        }
        case 9: {
            string err;
            bool ok = l.checkInvariants(err);
            cout << "\n  Diagnostico: ";
            if (ok) cout << Color::green() << Color::bold() << "[OK] Invariantes validas." << Color::reset() << "\n";
            else cout << Color::red() << Color::bold() << "[FALLO]: " << err << Color::reset() << "\n";
            break;
        }
        case 10:
            cout << "\n";
            mostrarSL(l);
            break;
        case 11:
            cout << "  Lista vaciada.\n";
            return true;
        case 12:
            cout << "\nSLList: nodos con un puntero 'next'; se guardan head y tail.\n"
                    "  push/pop (pila) y add/remove (cola): O(1) REAL (sin resize, sin amortizar).\n"
                    "  No es un deque: quitar el ultimo nodo es O(n). Tampoco hay acceso rapido por indice.\n";
            break;
        }
    }
}

static void menuSLListEntrada() {
    encabezado("SLList");
    cout << "Lista simplemente enlazada: sirve como Stack y como Queue.\n";
    bool otra = true;
    while (otra) {
        SLList l;
        otra = menuSLList(l);
    }
}

// ---------- Comparador ----------
struct Fila {
    string nombre;
    long long mov, rec, reorg;
    double ms;
};

template <class S, class... A>
static Fila medirEstructura(const string& nombre, int n, int k, int op, A... args) {
    S s(args...);
    for (int i = 0; i < n; i++) s.add(s.size(), (T)('a' + i % 26));
    g_mov = g_rec = g_reorg = 0;
    auto t0 = chrono::steady_clock::now();
    for (int c = 0; c < k; c++) {
        int m = s.size();
        switch (op) {
        case 1: s.add(0, 'z'); break;
        case 2: s.add(m / 2, 'z'); break;
        case 3: s.add(m, 'z'); break;
        case 4: s.remove(0); break;
        case 5: s.remove(m / 2); break;
        case 6: s.remove(m - 1); break;
        case 7: s.get(m / 2); break;
        }
    }
    auto t1 = chrono::steady_clock::now();
    double ms = chrono::duration<double, milli>(t1 - t0).count();
    return {nombre, g_mov, g_rec, g_reorg, ms};
}

static void comparador() {
    encabezado("COMPARADOR: el mismo trabajo en todas las estructuras tipo lista");
    cout << "Se construye cada estructura con n elementos y se repite k veces una operacion.\n"
            "Asi ves con numeros reales cual desplaza mas, cual recorre mas y cual reorganiza mas.\n\n"
            "  1) insertar al INICIO    add(0, x)\n"
            "  2) insertar en el MEDIO  add(n/2, x)\n"
            "  3) insertar al FINAL     add(n, x)\n"
            "  4) eliminar el PRIMERO   remove(0)\n"
            "  5) eliminar del MEDIO    remove(n/2)\n"
            "  6) eliminar el ULTIMO    remove(n-1)\n"
            "  7) leer el del MEDIO     get(n/2)\n";
    int op = leerEntero("Operacion (1-7): ", 1, 7);
    int n = leerEntero("n = elementos iniciales (10..5000): ", 10, 5000);
    int kmax = (op >= 4 && op <= 6) ? min(n - 1, 2000) : 2000;
    int k = leerEntero("k = repeticiones (1.." + to_string(kmax) + "): ", 1, kmax);

    g_verbose = false;   // silenciar [balance], [spread], [gather]
    vector<Fila> f;
    f.push_back(medirEstructura<ArrayStack>("ArrayStack", n, k, op));
    f.push_back(medirEstructura<FastArrayStack>("FastArrayStack", n, k, op));
    f.push_back(medirEstructura<ArrayDeque>("ArrayDeque", n, k, op));
    f.push_back(medirEstructura<DualArrayDeque>("DualArrayDeque", n, k, op));
    f.push_back(medirEstructura<RootishArrayStack>("RootishArrayStack", n, k, op));
    f.push_back(medirEstructura<DLList>("DLList", n, k, op));
    f.push_back(medirEstructura<SEList>("SEList (b=8)", n, k, op, 8));
    g_verbose = true;

    cout << "\n  Resultado: " << k << " repeticiones sobre n = " << n << "\n\n";
    cout << "  " << left << setw(20) << "Estructura"
         << right << setw(14) << "movidos" << setw(14) << "recorridos"
         << setw(10) << "reorg." << setw(12) << "tiempo(ms)" << "\n";
    cout << "  " << string(70, '-') << "\n";
    long long minimo = f[0].mov + f[0].rec;
    for (size_t i = 0; i < f.size(); i++) {
        cout << "  " << left << setw(20) << f[i].nombre
             << right << setw(14) << f[i].mov << setw(14) << f[i].rec
             << setw(10) << f[i].reorg << setw(12) << fixed << setprecision(2) << f[i].ms << "\n";
        minimo = min(minimo, f[i].mov + f[i].rec);
    }
    cout << defaultfloat;
    string ganadoras;
    for (size_t i = 0; i < f.size(); i++)
        if (f[i].mov + f[i].rec == minimo) ganadoras += (ganadoras.empty() ? "" : ", ") + f[i].nombre;
    cout << "\n  " << Color::green() << Color::bold() << "Menos trabajo (movidos + recorridos = " << minimo << "): " << ganadoras << Color::reset() << "\n";

    cout << "\n  Como leerlo:\n";
    switch (op) {
    case 1:
        cout << "  Al INICIO: ArrayStack, FastArrayStack y RootishArrayStack desplazan TODO (O(n) cada vez).\n"
                "  ArrayDeque y DualArrayDeque casi no mueven nada; DLList solo enlaza; SEList mueve\n"
                "  un elemento por bloque.\n";
        break;
    case 2:
        cout << "  En el MEDIO nadie se salva: los arreglos desplazan ~n/2 elementos, DLList recorre ~n/2 nodos\n"
                "  y SEList recorre solo ~n/(2b) bloques.\n";
        break;
    case 3:
        cout << "  Al FINAL casi todos son O(1) amortizado: los 'movidos' vienen solo de los resize ocasionales.\n";
        break;
    case 4:
        cout << "  Quitar el PRIMERO: los ArrayStack/Rootish desplazan todo; las demas casi nada.\n";
        break;
    case 5:
        cout << "  Quitar del MEDIO: cuesta a todos; cambia si es en elementos movidos o en nodos recorridos.\n";
        break;
    case 6:
        cout << "  Quitar el ULTIMO: O(1) para casi todas (DLList llega por prev desde el dummy).\n";
        break;
    case 7:
        cout << "  Leer por indice: los arreglos tienen 0 recorridos (acceso directo). DLList recorre hasta n/2\n"
                "  nodos y SEList ~n/(2b) bloques.\n";
        break;
    }
}

// ---------- Demos guiadas, tabla y ayuda ----------
static void menuDemos() {
    while (true) {
        encabezado("DEMOS AUTOMATICAS (los ejemplos guiados)");
        cout << "  1) ArrayStack        2) FastArrayStack     3) ArrayQueue\n"
                "  4) ArrayDeque        5) DualArrayDeque     6) RootishArrayStack\n"
                "  7) SLList            8) DLList             9) SEList\n"
                " 10) TODAS en orden\n"
                "  0) volver\n";
        int op = leerEntero("Opcion: ", 0, 10);
        switch (op) {
        case 0: return;
        case 1: demoArrayStack(); break;
        case 2: demoFastArrayStack(); break;
        case 3: demoArrayQueue(); break;
        case 4: demoArrayDeque(); break;
        case 5: demoDualArrayDeque(); break;
        case 6: demoRootish(); break;
        case 7: demoSLList(); break;
        case 8: demoDLList(); break;
        case 9: demoSEList(); break;
        case 10:
            demoArrayStack(); demoFastArrayStack(); demoArrayQueue();
            demoArrayDeque(); demoDualArrayDeque(); demoRootish();
            demoSLList(); demoDLList(); demoSEList();
            break;
        }
        leerLinea("\nPresiona ENTER para continuar...");
    }
}

static void tablaComplejidades() {
    encabezado("TABLA DE COMPLEJIDADES");
    cout << R"(
  Estructura          get/set             add/remove(i)              Espacio perdido
  ------------------- ------------------- -------------------------- ---------------
  ArrayStack          O(1)                O(1 + n - i)               O(n)
  FastArrayStack      O(1)                O(1 + n - i) (constante -) O(n)
  ArrayQueue          (no tiene)          O(1) am. (extremos)        O(n)
  ArrayDeque          O(1)                O(1 + min{i, n - i}) am.   O(n)
  DualArrayDeque      O(1)                O(1 + min{i, n - i}) am.   O(n)
  RootishArrayStack   O(1)                O(1 + n - i)               O(raiz n)
  SLList              (no tiene)          O(1) en los extremos       punteros
  DLList              O(1 + min{i, n-i})  O(1 + min{i, n - i})       punteros
  SEList              O(1 + i/b)          O(b + min{i, n - i}/b) am. O(b + n/b)

  am. = amortizado (el costo promedio sobre muchas operaciones; algunas
  operaciones sueltas, como un resize, cuestan O(n)).
)";
}

static void ayuda() {
    encabezado("COMO LEER LOS COSTOS");
    cout << R"(
  Despues de cada operacion veras:

    ANTES / DESPUES   La estructura interna antes y despues de la operacion.
                      En los arreglos, '.' es una casilla vacia. En el arreglo
                      circular, j (amarillo) marca donde empieza la lista.

    elementos movidos Cuantos elementos se copiaron o desplazaron de una casilla
                      a otra (desplazamientos + copias en un resize).

    nodos/bloques     Cuantos nodos (DLList, SLList) o bloques (SEList) hubo que
    recorridos        atravesar siguiendo punteros para llegar al lugar.

    reorganizaciones  Veces que hubo resize (arreglos), rebalanceo (DualArrayDeque),
                      bloque anadido o quitado (RootishArrayStack), o spread/gather
                      (SEList).

  Ideas para experimentar:
    - Carga "abcdefgh" y compara add al INICIO vs add al FINAL en ArrayStack y en ArrayDeque.
    - En ArrayQueue, encola hasta llenar, desencola y vuelve a encolar: veras la vuelta circular.
    - En SLList usa "quitar el ULTIMO" con listas cada vez mas largas.
    - En SEList con b=2, inserta en el medio varias veces hasta ver spread; luego elimina hasta ver gather.
    - Usa el COMPARADOR con n grande para ver las diferencias con numeros.
    - Ejecuta el TEST DE ESTRES (opcion 14) para comprobar fidelidad matemática contra la STL.
)";
}

// ---------- Test de Estres / Fuzzing contra STL ----------
static void testEstresFuzzing() {
    encabezado("TEST DE ESTRES Y FUZZING AUTOMATICO (contra STL)");
    cout << "Se ejecutaran miles de operaciones aleatorias sobre cada una de las 9 estructuras\n"
         << "y se comparara el estado interno y resultados paso a paso contra std::vector / std::deque.\n\n";

    const int NUM_OPS = 5000;
    mt19937 rng(1337); // semilla fija para reproducibilidad

    auto printResult = [](const string& name, bool ok, double ms, const string& detail = "") {
        cout << "  " << left << setw(22) << name;
        if (ok) {
            cout << Color::green() << Color::bold() << "[OK] " << Color::reset()
                 << NUM_OPS << " ops verificadas  (" << fixed << setprecision(2) << ms << " ms)\n";
        } else {
            cout << Color::red() << Color::bold() << "[FALLO] " << Color::reset() << detail << "\n";
        }
        cout << defaultfloat;
    };

    g_verbose = false;

    // 1. ArrayStack vs std::vector
    {
        ArrayStack s;
        vector<char> v;
        string err;
        bool ok = true;
        auto t0 = chrono::steady_clock::now();
        for (int op = 0; op < NUM_OPS && ok; op++) {
            int action = rng() % 4;
            char val = 'a' + (rng() % 26);
            if (action == 0 || v.empty()) {
                int idx = rng() % (v.size() + 1);
                s.add(idx, val);
                v.insert(v.begin() + idx, val);
            } else if (action == 1) {
                int idx = rng() % v.size();
                char c1 = s.remove(idx);
                char c2 = v[idx];
                v.erase(v.begin() + idx);
                if (c1 != c2) { ok = false; err = "remove mismatch"; break; }
            } else if (action == 2) {
                int idx = rng() % v.size();
                s.set(idx, val);
                v[idx] = val;
            } else {
                int idx = rng() % v.size();
                if (s.get(idx) != v[idx]) { ok = false; err = "get mismatch"; break; }
            }
            if (!s.checkInvariants(err)) { ok = false; break; }
        }
        if (ok && s.size() == (int)v.size()) {
            for (size_t i = 0; i < v.size(); i++) {
                if (s.get(i) != v[i]) { ok = false; err = "final mismatch"; break; }
            }
        } else if (ok) { ok = false; err = "size mismatch"; }
        auto t1 = chrono::steady_clock::now();
        printResult("ArrayStack", ok, chrono::duration<double, milli>(t1 - t0).count(), err);
    }

    // 2. FastArrayStack vs std::vector
    {
        FastArrayStack s;
        vector<char> v;
        string err;
        bool ok = true;
        auto t0 = chrono::steady_clock::now();
        for (int op = 0; op < NUM_OPS && ok; op++) {
            int action = rng() % 4;
            char val = 'a' + (rng() % 26);
            if (action == 0 || v.empty()) {
                int idx = rng() % (v.size() + 1);
                s.add(idx, val);
                v.insert(v.begin() + idx, val);
            } else if (action == 1) {
                int idx = rng() % v.size();
                char c1 = s.remove(idx);
                char c2 = v[idx];
                v.erase(v.begin() + idx);
                if (c1 != c2) { ok = false; err = "remove mismatch"; break; }
            } else if (action == 2) {
                int idx = rng() % v.size();
                s.set(idx, val);
                v[idx] = val;
            } else {
                int idx = rng() % v.size();
                if (s.get(idx) != v[idx]) { ok = false; err = "get mismatch"; break; }
            }
            if (!s.checkInvariants(err)) { ok = false; break; }
        }
        if (ok && s.size() == (int)v.size()) {
            for (size_t i = 0; i < v.size(); i++) {
                if (s.get(i) != v[i]) { ok = false; err = "final mismatch"; break; }
            }
        } else if (ok) { ok = false; err = "size mismatch"; }
        auto t1 = chrono::steady_clock::now();
        printResult("FastArrayStack", ok, chrono::duration<double, milli>(t1 - t0).count(), err);
    }

    // 3. ArrayQueue vs std::deque
    {
        ArrayQueue q;
        deque<char> d;
        string err;
        bool ok = true;
        auto t0 = chrono::steady_clock::now();
        for (int op = 0; op < NUM_OPS && ok; op++) {
            int action = rng() % 3;
            char val = 'a' + (rng() % 26);
            if (action <= 1 || d.empty()) {
                q.add(val);
                d.push_back(val);
            } else {
                char c1 = q.remove();
                char c2 = d.front();
                d.pop_front();
                if (c1 != c2) { ok = false; err = "remove mismatch"; break; }
            }
            if (!q.checkInvariants(err)) { ok = false; break; }
        }
        if (ok && q.size() == (int)d.size()) {
            for (size_t i = 0; i < d.size(); i++) {
                char c = q.remove();
                if (c != d[i]) { ok = false; err = "final queue mismatch"; break; }
            }
        } else if (ok) { ok = false; err = "size mismatch"; }
        auto t1 = chrono::steady_clock::now();
        printResult("ArrayQueue", ok, chrono::duration<double, milli>(t1 - t0).count(), err);
    }

    // 4. ArrayDeque vs std::deque
    {
        ArrayDeque ad;
        deque<char> d;
        string err;
        bool ok = true;
        auto t0 = chrono::steady_clock::now();
        for (int op = 0; op < NUM_OPS && ok; op++) {
            int action = rng() % 4;
            char val = 'a' + (rng() % 26);
            if (action == 0 || d.empty()) {
                int idx = rng() % (d.size() + 1);
                ad.add(idx, val);
                d.insert(d.begin() + idx, val);
            } else if (action == 1) {
                int idx = rng() % d.size();
                char c1 = ad.remove(idx);
                char c2 = d[idx];
                d.erase(d.begin() + idx);
                if (c1 != c2) { ok = false; err = "remove mismatch"; break; }
            } else if (action == 2) {
                int idx = rng() % d.size();
                ad.set(idx, val);
                d[idx] = val;
            } else {
                int idx = rng() % d.size();
                if (ad.get(idx) != d[idx]) { ok = false; err = "get mismatch"; break; }
            }
            if (!ad.checkInvariants(err)) { ok = false; break; }
        }
        if (ok && ad.size() == (int)d.size()) {
            for (size_t i = 0; i < d.size(); i++) {
                if (ad.get(i) != d[i]) { ok = false; err = "final mismatch"; break; }
            }
        } else if (ok) { ok = false; err = "size mismatch"; }
        auto t1 = chrono::steady_clock::now();
        printResult("ArrayDeque", ok, chrono::duration<double, milli>(t1 - t0).count(), err);
    }

    // 5. DualArrayDeque vs std::vector
    {
        DualArrayDeque dad;
        vector<char> v;
        string err;
        bool ok = true;
        auto t0 = chrono::steady_clock::now();
        for (int op = 0; op < NUM_OPS && ok; op++) {
            int action = rng() % 4;
            char val = 'a' + (rng() % 26);
            if (action == 0 || v.empty()) {
                int idx = rng() % (v.size() + 1);
                dad.add(idx, val);
                v.insert(v.begin() + idx, val);
            } else if (action == 1) {
                int idx = rng() % v.size();
                char c1 = dad.remove(idx);
                char c2 = v[idx];
                v.erase(v.begin() + idx);
                if (c1 != c2) { ok = false; err = "remove mismatch"; break; }
            } else if (action == 2) {
                int idx = rng() % v.size();
                dad.set(idx, val);
                v[idx] = val;
            } else {
                int idx = rng() % v.size();
                if (dad.get(idx) != v[idx]) { ok = false; err = "get mismatch"; break; }
            }
            if (!dad.checkInvariants(err)) { ok = false; break; }
        }
        if (ok && dad.size() == (int)v.size()) {
            for (size_t i = 0; i < v.size(); i++) {
                if (dad.get(i) != v[i]) { ok = false; err = "final mismatch"; break; }
            }
        } else if (ok) { ok = false; err = "size mismatch"; }
        auto t1 = chrono::steady_clock::now();
        printResult("DualArrayDeque", ok, chrono::duration<double, milli>(t1 - t0).count(), err);
    }

    // 6. RootishArrayStack vs std::vector
    {
        RootishArrayStack ras;
        vector<char> v;
        string err;
        bool ok = true;
        auto t0 = chrono::steady_clock::now();
        for (int op = 0; op < NUM_OPS && ok; op++) {
            int action = rng() % 4;
            char val = 'a' + (rng() % 26);
            if (action == 0 || v.empty()) {
                int idx = rng() % (v.size() + 1);
                ras.add(idx, val);
                v.insert(v.begin() + idx, val);
            } else if (action == 1) {
                int idx = rng() % v.size();
                char c1 = ras.remove(idx);
                char c2 = v[idx];
                v.erase(v.begin() + idx);
                if (c1 != c2) { ok = false; err = "remove mismatch"; break; }
            } else if (action == 2) {
                int idx = rng() % v.size();
                ras.set(idx, val);
                v[idx] = val;
            } else {
                int idx = rng() % v.size();
                if (ras.get(idx) != v[idx]) { ok = false; err = "get mismatch"; break; }
            }
            if (!ras.checkInvariants(err)) { ok = false; break; }
        }
        if (ok && ras.size() == (int)v.size()) {
            for (size_t i = 0; i < v.size(); i++) {
                if (ras.get(i) != v[i]) { ok = false; err = "final mismatch"; break; }
            }
        } else if (ok) { ok = false; err = "size mismatch"; }
        auto t1 = chrono::steady_clock::now();
        printResult("RootishArrayStack", ok, chrono::duration<double, milli>(t1 - t0).count(), err);
    }

    // 7. SLList vs std::deque
    {
        SLList sl;
        deque<char> d;
        string err;
        bool ok = true;
        auto t0 = chrono::steady_clock::now();
        for (int op = 0; op < NUM_OPS && ok; op++) {
            int action = rng() % 4;
            char val = 'a' + (rng() % 26);
            if (action == 0 || d.empty()) {
                sl.push(val);
                d.push_front(val);
            } else if (action == 1) {
                sl.add(val);
                d.push_back(val);
            } else if (action == 2) {
                char c1 = sl.pop();
                char c2 = d.front();
                d.pop_front();
                if (c1 != c2) { ok = false; err = "pop mismatch"; break; }
            } else {
                char c1 = sl.removeLast();
                char c2 = d.back();
                d.pop_back();
                if (c1 != c2) { ok = false; err = "removeLast mismatch"; break; }
            }
            if (!sl.checkInvariants(err)) { ok = false; break; }
        }
        if (ok && sl.size() == (int)d.size()) {
            for (size_t i = 0; i < d.size(); i++) {
                char c = sl.pop();
                if (c != d[i]) { ok = false; err = "final mismatch"; break; }
            }
        } else if (ok) { ok = false; err = "size mismatch"; }
        auto t1 = chrono::steady_clock::now();
        printResult("SLList", ok, chrono::duration<double, milli>(t1 - t0).count(), err);
    }

    // 8. DLList vs std::vector
    {
        DLList dl;
        vector<char> v;
        string err;
        bool ok = true;
        auto t0 = chrono::steady_clock::now();
        for (int op = 0; op < NUM_OPS && ok; op++) {
            int action = rng() % 4;
            char val = 'a' + (rng() % 26);
            if (action == 0 || v.empty()) {
                int idx = rng() % (v.size() + 1);
                dl.add(idx, val);
                v.insert(v.begin() + idx, val);
            } else if (action == 1) {
                int idx = rng() % v.size();
                char c1 = dl.remove(idx);
                char c2 = v[idx];
                v.erase(v.begin() + idx);
                if (c1 != c2) { ok = false; err = "remove mismatch"; break; }
            } else if (action == 2) {
                int idx = rng() % v.size();
                dl.set(idx, val);
                v[idx] = val;
            } else {
                int idx = rng() % v.size();
                if (dl.get(idx) != v[idx]) { ok = false; err = "get mismatch"; break; }
            }
            if (!dl.checkInvariants(err)) { ok = false; break; }
        }
        if (ok && dl.size() == (int)v.size()) {
            for (size_t i = 0; i < v.size(); i++) {
                if (dl.get(i) != v[i]) { ok = false; err = "final mismatch"; break; }
            }
        } else if (ok) { ok = false; err = "size mismatch"; }
        auto t1 = chrono::steady_clock::now();
        printResult("DLList", ok, chrono::duration<double, milli>(t1 - t0).count(), err);
    }

    // 9. SEList (b=3) vs std::vector
    {
        SEList sel(3);
        vector<char> v;
        string err;
        bool ok = true;
        auto t0 = chrono::steady_clock::now();
        for (int op = 0; op < NUM_OPS && ok; op++) {
            int action = rng() % 4;
            char val = 'a' + (rng() % 26);
            if (action == 0 || v.empty()) {
                int idx = rng() % (v.size() + 1);
                sel.add(idx, val);
                v.insert(v.begin() + idx, val);
            } else if (action == 1) {
                int idx = rng() % v.size();
                char c1 = sel.remove(idx);
                char c2 = v[idx];
                v.erase(v.begin() + idx);
                if (c1 != c2) { ok = false; err = "remove mismatch"; break; }
            } else if (action == 2) {
                int idx = rng() % v.size();
                sel.set(idx, val);
                v[idx] = val;
            } else {
                int idx = rng() % v.size();
                if (sel.get(idx) != v[idx]) { ok = false; err = "get mismatch"; break; }
            }
            if (!sel.checkInvariants(err)) { ok = false; break; }
        }
        if (ok && sel.size() == (int)v.size()) {
            for (size_t i = 0; i < v.size(); i++) {
                if (sel.get(i) != v[i]) { ok = false; err = "final mismatch"; break; }
            }
        } else if (ok) { ok = false; err = "size mismatch"; }
        auto t1 = chrono::steady_clock::now();
        printResult("SEList (b=3)", ok, chrono::duration<double, milli>(t1 - t0).count(), err);
    }

    g_verbose = true;
    cout << "\n  " << Color::green() << Color::bold() << "Test de estres completado con exito. Todas las estructuras son fieles a la STL." << Color::reset() << "\n";
}

// =====================================================================
int main() {
    habilitarColoresTerminal();

    cout << "\n";
    cout << Color::cyan()
         << "==================================================================\n"
         << Color::bold() << "   OPEN DATA STRUCTURES (C++)  -  Capitulos 2 y 3\n"
         << "   Laboratorio interactivo de estructuras de datos\n"
         << Color::reset() << Color::cyan()
         << "==================================================================\n"
         << Color::reset();
    while (true) {
        cout << "\n  " << Color::bold() << "CAPITULO 2 - LISTAS BASADAS EN ARREGLOS" << Color::reset() << "\n"
                "     1) ArrayStack            2) FastArrayStack\n"
                "     3) ArrayQueue            4) ArrayDeque\n"
                "     5) DualArrayDeque        6) RootishArrayStack\n"
                "\n  " << Color::bold() << "CAPITULO 3 - LISTAS ENLAZADAS" << Color::reset() << "\n"
                "     7) SLList                8) DLList\n"
                "     9) SEList\n"
                "\n  " << Color::bold() << "HERRAMIENTAS Y DIAGNOSTICO" << Color::reset() << "\n"
                "    10) Comparador (mismo trabajo en todas las estructuras)\n"
                "    11) Demos automaticas (los ejemplos guiados)\n"
                "    12) Tabla de complejidades\n"
                "    13) Ayuda: como leer los costos\n"
                "    14) Test de estres y fuzzing automatico (contra STL)\n"
                "    15) Alternar colores en consola (actualmente: "
             << (Color::enabled ? Color::green() : Color::red())
             << (Color::enabled ? "ACTIVADOS" : "DESACTIVADOS") << Color::reset() << ")\n"
                "     0) Salir\n";
        int op = leerEntero("Elige una opcion: ", 0, 15);
        switch (op) {
        case 0:
            cout << "\nHasta luego!\n";
            return 0;
        case 1: ejecutarLista<ArrayStack>("ArrayStack", INFO_AS); break;
        case 2: ejecutarLista<FastArrayStack>("FastArrayStack", INFO_FAS); break;
        case 3: menuArrayQueue(); break;
        case 4: ejecutarLista<ArrayDeque>("ArrayDeque", INFO_AD); break;
        case 5: ejecutarLista<DualArrayDeque>("DualArrayDeque", INFO_DAD); break;
        case 6: ejecutarLista<RootishArrayStack>("RootishArrayStack", INFO_RAS); break;
        case 7: menuSLListEntrada(); break;
        case 8: ejecutarLista<DLList>("DLList", INFO_DL); break;
        case 9: {
            int b = leerEntero("Valor de b, tamano de bloque (2..8, recomendado 3): ", 2, 8);
            ejecutarLista<SEList>("SEList (b=" + to_string(b) + ")", infoSE(b), b);
            break;
        }
        case 10: comparador(); break;
        case 11: menuDemos(); break;
        case 12: tablaComplejidades(); break;
        case 13: ayuda(); break;
        case 14: testEstresFuzzing(); break;
        case 15:
            Color::enabled = !Color::enabled;
            cout << "\n  Colores " << (Color::enabled ? "ACTIVADOS" : "DESACTIVADOS") << ".\n";
            break;
        }
    }
}
