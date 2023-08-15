#pragma once

template<typename T>
class span
{
public:
    using iterator  = T*;
    using const_iterator = const T*;

    span() = default;
    span(T* data, size_t size) : m_data(data), m_size(size) { }
    span(iterator begin, iterator end) : m_data(begin), m_size(end - begin) { }

    template<typename Iterator>
    span(Iterator begin, Iterator end) : m_data(begin), m_size(end - begin) { }

    size_t size() const { return m_size; }

    T& operator[](size_t index) { return m_data[index]; }
    const T& operator[](size_t index) const { return m_data[index]; }

    iterator begin() { return m_data; }
    iterator end() { return m_data + m_size; }

    const_iterator begin() const { return m_data; }
    const_iterator end() const { return m_data + m_size; }

    const_iterator cbegin() const { return m_data; }
    const_iterator cend() const { return m_data + m_size; }
private:
    T* m_data { nullptr };
    size_t m_size { 0u };
};