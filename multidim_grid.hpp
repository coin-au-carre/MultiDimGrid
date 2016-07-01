/*
Copyright (c) 2015 Florian Dang

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

#pragma once
 
#include <ostream>
#include <array>
#include <type_traits>
#include <utility> // std::index_sequence

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

template<typename Iter, size_t D0>
struct flatten_t<Iter, D0>
{
	static constexpr size_t compute(Iter first, Iter, std::index_sequence<D0>) 
	{
		return *first;
	}
};


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
class Grid 
{

public:
	static constexpr size_t num_dims = sizeof...(DIMS);
	static_assert(num_dims > 0, "Grid dimension needs to be > 0");
	static constexpr size_t prod_dims = meta_prod(DIMS...);
 
	using ArrayValues     = std::array<T,prod_dims>;
	using ArrayCoord      = std::array<size_t,num_dims>;
	using MapIndexToCoord = std::array<ArrayCoord,prod_dims>;
 
	using value_type       = typename ArrayValues::value_type;      // T
	using reference        = typename ArrayValues::reference;       // T&
	using const_reference  = typename ArrayValues::const_reference; // const T&
	using size_type        = typename ArrayValues::size_type;       // size_t
	using iterator         = typename ArrayValues::iterator;        // random access iterator
	using const_iterator   = typename ArrayValues::const_iterator;
	using difference_type  = typename ArrayValues::difference_type;
 
	Grid() : Grid(ArrayValues{}) {} // default constructor use delegating constructor
	Grid(const ArrayValues& values) 
		: map_idx_to_coord_(fill_map_idx_to_coord())
		, values_(values)
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

	void swap(const Grid& other) noexcept { using std::swap; swap(other.map_idx_to_coord_); swap(other.values_); }

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
		return values_[flatten(
			coord.begin(), coord.end(), std::index_sequence<DIMS...>{})];
	}
	const_reference operator[] (const ArrayCoord& coord) const 
	{ 
		return const_cast<reference>(static_cast<const Grid&>(*this)[coord]); 
	};

	// reference operator()(std::initializer_list<size_type> l) {
	// 	return (*this)[ArrayCoord(l)];
	// }
 
 	// 	const_reference operator()(std::initializer_list<size_type> l) const { 
	// 	return const_cast<reference>(static_cast<const Grid&>(*this)(l)); 
	// };

	const auto& get_coord_from_index(size_type idx) const 
	{
		return map_idx_to_coord_.at(idx);
	}
 
	size_type get_index_from_coord(const ArrayCoord& coord) const 
	{
		return flatten(coord.begin(), coord.end(), std::index_sequence<DIMS...>{});
	}
 
private:
	auto fill_map_idx_to_coord() const 
	{
		MapIndexToCoord coord;
		std::array<size_t,num_dims> size_per_dim{{DIMS...}};
		for (size_t j = 0; j < meta_prod(DIMS...); j ++) {
			size_t a = j, b = meta_prod(DIMS...), r = 0;
			for(size_t i = 0; i <= num_dims - 2; i ++) {
				b /= size_per_dim[num_dims - i - 1];
				coord[j][num_dims-i-1] = a / b;
				r = a % b;
				a = r;
			}
			coord[j][0] = r;
		}
		return coord;
	}

	// for debugging/illustration purpose following ostream operator might be removed in the future
	friend auto& operator<<(std::ostream &os, const Grid& other) 
	{
		os << "Values : {";
		for (auto&& v : other.values_)  { os << v << ";"; }
		os << "\b}\nMapping index to coord :\n";
		static size_t count{0};
		for (auto&& m : other.map_idx_to_coord_) { 
			os << count ++ << ":{";
			for (auto&& el : m) {
				os << el << ";";
			}
			os << "} "; 
		}
		return os << "\n";
	}
 
private:
	MapIndexToCoord map_idx_to_coord_;    // O(1) access flat index -> dim coordinates
	ArrayValues     values_;              // same behaviour as declaring  `float values_[meta_prod(DIMS)];`
};

} // namespace _impl_multi_grid

using _impl_multi_grid::Grid;

} // namespace multidim


#endif // MULTIDIM_GRID_HPP_

