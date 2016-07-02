# MultiDim Grid

A (future) efficient C++14 flatten multi dimensional container.  

## Disclaimer

This is an ongoing work. 
I would be glad to receive any contributions and remarks for improvements. 


## Quick start

Just include the `multidim_grid.hpp` header in your project.

```c++
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

    std::cout << grid.get_index_from_coord(coord);
}
```

Produces the output :

> Values : {0.5;1;1.5;2;2.5;3;3.5;4;4.5;5;5.5;6;6.5;7;7.5;8;8.5;9;9.5;10;10.5;11;11.5;12}
> Mapping index to coord :
> 0:{0;0;0;} 1:{1;0;0;} 2:{2;0;0;} 3:{3;0;0;} 4:{0;1;0;} 5:{1;1;0;} 6:{2;1;0;} 7:{3;1;0;} 8:{0;0;1;} 9:{1;0;1;} 10:{2;0;1;} 11:{3;0;1;} 12:{0;1;1;} 13:{1;1;1;} 14:{2;1;1;} 15:{3;1;1;} 16:{0;0;2;} 17:{1;0;2;} 18:{2;0;2;} 19:{3;0;2;} 20:{0;1;2;} 21:{1;1;2;} 22:{2;1;2;} 23:{3;1;2;} > 

> Grid[{2,0,2}] = 7.5


## Motivations

Using nested C++ containers such as `std::vector<std::vector<std::vector<int>>>` does not offer an 
efficient and generic solutions to manage multi-dimensional arrays. 

[boost::multi_array](http://www.boost.org/doc/libs/1_59_0/libs/multi_array/doc/user.html) might be the closest solution but does not use a flatten storage nor stack memory based storage. Some conversions and informations are missing compared to `multidim::Grid`.

**MultiDim Grid** proposes a flat uni-dimensional array which offer a generic fast access between multi-dimension coordinates and flatten index.  

## Features
- flatten storage for efficient contiguous memory access
- provide index to coordinates access with constant complexity
- provide coordinates to index access with  constant complexity
- `multidim::Grid` should behave like a C++ container as detailed in [C++ concept container](http://en.cppreference.com/w/cpp/concept/Container)
- efficient storages, such as statically-sized array, similar to C-style array


## Implementation details

- Uses static storage with `std::array`
- Uses variadic template parameters for dimensions size
- Uses compile-time computations with constexpr meta functions


## Notes

This project idea comes from a need in HPC and computational science for a multi-dimensional regular grid 
class. Discussions on stack exchange can be found [here](https://stackoverflow.com/questions/31449433/generic-c-multidimensional-iterators) and [here](https://codereview.stackexchange.com/questions/97260/generic-multi-dimension-grid-array-class-in-c). Thanks to Barry for some great insights !


## License

MultiDimGrid is licensed under the terms of the MIT license.
