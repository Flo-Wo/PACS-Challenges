# TODO:
- test the compressions (both)
- test setter/getter method
- norm implementation
    - test norms
    - use better power function (non-default)

- documentation -> doxygen

# Compile and run

For testing
```sh
./main small_example.mtx
```


# Questions
- two specializations instead of constexpr all the time?

- resize not needed, since size is internally not stored, could be more
efficient, if the size of the matrix is a class attribute and is updated
every time compression or decompression is executed

# Design Decisions
- no bound checks for the getter methods, to achieve maximal performance
- command line arguments

- not using upper and lower bound, this included more searches and and an error with the comparison operator
manually checking
    - other considered options: computing the number of columns and searching for all elements -> lots of conditional jumps in the loop
    - manually increasing the row number, also one conditional jump
my solution needs zero jumps and only one loop, no comparison operations (uncomment the debug lines in the comparison operator for the mapping)

improvements:
- one could compute the indices of where to look based on a constexpr, this would condense the
code into one function, but it would be even harder to read

# Improvements
- save the number of rows and columns as an attribute in the class

# Learning
- class members can only be template specialized, if the class itself is already specialized