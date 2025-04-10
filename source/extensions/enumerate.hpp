#pragma once

#include <ranges>
#include <iterator>

namespace notsa{
namespace detail {
template <std::input_iterator ItType>
class enumerate_iterator {
public:
    using underlaying_iter_traits = typename std::iterator_traits<ItType>;

    using iterator_category = std::input_iterator_tag;
    using value_type = std::pair<ptrdiff_t, typename underlaying_iter_traits::reference>;
    using difference_type = ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

public: // Public for convenience, but don't access it pls
    ItType m_iter{};
    ptrdiff_t m_cntr{};

public:
    enumerate_iterator() = default;

    enumerate_iterator(ItType it, ptrdiff_t start) : m_iter{std::move(it)}, m_cntr{start} {}

    auto operator++() -> enumerate_iterator& { // pre-increment
        ++m_iter;
        ++m_cntr;
        return *this;
    }

    auto operator++(int) -> enumerate_iterator& { // post-increment
        auto ret = *this;
        ++*this;
        return ret;
    }

    auto operator--() -> enumerate_iterator& { // pre-decrement
        --m_iter;
        --m_cntr;
        return *this;
    }

    auto operator--(int) -> enumerate_iterator& { // post-decrement
        auto ret = *this;
        --*this;
        return ret;
    }

    template <typename OtherItType> auto operator==(const enumerate_iterator<OtherItType>& other) const {
        return m_iter == other.m_iter;
    }

    template <typename OtherItType> auto operator!=(const enumerate_iterator<OtherItType>& other) const {
        return !(*this == other);
    }

    value_type operator*() {
        return {m_cntr, *m_iter};
    }
    value_type operator*() const {
        return {m_cntr, *m_iter};
    }
};

template <std::input_iterator S, std::sentinel_for<S> E> class enumerate_object {
    ptrdiff_t m_start{};
    S m_begin{};
    E m_end{};

public:
    using iterator = enumerate_iterator<S>;
    using reverse_iterator = std::reverse_iterator<iterator>;

    using const_iterator = iterator; // TODO - Is this even needed? We aren't a container actually, so.. perhaps const iterator wouldn't make sense
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using value_type = typename iterator::value_type;

    using difference_type = ptrdiff_t;
    using size_type = size_t;

    using reference = value_type&;
    using const_reference = const value_type&;

public:
    enumerate_object(S begin, E end, ptrdiff_t start = 0) : m_start(start), m_begin(begin), m_end(end) {}

    auto begin() -> iterator {
        return {m_begin, m_start};
    }

    auto end() -> iterator {
        return {m_end, {}};
    }

    auto begin() const -> const_iterator {
        return {m_begin, m_start};
    }

    auto end() const -> const_iterator {
        return {m_end, {}};
    }

    auto rbegin() -> reverse_iterator {
        return {begin()};
    }

    auto rend() -> reverse_iterator {
        return {end()};
    }

    auto rbegin() const -> const_reverse_iterator {
        return {begin()};
    }

    auto rend() const -> const_reverse_iterator {
        return {end()};
    }
};
}; // namespace detail

template <std::input_iterator S, std::sentinel_for<S> E> auto enumerate(S begin, E end, ptrdiff_t start = 0) {
    return detail::enumerate_object<S, E>{begin, end, start};
}

template <std::ranges::input_range R> auto enumerate(R&& r, ptrdiff_t start = 0) {
    return detail::enumerate_object{std::ranges::begin(r), std::ranges::end(r), start};
}
}; // namespace notsa
