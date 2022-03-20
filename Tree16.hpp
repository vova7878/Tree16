#ifndef TREE16_HPP
#define TREE16_HPP

#include "jtypes.hpp"
#include <stdexcept>

u2 numberOfTrailingZeros(u2 i) {
    // HD, Figure 5-14
    if (i == 0) return 16;
    u2 y;
    u1 n = 15;
    y = i << 8;
    if (y) {
        n -= 8;
        i = y;
    }
    y = i << 4;
    if (y) {
        n -= 4;
        i = y;
    }
    y = i << 2;
    if (y) {
        n -= 2;
        i = y;
    }
    y = i << 1;
    return n - (y >> 15);
}

inline bool getFlag(u2 data, u4 index) {
    return data & 1 << index;
}

inline void setFlag(u2& data, u4 index, bool value) {
    data = value ? data | 1 << index : data & ~(1 << index);
}

template<typename T, T nullValue>
class Tree16;

template<typename T, T nullValue, unsigned int level>
class Tree16_Impl {
    using data_type = Tree16_Impl<T, nullValue, level - 1 >;
public:

    Tree16_Impl() = default;
    ~Tree16_Impl() = default;

    inline bool isFull() const {
        u2 tmp = ~full;
        return !tmp;
    }

    inline bool isEmpty() const {
        return !full;
    }

    u4 put(T value) {
        u4 index = numberOfTrailingZeros(~full);
        if (index == 16) {
            throw std::runtime_error("Unable to put object, tree is full");
        }
        if (!getFlag(exists, index)) {
            if (value == nullValue) {
                return index;
            }
            data[index].init();
            int out = data[index].put(value);
            setFlag(exists, index, true);
            setFlag(full, index, false);
            return (out << 4) | index;
        }
        int out = data[index].put(value);
        setFlag(full, index, data[index].isFull());
        return (out << 4) | index;
    }

    T put(T value, u4 position) {
        u4 index = position & 0b1111;
        if (!getFlag(exists, index)) {
            if (value == nullValue) {
                return nullValue;
            }
            data[index].init();
            T out = data[index].put(value, position >> 4);
            setFlag(exists, index, true);
            setFlag(full, index, false);
            return out;
        }
        T out = data[index].put(value, position >> 4);
        if (data[index].isEmpty()) {
            data[index].exit();
            setFlag(exists, index, false);
            setFlag(full, index, false);
            return out;
        }
        setFlag(full, index, data[index].isFull());
        return out;
    }

    T get(u4 position) const {
        u4 index = position & 0b1111;
        return getFlag(exists, index) ? data[index].get(position >> 4) : nullValue;
    }

    inline T remove(u4 position) {
        return put(nullValue, position);
    }

private:

    void init() {
        full = 0;
        exists = 0;
        data = reinterpret_cast<data_type*> (malloc(sizeof (data_type) * 16));
        if (data == nullptr) {
            throw std::bad_alloc();
        }
    }

    void exit() {
        if (data != nullptr) {
            for (int i = 0; i < 16; i++) {
                if (getFlag(exists, i)) {
                    data[i].exit();
                }
            }
            free(data);
        }
    }

    Tree16_Impl(const Tree16_Impl& orig);

    data_type *data;
    u2 full;
    u2 exists;

    friend Tree16_Impl<T, nullValue, level + 1 >;
    friend Tree16<T, nullValue>;
};

template<typename T, T nullValue>
class Tree16_Impl<T, nullValue, 0> {
public:

    Tree16_Impl() = default;
    ~Tree16_Impl() = default;

    inline bool isFull() const {
        u2 tmp = ~full;
        return !tmp;
    }

    inline bool isEmpty() const {
        return !full;
    }

    u4 put(T value) {
        u4 index = numberOfTrailingZeros(~full);
        if (index == 16) {
            throw std::runtime_error("Unable to put object, tree is full");
        }
        data[index] = value;
        setFlag(full, index, value != nullValue);
        return index;
    }

    T put(T value, u4 index) {
        T out = get(index);
        data[index] = value;
        setFlag(full, index, value != nullValue);
        return out;
    }

    inline T get(u4 index) const {
        return getFlag(full, index) ? data[index] : nullValue;
    }

    inline T remove(u4 index) {
        return put(nullValue, index);
    }

private:

    void init() {
        full = 0;
        data = reinterpret_cast<T*> (malloc(sizeof (T) * 16));
        if (data == nullptr) {
            throw std::bad_alloc();
        }
    }

    void exit() {
        if (data != nullptr) {
            free(data);
        }
    }

    Tree16_Impl(const Tree16_Impl& orig);

    T *data;
    u2 full;

    friend Tree16_Impl<T, nullValue, 1>;
};

template<typename T, T nullValue>
class Tree16 : public Tree16_Impl<T, nullValue, 7> {
public:

    Tree16() {
        Tree16_Impl<T, nullValue, 7>::init();
    }

    ~Tree16() {
        Tree16_Impl<T, nullValue, 7>::exit();
    }

private:

    Tree16(const Tree16& orig);
};

#endif /* TREE16_HPP */
