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
#include <array>
#include <type_traits>
#include <utility>
#include <iterator>

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
			flatten(std::next(first), last, std::index_sequence<DIMS...>{} );
	}
};

template<typename T, size_t... DIMS>
constexpr size_t flatten_to_index(const std::array<size_t, sizeof...(DIMS)>& coord) 
{
	return flatten(coord.cbegin(), coord.cend(), std::index_sequence<DIMS...>{});
}

template<typename T, size_t... DIMS>
constexpr auto unflatten(size_t idx) -> std::array<size_t, sizeof...(DIMS)>
{
	const size_t num_dims = sizeof...(DIMS);
	auto size_per_dim = std::array<size_t, num_dims>{{DIMS...}};
	auto coord = std::array<size_t, num_dims>{};

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
 
	using ArrayValues     = std::array<T,prod_dims>;
	using ArrayCoord      = std::array<size_t,num_dims>;
 
	using value_type       = typename ArrayValues::value_type;      // T
	using reference        = typename ArrayValues::reference;       // T&
	using const_reference  = typename ArrayValues::const_reference; // const T&
	using size_type        = typename ArrayValues::size_type;       // size_t
	using iterator         = typename ArrayValues::iterator;        // random access iterator
	using const_iterator   = typename ArrayValues::const_iterator;
	using difference_type  = typename ArrayValues::difference_type;
 
	Grid() : Grid(ArrayValues{}) {} // default constructor use delegating constructor
	Grid(const ArrayValues& values) 
		: 
		// map_idx_to_coord_(fill_map_idx_to_coord())
		values_(values)
		{}
 
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

	iterator       begin()        { return values_.begin();  }
	const_iterator begin()  const { return values_.begin();  }
	const_iterator cbegin() const { return values_.cbegin(); }
	iterator       end()          { return values_.end();    }
	const_iterator end()    const { return values_.end();    }
	const_iterator cend()   const { return values_.cend();   }
 
	reference       operator[] (size_type idx)       { return values_[idx]; };
	const_reference operator[] (size_type idx) const { return values_[idx]; };
 
	reference operator[] (const ArrayCoord& coord) 
	{ 
		return values_[flatten_to_index<T,DIMS...>(coord)];
	}

	const_reference operator[] (const ArrayCoord& coord) const 
	{ 
		return values_[flatten_to_index<T,DIMS...>(coord)];
	};

	// reference operator()(std::initializer_list<size_type> l) {
	// 	return (*this)[ArrayCoord(l)];
	// }
 
 	// 	const_reference operator()(std::initializer_list<size_type> l) const { 
	// 	return const_cast<reference>(static_cast<const Grid&>(*this)(l)); 
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
	ArrayValues     values_;              // same behaviour as declaring  `float values_[meta_prod(DIMS)];`
};

} // namespace _impl_multi_grid

using _impl_multi_grid::Grid;
using _impl_multi_grid::flatten_to_index;
using _impl_multi_grid::unflatten;

} // namespace multidim


#endif // MULTIDIM_GRID_HPP_

