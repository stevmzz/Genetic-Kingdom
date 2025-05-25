#pragma once

#include <memory>
#include <stdexcept>
#include <algorithm>
#include <initializer_list>

template<typename T>
class DynamicArray {
private:
    T* data; // puntero a los datos del arreglo
    size_t capacity; // capacidad total del arreglo
    size_t currentSize; // tamano actual del arreglo

    // redimensiona el arreglo cuando se necesita mas espacio
    void resize() {
        capacity = capacity == 0 ? 1 : capacity * 2;
        T* newData = new T[capacity];
        for (size_t i = 0; i < currentSize; i++) {
            newData[i] = std::move(data[i]);
        }
        delete[] data;
        data = newData;
    }



public:
    // constructor por defecto
    DynamicArray() : data(nullptr), capacity(0), currentSize(0) {}



    // constructor con tamano inicial
    DynamicArray(size_t initialSize) : capacity(initialSize), currentSize(initialSize) {
        data = new T[capacity];
    }



    // constructor de copia
    DynamicArray(const DynamicArray& other) : capacity(other.capacity), currentSize(other.currentSize) {
        data = new T[capacity];
        for (size_t i = 0; i < currentSize; i++) {
            data[i] = other.data[i];
        }
    }



    // constructor de movimiento
    DynamicArray(DynamicArray&& other) noexcept
        : data(other.data), capacity(other.capacity), currentSize(other.currentSize) {
        other.data = nullptr;
        other.capacity = 0;
        other.currentSize = 0;
    }



    // destructor que libera la memoria
    ~DynamicArray() {
        delete[] data;
    }



    // operador de asignacion por copia
    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            delete[] data;
            capacity = other.capacity;
            currentSize = other.currentSize;
            data = new T[capacity];
            for (size_t i = 0; i < currentSize; i++) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }



    // anade un elemento al final por copia
    void push_back(const T& value) {
        if (currentSize >= capacity) {
            resize();
        }
        data[currentSize++] = value;
    }



    // anade un elemento al final por movimiento
    void push_back(T&& value) {
        if (currentSize >= capacity) {
            resize();
        }
        data[currentSize++] = std::move(value);
    }



    // construye un elemento al final
    template<typename... Args>
    void emplace_back(Args&&... args) {
        if (currentSize >= capacity) {
            resize();
        }
        data[currentSize++] = T(std::forward<Args>(args)...);
    }



    // obtiene el tamano actual
    size_t size() const {
        return currentSize;
    }



    // verifica si esta vacio
    bool empty() const {
        return currentSize == 0;
    }



    // limpia todos los elementos
    void clear() {
        currentSize = 0;
    }



    // operador de acceso por indice
    T& operator[](size_t index) {
        return data[index];
    }



    // operador de acceso por indice constante
    const T& operator[](size_t index) const {
        return data[index];
    }



    // acceso con verificacion de limites
    T& at(size_t index) {
        if (index >= currentSize) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }



    // acceso al primer elemento
    T& front() { return data[0]; }



    // acceso constante al primer elemento
    const T& front() const { return data[0]; }



    // acceso al ultimo elemento
    T& back() { return data[currentSize - 1]; }



    // acceso constante al ultimo elemento
    const T& back() const { return data[currentSize - 1]; }



    // elimina un elemento por indice
    void erase(size_t index) {
        if (index >= currentSize) return;
        for (size_t i = index; i < currentSize - 1; i++) {
            data[i] = std::move(data[i + 1]);
        }
        currentSize--;
    }



    // iterador al inicio
    T* begin() { return data; }



    // iterador al final
    T* end() { return data + currentSize; }



    // iterador constante al inicio
    const T* begin() const { return data; }



    // iterador constante al final
    const T* end() const { return data + currentSize; }



    // reserva capacidad
    void reserve(size_t newCapacity) {
        if (newCapacity > capacity) {
            T* newData = new T[newCapacity];
            for (size_t i = 0; i < currentSize; i++) {
                newData[i] = std::move(data[i]);
            }
            delete[] data;
            data = newData;
            capacity = newCapacity;
        }
    }



    // cambia el tamano del arreglo
    void resize(size_t newSize) {
        if (newSize > capacity) {
            reserve(newSize);
        }
        currentSize = newSize;
    }
};