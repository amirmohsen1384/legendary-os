#ifndef RESTQUEUE_HPP
#define RESTQUEUE_HPP

#include <cstdlib>
#include <utility>
#include <stdexcept>

template<typename T>
class RestQueue
{
public:
    RestQueue()
    {
        _data = nullptr;
        _capacity = 0;
        _front = 0;
        _size = 0;
        _back = 0;
    }

    explicit RestQueue(size_t reserveCount) : RestQueue()
    {
        reserve(reserveCount);
    }

    RestQueue(const RestQueue &other) : RestQueue()
    {
        reserve(other._capacity);
        for (size_t i = 0; i < other._size; ++i)
        {
            _data[i] = other.atIndex(i);
        }
        _size = other._size;
        _front = 0;
        _back = _size % _capacity;
    }

    RestQueue(RestQueue &&other) noexcept :
        _data(other._data),
        _size(other._size),
        _capacity(other._capacity),
        _front(other._front),
        _back(other._back)
    {
        other._data = nullptr;
        other._size = 0;
        other._capacity = 0;
        other._front = 0;
        other._back = 0;
    }

    ~RestQueue()
    {
        delete[] _data;
    }

    RestQueue& operator=(const RestQueue &other)
    {
        if (this != &other)
        {
            delete[] _data;
            _data = nullptr;

            reserve(other._capacity);

            for (size_t i = 0; i < other._size; ++i)
                _data[i] = other.atIndex(i);

            _size = other._size;
            _front = 0;
            _back = _size % _capacity;
        }
        return *this;
    }

    RestQueue& operator=(RestQueue &&other) noexcept
    {
        if (this != &other)
        {
            delete[] _data;

            _data = other._data;
            _size = other._size;
            _capacity = other._capacity;
            _front = other._front;
            _back = other._back;

            other._data = nullptr;
            other._capacity = 0;
            other._size = 0;
            other._front = 0;
            other._back = 0;
        }
        return *this;
    }

public:
    bool isEmpty() const noexcept
    {
        return _size == 0;
    }

    size_t size() const noexcept
    {
        return _size;
    }

    size_t capacity() const noexcept
    {
        return _capacity;
    }

    const T& front() const
    {
        if (_size == 0)
        {
            throw std::out_of_range("Empty Queue");
        }
        return _data[_front];
    }

    const T& back() const
    {
        if (_size == 0)
        {
            throw std::out_of_range("Empty Queue");
        }
        size_t idx = (_back == 0 ? _capacity - 1 : _back - 1);
        return _data[idx];
    }

    void push(const T &value)
    {
        ensureCapacityForOneMore();
        _data[_back] = value;
        _back = (_back + 1) % _capacity;
        _size++;
    }

    void push(T &&value)
    {
        ensureCapacityForOneMore();
        _data[_back] = std::move(value);
        _back = (_back + 1) % _capacity;
        _size++;
    }

    T pop()
    {
        if (_size == 0)
        {
            throw std::out_of_range("Empty Queue");
        }
        T value = std::move(_data[_front]);
        _front = (_front + 1) % _capacity;
        _size--;
        return value;
    }

    void reserve(size_t count)
    {
        if (count <= _capacity)
        {
            return;
        }

        T *newData = new T[count];

        for (size_t i = 0; i < _size; ++i)
        {
            newData[i] = std::move(atIndex(i));
        }

        delete[] _data;

        _data = newData;
        _capacity = count;
        _front = 0;
        _back = _size % _capacity;
    }

private:
    void ensureCapacityForOneMore()
    {
        if (_size < _capacity)
        {
            return;
        }
        size_t newCap = (_capacity == 0 ? 4 : _capacity * 2);
        reserve(newCap);
    }

    T& atIndex(size_t i) const
    {
        return _data[(_front + i) % _capacity];
    }

private:
    T *_data;
    size_t _size;
    size_t _back;
    size_t _front;
    size_t _capacity;
};

#endif // RESTQUEUE_HPP
