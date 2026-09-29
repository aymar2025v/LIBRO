
# Capítulo 3

## Listas Enlazadas

En este capítulo, continuamos estudiando implementaciones de la interfaz `List`, esta vez utilizando estructuras de datos basadas en punteros en lugar de arreglos. Las estructuras de este capítulo están formadas por nodos que contienen los elementos de la lista. Utilizando referencias (punteros), los nodos se enlazan entre sí formando una secuencia. Primero estudiamos las listas simplemente enlazadas, que pueden implementar operaciones de Pila (*Stack*) y Cola (FIFO) en tiempo constante por operación, y luego pasamos a las listas doblemente enlazadas, que pueden implementar operaciones de Deque en tiempo constante.

Las listas enlazadas tienen ventajas y desventajas en comparación con las implementaciones basadas en arreglos de la interfaz `List`. La principal desventaja es que perdemos la capacidad de acceder a cualquier elemento utilizando `get(i)` o `set(i,x)` en tiempo constante. En su lugar, tenemos que recorrer la lista, un elemento a la vez, hasta llegar al i-ésimo elemento. La principal ventaja es que son más dinámicas: Dada una referencia a cualquier nodo `u` de la lista, podemos eliminar `u` o insertar un nodo adyacente a `u` en tiempo constante. Esto es cierto sin importar dónde se encuentre `u` en la lista.

### 3.1 SLList: Una Lista Simplemente Enlazada

Una `SLList` (lista simplemente enlazada) es una secuencia de `Node`s (nodos). Cada nodo `u` almacena un valor de dato `u.x` y una referencia `u.next` al siguiente nodo en la secuencia. Para el último nodo `w` en la secuencia, `w.next = null`

![xddd](/Screenshot%202026-09-17%20172105.png)

**Figura 3.1:** Una secuencia de operaciones de Cola (`add(x)` y `remove()`) y Pila (`push(x)` y `pop()`) en una `SLList`.

**SLList**

```cpp
class Node {
public:
    T x;
    Node *next;
    Node(T x0) {
        x = x0;
        next = NULL;
    }
};
```

Por eficiencia, una `SLList` utiliza las variables `head` y `tail` para llevar un registro del primer y último nodo en la secuencia, así como un entero `n` para llevar un registro de la longitud de la secuencia:

**SLList**

```cpp
Node *head;
Node *tail;
int n;
```


Una secuencia de operaciones de Pila y Cola en una `SLList` se ilustra en la Figura 3.1.

Una `SLList` puede implementar eficientemente las operaciones de Pila `push()` y `pop()` agregando y eliminando elementos en la cabeza de la secuencia. La operación `push()` simplemente crea un nuevo nodo `u` con valor de dato `x`, establece `u.next` en la antigua cabeza de la lista y hace que `u` sea la nueva cabeza de la lista. Finalmente, incrementa `n` ya que el tamaño de la `SLList` ha aumentado en uno:

**SLList**

```cpp
T push(T x) {
    Node *u = new Node(x);
    u->next = head;
    head = u;
    if (n == 0)
        tail = u;
    n++;
    return x;
}
```

La operación `pop()`, después de verificar que la `SLList` no está vacía, elimina la cabeza estableciendo `head = head.next` y decrementando `n`. Un caso especial ocurre cuando se está eliminando el último elemento, en cuyo caso `tail` se establece en `null`:

```cpp
T pop() {
    if (n == 0) return null;
    T x = head->x;
    Node *u = head;
    head = head->next;
    delete u;
    if (--n == 0) tail = NULL;
    return x;
}
```

Claramente, tanto las operaciones `push(x)` como `pop()` se ejecutan en tiempo $O(1)$.

### 3.1.1 Operaciones de Cola

Una `SLList` también puede implementar las operaciones de cola FIFO `add(x)` y `remove()` en tiempo constante. Las eliminaciones se realizan desde la cabeza de la lista, y son idénticas a la operación `pop()`:

**SLList**

```cpp
T remove() {
    if (n == 0) return null;
    T x = head->x;
    Node *u = head;
    head = head->next;
    delete u;
    if (--n == 0) tail = NULL;
    return x;
}
```

Las adiciones, por otro lado, se realizan al final de la lista. En la mayoría de los casos, esto se hace estableciendo `tail.next = u`, donde `u` es el nodo recién creado que contiene `x`. Sin embargo, ocurre un caso especial cuando `n = 0`, en cuyo caso `tail = head = null`. En este caso, tanto `tail` como `head` se establecen en `u`.

**SLList**

```cpp
bool add(T x) {
    Node *u = new Node(x);
    if (n == 0) {
        head = u;
    } else {
        tail->next = u;
    }
    tail = u;
    n++;
    return true;
}
```

Claramente, tanto `add(x)` como `remove()` toman tiempo constante.

### 3.1.2 Resumen

El siguiente teorema resume el rendimiento de una `SLList`:

**Teorema 3.1.** *Una `SLList` implementa las interfaces de Pila (`Stack`) y Cola (FIFO). Las operaciones `push(x)`, `pop()`, `add(x)` y `remove()` se ejecutan en tiempo $O(1)$ por operación.*

Una `SLList` casi implementa el conjunto completo de operaciones de Deque. La única operación que falta es la eliminación desde la cola de una `SLList`. Eliminar desde la cola de una `SLList` es difícil porque requiere actualizar el valor de `tail` para que apunte al nodo `w` que precede a `tail` en la `SLList`; este es el nodo `w` tal que `w.next = tail`. Desafortunadamente, la única forma de llegar a `w` es recorriendo la `SLList` comenzando en `head` y dando $n - 2$ pasos.

## 3.2 DLList: Una Lista Doblemente Enlazada

Una `DLList` (lista doblemente enlazada) es muy similar a una `SLList`, excepto que cada nodo `u` en una `DLList` tiene referencias tanto al nodo `u.next` que le sigue como al nodo `u.prev` que le precede.

**DLList**

```cpp
struct Node {
    T x;
    Node *prev, *next;
};
```

Al implementar una `SLList`, vimos que siempre había varios casos especiales de los que preocuparse. Por ejemplo, eliminar el último elemento de una `SLList` o agregar un elemento a una `SLList` vacía requiere tener cuidado para asegurar que `head` y `tail` se actualicen correctamente. En una `DLList`, el número de estos casos especiales aumenta considerablemente. Quizás la forma más limpia de manejar todos estos casos especiales en una `DLList` es introducir un nodo `dummy` (ficticio). Este es un nodo que no contiene ningún dato, sino que actúa como un marcador de posición para que no haya nodos especiales; cada nodo tiene tanto un `next` como un `prev`, con `dummy` actuando como el nodo que sigue al último nodo de la lista y que precede al primer nodo de la lista. De esta manera, los nodos de la lista están (doblemente) enlazados formando un ciclo, como se ilustra en la Figura 3.2.

**DLList**

```cpp
Node dummy;
int n;
DLList() {
    dummy.next = &dummy;
    dummy.prev = &dummy;
    n = 0;
```


![ihh](/Screenshot%202026-09-17%20173353.png)

```cpp
}
```

Encontrar el nodo con un índice particular en una `DLList` es fácil; podemos comenzar en la cabeza de la lista (`dummy.next`) y avanzar, o comenzar en la cola de la lista (`dummy.prev`) y retroceder. Esto nos permite alcanzar el i-ésimo nodo en tiempo $O(1 + \min\{i, n-i\})$:

**DLList**

```cpp
Node* getNode(int i) {
    Node* p;
    if (i < n / 2) {
        p = dummy.next;
        for (int j = 0; j < i; j++)
            p = p->next;
    } else {
        p = &dummy;
        for (int j = n; j > i; j--)
            p = p->prev;
    }
    return (p);
}
```

Las operaciones `get(i)` y `set(i,x)` ahora también son sencillas. Primero encontramos el i-ésimo nodo y luego obtenemos o establecemos su valor `x`:

**DLList**

```cpp
T get(int i) {
    return getNode(i)->x;
}
T set(int i, T x) {
    Node* u = getNode(i);
```
![kkk](/Screenshot%202026-09-17%20173641.png)
```
    T y = u->x;
    u->x = x;
    return y;
}
```

El tiempo de ejecución de estas operaciones está dominado por el tiempo que toma encontrar el i-ésimo nodo, y por lo tanto es $O(1 + \min\{i, n - i\})$.

### 3.2.1 Agregar y Eliminar

Si tenemos una referencia a un nodo `w` en una `DLList` y queremos insertar un nodo `u` antes de `w`, entonces esto es solo cuestión de establecer `u.next = w`, `u.prev = w.prev`, y luego ajustar `u.prev.next` y `u.next.prev`. (Véase la Figura 3.3.) Gracias al nodo ficticio (`dummy`), no hay necesidad de preocuparse por si `w.prev` o `w.next` no existen.

**DLList**

```cpp
Node* addBefore(Node *w, T x) {
    Node *u = new Node;
    u->x = x;
    u->prev = w->prev;
    u->next = w;
    u->next->prev = u;
    u->prev->next = u;
    n++;
    return u;
}
```

Ahora, la operación de lista `add(i,x)` es trivial de implementar. Encontramos el i-ésimo nodo en la `DLList` e insertamos un nuevo nodo `u` que contiene `x` justo antes de él.

**DLList**

```cpp
void add(int i, T x) {
    addBefore(getNode(i), x);
}
```

La única parte no constante del tiempo de ejecución de `add(i, x)` es el tiempo que toma encontrar el i-ésimo nodo (usando `getNode(i)`). Por lo tanto, `add(i, x)` se ejecuta en tiempo $O(1 + \min\{i, n - i\})$.

Eliminar un nodo `w` de una `DLList` es fácil. Solo necesitamos ajustar los punteros en `w.next` y `w.prev` para que se salten `w`. De nuevo, el uso del nodo ficticio (`dummy`) elimina la necesidad de considerar cualquier caso especial:

**DLList**

```cpp
void remove(Node *w) {
    w->prev->next = w->next;
    w->next->prev = w->prev;
    delete w;
    n--;
}
```

Ahora la operación `remove(i)` es trivial. Encontramos el nodo con índice `i` y lo eliminamos:

**DLList**

```cpp
T remove(int i) {
    Node *w = getNode(i);
    T x = w->x;
    remove(w);
    return x;
}
```

Nuevamente, la única parte costosa de esta operación es encontrar el i-ésimo nodo usando `getNode(i)`, por lo que `remove(i)` se ejecuta en tiempo $O(1 + \min\{i, n - i\})$.

### 3.2.2 Resumen

El siguiente teorema resume el rendimiento de una `DLList`:


**Teorema 3.2.** *Una `DLList` implementa la interfaz `List`. En esta implementación, las operaciones `get(i)`, `set(i, x)`, `add(i, x)` y `remove(i)` se ejecutan en tiempo $O(1 + \min\{i, n - i\})$ por operación.*

Vale la pena señalar que, si ignoramos el costo de la operación `getNode(i)`, entonces todas las operaciones en una `DLList` toman tiempo constante. Por lo tanto, la única parte costosa de las operaciones en una `DLList` es encontrar el nodo relevante. Una vez que tenemos el nodo relevante, agregar, eliminar o acceder a los datos en ese nodo toma solo tiempo constante.

Esto está en marcado contraste con las implementaciones de `List` basadas en arreglos del Capítulo 2; en esas implementaciones, el elemento relevante del arreglo se puede encontrar en tiempo constante. Sin embargo, la adición o eliminación requiere desplazar elementos en el arreglo y, en general, toma un tiempo no constante.

Por esta razón, las estructuras de listas enlazadas son adecuadas para aplicaciones donde se pueden obtener referencias a los nodos de la lista a través de medios externos. Por ejemplo, los punteros a los nodos de una lista enlazada podrían almacenarse en un `USet`. Entonces, para eliminar un elemento `x` de la lista enlazada, el nodo que contiene `x` se puede encontrar rápidamente usando el `USet` y el nodo se puede eliminar de la lista en tiempo constante.

## 3.3 SEList: Una Lista Enlazada Eficiente en Espacio

Uno de los inconvenientes de las listas enlazadas (además del tiempo que toma acceder a elementos que están en lo profundo de la lista) es su uso del espacio. Cada nodo en una `DLList` requiere dos referencias adicionales a los nodos siguiente y anterior en la lista. ¡Dos de los campos en un `Node` están dedicados a mantener la lista, y solo uno de los campos es para almacenar datos!

Una `SEList` (lista eficiente en espacio) reduce este espacio desperdiciado utilizando una idea simple: En lugar de almacenar elementos individuales en una `DLList`, almacenamos un bloque (arreglo) que contiene varios elementos. Más precisamente, una `SEList` está parametrizada por un tamaño de bloque `b`. Cada nodo individual en una `SEList` almacena un bloque que puede contener hasta `b + 1` elementos.

Por razones que se aclararán más adelante, será útil si podemos realizar operaciones de Deque en cada bloque. La estructura de datos que elegimos para esto es un `BDeque` (deque acotado), derivado de la estructura `ArrayDeque` descrita en la Sección 2.4. El `BDeque` difiere del `ArrayDeque` en un pequeño aspecto: Cuando se crea un nuevo `BDeque`, el tamaño del arreglo de respaldo `a` se fija en `b + 1` y nunca crece ni se contrae. La propiedad importante de un `BDeque` es que permite la adición o eliminación de elementos tanto al frente como al final en tiempo constante. Esto será útil a medida que los elementos se desplazan de un bloque a otro.

**SEList**

```cpp
class BDeque : public ArrayDeque<T> {
public:
    BDeque(int b) {
        n = 0;
        j = 0;
        array<int> z(b+1);
        a = z;
    }
    ~BDeque() { }
    // Pregunta de C++: ¿Por qué es esto necesario?
    void add(int i, T x) {
        ArrayDeque<T>::add(i, x);
    }
    bool add(T x) {
        ArrayDeque<T>::add(size(), x);
        return true;
    }
    void resize() { }
};
```

Una SEList es entonces una lista doblemente enlazada de bloques:

**SEList**

```cpp
class Node {
public:
    BDeque d;
    Node *prev, *next;
    Node(int b) : d(b) { }
};
```

**SEList**

```cpp
int n;
Node dummy;
```

### 3.3.1 Requisitos de Espacio

Una `SEList` impone restricciones muy estrictas sobre el número de elementos en un bloque: A menos que un bloque sea el último bloque, entonces ese bloque contiene al menos `b - 1` y como máximo `b + 1` elementos. Esto significa que, si una `SEList` contiene `n` elementos, entonces tiene como máximo

$$
n/(b-1) + 1 = O(n/b)
$$

bloques. El `BDeque` para cada bloque contiene un arreglo de longitud `b + 1`, pero para cada bloque excepto el último, se desperdicia a lo sumo una cantidad constante de espacio en este arreglo. La memoria restante utilizada por un bloque también es constante. Esto significa que el espacio desperdiciado en una `SEList` es solo $O(b + n/b)$. Al elegir un valor de `b` dentro de un factor constante de $\sqrt{n}$, podemos hacer que el exceso de espacio de una `SEList` se aproxime a la cota inferior de $\sqrt{n}$ dada en la Sección 2.6.2.

### 3.3.2 Encontrar Elementos

El primer desafío que enfrentamos con una `SEList` es encontrar el elemento de la lista con un índice `i` dado. Tenga en cuenta que la ubicación de un elemento consta de dos partes:

1.  El nodo `u` que contiene el bloque que contiene el elemento con índice `i`; y
2.  El índice `j` del elemento dentro de su bloque.

**SEList**

```cpp
class Location {
public:
    Node *u;
    int j;
    Location() { }
    Location(Node *u, int j) {
        this->u = u;
        this->j = j;
    }
};
```

Para encontrar el bloque que contiene un elemento en particular, procedemos de la misma manera que lo hacemos en una `DLList`. O comenzamos al frente de la lista y recorremos en dirección hacia adelante, o en la parte posterior de la lista y recorremos hacia atrás hasta que llegamos al nodo que queremos. La única diferencia es que, cada vez que pasamos de un nodo al siguiente, saltamos un bloque completo de elementos.

**SEList**

```cpp
void getLocation(int i, Location &ell) {
    if (i < n / 2) {
        Node *u = dummy.next;
        while (i >= u->d.size()) {
            i -= u->d.size();
            u = u->next;
        }
        ell.u = u;
        ell.j = i;
    } else {
        Node *u = &dummy;
        int idx = n;
        while (i < idx) {
            u = u->prev;
            idx -= u->d.size();
        }
        ell.u = u;
        ell.j = i - idx;
    }
}
```

Recuerde que, con la excepción de como máximo un bloque, cada bloque contiene al menos $b - 1$ elementos, por lo que cada paso en nuestra búsqueda nos acerca $b - 1$ elementos al elemento que estamos buscando. Si buscamos hacia adelante, esto significa que llegamos al nodo que queremos después de $O(1 + i/b)$ pasos. Si buscamos hacia atrás, entonces llegamos al nodo que queremos después de $O(1 + (n - i)/b)$ pasos. El algoritmo toma la menor de estas dos cantidades dependiendo del valor de `i`, por lo que el tiempo para localizar el elemento con índice `i` es $O(1 + \min\{i, n - i\}/b)$.

Una vez que sabemos cómo localizar el elemento con índice `i`, las operaciones `get(i)` y `set(i,x)` se traducen en obtener o establecer un índice particular en el bloque correcto:

**SEList**

```cpp
T get(int i) {
    Location l;
    getLocation(i, l);
    return l.u->d.get(l.j);
}
T set(int i, T x) {
    Location l;
    getLocation(i, l);
    T y = l.u->d.get(l.j);
    l.u->d.set(l.j, x);
    return y;
}
```

Los tiempos de ejecución de estas operaciones están dominados por el tiempo que toma localizar el elemento, por lo que también se ejecutan en tiempo $O(1 + \min\{i, n - i\}/b)$.

### 3.3.3 Agregar un Elemento

Agregar elementos a una `SEList` es un poco más complicado. Antes de considerar el caso general, consideramos la operación más fácil, `add(x)`, en la cual `x` se agrega al final de la lista. Si el último bloque está lleno (o no existe porque todavía no hay bloques), entonces primero asignamos un nuevo bloque y lo añadimos a la lista de bloques. Ahora que estamos seguros de que el último bloque existe y no está lleno, agregamos `x` al último bloque.

```cpp
void add(T x) {
    Node *last = dummy.prev;
    if (last == &dummy || last->d.size() == b+1) {
        last = addBefore(&dummy);
    }
    last->d.add(x);
    n++;
}
```

Las cosas se complican más cuando agregamos al interior de la lista usando `add(i, x)`. Primero localizamos `i` para obtener el nodo `u` cuyo bloque contiene

![ddd](/Screenshot%202026-09-17%20175536.png)

el i-ésimo elemento de la lista. El problema es que queremos insertar `x` en el bloque de `u`, pero tenemos que estar preparados para el caso en que el bloque de `u` ya contenga `b+1` elementos, de modo que esté lleno y no haya espacio para `x`.

Sean $u_0, u_1, u_2, ...$ denotan `u`, `u.next`, `u.next.next`, y así sucesivamente. Exploramos $u_0, u_1, u_2, ...$ buscando un nodo que pueda proporcionar espacio para `x`. Pueden ocurrir tres casos durante nuestra exploración de espacio (véase la Figura 3.4):

1.  Encontramos rápidamente (en $r+1 \le b$ pasos) un nodo $u_r$ cuyo bloque no está lleno. En este caso, realizamos $r$ desplazamientos de un elemento de un bloque al siguiente, de modo que el espacio libre en $u_r$ se convierte en un espacio libre en $u_0$. Luego podemos insertar `x` en el bloque de $u_0$.
2.  Rápidamente (en $r+1 \le b$ pasos) llegamos al final de la lista de bloques. En este caso, agregamos un nuevo bloque vacío al final de la lista de bloques y procedemos como en el primer caso.
3.  Después de $b$ pasos no encontramos ningún bloque que no esté lleno. En este caso, $u_0, ..., u_{b-1}$ es una secuencia de $b$ bloques que contienen $b+1$ elementos cada uno. Insertamos un nuevo bloque $u_b$ al final de esta secuencia y distribuimos los $b(b+1)$ elementos originales de modo que cada bloque de $u_0, ..., u_b$ contenga exactamente $b$ elementos. Ahora el bloque de $u_0$ contiene solo $b$ elementos, por lo que tiene espacio para que insertemos `x`.

**SEList**

```cpp
void add(int i, T x) {
    if (i == n) {
        add(x);
        return;
    }
    Location l; getLocation(i, l);
    Node *u = l.u;
    int r = 0;
    while (r < b && u != &dummy && u->d.size() == b+1) {
        u = u->next;
        r++;
    }
    if (r == b) { // b bloques, cada uno con b+1 elementos
        spread(l.u);
        u = l.u;
    }
    if (u == &dummy) { // se salió del final - agregar nuevo nodo
        u = addBefore(u);
    }
    while (u != l.u) { // trabajar hacia atrás, desplazando elementos
        u->d.add(0, u->prev->d.remove(u->prev->d.size()-1));
        u = u->prev;
    }
    u->d.add(l.j, x);
    n++;
}
```

El tiempo de ejecución de la operación `add(i,x)` depende de cuál de los tres casos anteriores ocurra. Los casos 1 y 2 implican examinar y desplazar elementos a través de como máximo `b` bloques y toman tiempo $O(b)$. El caso 3 implica llamar al método `spread(u)`, el cual mueve $b(b + 1)$ elementos y toma tiempo $O(b^2)$. Si ignoramos el costo del Caso 3 (el cual contabilizaremos más adelante con amortización), esto significa que el tiempo de ejecución total para localizar `i` y realizar la inserción de `x` es $O(b + \min\{i, n - i\}/b)$.

![www](/Screenshot%202026-09-17%20175912.png)

### 3.3.4 Eliminación de un Elemento

Eliminar un elemento de una `SEList` es similar a agregar un elemento. Primero localizamos el nodo `u` que contiene el elemento con índice `i`. Ahora, tenemos que estar preparados para el caso en el que no podemos eliminar un elemento de `u` sin causar que el bloque de `u` se vuelva más pequeño que $b - 1$.

De nuevo, sean $u_0, u_1, u_2, ...$ que denotan `u`, `u.next`, `u.next.next`, y así sucesivamente. Examinamos $u_0, u_1, u_2, ...$ en orden para buscar un nodo del cual podamos tomar prestado un elemento para hacer que el tamaño del bloque de $u_0$ sea al menos $b - 1$. Hay tres casos a considerar (véase la Figura 3.5):

1.  Encontramos rápidamente (en $r + 1 \le b$ pasos) un nodo cuyo bloque contiene más de $b - 1$ elementos. En este caso, realizamos $r$ desplazamientos de un elemento de un bloque al anterior, de modo que el elemento extra en $u_r$ se convierte en un elemento extra en $u_0$. Luego podemos eliminar el elemento apropiado del bloque de $u_0$.
2.  Rápidamente (en $r + 1 \le b$ pasos) llegamos al final de la lista de bloques. En este caso, $u_r$ es el último bloque, y no hay necesidad de que el bloque de $u_r$ contenga al menos $b - 1$ elementos. Por lo tanto, procedemos como se indicó anteriormente, tomando prestado un elemento de $u_r$ para hacer un elemento extra en $u_0$. Si esto causa que el bloque de $u_r$ se vuelva vacío, entonces lo eliminamos.

3.  Después de `b` pasos, no encontramos ningún bloque que contenga más de `b - 1` elementos. En este caso, $u_0, ..., u_{b-1}$ es una secuencia de `b` bloques que contienen cada uno `b - 1` elementos. Reunimos estos $b(b-1)$ elementos en $u_0, ..., u_{b-2}$ de modo que cada uno de estos `b - 1` bloques contenga exactamente `b` elementos y eliminamos $u_{b-1}$, que ahora está vacío. Ahora el bloque de $u_0$ contiene `b` elementos y podemos entonces eliminar el elemento apropiado de él.

**SEList**

```cpp
T remove(int i) {
    Location l; getLocation(i, l);
    T y = l.u->d.get(l.j);
    Node *u = l.u;
    int r = 0;
    while (r < b && u != &dummy && u->d.size() == b - 1) {
        u = u->next;
        r++;
    }
    if (r == b) { // se encontraron b bloques, cada uno con b-1 elementos
        gather(l.u);
    }
    u = l.u;
    u->d.remove(l.j);
    while (u->d.size() < b - 1 && u->next != &dummy) {
        u->d.add(u->next->d.remove(0));
        u = u->next;
    }
    if (u->d.size() == 0)
        remove(u);
    n--;
    return y;
}
```

Al igual que la operación `add(i,x)`, el tiempo de ejecución de la operación `remove(i)` es $O(b + \min\{i, n - i\}/b)$ si ignoramos el costo del método `gather(u)` que ocurre en el Caso 3.

### 3.3.5 Análisis Amortizado de Dispersión y Recolección

A continuación, consideramos el costo de los métodos `gather(u)` y `spread(u)` que pueden ser ejecutados por los métodos `add(i,x)` y `remove(i)`. En aras de la exhaustividad, aquí están:

**SEList**

```cpp
void spread(Node *u) {
    Node *w = u;
    for (int j = 0; j < b; j++) {
        w = w->next;
    }
    w = addBefore(w);
    while (w != u) {
        while (w->d.size() < b)
            w->d.add(0, w->prev->d.remove(w->prev->d.size()-1));
        w = w->prev;
    }
}
```

**SEList**

```cpp
void gather(Node *u) {
    Node *w = u;
    for (int j = 0; j < b-1; j++) {
        while (w->d.size() < b)
            w->d.add(w->next->d.remove(0));
        w = w->next;
    }
    remove(w);
}
```

El tiempo de ejecución de cada uno de estos métodos está dominado por los dos bucles anidados. Tanto el bucle interno como el externo se ejecutan como máximo $b + 1$ veces, por lo que el tiempo de ejecución total de cada uno de estos métodos es $O((b + 1)^2) = O(b^2)$. Sin embargo, el siguiente lema muestra que estos métodos se ejecutan como máximo una de cada $b$ llamadas a `add(i,x)` o `remove(i)`.

**Lema 3.1.** *Si se crea una `SEList` vacía y se realiza cualquier secuencia de $m \ge 1$ llamadas a `add(i,x)` y `remove(i)`, entonces el tiempo total empleado durante todas las llamadas a `spread()` y `gather()` es $O(m)$.*

*Prueba.* Utilizaremos el método de análisis amortizado de potencial. Decimos que un nodo `u` es *frágil* si su bloque no contiene $b$ elementos (es decir, si es el último nodo, o contiene $b - 1$ o $b + 1$ elementos). Cualquier nodo cuyo bloque contiene $b$ elementos es *robusto*. Defina el potencial de una `SEList` como el número de nodos frágiles que contiene. Consideraremos solo la operación `add(i,x)` y su relación con el número de llamadas a `spread(u)`. El análisis de `remove(i)` y `gather(u)` es idéntico.

Observe que, si el Caso 1 ocurre durante `add(i,x)`, entonces solo un nodo, $u_i$, cambia el tamaño de su bloque. Por lo tanto, como máximo un nodo, a saber, $u_i$, pasa de ser robusto a frágil. Si ocurre el Caso 2, entonces se crea un nuevo nodo, y este nodo es frágil, pero ningún otro nodo cambia de tamaño, por lo que el número de nodos frágiles aumenta en uno. Así, en el Caso 1 o en el Caso 2 el potencial de la `SEList` aumenta como máximo en uno.

Finalmente, si ocurre el Caso 3, es porque $u_0, ..., u_{b-1}$ son todos bloques frágiles. Luego `spread(u0)` es llamado y estos $b$ nodos frágiles son reemplazados por $b + 1$ nodos robustos. Finalmente, se agrega `x` al bloque de $u_0$, haciendo que $u_0$ sea frágil. En total, el potencial disminuye en $b - 1$.

En resumen, el potencial comienza en 0 (no hay nodos en la lista). Cada vez que ocurre el Caso 1 o el Caso 2, el potencial aumenta como máximo en 1. Cada vez que ocurre el Caso 3, el potencial disminuye en $b - 1$. El potencial (que cuenta el número de nodos frágiles) nunca es menor que 0. Concluimos que, por cada ocurrencia del Caso 3, hay al menos $b - 1$ ocurrencias del Caso 1 o del Caso 2. Así, por cada llamada a `spread(u)` hay al menos $b$ llamadas a `add(i,x)`. Esto completa la prueba. □

### 3.3.6 Resumen

El siguiente teorema resume el rendimiento de la estructura de datos `SEList`:

**Teorema 3.3.** *Una `SEList` implementa la interfaz `List`. Ignorando el costo de las llamadas a `spread(u)` y `gather(u)`, una `SEList` con tamaño de bloque `b` soporta las operaciones*


*   `get(i)` y `set(i,x)` en tiempo $O(1 + \min\{i, n - i\}/b)$ por operación; y
*   `add(i,x)` y `remove(i)` en tiempo $O(b + \min\{i, n - i\}/b)$ por operación.

*Además, comenzando con una `SEList` vacía, cualquier secuencia de $m$ operaciones `add(i,x)` y `remove(i)` resulta en un total de $O(bm)$ tiempo empleado durante todas las llamadas a `spread(u)` y `gather(u)`.*

El espacio (medido en palabras)<sup>3</sup> utilizado por una `SEList` que almacena $n$ elementos es $n + O(b + n/b)$.

La `SEList` es un compromiso entre un `ArrayStack` y una `DLList`, donde la mezcla relativa de estas dos estructuras depende del tamaño de bloque `b`. En el extremo $b = 2$, cada nodo de la `SEList` almacena como máximo tres valores, lo que no es muy diferente de una `DLList`. En el otro extremo, $b > n$, todos los elementos se almacenan en un solo arreglo, al igual que en un `ArrayStack`. Entre estos dos extremos se encuentra un compromiso entre el tiempo que toma agregar o eliminar un elemento de la lista y el tiempo que toma localizar un elemento particular de la lista.

## 3.4 Discusión y Ejercicios

Tanto las listas simplemente enlazadas como las doblemente enlazadas son técnicas establecidas, que se han utilizado en programas durante más de 40 años. Son discutidas, por ejemplo, por Knuth [46, Secciones 2.2.3-2.2.5]. Incluso la estructura de datos `SEList` parece ser un ejercicio bien conocido de estructuras de datos. A la `SEList` a veces se la conoce como una lista enlazada desenrollada (*unrolled linked list*) [67].

Otra forma de ahorrar espacio en una lista doblemente enlazada es utilizar las llamadas listas XOR. En una lista XOR, cada nodo, `u`, contiene solo un puntero, llamado `u.nextprev`, que contiene el XOR bit a bit de `u.prev` y `u.next`. La lista en sí necesita almacenar dos punteros, uno al nodo ficticio (`dummy`) y otro a `dummy.next` (el primer nodo, o `dummy` si la lista está vacía). Esta técnica utiliza el hecho de que, si tenemos punteros a `u` y `u.prev`, entonces podemos extraer `u.next` usando la fórmula

$$
\text{u.next} = \text{u.prev} \text{ \^{} } \text{u.nextprev} .
$$


(Aquí `^` calcula el XOR bit a bit de sus dos argumentos.) Esta técnica complica un poco el código y no es posible en algunos lenguajes, como Java y Python, que tienen recolección de basura, pero proporciona una implementación de lista doblemente enlazada que requiere solo un puntero por nodo. Véase el artículo de revista de Sinha [68] para una discusión detallada de las listas XOR.

**Ejercicio 3.1.** ¿Por qué no es posible usar un nodo ficticio (`dummy`) en una `SLList` para evitar todos los casos especiales que ocurren en las operaciones `push(x)`, `pop()`, `add(x)` y `remove()`?

**Ejercicio 3.2.** Diseñe e implemente un método `SLList`, `secondLast()`, que devuelva el penúltimo elemento de una `SLList`. Haga esto sin usar la variable miembro, `n`, que lleva el registro del tamaño de la lista.

**Ejercicio 3.3.** Implemente las operaciones de `List` `get(i)`, `set(i,x)`, `add(i,x)` y `remove(i)` en una `SLList`. Cada una de estas operaciones debe ejecutarse en tiempo $O(1 + i)$.

**Ejercicio 3.4.** Diseñe e implemente un método `SLList`, `reverse()`, que invierta el orden de los elementos en una `SLList`. Este método debe ejecutarse en tiempo $O(n)$, no debe usar recursión, no debe usar estructuras de datos secundarias y no debe crear nuevos nodos.

**Ejercicio 3.5.** Diseñe e implemente métodos `SLList` y `DLList` llamados `checkSize()`. Estos métodos recorren la lista y cuentan el número de nodos para ver si esto coincide con el valor, `n`, almacenado en la lista. Estos métodos no devuelven nada, pero lanzan una excepción si el tamaño que calculan no coincide con el valor de `n`.

**Ejercicio 3.6.** Intente recrear el código para la operación `addBefore(w)` que crea un nodo, `u`, y lo agrega en una `DLList` justo antes del nodo `w`. No consulte este capítulo. Incluso si su código no coincide exactamente con el código dado en este libro, aún puede ser correcto. Pruébelo y vea si funciona.

Los siguientes ejercicios implican realizar manipulaciones en `DLLists`. Debe completarlos sin asignar nuevos nodos ni arreglos temporales. Todos pueden hacerse solo cambiando los valores `prev` y `next` de los nodos existentes.

**Ejercicio 3.7.** Escriba un método de `DLList` `isPalindrome()` que devuelva `true` si la lista es un palíndromo, es decir, el elemento en la posición `i` es igual al elemento en la posición `n - i - 1` para todo $i \in \{0, ..., n - 1\}$. Su código debe ejecutarse en tiempo $O(n)$.

**Ejercicio 3.8.** Implemente un método `rotate(r)` que "rote" una `DLList` de modo que el elemento de lista `i` se convierta en el elemento de lista `(i + r) mod n`. Este método debe ejecutarse en tiempo $O(1 + \min\{r, n - r\})$ y no debe modificar ningún nodo en la lista.

**Ejercicio 3.9.** Escriba un método, `truncate(i)`, que trunca una `DLList` en la posición `i`. Después de ejecutar este método, el tamaño de la lista será `i` y solo debería contener los elementos en los índices $0, ..., i - 1$. El valor de retorno es otra `DLList` que contiene los elementos en los índices $i, ..., n - 1$. Este método debe ejecutarse en tiempo $O(\min\{i, n - i\})$.

**Ejercicio 3.10.** Escriba un método de `DLList`, `absorb(l2)`, que toma como argumento una `DLList`, `l2`, la vacía y agrega su contenido, en orden, al receptor. Por ejemplo, si `l1` contiene `a, b, c` y `l2` contiene `d, e, f`, entonces después de llamar a `l1.absorb(l2)`, `l1` contendrá `a, b, c, d, e, f` y `l2` estará vacía.

**Ejercicio 3.11.** Escriba un método `deal()` que elimina todos los elementos con índices impares de una `DLList` y devuelve una `DLList` que contiene estos elementos. Por ejemplo, si `l1` contiene los elementos `a, b, c, d, e, f`, entonces después de llamar a `l1.deal()`, `l1` debería contener `a, c, e` y una lista que contiene `b, d, f` debería ser devuelta.

**Ejercicio 3.12.** Escriba un método, `reverse()`, que invierta el orden de los elementos en una `DLList`.

**Ejercicio 3.13.** Este ejercicio lo guía a través de una implementación del algoritmo de ordenamiento por mezcla (*merge-sort*) para ordenar una `DLList`, como se discute en la Sección 11.1.1.

1.  Escriba un método de `DLList` llamado `takeFirst(l2)`. Este método toma el primer nodo de `l2` y lo agrega a la lista receptora. Esto es equivalente a `add(size(), l2.remove(0))`, excepto que no debería crear un nuevo nodo.

2. Escriba un método estático de `DLList`, `merge(l1,l2)`, que tome dos listas ordenadas `l1` y `l2`, las fusione, y devuelva una nueva lista ordenada que contenga el resultado. Esto hace que `l1` y `l2` se vacíen en el proceso.

Por ejemplo, si `l1` contiene `a,c,d` y `l2` contiene `b,e,f`, entonces este método devuelve una nueva lista que contiene `a,b,c,d,e,f`.

3. Escriba un método de `DLList` `sort()` que ordene los elementos contenidos en la lista utilizando el algoritmo de ordenamiento por mezcla (*merge sort*). Este algoritmo recursivo funciona de la siguiente manera:

    (a) Si la lista contiene 0 o 1 elementos, entonces no hay nada que hacer. De lo contrario
    (b) Utilizando el método `truncate(size()/2)`, divida la lista en dos listas de longitud aproximadamente igual, `l1` y `l2`;

    (c) Ordene recursivamente `l1`;

    (d) Ordene recursivamente `l2`; y, finalmente,

    (e) Fusione `l1` y `l2` en una sola lista ordenada.

Los siguientes ejercicios son más avanzados y requieren una comprensión clara de lo que sucede con el valor mínimo almacenado en una Pila (`Stack`) o Cola (`Queue`) a medida que se agregan y eliminan elementos.

**Ejercicio 3.14.** Diseñe e implemente una estructura de datos `MinStack` que pueda almacenar elementos comparables y soporte las operaciones de pila `push(x)`, `pop()` y `size()`, así como la operación `min()`, que devuelve el valor mínimo actualmente almacenado en la estructura de datos. Todas las operaciones deben ejecutarse en tiempo constante.

**Ejercicio 3.15.** Diseñe e implemente una estructura de datos `MinQueue` que pueda almacenar elementos comparables y soporte las operaciones de cola `add(x)`, `remove()` y `size()`, así como la operación `min()`, que devuelve el valor mínimo actualmente almacenado en la estructura de datos. Todas las operaciones deben ejecutarse en tiempo constante amortizado.

**Ejercicio 3.16.** Diseñe e implemente una estructura de datos `MinDeque` que pueda almacenar elementos comparables y soporte todas las operaciones de deque `addFirst(x)`, `addLast(x)`, `removeFirst()`, `removeLast()` y `size()`, y la operación `min()`, que devuelve el valor mínimo actualmente almacenado en la estructura de datos. Todas las operaciones deben ejecutarse en tiempo constante amortizado.

Los siguientes ejercicios están diseñados para poner a prueba la comprensión del lector sobre la implementación y el análisis de la `SEList` eficiente en espacio:

**Ejercicio 3.17.** Demuestre que, si se utiliza una `SEList` como una Pila (Pila: de modo que las únicas operaciones sobre la `SEList` sean `push(x) = add(size(), x)` y `pop() = remove(size() - 1)`), entonces estas operaciones se ejecutan en tiempo constante amortizado, independientemente del valor de `b`.

**Ejercicio 3.18.** Diseñe e implemente una versión de una `SEList` que soporte todas las operaciones de Deque en tiempo constante amortizado por operación, independientemente del valor de `b`.

**Ejercicio 3.19.** Explique cómo usar el operador exclusivo bit a bit (XOR), `^`, para intercambiar los valores de dos variables `int` sin utilizar una tercera variable.