#ifndef TREE16_HPP
#define TREE16_HPP

#include "jtypes.hpp"
#include <stdexcept>
#include <cstring>

template<typename T, unsigned int level>
class Tree16 {
    using data_type = Tree16<T, level - 1 >;
public:

    Tree16() {
        memset(data, 0, sizeof (data));
    }

    ~Tree16() {
        for (int i = 0; i < 16; i++) {
            if (data[i] != nullptr) {
                delete data[i];
            }
        }
    }

    bool isFull() {
        return full;
    }

    bool isEmpty() {
        bool empty = true;
        for (int i = 0; i < 16; i++) {
            if (data[i] != nullptr) {
                empty = false;
                break;
            }
        }
        return empty;
    }

    u4 put(T* value) {
        if (value == nullptr) {
            throw std::runtime_error("Unable to put null pointer");
        }
        if (isFull()) {
            throw std::runtime_error("Unable to put object, tree is full");
        }
        u4 out = 0;
        bool tmp_full = true;
        bool set = false;
        for (u4 i = 0; i < 16; i++) {
            data_type* child = data[i];
            if (child == nullptr || !child->isFull()) {
                if (set) {
                    tmp_full = false;
                    break;
                } else {
                    if (child == nullptr) {
                        child = new data_type();
                        data[i] = child;
                    }
                    u4 tmp_position = child->put(value);
                    out = (tmp_position << 4) | i;
                    if (!child-->isFull()) {
                        tmp_full = false;
                        break;
                    }
                    set = true;
                }
            }
        }
        full = tmp_full;
        return out;
    }

    T* put(T* value, u4 position) {
        u4 index = position & 0b1111;
        data_type* child = data[index];
        if (child == nullptr) {
            if (value != nullptr) {
                child = new data_type();
                data[index] = child;
                child->put(value, position >> 4);
            }
            return nullptr;
        }
        T* out = child->put(value, position >> 4);
        if (value == nullptr) {
            full = false;
            return out;
        }
        bool tmp_full = true;
        for (int i = 0; i < 16; i++) {
            if (data[i] == nullptr || !data[i]->isFull()) {
                full = false;
                break;
            }
        }
        full = tmp_full;
        return out;
    }

    T* get(u4 position) {
        u4 index = position & 0b1111;
        data_type* child = data[index];
        return child == nullptr ? T() : child->get(position >> 4);
    }

    T* remove(u4 position) {
        u4 index = position & 0b1111;
        data_type* child = data[index];
        T* out;
        if (child == nullptr) {
            out = nullptr;
        } else {
            out = child->remove(position >> 4);
            if (child->isEmpty()) {
                delete child;
                data[index] = nullptr;
            }
        }
        if (out != nullptr && full) {
            full = false;
        }
        return out;
    }
private:
    bool full = false;
    data_type * data[16];
    Tree16(const Tree16& orig);
};

template<typename T>
class Tree16<T, 0> {
public:

    Tree16() {
        memset(data, 0, sizeof (data));
    }
    ~Tree16() = default;

    bool isFull() const {
        return full;
    }

    bool isEmpty() const {
        bool empty = true;
        for (int i = 0; i < 16; i++) {
            if (data[i] != nullptr) {
                empty = false;
                break;
            }
        }
        return empty;
    }

    u4 put(T* value) {
        if (value == nullptr) {
            throw std::runtime_error("Unable to put null pointer");
        }
        if (isFull()) {
            throw std::runtime_error("Unable to put object, tree is full");
        }
        u4 out = 0;
        bool tmp_full = true;
        bool set = false;
        for (u4 i = 0; i < 16; i++) {
            if (data[i] == nullptr) {
                if (set) {
                    tmp_full = false;
                    break;
                } else {
                    data[i] = value;
                    out = i;
                    set = true;
                }
            }
        }
        full = tmp_full;
        return out;
    }

    T* put(T* value, u4 position) {
        u4 index = position & 0b1111;
        T* out = data[index];
        data[index] = value;
        if (value == nullptr) {
            full = false;
            return out;
        }
        bool tmp_full = true;
        for (int i = 0; i < 16; i++) {
            if (data[i] == nullptr) {
                tmp_full = false;
                break;
            }
        }
        full = tmp_full;
        return out;
    }

    T* get(u4 position) {
        u4 index = position & 0b1111;
        return data[index];
    }

    T* remove(u4 position) {
        u4 index = position & 0b1111;
        T* out = data[index];
        data[index] = nullptr;
        if (out != nullptr && full) {
            full = false;
        }
        return out;
    }
private:
    bool full = false;
    T* data[16];
    Tree16(const Tree16& orig);
};

#endif /* TREE16_HPP */