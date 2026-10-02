/*
 * main.cpp
 *
 * Menú interactivo para probar RootishArrayStack.
 */

 // g++ -std=c++17 -Wall -Wextra -O2 main.cpp -o rootish.exe

#include <iostream>
#include <string>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <sstream>

#include "RootishArrayStack.h"

using namespace std;
using namespace ods;

// Cambia este typedef si quieres probar con otro tipo (double, string, etc.)
typedef int T;

// ---------- Helpers de entrada ----------

static void limpiar() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

static T pedirValor(const string& msg) {
    T v;
    while (true) {
        cout << msg;
        if (cin >> v) { limpiar(); return v; }
        cout << "  ⚠ Entrada inválida. Intenta de nuevo.\n";
        cin.clear();
        limpiar();
    }
}

static int pedirEntero(const string& msg) {
    int v;
    while (true) {
        cout << msg;
        if (cin >> v) { limpiar(); return v; }
        cout << "  ⚠ Entrada inválida. Intenta de nuevo.\n";
        cin.clear();
        limpiar();
    }
}

// ---------- Impresión de contenido ----------

static void imprimirContenido(const RootishArrayStack<T>& s) {
    cout << "  [ ";
    for (int i = 0; i < s.size(); i++) {
        cout << s.get(i);
        if (i + 1 < s.size()) cout << ", ";
    }
    cout << " ]\n";
}

// ---------- Menú ----------

static void mostrarMenu() {
    cout << "\n";
    cout << "==========================================\n";
    cout << "     RootishArrayStack - Menú Principal   \n";
    cout << "==========================================\n";
    cout << " 1) push(x)              -> add(size(), x)\n";
    cout << " 2) add(i, x)            -> insertar en posición i\n";
    cout << " 3) get(i)               -> leer posición i\n";
    cout << " 4) set(i, x)            -> escribir posición i\n";
    cout << " 5) remove(i)            -> eliminar posición i\n";
    cout << " 6) size / empty         -> estado\n";
    cout << " 7) clear()              -> vaciar estructura\n";
    cout << " 8) print()              -> mostrar contenido\n";
    cout << " 9) structure()          -> mostrar bloques internos\n";
    cout << "10) fillRandom(N)        -> llenar con N aleatorios\n";
    cout << "11) test automático      -> comparar con std::vector\n";
    cout << " 0) salir\n";
    cout << "------------------------------------------\n";
    cout << " Opción: ";
}

// ---------- Acciones ----------

static void opPush(RootishArrayStack<T>& s) {
    T x = pedirValor("  Valor a apilar: ");
    s.add(x);
    cout << "  ✔ push(" << x << ") realizado.\n";
}

static void opAdd(RootishArrayStack<T>& s) {
    int i = pedirEntero("  Índice i: ");
    if (i < 0 || i > s.size()) {
        cout << "  ⚠ Índice fuera de rango [0, " << s.size() << "].\n";
        return;
    }
    T x = pedirValor("  Valor x: ");
    s.add(i, x);
    cout << "  ✔ add(" << i << ", " << x << ") realizado.\n";
}

static void opGet(const RootishArrayStack<T>& s) {
    if (s.empty()) { cout << "  ⚠ Estructura vacía.\n"; return; }
    int i = pedirEntero("  Índice i: ");
    if (i < 0 || i >= s.size()) {
        cout << "  ⚠ Índice fuera de rango [0, " << s.size() - 1 << "].\n";
        return;
    }
    cout << "  → s[" << i << "] = " << s.get(i) << "\n";
}

static void opSet(RootishArrayStack<T>& s) {
    if (s.empty()) { cout << "  ⚠ Estructura vacía.\n"; return; }
    int i = pedirEntero("  Índice i: ");
    if (i < 0 || i >= s.size()) {
        cout << "  ⚠ Índice fuera de rango [0, " << s.size() - 1 << "].\n";
        return;
    }
    T x = pedirValor("  Nuevo valor: ");
    T old = s.set(i, x);
    cout << "  ✔ s[" << i << "] = " << old << " → " << x << "\n";
}

static void opRemove(RootishArrayStack<T>& s) {
    if (s.empty()) { cout << "  ⚠ Estructura vacía.\n"; return; }
    int i = pedirEntero("  Índice i: ");
    if (i < 0 || i >= s.size()) {
        cout << "  ⚠ Índice fuera de rango [0, " << s.size() - 1 << "].\n";
        return;
    }
    T x = s.remove(i);
    cout << "  ✔ remove(" << i << ") = " << x << "\n";
}

static void opStatus(const RootishArrayStack<T>& s) {
    cout << "  size()  = " << s.size() << "\n";
    cout << "  empty() = " << (s.empty() ? "true" : "false") << "\n";
    cout << "  bloques = " << s.numBlocks() << "\n";
}

static void opClear(RootishArrayStack<T>& s) {
    s.clear();
    cout << "  ✔ Estructura vaciada.\n";
}

static void opFillRandom(RootishArrayStack<T>& s) {
    int n = pedirEntero("  ¿Cuántos elementos aleatorios? ");
    if (n <= 0) { cout << "  ⚠ N debe ser > 0.\n"; return; }
    for (int i = 0; i < n; i++) {
        T v = (T)(rand() % 100);
        s.add(v);
    }
    cout << "  ✔ Se agregaron " << n << " elementos aleatorios.\n";
}

// Prueba de estrés: compara contra std::vector
static void opTest(RootishArrayStack<T>& s) {
    cout << "  Ejecutando prueba automática (1000 operaciones)...\n";
    s.clear();
    vector<T> ref;
    int errores = 0;

    for (int k = 0; k < 1000; k++) {
        int op = rand() % 4;
        if (op == 0 || ref.empty()) {
            // add al final
            T v = (T)(rand() % 1000);
            s.add(v);
            ref.push_back(v);
        } else if (op == 1) {
            // add en posición aleatoria
            int i = rand() % (ref.size() + 1);
            T v = (T)(rand() % 1000);
            s.add(i, v);
            ref.insert(ref.begin() + i, v);
        } else if (op == 2) {
            // remove
            int i = rand() % ref.size();
            T got = s.remove(i);
            T exp = ref[i];
            if (got != exp) errores++;
            ref.erase(ref.begin() + i);
        } else {
            // set
            int i = rand() % ref.size();
            T v = (T)(rand() % 1000);
            s.set(i, v);
            ref[i] = v;
        }

        // Verificación completa
        if ((int)ref.size() != s.size()) { errores++; break; }
        for (size_t i = 0; i < ref.size(); i++) {
            if (s.get((int)i) != ref[i]) { errores++; break; }
        }
        if (errores) break;
    }

    if (errores == 0) cout << "  ✔ Prueba superada sin errores.\n";
    else              cout << "  ✘ Se detectaron " << errores << " errores.\n";

    cout << "  Estado final: size=" << s.size()
         << ", bloques=" << s.numBlocks() << "\n";
}

// ---------- Main ----------

int main() {
    srand((unsigned)time(nullptr));

    RootishArrayStack<T> stack;
    int opcion;

    cout << "==========================================\n";
    cout << "  RootishArrayStack - Demo interactiva    \n";
    cout << "  (tipo T = int; edita el typedef para   \n";
    cout << "   probar con otros tipos)               \n";
    cout << "==========================================\n";

    do {
        mostrarMenu();
        if (!(cin >> opcion)) {
            cin.clear();
            limpiar();
            opcion = -1;
        } else {
            limpiar();
        }

        switch (opcion) {
            case 1:  opPush(stack);          break;
            case 2:  opAdd(stack);           break;
            case 3:  opGet(stack);           break;
            case 4:  opSet(stack);           break;
            case 5:  opRemove(stack);        break;
            case 6:  opStatus(stack);        break;
            case 7:  opClear(stack);         break;
            case 8:
                cout << "  Contenido: ";
                imprimirContenido(stack);
                break;
            case 9:
                cout << "  Estructura interna:\n";
                stack.printStructure();
                break;
            case 10: opFillRandom(stack);    break;
            case 11: opTest(stack);          break;
            case 0:
                cout << "\n  ¡Hasta luego! 👋\n";
                break;
            default:
                cout << "  ⚠ Opción inválida.\n";
        }
    } while (opcion != 0);

    return 0;
}