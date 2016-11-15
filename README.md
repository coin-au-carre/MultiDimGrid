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

int main() 
{
    // Create a 3D grid with 4x2x3 vertices
    multidim::Grid<float,4,2,3> grid;

    // grid behaves like a STL container and we can fill values with std::generate
    std::generate(grid.begin(), grid.end(), [](){ static float n{0.0f}; return n+=0.5f; } );

    // We have a pretty printer
    std::cout << grid << '\n';

    // We can access efficiently to values from appropriate coordinates
    std::cout << "grid[{{2,0,2}}] = " << grid[{{2,0,2}}] << '\n';

    // Access to indexes or coords are O(1)
    std::cout << "grid.flatten({{2,0,2}}) =" << grid.flatten({{2,0,2}}) << '\n';

    // Indeed it is evaluated at compile time !
    static_assert(grid.flatten({{2,0,2}}) == 14, "Should be evaluated at compile time !");

    constexpr auto coord = grid.unflatten(14);
    std::cout << "grid.unflatten(14) = {" << coord[0] << "," << coord[1] << "," << coord[2] << "}\n";
}
```

Produces the output :

> Values : [ 0.5 1 1.5 2 2.5 3 3.5 4 4.5 5 5.5 6 6.5 7 7.5 8 8.5 9 9.5 10 10.5 11 11.5 12 ]  
> Mapping index to coord :  
> 0:{0,0,0,}:0 1:{0,0,1,}:1 2:{0,0,2,}:2 3:{0,1,0,}:3 4:{0,1,1,}:4 5:{0,1,2,}:5 6:{1,0,0,}:6 7:{1,0,1,}:7 8:{1,0,2,}:8 9:{1,1,0,}:9 10:{1,1,1,}:10 11:{1,1,2,}:11 12:{2,0,0,}:12 13:{2,0,1,}:13 14:{2,0,2,}:14 15:{2,1,0,}:15 16:{2,1,1,}:16 17:{2,1,2,}:17 18:{3,0,0,}:18 19:{3,0,1,}:19 20:{3,0,2,}:20 21:{3,1,0,}:21 22:{3,1,1,}:22 23:{3,1,2,}:23  
> grid[{{2,0,2}}] = 7.5  
> grid.flatten({{2,0,2}}) =14  
> grid.unflatten(14) = {2,0,2}


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

- Uses static storage with a custom array for accessing `constexpr` possibilities instead of `std::array` (we have to wait C++17 for that).
- Uses variadic template parameters for dimensions size
- Uses compile-time computations with constexpr meta functions


## Notes

This project idea comes from a need in HPC and computational science for a multi-dimensional regular grid 
class. Discussions on stack exchange can be found [here](https://stackoverflow.com/questions/31449433/generic-c-multidimensional-iterators) and [here](https://codereview.stackexchange.com/questions/97260/generic-multi-dimension-grid-array-class-in-c). Thanks to Barry for some great insights !


