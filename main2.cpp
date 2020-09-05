#include <iostream>
#include "bits/stdc++.h"

template <typename T>
class my_vector {
    char *m_begin;
    char *m_end;
    char *m_cap;

private:

    char * alloc(size_t size) {
        return new (std::align_val_t(alignof(T))) char[size * sizeof(T)];
    }

    void dealloc(char * ptr) {
        ::operator delete[]((T *)ptr, std::align_val_t(alignof(T)));
    }

    void extend(size_t new_size){
        const size_t cur_size = size();
        auto new_mem = (T*) alloc(new_size);
        for (auto cur_new = new_mem, cur_old = (T *)m_begin ; cur_new != new_mem + cur_size; ++cur_new, ++cur_old){
            if constexpr (std::is_nothrow_move_constructible_v<T>){
                new (cur_new) T(std::move(*cur_old));
            } else {
                new (cur_new) T(*cur_old);
            }
            cur_old->~T();
        }
        dealloc(m_begin);
        m_begin = (char *)new_mem;
        m_end = (char *)(new_mem + cur_size);
        m_cap = (char *)(new_mem + new_size);
    }

    [[nodiscard]] size_t capacity() {
        return (m_cap - m_begin) / sizeof(T);
    }

public:

    using iterator = T *;
    using const_iterator = T const *;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = std::reverse_iterator<iterator>;

    [[nodiscard]] iterator begin() {
        return (T*)m_begin;
    }

    [[nodiscard]] const_iterator begin() const {
        return (T*)m_begin;
    }

    [[nodiscard]] iterator end() {
        return (T*)m_end;
    }

    [[nodiscard]] const_iterator end() const {
        return (T*)m_end;
    }

    [[nodiscard]] const_iterator cbegin() const {
        return (T*)m_begin;
    }

    [[nodiscard]] const_iterator cend() const {
        return (T*)m_end;
    }

    [[nodiscard]] reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    [[nodiscard]] const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    [[nodiscard]] reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    [[nodiscard]] const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    [[nodiscard]] const_reverse_iterator rcbegin() const {
        return const_reverse_iterator(end());
    }

    [[nodiscard]] const_reverse_iterator rcend() const {
        return const_reverse_iterator(begin());
    }

    my_vector() : m_begin(alloc(4)), m_end(m_begin), m_cap(m_begin + 4 * sizeof(T)) {}

    explicit my_vector(const size_t val) : m_begin(alloc(val)), m_end(m_begin + val * sizeof(T)), m_cap(m_begin + val * sizeof(T)) {
        for (auto cur = (T*)m_begin; cur != (T*)m_end; ++cur){
            new (cur) T();
        }
    }

    my_vector(const my_vector &other) {
        const size_t size = other.m_end - other.m_begin;
        const size_t cap = other.m_cap - other.m_begin;
        m_begin = alloc(cap / sizeof(T));
        m_end = m_begin + size;
        m_cap = m_begin + cap;
    }

    my_vector<T> &operator=(const my_vector<T> &other) noexcept {
        my_vector<T> tmp(other);
        std::swap(tmp, *this);
        return *this;
    }

    template <typename U>
    my_vector(const std::initializer_list<U> list) : m_begin(alloc(list.size())), m_end(m_begin), m_cap(m_begin + list.size() * sizeof(T)) {
        auto cur = (T*)m_begin;
        for (auto && val : list) {
            new (cur++) T(val);
        }
    }

    my_vector(my_vector<T> &&other) noexcept : m_begin(std::exchange(m_begin,other.m_begin)), m_end(std::move(other.m_end())), m_cap(std::move(other.m_end())) {}

    my_vector<T> &operator=(my_vector<T> &&other) noexcept {
        m_begin = (std::exchange(m_begin, other.m_begin));
        m_end = (std::move(other.m_end()));
        m_cap = (std::move(other.m_end()));
        return *this;
    }

    [[nodiscard]] size_t size() {
        return (m_end - m_begin) / sizeof(T);
    }

    [[nodiscard]] size_t max_size() {
        return capacity();
    }

    T & operator[](size_t num) {
        return begin()[num];
    }
    const T & operator[](size_t num) const {
        return begin()[num];
    }

    void push_back(const T & elem) {
        if (m_end == m_cap) {
            extend(2 * size());
        }
        new ((T *)m_end) T(elem);
        m_end += sizeof(T);
    }

    [[nodiscard]] T &back() {
        return *(T*)(end() - 1);
    }
    [[nodiscard]] const T &back() const {
        return *(T*)(end() - 1);
    }

    [[nodiscard]] T &front() {
        return *(T*)(begin());
    }
    [[nodiscard]] const T &front() const {
        return *(T*)(begin());
    }

    template <typename... Args>
    void emplace_back(Args&&... args) {
        if (m_end == m_cap) {
            extend(2 * size());
        }
        new ((T *)m_end) T(std::forward<Args>(args)...);
        m_end += sizeof(T);
    }

    void clear() {
        my_vector tmp;
        std::swap(tmp, *this);
    }

    void resize(size_t new_size) {
        const auto cur_size = size();
        if (cur_size == new_size){
            return;
        }
        if (cur_size > new_size) {
            auto size_tmp = cur_size;
            while (size_tmp != new_size) {
                m_end -= sizeof(T);
                --size_tmp;
                ((T *)m_end)->~T();
            }
            m_end = m_begin + sizeof(T) * size_tmp;
        } else {
            auto size_tmp = cur_size;
            if ((capacity()) >= new_size){
                while (size_tmp != new_size) {
                    new ((T *)m_end) T();
                    m_end += sizeof(T);
                    ++size_tmp;
                }
            } else {
                extend(new_size);
                while (size_tmp != new_size) {
                    new ((T *)m_end) T();
                    m_end += sizeof(T);
                    ++size_tmp;
                }
            }
            m_end = m_begin + sizeof(T) * size_tmp;
        }
    }

    void shrink_to_fit() {
        if (capacity() == size()) {
            return;
        }
        extend(size());
    }

    void reserve(size_t new_size) {
        const auto cur_cap = capacity();
        if (cur_cap > new_size) {
            return;
        }
        extend(new_size);
    }

    ~my_vector() {
        std::destroy(begin(), end());
        dealloc(m_begin);
    }

};


int main() {

    std::vector<int> ex{};
    my_vector<std::vector<int>> vec;
    my_vector<std::tuple<int, std::string, float>> vec1(1);
    my_vector<int> listik = {1, 4, 5, 6};
    vec.reserve(180);
    vec.push_back({0});
    vec.push_back({1});
    vec.push_back({2});
    vec.push_back({3});
    vec.push_back({4});
    vec.push_back({5});
    vec.push_back({6});
    vec.resize(180);
    vec1.emplace_back(2, "test", 4.4);
    std::cout << vec[0][0] << " " << vec[2][0] << std::endl;
    std::cout << std::get<1>(vec1.back()) << " " << std::get<2>(vec1.back()) << std::endl;
    std::cout << listik[3] << std::endl;
}