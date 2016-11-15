#include "multidim_grid.hpp"

#include <iostream>
#include <algorithm>

int main() {
	// Create a 4D grid with 3x2x3x5 vertices
	multidim::Grid<float,3,2,3,5> grid;
	// grid behaves like a STL container and we can fill values with std::generate
	std::generate(grid.begin(), grid.end(), [](){ static float n{0.0f}; return n+=0.5f; } );
	std::cout << grid << std::endl;

	// get coordinates from index 43
	auto coord = grid.unflatten(43);
	std::cout << "unflatten(43) = {";
	for (auto el : coord) {
		std::cout << el << ";";
	}
	std::cout << "}" << std::endl;
	// and vice versa
	std::cout << "grid.flatten({{2,0,2,3}}) = " << grid.flatten({{2,0,2,3}}) << '\n';
	// print value at specific coordinates
	std::cout << "Grid[{2,0,2,3}] = " << grid[{{2,0,2,3}}] << std::endl;
	// print value at specific index
	std::cout << "Grid[42] = " << grid[42] << "\n\n";

	multidim::Grid<float, 2, 2> little_grid;
	// std::cout << little_grid << std::endl;

	std::cout << "sizeof(MultiGrid<5,int,2,2,2,2,2>) = " << sizeof(multidim::Grid<int,2,2,2,2,2>) << '\n';

	auto neighbors = grid.stencil<1>(4);
	for (auto n : neighbors) std::cout << n << " ";
}

