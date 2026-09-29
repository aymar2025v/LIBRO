# Capítulo 2

## Listas Basadas en Arreglos

En este capítulo, estudiaremos implementaciones de las interfaces `List` y `Queue` donde los datos subyacentes se almacenan en un arreglo, llamado arreglo de respaldo (*backing array*). La siguiente tabla resume los tiempos de ejecución de las operaciones para las estructuras de datos presentadas en este capítulo:

| | `get(i)/set(i,x)` | `add(i,x)/remove(i)` |
|---|---|---|
| `ArrayStack` | `O(1)` | `O(n-1)` |
| `ArrayDeque` | `O(1)` | `O(min{i, n-i})` |
| `DualArrayDeque` | `O(1)` | `O(min{i, n-i})` |
| `RootishArrayStack` | `O(1)` | `O(n-i)` |

Las estructuras de datos que funcionan almacenando datos en un solo arreglo tienen muchas ventajas y limitaciones en común:

* Los arreglos ofrecen acceso en tiempo constante a cualquier valor en el arreglo. Esto es lo que permite que `get(i)` y `set(i, x)` se ejecuten en tiempo constante.

* Los arreglos no son muy dinámicos. Agregar o eliminar un elemento cerca de la mitad de una lista significa que un gran número de elementos en el arreglo deben desplazarse para hacer espacio para el elemento recién agregado o para llenar el vacío creado por el elemento eliminado. Es por esto que las operaciones `add(i, x)` y `remove(i)` tienen tiempos de ejecución que dependen de `n` y de `i`.

* Los arreglos no pueden expandirse ni contraerse. Cuando el número de elementos en la estructura de datos excede el tamaño del arreglo de respaldo, se debe asignar un nuevo arreglo y los datos del arreglo antiguo deben copiarse en el nuevo arreglo. Esta es una operación costosa.

El tercer punto es importante. Los tiempos de ejecución citados en la tabla anterior no incluyen el costo asociado con el crecimiento y la contracción del arreglo de respaldo. Veremos que, si se gestiona cuidadosamente, el costo de hacer crecer y encoger el arreglo de respaldo no añade mucho al costo de una *operación promedio*. Más precisamente, si comenzamos con una estructura de datos vacía, y realizamos cualquier secuencia de $m$ operaciones `add(i,x)` o `remove(i)`, entonces el costo total de hacer crecer y encoger el arreglo de respaldo, sobre la secuencia completa de $m$ operaciones es $O(m)$. Aunque algunas operaciones individuales son más costosas, el costo amortizado, cuando se amortiza sobre todas las $m$ operaciones, es solo $O(1)$ por operación.

En este capítulo, y a lo largo de este libro, será conveniente tener arreglos que hagan un seguimiento de su tamaño. Los arreglos habituales de C++ no hacen esto, por lo que hemos definido una clase, `array`, que hace un seguimiento de su longitud. La implementación de esta clase es sencilla. Se implementa como un arreglo estándar de C++, `a`, y un entero, `length`:

**array**
```cpp
T *a;
int length;
```

El tamaño de un `array` se especifica en el momento de la creación:

**array**
```cpp
array(int len) {
    length = len;
    a = new T[length];
}
```
Se puede acceder a los elementos de un arreglo mediante índices:    
Los elementos de un arreglo pueden indexarse:

**array**
```cpp
T& operator[](int i) {
    assert(i >= 0 && i < length);
    return a[i];
}
```
Por último, cuando se asigna un array a otro, se trata simplemente de una manipulación de punteros que requiere un tiempo constante:

## ArrayStack: Operaciones Rápidas de Pila Usando un Arreglo
### §2.1

**array**

```cpp
array<T>& operator=(array<T> &b) {
    if (a != NULL) delete[] a;
    a = b.a;
    b.a = NULL;
    length = b.length;
    return *this;
}
```

## 2.1 ArrayStack: Operaciones Rápidas de Pila Usando un Arreglo

Un `ArrayStack` implementa la interfaz de lista utilizando un arreglo `a`, llamado el *arreglo de respaldo*. El elemento de la lista con índice `i` se almacena en `a[i]`. La mayoría de las veces, `a` es más grande de lo estrictamente necesario, por lo que se utiliza un entero `n` para llevar un registro del número de elementos realmente almacenados en `a`. De esta manera, los elementos de la lista se almacenan en `a[0], ..., a[n-1]` y, en todo momento, `a.length >= n`.

**ArrayStack**

```cpp
array<T> a;
int n;
int size() {
    return n;
}
```

### 2.1.1 Los Fundamentos

Acceder y modificar los elementos de un `ArrayStack` usando `get(i)` y `set(i,x)` es trivial. Después de realizar las comprobaciones de límites necesarias, simplemente devolvemos o establecemos, respectivamente, `a[i]`.

**ArrayStack**

```cpp
T get(int i) {
    return a[i];
}
T set(int i, T x) {
    T y = a[i];
    a[i] = x;
    return y;
}
```

Las operaciones de añadir y eliminar elementos de un `ArrayStack` se ilustran en la Figura 2.1. Para implementar la operación `add(i, x)`, primero comprobamos si `a` está ya lleno. Si es así, llamamos al método `resize()` para incrementar el tamaño de `a`. Cómo se implementa `resize()` se discutirá más adelante. Por ahora, es suficiente saber que, tras una llamada a `resize()`, podemos estar seguros de que `a.length > n`. Una vez solucionado esto, ahora desplazamos los elementos `a[i],...,a[n-1]` una posición a la derecha para hacer sitio para `x`, establecemos `a[i]` igual a `x`, e incrementamos `n`.

**ArrayStack**

```cpp
void add(int i, T x) {
    if (n + 1 > a.length) resize();
    for (int j = n; j > i; j--)
        a[j] = a[j - 1];
    a[i] = x;
    n++;
}
```
Si ignoramos el costo de la posible llamada a `resize()`, entonces el costo de la operación `add(i,x)` es proporcional al número de elementos que tenemos que desplazar para hacer espacio para `x`. Por lo tanto, el costo de esta operación (ignorando el costo de redimensionar `a`) es $O(n-i)$.

Implementar la operación `remove(i)` es similar. Desplazamos los elementos `a[i+1],...,a[n-1]` una posición a la izquierda (sobrescribiendo `a[i]`) y decrementamos el valor de `n`. Después de hacer esto, comprobamos si `n` se está volviendo mucho más pequeño que `a.length` verificando si `a.length \ge 3n`. Si es así, entonces llamamos a `resize()` para reducir el tamaño de `a`.

**ArrayStack**

```cpp
T remove(int i) {
    T x = a[i];
    for (int j = i; j < n - 1; j++)
        a[j] = a[j + 1];
    n--;
    if (a.length >= 3 * n) resize();
```

**IMG**

![sss](/Screenshot%202026-09-17%20182406.png)

<p align="center">Figura 2.1: Una secuencia de operaciones <code>add(i,x)</code> y <code>remove(i)</code> en un <code>ArrayStack</code>. Las flechas denotan elementos que están siendo copiados. Las operaciones que resultan en una llamada a <code>resize()</code> están marcadas con un asterisco.</p>

```cpp
    return x;
}
```

Si ignoramos el costo del método `resize()`, el costo de una operación `remove(i)` es proporcional al número de elementos que desplazamos, que es $O(n-i)$.

### 2.1.2 Crecimiento y Contracción

El método `resize()` es bastante sencillo; asigna un nuevo arreglo `b` cuyo tamaño es $2n$ y copia los $n$ elementos de `a` en las primeras $n$ posiciones de `b`, y luego establece `a` en `b`. Por lo tanto, después de una llamada a `resize()`, `a.length = 2n`.

**ArrayStack**

<pre style="border: 1px solid #ccc; padding: 10px; background-color: #f9f9f9; font-family: monospace; overflow-x: auto;">
void resize() {
    array&lt;T&gt; b(max(2 * n, 1));
    for (int i = 0; i &lt; n; i++)
        b[i] = a[i];
    a = b;
}
</pre>

Analizar el costo real de la operación `resize()` es fácil. Asigna un arreglo `b` de tamaño $2n$ y copia los $n$ elementos de `a` en `b`. Esto toma un tiempo $O(n)$.

El análisis del tiempo de ejecución de la sección anterior ignoró el costo de las llamadas a `resize()`. En esta sección analizamos este costo utilizando una técnica conocida como análisis amortizado. Esta técnica no intenta determinar el costo de redimensionar durante cada operación individual `add(i,x)` y `remove(i)`. En su lugar, considera el costo de todas las llamadas a `resize()` durante una secuencia de $m$ llamadas a `add(i,x)` o `remove(i)`. En particular, mostraremos:

**Lema 2.1.** Si se crea un `ArrayStack` vacío y se realiza cualquier secuencia de $m \ge 1$ llamadas a `add(i,x)` y `remove(i)`, entonces el tiempo total empleado durante todas las llamadas a `resize()` es $O(m)$.

*Prueba.* Demostraremos que cada vez que se llama a `resize()`, el número de llamadas a `add` o `remove` desde la última llamada a `resize()` es al menos $n/2 - 1$. Por lo tanto, si $n_i$ denota el valor de $n$ durante la $i$-ésima llamada a `resize()` y $r$ denota el número de llamadas a `resize()`, entonces el número total de llamadas a `add(i,x)` o `remove(i)` es al menos

$$
\sum_{i=1}^{r} (n_i / 2 - 1) \le m ,
$$

lo cual es equivalente a

$$
\sum_{i=1}^{r} n_i \le 2m + 2r .
$$

Por otro lado, el tiempo total empleado durante todas las llamadas a `resize()` es

$$
\sum_{i=1}^{r} O(n_i) \le O(m + r) = O(m) ,
$$

dado que $r$ no es mayor que $m$. Todo lo que queda es demostrar que el número de llamadas a `add(i,x)` o `remove(i)` entre la $(i-1)$-ésima y la $i$-ésima llamada a `resize()` es al menos $n_i/2$.

Hay dos casos a considerar. En el primer caso, `resize()` está siendo llamado por `add(i,x)` porque el arreglo de respaldo `a` está lleno, es decir, `a.length = n = n_i`. Consideremos la llamada anterior a `resize()`: después de esta llamada anterior, el tamaño de `a` era `a.length`, pero el número de elementos almacenados en `a` era como máximo `a.length/2 = n_i/2`. Pero ahora el número de elementos almacenados en `a` es `n_i = a.length`, por lo que debe haber habido al menos `n_i/2` llamadas a `add(i,x)` desde la llamada anterior a `resize()`.

El segundo caso ocurre cuando `remove(i)` llama a `resize()` porque `a.length >= 3n = 3n_i`. De nuevo, después de la llamada anterior a `resize()` el número de elementos almacenados en `a` era al menos `a.length/2 - 1`.^1 Ahora hay `n_i <= a.length/3` elementos almacenados en `a`. Por lo tanto, el número de operaciones `remove(i)` desde la última llamada a `resize()` es al menos

$$
\begin{aligned}
R &\ge a.length/2 - 1 - a.length/3 \\
&= a.length/6 - 1 \\
&= (a.length/3)/2 - 1 \\
&\ge n_i/2 - 1 .
\end{aligned}
$$

En cualquier caso, el número de llamadas a `add(i,x)` o `remove(i)` que ocurren entre la $(i-1)$-ésima llamada a `resize()` y la $i$-ésima llamada a `resize()` es al menos $n_i/2 - 1$, como se requiere para completar la prueba. □

---
<sup>1</sup>El $-1$ en esta fórmula tiene en cuenta el caso especial que ocurre cuando $n = 0$ y `a.length = 1`.

### 2.1.3 Resumen

El siguiente teorema resume el rendimiento de un `ArrayStack`:

**Teorema 2.1.** Un `ArrayStack` implementa la interfaz `List`. Ignorando el costo de las llamadas a `resize()`, un `ArrayStack` soporta las operaciones

*   `get(i)` y `set(i,x)` en tiempo $O(1)$ por operación; y
*   `add(i,x)` y `remove(i)` en tiempo $O(1 + n - i)$ por operación.

Además, comenzando con un `ArrayStack` vacío y realizando cualquier secuencia de $m$ operaciones `add(i,x)` y `remove(i)` resulta en un total de $O(m)$ tiempo empleado durante todas las llamadas a `resize()`.

El `ArrayStack` es una forma eficiente de implementar una Pila (*Stack*). En particular, podemos implementar `push(x)` como `add(n,x)` y `pop()` como `remove(n - 1)`, en cuyo caso estas operaciones se ejecutarán en tiempo amortizado $O(1)$.

## 2.2 FastArrayStack: Un ArrayStack Optimizado

Gran parte del trabajo realizado por un `ArrayStack` implica desplazar (mediante `add(i,x)` y `remove(i)`) y copiar (mediante `resize()`) datos. En las implementaciones mostradas anteriormente, esto se hacía usando bucles `for`. Resulta que muchos entornos de programación tienen funciones específicas que son muy eficientes para copiar y mover bloques de datos. En el lenguaje de programación C, están las funciones `memcpy(d,s,n)` y `memmove(d,s,n)`. En el lenguaje C++ está el algoritmo `std::copy(a0,a1,b)`. En Java está el método `System.arraycopy(s,i,d,j,n)`.

**FastArrayStack**

```cpp
void resize() {
    array<T> b(max(1, 2*n));
    std::copy(a+0, a+n, b+0);
    a = b;
}
void add(int i, T x) {
    if (n + 1 > a.length) resize();
    std::copy_backward(a+i, a+n, a+n+1);
    a[i] = x;
    n++;
}
```

Estas funciones suelen estar altamente optimizadas e incluso pueden usar instrucciones de máquina especiales que pueden hacer esta copia mucho más rápido de lo que podríamos hacerlo usando un bucle `for`. Aunque el uso de estas funciones no disminuye asintóticamente los tiempos de ejecución, aún puede ser una optimización que vale la pena.

En las implementaciones de C++, el uso del nativo `std::copy(a0,a1,b)` resultó en aceleraciones de un factor entre 2 y 3, dependiendo de los tipos de operaciones realizadas. Su experiencia puede variar.

## 2.3 ArrayQueue: Una Cola Basada en Arreglos

En esta sección, presentamos la estructura de datos `ArrayQueue`, que implementa una cola FIFO (primero en entrar, primero en salir); los elementos se eliminan (usando la operación `remove()`) de la cola en el mismo orden en que se agregan (usando la operación `add()`).

Observe que un `ArrayStack` es una mala elección para la implementación de una cola FIFO. No es una buena elección porque debemos elegir un extremo de la lista en el cual agregar elementos y luego eliminar elementos del otro extremo. Una de las dos operaciones trabajará en la cabeza de la lista, lo que implica llamar a `add(i,x)` o `remove(i)` con un valor de $i = 0$. Esto da un tiempo de ejecución proporcional a $n$.

Para obtener una implementación eficiente de una cola basada en arreglos, primero notamos que el problema sería fácil si tuviéramos un arreglo infinito `a`. Podríamos mantener un índice `j` que realice un seguimiento del siguiente elemento a eliminar y un entero `n` que cuente el número de elementos en la cola. Los elementos de la cola siempre se almacenarían en

$$
a[j], a[j+1], ..., a[j+n-1] .
$$

Inicialmente, tanto `j` como `n` se establecerían en 0. Para agregar un elemento, lo colocaríamos en `a[j + n]` e incrementaríamos `n`. Para eliminar un elemento, lo eliminaríamos de `a[j]`, incrementaríamos `j` y decrementaríamos `n`.

---

Por supuesto, el problema con esta solución es que requiere un arreglo infinito. Un `ArrayQueue` simula esto utilizando un arreglo finito `a` y *aritmética modular*. Este es el tipo de aritmética que se utiliza cuando hablamos de la hora del día. Por ejemplo, las 10:00 más cinco horas dan las 3:00. Formalmente, decimos que

$$
10 + 5 = 15 \equiv 3 \pmod{12} .
$$

Leemos la última parte de esta ecuación como "15 es congruente con 3 módulo 12". También podemos tratar `mod` como un operador binario, de modo que

$$
15 \bmod 12 = 3 .
$$

De manera más general, para un entero $a$ y un entero positivo $m$, $a \bmod m$ es el único entero $r \in \{0,...,m-1\}$ tal que $a = r + km$ para algún entero $k$. De manera menos formal, el valor $r$ es el resto que obtenemos cuando dividimos $a$ entre $m$. En muchos lenguajes de programación, incluido C++, el operador mod se representa usando el símbolo `%`.^2

La aritmética modular es útil para simular un arreglo infinito, ya que $i \bmod \text{a.length}$ siempre da un valor en el rango $0,...,\text{a.length}-1$. Usando aritmética modular podemos almacenar los elementos de la cola en las ubicaciones del arreglo

$$
a[j\%\text{a.length}], a[(j+1)\%\text{a.length}],...,a[(j+n-1)\%\text{a.length}] .
$$

Esto trata al arreglo `a` como un *arreglo circular* en el cual los índices mayores que `a.length - 1` "dan la vuelta" al principio del arreglo.

Lo único que queda por preocuparnos es asegurarnos de que el número de elementos en el `ArrayQueue` no exceda el tamaño de `a`.

**ArrayQueue**

```cpp
array<T> a;
int j;
int n;
```

Una secuencia de operaciones `add(x)` y `remove()` en un `ArrayQueue` se ilustra en la Figura 2.2. Para implementar `add(x)`, primero verificamos si `a` está lleno y, si es necesario, llamamos a `resize()` para incrementar el tamaño de `a`. A continuación, almacenamos `x` en `a[(j + n)%a.length]` e incrementamos `n`.

---
<sup>2</sup>Esto a veces se conoce como el operador mod "descerebrado", ya que no implementa correctamente el operador mod matemático cuando el primer argumento es negativo.


![iiii](/Screenshot%202026-09-17%20182548.png)

<div align="center">
Figura 2.2: Una secuencia de operaciones <code>add(x)</code> y <code>remove(i)</code> en un <code>ArrayQueue</code>. Las flechas denotan elementos que están siendo copiados. Las operaciones que resultan en una llamada a <code>resize()</code> están marcadas con un asterisco.
</div>

**ArrayQueue**

```cpp
bool add(T x) {
    if (n + 1 > a.length) resize();
    a[(j+n) % a.length] = x;
    n++;
    return true;
}
```

Para implementar `remove()`, primero almacenamos `a[j]` para poder devolverlo más tarde. A continuación, decrementamos `n` e incrementamos `j` (módulo `a.length`) estableciendo `j = (j + 1) mod a.length`. Finalmente, devolvemos el valor almacenado de `a[j]`. Si es necesario, podemos llamar a `resize()` para disminuir el tamaño de `a`.

**ArrayQueue**

```cpp
T remove() {
    T x = a[j];
    j = (j + 1) % a.length;
    n--;
    if (a.length >= 3*n) resize();
    return x;
}
```

Finalmente, la operación `resize()` es muy similar a la operación `resize()` de `ArrayStack`. Asigna un nuevo arreglo, `b`, de tamaño $2n$ y copia

$$
a[j], a[(j + 1)\%\text{a.length}], ..., a[(j + n - 1)\%\text{a.length}]
$$

en

$$
b[0], b[1], ..., b[n - 1]
$$

y establece `j = 0`.

**ArrayQueue**

```cpp
void resize() {
    array<T> b(max(1, 2*n));
    for (int k = 0; k < n; k++)
        b[k] = a[(j+k)%a.length];
    a = b;
    j = 0;
}
```

### 2.3.1 Resumen

El siguiente teorema resume el rendimiento de la estructura de datos `ArrayQueue`:

**Teorema 2.2.** *Un `ArrayQueue` implementa la interfaz `Queue` (FIFO). Ignorando el costo de las llamadas a `resize()`, un `ArrayQueue` soporta las operaciones `add(x)` y `remove()` en tiempo $O(1)$ por operación. Además, comenzando con un `ArrayQueue` vacío, cualquier secuencia de $m$ operaciones `add(i,x)` y `remove(i)` resulta en un total de $O(m)$ tiempo empleado durante todas las llamadas a `resize()`.*

## 2.4 ArrayDeque: Operaciones Rápidas de Deque Usando un Arreglo

El `ArrayQueue` de la sección anterior es una estructura de datos para representar una secuencia que nos permite agregar eficientemente a un extremo de la secuencia y eliminar del otro extremo. La estructura de datos `ArrayDeque` permite la adición y eliminación eficientes en ambos extremos. Esta estructura implementa la interfaz `List` utilizando la misma técnica de arreglo circular utilizada para representar un `ArrayQueue`.

**ArrayDeque**

```cpp
array<T> a;
int j;
int n;
```

Las operaciones `get(i)` y `set(i,x)` en un `ArrayDeque` son sencillas. Obtienen o establecen el elemento del arreglo `a[(j + i) % a.length]`.

**ArrayDeque**

```cpp
T get(int i) {
    return a[(j + i) % a.length];
}
T set(int i, T x) {
    T y = a[(j + i) % a.length];
    a[(j + i) % a.length] = x;
    return y;
}
```

  ![aaaa](/Screenshot%202026-09-17%20182720.png)

  <!-- Leyenda de la figura -->
  <div style="margin-top: 20px; font-size: 14px; text-align: center; width: 100%;">
    Figura 2.3: Una secuencia de operaciones <code>add(i,x)</code> y <code>remove(i)</code> en un <code>ArrayDeque</code>. Las flechas denotan elementos que están siendo copiados.
  </div>

</div>

La implementación de `add(i, x)` es un poco más interesante. Como de costumbre, primero verificamos si `a` está lleno y, si es necesario, llamamos a `resize()` para redimensionar `a`. Recordemos que queremos que esta operación sea rápida cuando `i` es pequeño (cerca de 0) o cuando `i` es grande (cerca de `n`). Por lo tanto, verificamos si `i < n/2`. Si es así, desplazamos los elementos `a[0],...,a[i-1]` una posición a la izquierda. De lo contrario (`i >= n/2`), desplazamos los elementos `a[i],...,a[n-1]` una posición a la derecha. Véase la Figura 2.3 para una ilustración de las operaciones `add(i,x)` y `remove(x)` en un `ArrayDeque`.

**ArrayDeque**

```cpp
void add(int i, T x) {
    if (n + 1 > a.length) resize();
    if (i < n/2) { // desplazar a[0],..,a[i-1] una posición a la izquierda
        j = (j == 0) ? a.length - 1 : j - 1;
        for (int k = 0; k <= i-1; k++)
            a[(j+k)%a.length] = a[(j+k+1)%a.length];
    } else { // desplazar a[i],..,a[n-1] una posición a la derecha
        for (int k = n; k > i; k--)
            a[(j+k)%a.length] = a[(j+k-1)%a.length];
    }
    a[(j+i)%a.length] = x;
    n++;
}
```

Al realizar el desplazamiento de esta manera, garantizamos que `add(i,x)` nunca tenga que desplazar más de $\min\{i, n-i\}$ elementos. Por lo tanto, el tiempo de ejecución de la operación `add(i,x)` (ignorando el costo de una operación `resize()`) es $O(1 + \min\{i, n-i\})$.

La implementación de la operación `remove(i)` es similar. Desplaza los elementos `a[0],...,a[i-1]` una posición a la derecha o desplaza los elementos `a[i+1],...,a[n-1]` una posición a la izquierda dependiendo de si $i < n/2$. De nuevo, esto significa que `remove(i)` nunca emplea más de $O(1 + \min\{i, n-i\})$ tiempo en desplazar elementos.

**ArrayDeque**

```cpp
T remove(int i) {
    T x = a[(j+i)%a.length];
    if (i < n/2) { // desplazar a[0],..,[i-1] una posición a la derecha
        for (int k = i; k > 0; k--)
            a[(j+k)%a.length] = a[(j+k-1)%a.length];
        j = (j + 1) % a.length;
    } else { // desplazar a[i+1],..,a[n-1] una posición a la izquierda
        for (int k = i; k < n-1; k++)
            a[(j+k)%a.length] = a[(j+k+1)%a.length];
    }
    n--;
    if (3*n < a.length) resize();
    return x;
}
```

### 2.4.1 Resumen

El siguiente teorema resume el rendimiento de la estructura de datos `ArrayDeque`:

**Teorema 2.3.** *Un `ArrayDeque` implementa la interfaz `List`. Ignorando el costo de las llamadas a `resize()`, un `ArrayDeque` soporta las operaciones*

*   `get(i)` y `set(i, x)` en tiempo $O(1)$ por operación; y
*   `add(i, x)` y `remove(i)` en tiempo $O(1 + \min\{i, n - i\})$ por operación.

*Además, comenzando con un `ArrayDeque` vacío, realizar cualquier secuencia de $m$ operaciones `add(i, x)` y `remove(i)` resulta en un total de $O(m)$ tiempo empleado durante todas las llamadas a `resize()`.*


## 2.5 DualArrayDeque: Construyendo un Deque a partir de Dos Pilas

A continuación, presentamos una estructura de datos, el `DualArrayDeque`, que logra los mismos límites de rendimiento que un `ArrayDeque` utilizando dos `ArrayStack`. Aunque el rendimiento asintótico del `DualArrayDeque` no es mejor que el del `ArrayDeque`, todavía vale la pena estudiarlo, ya que ofrece un buen ejemplo de cómo construir una estructura de datos sofisticada combinando dos estructuras de datos más simples.

Un `DualArrayDeque` representa una lista utilizando dos `ArrayStack`. Recordemos que un `ArrayStack` es rápido cuando las operaciones sobre él modifican elementos cerca del final. Un `DualArrayDeque` coloca dos `ArrayStack`, llamados `front` y `back`, uno tras otro (espalda con espalda) para que las operaciones sean rápidas en cualquiera de los dos extremos.

**DualArrayDeque**

```cpp
ArrayStack<T> front;
ArrayStack<T> back;
```

Un `DualArrayDeque` no almacena explícitamente el número, `n`, de elementos que contiene. No necesita hacerlo, ya que contiene `n = front.size() + back.size()` elementos. Sin embargo, al analizar el `DualArrayDeque` seguiremos utilizando `n` para denotar el número de elementos que contiene.

**DualArrayDeque**

```cpp
int size() {
    return front.size() + back.size();
}
```

El `ArrayStack` `front` almacena los elementos de la lista cuyos índices son `0,...,front.size()-1`, pero los almacena en orden inverso. El `ArrayStack` `back` contiene elementos de la lista con índices en `front.size(),...,size()-1` en el orden normal. De esta manera, `get(i)` y `set(i,x)` se traducen en llamadas apropiadas a `get(i)` o `set(i,x)` ya sea en `front` o en `back`, las cuales toman un tiempo $O(1)$ por operación.

**DualArrayDeque**

```cpp
T get(int i) {
    if (i < front.size()) {
        return front.get(front.size() - i - 1);
    } else {
        return back.get(i - front.size());
    }
}
T set(int i, T x) {
    if (i < front.size()) {
        return front.set(front.size() - i - 1, x);
    } else {
        return back.set(i - front.size(), x);
    }
}
```

Observe que si un índice `i < front.size()`, entonces corresponde al elemento de `front` en la posición `front.size() - i - 1`, ya que los elementos de `front` se almacenan en orden inverso.

Agregar y eliminar elementos de un `DualArrayDeque` se ilustra en la Figura 2.4. La operación `add(i,x)` manipula ya sea `front` o `back`, según corresponda:

**DualArrayDeque**

```cpp
void add(int i, T x) {
    if (i < front.size()) {
        front.add(front.size() - i, x);
    } else {
        back.add(i - front.size(), x);
    }
    balance();
}
```

El método `add(i,x)` realiza un reequilibrio de los dos `ArrayStacks` `front` y `back`, llamando al método `balance()`. La implementación de `balance()` se describe a continuación, pero por ahora es suficiente saber que `balance()` asegura que, a menos que `size() < 2`, `front.size()` y `back.size()` no difieran en más de un factor de 3. En particular, $3 \cdot \text{front.size()} \ge \text{back.size()}$ y $3 \cdot \text{back.size()} \ge \text{front.size()}$.


  ![rrrr](/Screenshot%202026-09-17%20182821.png)

  <!-- Leyenda de la figura -->
  <div style="margin-top: 20px; font-size: 14px; text-align: center; width: 100%;">
    Figura 2.4: Una secuencia de operaciones <code>add(i,x)</code> y <code>remove(i)</code> en un <code>DualArrayDeque</code>. Las flechas denotan elementos que están siendo copiados. Las operaciones que resultan en un reequilibrio mediante <code>balance()</code> están marcadas con un asterisco.
  </div>

</div>


A continuación analizamos el costo de `add(i, x)`, ignorando el costo de las llamadas a `balance()`. Si $i < \text{front.size()}$, entonces `add(i, x)` se implementa mediante la llamada a `front.add(front.size() - i - 1, x)`. Dado que `front` es un `ArrayStack`, el costo de esto es

$$
O(\text{front.size()} - (\text{front.size()} - i - 1) + 1) = O(i + 1) . \tag{2.1}
$$

Por otro lado, si $i \ge \text{front.size()}$, entonces `add(i, x)` se implementa como `back.add(i - front.size(), x)`. El costo de esto es

$$
O(\text{back.size()} - (i - \text{front.size()}) + 1) = O(n - i + 1) . \tag{2.2}
$$

Observe que el primer caso (2.1) ocurre cuando $i < n/4$. El segundo caso (2.2) ocurre cuando $i \ge 3n/4$. Cuando $n/4 \le i < 3n/4$, no podemos estar seguros de si la operación afecta a `front` o a `back`, pero en cualquier caso, la operación toma $O(n) = O(i) = O(n - i)$ tiempo, ya que $i \ge n/4$ y $n - i > n/4$. Resumiendo la situación, tenemos

$$
\text{Tiempo de ejecución de } \texttt{add(i, x)} \le
\begin{cases}
O(1 + i) & \text{si } i < n/4 \\
O(n) & \text{si } n/4 \le i < 3n/4 \\
O(1 + n - i) & \text{si } i \ge 3n/4
\end{cases}
$$

Por lo tanto, el tiempo de ejecución de `add(i,x)`, si ignoramos el costo de la llamada a `balance()`, es $O(1 + \min\{i, n-i\})$.

La operación `remove(i)` y su análisis se asemejan a la operación `add(i,x)` y su análisis.

**DualArrayDeque**

```cpp
T remove(int i) {
    T x;
    if (i < front.size()) {
        x = front.remove(front.size() - i - 1);
    } else {
        x = back.remove(i - front.size());
    }
    balance();
    return x;
}
```

### 2.5.1 Equilibrio

Finalmente, nos centramos en la operación `balance()` realizada por `add(i,x)` y `remove(i)`. Esta operación asegura que ni `front` ni `back` se vuelvan demasiado grandes (o demasiado pequeños). Asegura que, a menos que haya menos de dos elementos, tanto `front` como `back` contengan al menos $n/4$ elementos. Si este no es el caso, entonces mueve elementos entre ellos para que `front` y `back` contengan exactamente $\lfloor n/2 \rfloor$ elementos y $\lceil n/2 \rceil$ elementos, respectivamente.

**DualArrayDeque**

```cpp
void balance() {
    if (3*front.size() < back.size()
        || 3*back.size() < front.size()) {
        int n = front.size() + back.size();
        int nf = n/2;
        array<T> af(max(2*nf, 1));
        for (int i = 0; i < nf; i++)
            af[nf-i-1] = get(i);
        int nb = n - nf;
        array<T> ab(max(2*nb, 1));
        for (int i = 0; i < nb; i++)
            ab[i] = get(nf+i);
        front.a = af;
        front.n = nf;
        back.a = ab;
        back.n = nb;
    }
}
```

Aquí hay poco que analizar. Si la operación `balance()` realiza un reequilibrio, entonces mueve $O(n)$ elementos y esto toma $O(n)$ tiempo. Esto es malo, ya que `balance()` se llama con cada llamada a `add(i,x)` y `remove(i)`. Sin embargo, el siguiente lema muestra que, en promedio, `balance()` solo emplea una cantidad constante de tiempo por operación.

**Lema 2.2.** *Si se crea un `DualArrayDeque` vacío y se realiza cualquier secuencia de $m \ge 1$ llamadas a `add(i,x)` y `remove(i)`, entonces el tiempo total empleado durante todas las llamadas a `balance()` es $O(m)$.*

*Prueba.* Demostraremos que, si `balance()` se ve forzado a desplazar elementos, entonces el número de operaciones `add(i,x)` y `remove(i)` desde la última vez que `balance()` desplazó elementos es al menos $n/2 - 1$. Al igual que en la prueba del Lema 2.1, esto es suficiente para demostrar que el tiempo total empleado por `balance()` es $O(m)$.

Realizaremos nuestro análisis utilizando una técnica conocida como el método del potencial. Definimos el potencial, $\Phi$, del `DualArrayDeque` como la diferencia de tamaño entre `front` y `back`:

$$
\Phi = |\text{front.size}() - \text{back.size}()| .
$$

Lo interesante de este potencial es que una llamada a `add(i,x)` o `remove(i)` que no realiza ningún equilibrio puede aumentar el potencial como máximo en 1.

Observe que, inmediatamente después de una llamada a `balance()` que desplaza elementos, el potencial, $\Phi_0$, es como máximo 1, ya que

$$
\Phi_0 = |\lfloor n/2 \rfloor - \lceil n/2 \rceil| \le 1 .
$$

Considere la situación inmediatamente antes de una llamada a `balance()` que desplaza elementos y suponga, sin pérdida de generalidad, que `balance()`

está desplazando elementos porque $3 \cdot \text{front.size}() < \text{back.size}()$. Observe que, en este caso,

$$
\begin{aligned}
n &= \text{front.size}() + \text{back.size}() \\
&< \text{back.size}()/3 + \text{back.size}() \\
&= \frac{4}{3} \text{back.size}()
\end{aligned}
$$

Además, el potencial en este punto en el tiempo es

$$
\begin{aligned}
\Phi_1 &= \text{back.size}() - \text{front.size}() \\
&> \text{back.size}() - \text{back.size}()/3 \\
&= \frac{2}{3} \text{back.size}() \\
&> \frac{2}{3} \times \frac{3}{4} n \\
&= n/2
\end{aligned}
$$

Por lo tanto, el número de llamadas a `add(i,x)` o `remove(i)` desde la última vez que `balance()` desplazó elementos es al menos $\Phi_1 - \Phi_0 > n/2 - 1$. Esto completa la prueba. □

### 2.5.2 Resumen

El siguiente teorema resume las propiedades de un `DualArrayDeque`:

**Teorema 2.4.** *Un `DualArrayDeque` implementa la interfaz `List`. Ignorando el costo de las llamadas a `resize()` y `balance()`, un `DualArrayDeque` soporta las operaciones*

*   `get(i)` y `set(i,x)` en tiempo $O(1)$ por operación; y
*   `add(i,x)` y `remove(i)` en tiempo $O(1 + \min\{i, n-i\})$ por operación.

*Además, comenzando con un `DualArrayDeque` vacío, cualquier secuencia de $m$ operaciones `add(i,x)` y `remove(i)` resulta en un total de $O(m)$ tiempo empleado durante todas las llamadas a `resize()` y `balance()`.*


## 2.6 RootishArrayStack: Una Pila Basada en Arreglos Eficiente en Espacio

Uno de los inconvenientes de todas las estructuras de datos anteriores en este capítulo es que, debido a que almacenan sus datos en uno o dos arreglos y evitan redimensionar estos arreglos con demasiada frecuencia, los arreglos con frecuencia no están muy llenos. Por ejemplo, inmediatamente después de una operación `resize()` en un `ArrayStack`, el arreglo de respaldo `a` solo está medio lleno. Peor aún, hay momentos en que solo un tercio de `a` contiene datos.

En esta sección, discutimos la estructura de datos `RootishArrayStack`, que aborda el problema del espacio desperdiciado. El `RootishArrayStack` almacena `n` elementos utilizando $O(\sqrt{n})$ arreglos. En estos arreglos, como máximo $O(\sqrt{n})$ ubicaciones de arreglo no se utilizan en ningún momento. Todas las ubicaciones de arreglo restantes se utilizan para almacenar datos. Por lo tanto, estas estructuras de datos desperdician como máximo $O(\sqrt{n})$ espacio al almacenar `n` elementos.

Un `RootishArrayStack` almacena sus elementos en una lista de `r` arreglos llamados *bloques* que están numerados $0, 1, ..., r-1$. Véase la Figura 2.5. El bloque `b` contiene `b+1` elementos. Por lo tanto, todos los `r` bloques contienen un total de

$$
1 + 2 + 3 + \cdots + r = r(r+1)/2
$$

elementos. La fórmula anterior se puede obtener como se muestra en la Figura 2.6.

**RootishArrayStack**

```cpp
ArrayStack<T*> blocks;
int n;
```

Como era de esperar, los elementos de la lista se distribuyen en orden dentro de los bloques. El elemento de la lista con índice 0 se almacena en el bloque 0, los elementos con índices de lista 1 y 2 se almacenan en el bloque 1, los elementos con índices de lista 3, 4 y 5 se almacenan en el bloque 2, y así sucesivamente. El principal problema que tenemos que abordar es el de determinar, dado un índice `i`, qué bloque contiene `i`, así como el índice correspondiente a `i` dentro de ese bloque.

Determinar el índice de `i` dentro de su bloque resulta ser fácil. Si el índice `i` está en el bloque `b`, entonces el número de elementos en los bloques $0, ..., b-1$ es $b(b+1)/2$. Por lo tanto, `i` se almacena en la ubicación

$$
j = i - b(b+1)/2
$$


  ![wwwww](/Screenshot%202026-09-17%20182919.png)

  <!-- Leyenda de la figura -->
  <div style="margin-top: 20px; font-size: 14px; text-align: center; width: 100%;">
    Figura 2.5: Una secuencia de operaciones <code>add(i,x)</code> y <code>remove(i)</code> en un <code>RootishArrayStack</code>. Las flechas denotan elementos que están siendo copiados.
  </div>

![Texto alternativo](/Screenshot%202026-09-17%20165733.png)

<div align="center">
Figura 2.6: El número de cuadrados blancos es 1 + 2 + 3 + ... + r. El número de cuadrados sombreados es el mismo. Juntos, los cuadrados blancos y sombreados forman un rectángulo que consta de r(r + 1) cuadrados.
</div>

dentro del bloque `b`. Un poco más desafiante es el problema de determinar el valor de `b`. El número de elementos que tienen índices menores o iguales a `i` es $i + 1$. Por otro lado, el número de elementos en los bloques $0,...,b$ es $(b + 1)(b + 2)/2$. Por lo tanto, `b` es el entero más pequeño tal que

$$
\frac{(b + 1)(b + 2)}{2} \ge i + 1 .
$$

Podemos reescribir esta ecuación como

$$
b^2 + 3b - 2i \ge 0 .
$$

La ecuación cuadrática correspondiente $b^2 + 3b - 2i = 0$ tiene dos soluciones: $b = (-3 + \sqrt{9 + 8i})/2$ y $b = (-3 - \sqrt{9 + 8i})/2$. La segunda solución no tiene sentido en nuestra aplicación, ya que siempre da un valor negativo. Por lo tanto, obtenemos la solución $b = (-3 + \sqrt{9 + 8i})/2$. En general, esta solución no es un entero, pero volviendo a nuestra desigualdad, queremos el entero más pequeño `b` tal que $b \ge (-3 + \sqrt{9 + 8i})/2$. Esto es simplemente

$$
b = \left\lceil \frac{-3 + \sqrt{9 + 8i}}{2} \right\rceil .
$$

**RootishArrayStack**

```cpp
int i2b(int i) {
    double db = (-3.0 + sqrt(9 + 8*i)) / 2.0;
    int b = (int)ceil(db);
    return b;
}
```

Una vez resuelto esto, los métodos `get(i)` y `set(i,x)` son sencillos. Primero calculamos el bloque `b` apropiado y el índice `j` apropiado dentro del bloque y luego realizamos la operación apropiada:

**RootishArrayStack**

```cpp
T get(int i) {
    int b = i2b(i);
    int j = i - b*(b+1)/2;
    return blocks.get(b)[j];
}
T set(int i, T x) {
    int b = i2b(i);
    int j = i - b*(b+1)/2;
    T y = blocks.get(b)[j];
    blocks.get(b)[j] = x;
    return y;
}
```

Si utilizamos cualquiera de las estructuras de datos de este capítulo para representar la lista `blocks`, entonces `get(i)` y `set(i,x)` se ejecutarán en tiempo constante.

El método `add(i,x)`, a estas alturas, nos resultará familiar. Primero verificamos si nuestra estructura de datos está llena, comprobando si el número de bloques, `r`, es tal que `r(r + 1)/2 = n`. Si es así, llamamos a `grow()` para agregar otro bloque. Hecho esto, desplazamos los elementos con índices `i,...,n-1` una posición a la derecha para hacer espacio para el nuevo elemento con índice `i`:

**RootishArrayStack**

```cpp
void add(int i, T x) {
    int r = blocks.size();
    if ((r*(r+1)/2 < n + 1) grow();
    n++;
    for (int j = n-1; j > i; j--)
        set(j, get(j-1));
    set(i, x);
}
```

El método `grow()` hace lo que esperamos. Agrega un nuevo bloque:

**RootishArrayStack**

```cpp
void grow() {
    blocks.add(blocks.size(), new T[blocks.size()+1]);
}
```

Ignorando el costo de la operación `grow()`, el costo de una operación `add(i,x)` está dominado por el costo del desplazamiento y, por lo tanto, es $O(1+n-i)$, al igual que en un `ArrayStack`.

La operación `remove(i)` es similar a `add(i,x)`. Desplaza los elementos con índices `i+1,...,n` una posición a la izquierda y luego, si hay más de un bloque vacío, llama al método `shrink()` para eliminar todos menos uno de los bloques no utilizados:


**RootishArrayStack**

```cpp
T remove(int i) {
    T x = get(i);
    for (int j = i; j < n-1; j++)
        set(j, get(j+1));
    n--;
    int r = blocks.size();
    if ((r-2)*(r-1)/2 >= n) shrink();
    return x;
}
```

**RootishArrayStack**


```cpp
void shrink() {
    int r = blocks.size();
    while (r > 0 && (r-2)*(r-1)/2 >= n) {
        delete [] blocks.remove(blocks.size()-1);
        r--;
    }
}
```

Una vez más, ignorando el costo de la operación `shrink()`, el costo de una operación `remove(i)` está dominado por el costo del desplazamiento y, por lo tanto, es $O(n - i)$.

### 2.6.1 Análisis del Crecimiento y la Contracción

El análisis anterior de `add(i,x)` y `remove(i)` no tiene en cuenta el costo de `grow()` y `shrink()`. Observe que, a diferencia de la operación `ArrayStack.resize()`, `grow()` y `shrink()` no copian ningún dato. Solo asignan o liberan un arreglo de tamaño `r`. En algunos entornos, esto toma solo tiempo constante, mientras que en otros, puede requerir tiempo proporcional a `r`.

Observamos que, inmediatamente después de una llamada a `grow()` o `shrink()`, la situación es clara. El bloque final está completamente vacío y todos los demás bloques están completamente llenos. Otra llamada a `grow()` o `shrink()` no ocurrirá hasta que se hayan agregado o eliminado al menos `r - 1` elementos. Por lo tanto, incluso si `grow()` y `shrink()` toman tiempo $O(r)$, este costo se puede amortizar sobre al menos `r - 1` operaciones `add(i,x)` o `remove(i)`, de modo que el costo amortizado de `grow()` y `shrink()` es $O(1)$ por operación.

### 2.6.2 Uso del Espacio

A continuación, analizamos la cantidad de espacio extra utilizado por un `RootishArrayStack`. En particular, queremos contar cualquier espacio utilizado por un `RootishArrayStack` que no sea un elemento de arreglo utilizado actualmente para contener un elemento de la lista. A todo ese espacio lo llamamos espacio desperdiciado.

La operación `remove(i)` asegura que un `RootishArrayStack` nunca tenga más de dos bloques que no estén completamente llenos. El número de bloques, `r`, utilizado por un `RootishArrayStack` que almacena `n` elementos, por lo tanto, satisface

$$
(r - 2)(r - 1) \le n .
$$

Nuevamente, usando la ecuación cuadrática en esto se obtiene

$$
r \le \frac{3 + \sqrt{1 + 4n}}{2} = O(\sqrt{n}) .
$$

Los dos últimos bloques tienen tamaños `r` y `r - 1`, por lo que el espacio desperdiciado por estos dos bloques es como máximo $2r - 1 = O(\sqrt{n})$. Si almacenamos los bloques en (por ejemplo) un `ArrayStack`, entonces la cantidad de espacio desperdiciado por la `List` que almacena esos `r` bloques también es $O(r) = O(\sqrt{n})$. El otro espacio necesario para almacenar `n` y otra información contable es $O(1)$. Por lo tanto, la cantidad total de espacio desperdiciado en un `RootishArrayStack` es $O(\sqrt{n})$.

A continuación, argumentamos que este uso del espacio es óptimo para cualquier estructura de datos que comience vacía y pueda soportar la adición de un elemento a la vez. Más precisamente, mostraremos que, en algún momento durante la adición de `n` elementos, la estructura de datos está desperdiciando una cantidad de espacio de al menos $\sqrt{n}$ (aunque puede ser desperdiciado solo por un momento).

Supongamos que comenzamos con una estructura de datos vacía y agregamos `n` elementos uno a la vez. Al final de este proceso, los `n` elementos se almacenan en la estructura y se distribuyen entre una colección de `r` bloques de memoria. Si $r \ge \sqrt{n}$, entonces la estructura de datos debe estar usando `r` punteros (o referencias) para realizar un seguimiento de estos `r` bloques, y estos punteros son espacio desperdiciado. Por otro lado, si $r < \sqrt{n}$, entonces por el principio del palomar, algún bloque debe tener un tamaño de al menos $n/r > \sqrt{n}$. Consideremos el momento en que este bloque fue asignado por primera vez. Inmediatamente después de ser asignado, este bloque estaba vacío y, por lo tanto, desperdiciaba $\sqrt{n}$ espacio. Por lo tanto, en algún momento durante la inserción de `n` elementos, la estructura de datos estaba desperdiciando $\sqrt{n}$ espacio.


### 2.6.3 Resumen

El siguiente teorema resume nuestra discusión sobre la estructura de datos `RootishArrayStack`:

**Teorema 2.5.** *Un `RootishArrayStack` implementa la interfaz `List`. Ignorando el costo de las llamadas a `grow()` y `shrink()`, un `RootishArrayStack` soporta las operaciones*

*   `get(i)` y `set(i,x)` en tiempo $O(1)$ por operación; y
*   `add(i,x)` y `remove(i)` en tiempo $O(1 + n - i)$ por operación.

*Además, comenzando con un `RootishArrayStack` vacío, cualquier secuencia de $m$ operaciones `add(i,x)` y `remove(i)` resulta en un total de $O(m)$ tiempo empleado durante todas las llamadas a `grow()` y `shrink()`.*

*El espacio (medido en palabras)<sup>3</sup> utilizado por un `RootishArrayStack` que almacena $n$ elementos es $n + O(\sqrt{n})$.*

### 2.6.4 Cálculo de Raíces Cuadradas

Un lector que haya tenido cierta exposición a los modelos de computación puede notar que el `RootishArrayStack`, como se describió anteriormente, no encaja en el modelo habitual de *word-RAM* (Sección 1.4) porque requiere calcular raíces cuadradas. La operación de raíz cuadrada generalmente no se considera una operación básica y, por lo tanto, no suele ser parte del modelo *word-RAM*.

En esta sección, mostramos que la operación de raíz cuadrada se puede implementar de manera eficiente. En particular, mostramos que para cualquier entero $x \in \{0,...,n\}$, $\lfloor\sqrt{x}\rfloor$ se puede calcular en tiempo constante, después de un preprocesamiento $O(\sqrt{n})$ que crea dos arreglos de longitud $O(\sqrt{n})$. El siguiente lema muestra que podemos reducir el problema de calcular la raíz cuadrada de $x$ a la raíz cuadrada de un valor relacionado $x'$.

**Lema 2.3.** *Sea $x \ge 1$ y sea $x' = x - a$, donde $0 \le a \le \sqrt{x}$. Entonces $\sqrt{x'} \ge \sqrt{x} - 1$.*

---
<sup>3</sup>Recuerde la Sección 1.4 para una discusión sobre cómo se mide la memoria.

*Prueba.* Es suficiente demostrar que

$$
\sqrt{x} - \sqrt{x'} \le 1 .
$$

Eleva al cuadrado ambos lados de esta desigualdad para obtener

$$
x - \sqrt{x} \ge x - 2\sqrt{x} + 1
$$

y agrupa los términos para obtener

$$
\sqrt{x} \ge 1
$$

lo cual es claramente cierto para cualquier $x \ge 1$. □

Comienza restringiendo un poco el problema, y suponga que $2^r \le x < 2^{r+1}$, de modo que $\lfloor\log x\rfloor = r$, es decir, $x$ es un entero que tiene $r + 1$ bits en su representación binaria. Podemos tomar $x' = x - (x \bmod 2^{\lfloor r/2\rfloor})$. Ahora, $x'$ satisface las condiciones del Lema 2.3, por lo que $\sqrt{x} - \sqrt{x'} \le 1$. Además, $x'$ tiene todos sus bits de orden inferior $\lfloor r/2\rfloor$ iguales a 0, por lo que solo hay

$$
2^{r+1-\lfloor r/2\rfloor} \le 4 \cdot 2^{r/2} \le 4\sqrt{x}
$$

valores posibles de $x'$. Esto significa que podemos usar un arreglo, `sqrttab`, que almacene el valor de $\lfloor\sqrt{x'}\rfloor$ para cada valor posible de $x'$. Un poco más precisamente, tenemos

$$
\text{sqrttab}[i] = \left\lfloor\sqrt{i2^{\lfloor r/2\rfloor}}\right\rfloor .
$$

De esta manera, `sqrttab[i]` está dentro de 2 de $\sqrt{x'}$ para todo $x' \in \{i2^{\lfloor r/2\rfloor}, ..., (i+1)2^{\lfloor r/2\rfloor}-1\}$. Dicho de otra manera, la entrada del arreglo $s = \text{sqrttab}[x \gg \lfloor r/2\rfloor]$ es igual a $\lfloor\sqrt{x'}\rfloor$, $\lfloor\sqrt{x'}\rfloor - 1$, o $\lfloor\sqrt{x'}\rfloor - 2$. A partir de $s$ podemos determinar el valor de $\lfloor\sqrt{x}\rfloor$ incrementando $s$ hasta que $(s+1)^2 > x$.

**FastSqrt**

```cpp
int sqrt(int x, int r) {
    int s = sqrttab[x>>r/2];
    while ((s+1)*(s+1) <= x) s++; // se ejecuta como máximo dos veces
    return s;
}
```

Ahora, esto solo funciona para $x \in [2^r,..., 2^{r+1}-1]$ y `sqrttab` es una tabla especial que solo funciona para un valor particular de $r = \lfloor\log x\rfloor$. Para superar esto, podríamos calcular $\lfloor\log n\rfloor$ diferentes arreglos `sqrttab`, uno para cada posible valor de $\lfloor\log x\rfloor$. Los tamaños de estas tablas forman una secuencia exponencial cuyo valor más grande es como máximo $4\sqrt{n}$, por lo que el tamaño total de todas las tablas es $O(\sqrt{n})$.

Sin embargo, resulta que más de un arreglo `sqrttab` es innecesario; solo necesitamos un arreglo `sqrttab` para el valor $r = \lfloor\log n\rfloor$. Cualquier valor $x$ con $\log x = r' < r$ puede ser actualizado multiplicando $x$ por $2^{r-r'}$ y usando la ecuación

$$
\sqrt{2^{r-r'}x} = 2^{(r-r')/2}\sqrt{x} .
$$

La cantidad $2^{r-r'}x$ está en el rango $[2^r,..., 2^{r+1}-1]$ por lo que podemos buscar su raíz cuadrada en `sqrttab`. El siguiente código implementa esta idea para calcular $\lfloor\sqrt{x}\rfloor$ para todos los enteros no negativos $x$ en el rango $[0,...,2^{30}-1]$ utilizando un arreglo, `sqrttab`, de tamaño $2^{16}$.

**FastSqrt**

```cpp
int sqrt(int x) {
    int rp = log(x);
    int upgrade = ((r-rp)/2) * 2;
    int xp = x << upgrade;  // xp tiene r o r-1 bits
    int s = sqrttab[xp>>(r/2)] >> (upgrade/2);
    while ((s+1)*(s+1) <= x) s++;  // se ejecuta como máximo dos veces
    return s;
}
```


Algo que hemos dado por sentado hasta ahora es la cuestión de cómo calcular $r' = \lfloor\log x\rfloor$. De nuevo, este es un problema que se puede resolver con un arreglo, `logtab`, de tamaño $2^{r/2}$. En este caso, el código es particularmente simple, ya que $\lfloor\log x\rfloor$ es solo el índice del bit 1 más significativo en la representación binaria de $x$. Esto significa que, para $x > 2^{r/2}$, podemos desplazar a la derecha los bits de $x$ en $r/2$ posiciones antes de usarlo como un índice en `logtab`. El siguiente código hace esto utilizando un arreglo `logtab` de tamaño $2^{16}$ para calcular $\lfloor\log x\rfloor$ para todos los $x$ en el rango $[1,...,2^{32}-1]$.

**FastSqrt**

```cpp
int log(int x) {
    if (x >= halfint)
        return 16 + logtab[x>>16];
    return logtab[x];
}
```

Finalmente, para completar, incluimos el siguiente código que inicializa `logtab` y `sqrttab`:

**FastSqrt**

```cpp
void inittabs() {
    sqrttab = new int[1<<(r/2)];
    logtab = new int[1<<(r/2)];
    for (int d = 0; d < r/2; d++)
        for (int k = 0; k < 1<<d; k++)
            logtab[1<<d+k] = d;
    int s = 1<<(r/4);                    // raíz cuadrada(2^(r/2))
    for (int i = 0; i < 1<<(r/2); i++) {
        if ((s+1)*(s+1) <= i << (r/2)) s++; // la raíz cuadrada aumenta
        sqrttab[i] = s;
    }
}
```

En resumen, los cálculos realizados por el método `i2b(i)` se pueden implementar en tiempo constante en la *word-RAM* utilizando $O(\sqrt{n})$ memoria adicional para almacenar los arreglos `sqrttab` y `logtab`. Estos arreglos se pueden reconstruir cuando $n$ aumenta o disminuye por un factor de dos, y el costo de esta reconstrucción se puede amortizar sobre el número de operaciones `add(i,x)` y `remove(i)` que causaron el cambio en $n$, de la misma manera que se analiza el costo de `resize()` en la implementación de `ArrayStack`.

## 2.7 Discusión y Ejercicios

La mayoría de las estructuras de datos descritas en este capítulo son folclore. Se pueden encontrar en implementaciones que datan de hace más de 30 años. Por ejemplo, las implementaciones de pilas, colas y deques, que se generalizan fácilmente a las estructuras `ArrayStack`, `ArrayQueue` y `ArrayDeque` descritas aquí, son discutidas por Knuth [46, Sección 2.2.2].

Brodnik et al. [13] parecen haber sido los primeros en describir el `RootishArrayStack` y probar una cota inferior de `sqrt(n)` como la de la Sección 2.6.2. También presentan una estructura diferente que utiliza una elección más sofisticada de tamaños de bloque para evitar calcular raíces cuadradas en el método `i2b(i)`. Dentro de su esquema, el bloque que contiene `i` es el bloque `floor(log(i + 1))`, que es simplemente el índice del bit 1 principal en la representación binaria de `i + 1`. Algunas arquitecturas de computadoras proporcionan una instrucción para calcular el índice del bit 1 principal en un entero.

Una estructura relacionada con el `RootishArrayStack` es el vector escalonado de dos niveles (*two-level tiered-vector*) de Goodrich y Kloss [35]. Esta estructura soporta las operaciones `get(i, x)` y `set(i, x)` en tiempo constante y `add(i, x)` y `remove(i)` en tiempo $O(\sqrt{n})$. Estos tiempos de ejecución son similares a lo que se puede lograr con la implementación más cuidadosa de un `RootishArrayStack` discutida en el Ejercicio 2.10.

**Ejercicio 2.1.** El método `List` `addAll(i, c)` inserta todos los elementos de la `Collection` `c` en la lista en la posición `i`. (El método `add(i, x)` es un caso especial donde `c = {x}`.) Explique por qué, para las estructuras de datos de este capítulo, no es eficiente implementar `addAll(i, c)` mediante llamadas repetidas a `add(i, x)`. Diseñe e implemente una implementación más eficiente.

**Ejercicio 2.2.** Diseñe e implemente una `RandomQueue`. Esta es una implementación de la interfaz `Queue` en la que la operación `remove()` elimina un elemento elegido uniformemente al azar entre todos los elementos actualmente en la cola. (Piense en una `RandomQueue` como una bolsa en la que podemos agregar elementos o alcanzar y eliminar ciegamente algún elemento aleatorio.) Las operaciones `add(x)` y `remove()` en una `RandomQueue` deben ejecutarse en tiempo constante por operación.

**Ejercicio 2.3.** Diseñe e implemente una `Treque` (cola de triple extremo). Esta es una implementación de `List` en la que `get(i)` y `set(i, x)` se ejecutan en tiempo constante y `add(i, x)` y `remove(i)` se ejecutan en tiempo

$$
O(1 + \min\{i, n - i, |n/2 - i|\}) .
$$

En otras palabras, las modificaciones son rápidas si están cerca de cualquiera de los extremos o cerca de la mitad de la lista.

**Ejercicio 2.4.** Implemente un método `rotate(a, r)` que "rote" el arreglo `a` de modo que `a[i]` se mueva a `a[(i + r) mod a.length]`, para todo $i \in \{0, ..., \text{a.length}\}$.

**Ejercicio 2.5.** Implemente un método `rotate(r)` que "rote" una `List` de modo que el elemento de la lista `i` se convierta en el elemento de la lista `(i + r) mod n`. Cuando se ejecuta en un `ArrayDeque`, o un `DualArrayDeque`, `rotate(r)` debe ejecutarse en tiempo $O(1 + \min\{r, n - r\})$.


**Ejercicio 2.6.** Modifique la implementación de `ArrayDeque` para que el desplazamiento realizado por `add(i,x)`, `remove(i)` y `resize()` se realice utilizando el método más rápido `System.arraycopy(s, i, d, j, n)`.

**Ejercicio 2.7.** Modifique la implementación de `ArrayDeque` para que no utilice el operador `%` (que es costoso en algunos sistemas). En su lugar, debería hacer uso del hecho de que, si `a.length` es una potencia de 2, entonces

$$
k\%\text{a.length} = k\&(\text{a.length} - 1) .
$$

(Aquí, `&` es el operador bit a bit AND.)

**Ejercicio 2.8.** Diseñe e implemente una variante de `ArrayDeque` que no realice ninguna aritmética modular. En su lugar, todos los datos se ubican en un bloque contiguo, en orden, dentro de un arreglo. Cuando los datos sobrepasan el principio o el final de este arreglo, se realiza una operación `rebuild()` modificada. El costo amortizado de todas las operaciones debería ser el mismo que en un `ArrayDeque`.

*Sugerencia:* Lograr que esto funcione se trata realmente de cómo se implementa la operación `rebuild()`. Uno querría que `rebuild()` colocara la estructura de datos en un estado en el que los datos no puedan salirse por ningún extremo hasta que se hayan realizado al menos $n/2$ operaciones.

Pruebe el rendimiento de su implementación contra el `ArrayDeque`. Optimice su implementación (usando `System.arraycopy(a, i, b, i, n)`) y vea si puede superar la implementación de `ArrayDeque`.

**Ejercicio 2.9.** Diseñe e implemente una versión de un `RootishArrayStack` que solo tenga $O(\sqrt{n})$ espacio desperdiciado, pero que pueda realizar operaciones `add(i,x)` y `remove(i,x)` en tiempo $O(1 + \min\{i, n - i\})$.

**Ejercicio 2.10.** Diseñe e implemente una versión de un `RootishArrayStack` que solo tenga $O(\sqrt{n})$ espacio desperdiciado, pero que pueda realizar operaciones `add(i, x)` y `remove(i, x)` en tiempo $O(1 + \min\{\sqrt{n}, n - i\})$. (Para una idea de cómo hacer esto, véase la Sección 3.3.)

**Ejercicio 2.11.** Diseñe e implemente una versión de un `RootishArrayStack` que solo tenga $O(\sqrt{n})$ espacio desperdiciado, pero que pueda realizar operaciones `add(i, x)` y `remove(i, x)` en tiempo $O(1 + \min\{i, \sqrt{n}, n - i\})$. (Véase la Sección 3.3 para ideas sobre cómo lograr esto.)

**Ejercicio 2.12.** Diseñe e implemente un `CubishArrayStack`. Esta estructura de tres niveles implementa la interfaz `List` utilizando $O(n^{2/3})$ espacio desperdiciado. En esta estructura, `get(i)` y `set(i, x)` toman tiempo constante; mientras que `add(i, x)` y `remove(i)` toman tiempo amortizado $O(n^{1/3})$.