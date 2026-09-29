/*
 * utils.h
 *
 *  Created on: 2011-11-23
 *      Author: morin
 *  Corregido: parámetros por referencia constante, hashCode(int) definido,
 *             eliminada la clase dodo (marcada como terrible en el original).
 */

#ifndef UTILS_H_
#define UTILS_H_

namespace ods {

template<class T>
inline T min(const T &a, const T &b) {
    return (a < b) ? a : b;
}

template<class T>
inline T max(const T &a, const T &b) {
    return (a > b) ? a : b;
}

template<class T>
inline int compare(const T &x, const T &y) {
    if (x < y) return -1;
    if (y < x) return 1;
    return 0;
}

template<class T>
inline bool equals(const T &x, const T &y) {
    return x == y;
}

inline unsigned intValue(int x) {
    return static_cast<unsigned>(x);
}

/**
 * Hash para enteros. Mezcla de bits estilo splitmix/murmur finalizer.
 * Devuelve un int con distribución razonable.
 */
inline int hashCode(int x) {
    unsigned h = static_cast<unsigned>(x);
    h ^= h >> 16;
    h *= 0x7feb352dU;
    h ^= h >> 15;
    h *= 0x846ca68bU;
    h ^= h >> 16;
    return static_cast<int>(h);
}

template<class T> class XFastTrieNode1;

template<class T>
inline unsigned hashCode(const XFastTrieNode1<T> *u) {
    return u->prefix;
}

} /* namespace ods */

#endif /* UTILS_H_ */