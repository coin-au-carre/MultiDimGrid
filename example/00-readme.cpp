#include "multidim_grid.hpp"

#include <iostream>
#include <algorithm> // std::generate


int main() {
	// Create a 3D grid with 4x2x3 vertices
	multidim::Grid<float,4,2,3> grid;

	// grid behaves like a STL container and we can fill values with std::generate
	std::generate(grid.begin(), grid.end(), [](){ static float n{0.0f}; return n+=0.5f; } );

	// We have a pretty printer
	std::cout << grid << std::endl;

	// We can access efficiently to values from appropriate coordinates
	std::cout << "grid[{{2,0,2}}] = " << grid[{{2,0,2}}] << '\n';

    // Access to indexes or coords are O(1)
    std::cout << grid.get_index_from_coord({{2,0,2}}) << '\n';

    // Indeed it is evaluated at compile time !
    static_assert(multidim::flatten_to_index<float,4,2,3>({{2,0,2}}) == 14, "Should be evaluated at compile time !");

    constexpr auto coord = multidim::unflatten<float,4,2,3>(14);
    std::cout << coord[0] << coord[1] << coord[2] << '\n';
}

