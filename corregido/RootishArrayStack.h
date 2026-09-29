/*
 * RootishArrayStack.h
 *
 *  Created on: 2011-11-23
 *      Author: morin
 *  Mejorado: RAII con unique_ptr, Regla de Cinco, const-correctness,
 *            validación de índices, i2b entero-seguro, shrink robusto.
 */

#ifndef ROOTISHARRAYSTACK_H_
#define ROOTISHARRAYSTACK_H_

#include <cassert>
#include <cmath>
#include <memory>
#include <utility>

#include "ArrayStack.h"

namespace ods
{

    template <class T>
    class RootishArrayStack
    {
    protected:
        using Block = std::unique_ptr<T[]>;
        ArrayStack<Block> blocks;
        int n;

        int i2b(int i) const;
        void grow();
        void shrink();

    public:
        RootishArrayStack();
        RootishArrayStack(const RootishArrayStack<T> &other);
        RootishArrayStack(RootishArrayStack<T> &&other) noexcept;
        RootishArrayStack<T> &operator=(const RootishArrayStack<T> &other);
        RootishArrayStack<T> &operator=(RootishArrayStack<T> &&other) noexcept;
        virtual ~RootishArrayStack() = default;

        int size() const;
        T get(int i) const;
        T set(int i, const T &x);

        virtual void add(int i, const T &x);
        virtual void add(const T &x) { add(size(), x); }
        virtual T remove(int i);
        virtual void clear();
    };

    /* ---------- constructores / asignaciones ---------- */

    template <class T>
    RootishArrayStack<T>::RootishArrayStack() : n(0) {}

    template <class T>
    RootishArrayStack<T>::RootishArrayStack(const RootishArrayStack<T> &other)
        : n(other.n)
    {
        int nb = other.blocks.size();
        for (int b = 0; b < nb; b++)
        {
            int len = b + 1;
            Block blk(new T[len]);
            for (int j = 0; j < len; j++)
                blk[j] = other.blocks.get(b)[j];
            blocks.add(std::move(blk));
        }
    }

    template <class T>
    RootishArrayStack<T>::RootishArrayStack(RootishArrayStack<T> &&other) noexcept
        : blocks(std::move(other.blocks)), n(other.n)
    {
        other.n = 0;
    }

    template <class T>
    RootishArrayStack<T> &
    RootishArrayStack<T>::operator=(const RootishArrayStack<T> &other)
    {
        if (this != &other)
        {
            RootishArrayStack<T> tmp(other); // copia
            *this = std::move(tmp);          // movimiento
        }
        return *this;
    }

    template <class T>
    RootishArrayStack<T> &
    RootishArrayStack<T>::operator=(RootishArrayStack<T> &&other) noexcept
    {
        if (this != &other)
        {
            blocks = std::move(other.blocks);
            n = other.n;
            other.n = 0;
        }
        return *this;
    }

    /* ---------- operaciones básicas ---------- */

    template <class T>
    inline int RootishArrayStack<T>::size() const { return n; }

    /**
     * Devuelve el menor b tal que  b(b+1)/2 <= i < (b+1)(b+2)/2.
     * Usa la fórmula cerrada y luego corrige posibles errores de redondeo
     * flotante con dos bucles (normalmente cero iteraciones).
     */
    template <class T>
    inline int RootishArrayStack<T>::i2b(int i) const
    {
        assert(i >= 0);
        int b = static_cast<int>(std::ceil((-3.0 + std::sqrt(9.0 + 8.0 * i)) / 2.0));
        if (b < 0)
            b = 0;
        while (b > 0 && b * (b + 1) / 2 > i)
            b--;
        while ((b + 1) * (b + 2) / 2 <= i)
            b++;
        return b;
    }

    template <class T>
    inline T RootishArrayStack<T>::get(int i) const
    {
        assert(0 <= i && i < n);
        int b = i2b(i);
        int j = i - b * (b + 1) / 2;
        return blocks.get(b)[j];
    }

    template <class T>
    inline T RootishArrayStack<T>::set(int i, const T &x)
    {
        assert(0 <= i && i < n);
        int b = i2b(i);
        int j = i - b * (b + 1) / 2;
        T &ref = blocks.get(b)[j];
        T y = ref;
        ref = x;
        return y;
    }

    /* ---------- gestión de bloques ---------- */

    template <class T>
    void RootishArrayStack<T>::grow()
    {
        int r = blocks.size();
        // Nuevo bloque de tamaño r+1
        blocks.add(Block(new T[r + 1]));
    }

    template <class T>
    void RootishArrayStack<T>::shrink()
    {
        int r = blocks.size();
        // Necesitamos al menos 2 bloques para poder eliminar uno
        while (r >= 2 && (r - 2) * (r - 1) / 2 >= n)
        {
            blocks.remove(blocks.size() - 1); // unique_ptr libera el bloque
            r--;
        }
    }

    /* ---------- operaciones de lista ---------- */

    template <class T>
    void RootishArrayStack<T>::add(int i, const T &x)
    {
        assert(0 <= i && i <= n);
        int r = blocks.size();
        if (r * (r + 1) / 2 < n + 1)
            grow();
        n++;
        for (int j = n - 1; j > i; j--)
            set(j, get(j - 1));
        set(i, x);
    }

    template <class T>
    T RootishArrayStack<T>::remove(int i)
    {
        assert(0 <= i && i < n);
        T x = get(i);
        for (int j = i; j < n - 1; j++)
            set(j, get(j + 1));
        n--;
        shrink();
        return x;
    }

    template <class T>
    void RootishArrayStack<T>::clear()
    {
        while (blocks.size() > 0)
            blocks.remove(blocks.size() - 1); // libera cada bloque
        n = 0;
    }

} /* namespace ods */

#endif /* ROOTISHARRAYSTACK_H_ */