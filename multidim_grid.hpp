/*
Copyright (c) 2015, 2016 Florian Dang

Permission is hereby granted, free of charge, to any person obtaining a copy of this 
software and associated documentation files (the "Software"), to deal in the Software 
without restriction, including without limitation the rights to use, copy, modify, merge, 
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons 
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or 
substantial portions of the Software.
*/


#ifndef MULTIDIM_GRID_HPP_
#define MULTIDIM_GRID_HPP_
 
#include <ostream>
#include <type_traits>
#include <utility>
#include <iterator>


#define MULTIDIM_GRID_CUSTOM_ARRAY

// We have to wait until C++17 constexpr iterators 
#ifdef MULTIDIM_GRID_CUSTOM_ARRAY

namespace multidim 
{

template<typename T, size_t N>
struct array
{
public:
    using value_type = T;
    using iterator   = T*;
    using const_iterator = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    static constexpr size_t size = N;

    constexpr iterator       begin()        { return iterator(data_); }
    constexpr const_iterator begin()  const { return const_iterator(data_); }
    constexpr const_iterator cbegin() const { return const_iterator(data_); }

    constexpr iterator       end()        { return iterator(data_ + N); }
    constexpr const_iterator end()  const { return const_iterator(data_ + N); }
    constexpr const_iterator cend() const { return const_iterator(data_ + N); }

    constexpr reference       operator[](size_type pos)       { return data_[pos]; }
    constexpr const_reference operator[](size_type pos) const { return data_[pos]; }

    bool operator==(const array& rhs) 
    {
        return data_ == rhs.data_;
    }

    T data_[N]; // must be public for aggregate initialization
};



template<class InputIterator, class Distance>
constexpr void advance_impl(InputIterator& i, Distance n, std::random_access_iterator_tag) 
{
    i += n;
}

template<class InputIterator, class Distance>
constexpr void advance_impl(InputIterator& i, Distance n, std::bidirectional_iterator_tag) 
{
    if (n < 0) 
    {
        while (n++) --i;
    } 
    else 
    {
        while (n--) ++i;
    }
}

template<class InputIterator, class Distance>
constexpr void advance_impl(InputIterator& i, Distance n, std::input_iterator_tag) {
    assert(n >= 0);
    while (n--) ++i;
}

template<class InputIterator, class Distance>
constexpr void advance (InputIterator& i, Distance n) 
{
    advance_impl(i, n,
        typename std::iterator_traits<InputIterator>::iterator_category()
    );
}


template<class ForwardIt>
constexpr ForwardIt next(
    ForwardIt it,
    typename std::iterator_traits<ForwardIt>::difference_type n = 1)
{
    advance(it, n);
    return it;
}


} // namespace multidim

#else

#include <array>
using array = std::array; // C++17
using next = std::next;  // C++17

#endif

namespace multidim 
{

namespace _impl_multi_grid 
{

// meta functions
template<typename T>
constexpr T meta_prod(T x) { return x; }

template<typename T, typename... Ts>
constexpr T meta_prod(T x, Ts... xs) { return x * meta_prod(xs...); }

template<typename Iter, size_t D0, size_t... DIMS> struct flatten_t;

template <typename Iter, size_t D0>
constexpr size_t flatten(Iter first, std::index_sequence<D0> seq) 
{
    return flatten_t<Iter, D0>::compute(first, seq);
}

template <typename Iter, size_t D0, size_t... DIMS>
constexpr size_t flatten(Iter first, Iter last, std::index_sequence<D0, DIMS...> seq) 
{
    return flatten_t<Iter, D0, DIMS...>::compute(first, last, seq);
}

template<typename Iter, size_t D0>
struct flatten_t<Iter, D0>
{
    static constexpr size_t compute(Iter first, Iter, std::index_sequence<D0>) 
    {
        return *first;
    }
};

template <typename Iter, size_t D0, size_t... DIMS>
struct flatten_t
{
    static constexpr size_t compute(Iter first, Iter last, std::index_sequence<D0, DIMS...>) 
    {
        return *first * meta_prod(DIMS...) + 
            flatten(next(first), last, std::index_sequence<DIMS...>{} );
    }
};

template<typename T, size_t... DIMS>
constexpr size_t flatten_to_index(const array<size_t, sizeof...(DIMS)>& coord) 
{
    // The problem here is that begin and end are constexpr since C++17 only...
    return flatten(coord.cbegin(), coord.cend(), std::index_sequence<DIMS...>{});
}

template<typename T, size_t... DIMS>
constexpr auto unflatten(size_t idx) -> array<size_t, sizeof...(DIMS)>
{
    const size_t num_dims = sizeof...(DIMS);
    constexpr auto size_per_dim = array<size_t, num_dims>{{DIMS...}};
    auto coord = array<size_t, num_dims>{};

    size_t prod_dims = meta_prod(DIMS...);
    size_t r = 0;
    for(size_t i = 0; i <= num_dims - 2; i ++) {
        prod_dims /= size_per_dim[num_dims - i - 1];
        coord[num_dims-i-1] = idx / prod_dims;
        r = idx % prod_dims;
        idx = r;
    }
    coord[0] = r;

    return coord;
}


template<typename T, size_t... DIMS>
class Grid 
{
public:
    static constexpr size_t num_dims = sizeof...(DIMS);
    static constexpr size_t prod_dims = meta_prod(DIMS...);
    static_assert(num_dims  > 0, "Grid dimension needs to be > 0");
    static_assert(prod_dims > 0, "All dimension size must be > 0");
 
    using ArrayValues     = array<T,prod_dims>;
    using ArrayCoord      = array<size_t,num_dims>;
 
    using value_type       = typename ArrayValues::value_type;      // T
    using reference        = typename ArrayValues::reference;       // T&
    using const_reference  = typename ArrayValues::const_reference; // const T&
    using size_type        = typename ArrayValues::size_type;       // size_t
    using iterator         = typename ArrayValues::iterator;        // random access iterator
    using const_iterator   = typename ArrayValues::const_iterator;
    using difference_type  = typename ArrayValues::difference_type;
 
    Grid() : Grid(ArrayValues{}) {}
    Grid(const ArrayValues& values) : values_(values) {}
 
    Grid(const Grid&) = default;
    Grid& operator=(const Grid&) noexcept = default;
    Grid(Grid&&) = default;
    Grid& operator=(Grid&&) noexcept = default;
    ~Grid() = default;

    bool operator==(const Grid& other) { return (values_ == other.values_); }
    bool operator!=(const Grid& other) { return (values_ != other.values_); }

    size_type size() { return prod_dims; }
    size_type max_size() { return values_.max_size(); }

    void swap(const Grid& other) noexcept { values_.swap(other.values_); }

    constexpr iterator       begin()        { return values_.begin();  }
    constexpr const_iterator begin()  const { return values_.begin();  }
    constexpr const_iterator cbegin() const { return values_.cbegin(); }

    constexpr iterator       end()          { return values_.end();    }
    constexpr const_iterator end()    const { return values_.end();    }
    constexpr const_iterator cend()   const { return values_.cend();   }
 
    constexpr reference       operator[] (size_type idx)       { return values_[idx]; };
    constexpr const_reference operator[] (size_type idx) const { return values_[idx]; };
 
    constexpr reference operator[] (const ArrayCoord& coord) 
    { 
        return values_[flatten_to_index<T,DIMS...>(coord)];
    }

    constexpr const_reference operator[] (const ArrayCoord& coord) const 
    { 
        return values_[flatten_to_index<T,DIMS...>(coord)];
    };

    // reference operator()(std::initializer_list<size_type> l) {
    //  return (*this)[ArrayCoord(l)];
    // }
 
    //  const_reference operator()(std::initializer_list<size_type> l) const { 
    //  return const_cast<reference>(static_cast<const Grid&>(*this)(l)); 
    // };

    constexpr auto get_coord_from_index(size_type idx) const -> ArrayCoord
    {
        return unflatten<T,DIMS...>(idx);
    }
 
    constexpr size_type get_index_from_coord(const ArrayCoord& coord) const 
    {
        return flatten(coord.begin(), coord.end(), std::index_sequence<DIMS...>{});
    }
 
private:
    // for debugging/illustration purpose following ostream operator might be removed in the future
    friend auto& operator<<(std::ostream &os, const Grid& rhs) 
    {
        os << "Values : [ ";
        std::copy(rhs.values_.begin(), rhs.values_.end(), 
            std::ostream_iterator<typename Grid::value_type>(os, " "));

        os << "]\nMapping index to coord :\n";
        for(size_t idx = 0; idx < rhs.prod_dims; idx ++) {
            os << idx << ":{";
            auto coord = rhs.get_coord_from_index(idx);
            std::copy(coord.begin(), coord.end(), 
                std::ostream_iterator<typename ArrayCoord::value_type>(os, ","));
            os << "} ";
        }

        return os << "\n";
    }
 
private:
    ArrayValues     values_;
};

} // namespace _impl_multi_grid

using _impl_multi_grid::Grid;
using _impl_multi_grid::flatten_to_index;
using _impl_multi_grid::unflatten;

} // namespace multidim


#endif // MULTIDIM_GRID_HPP_

