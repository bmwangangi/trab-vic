## Custom Memory Allocator

### Overview
This project aims to develop a custom memory allocator for user-level processes. The allocator provides functionalities similar to the standard `malloc()` and `free()` functions but with additional features and optimizations.

### Objectives
The main objectives of this project are outlined as follows:
1. **Understanding Memory Allocation**: Gain a deep understanding of memory management techniques and the implementation of memory allocators.
2. **Performance Optimization**: Implement memory allocation algorithms that are efficient in terms of time and space complexity.
3. **Shared Library Creation**: Develop a shared library that can be dynamically linked to other programs for memory management.

### Features
#### Memory Allocation and Deallocation
- Provides functions `mem_alloc()` and `mem_free()` for allocating and releasing memory, respectively.
- These functions behave similarly to `malloc()` and `free()` from the standard C library but offer additional features and optimizations.

#### Supporting Functionality
- Includes a `mem_dump()` function for debugging purposes, which prints detailed information about allocated and free memory regions.
- Enables developers to analyze memory usage and identify potential memory leaks or fragmentation issues.

#### Initialization
- Offers the `mem_init()` function to initialize the memory allocator with a specified size of memory region.
- Ensures proper initialization of data structures and allocation of memory from the operating system.

#### Error Handling
- Defines error codes such as `E_NO_SPACE`, `E_CORRUPT_FREESPACE`, etc., to handle memory allocation errors gracefully.
- Provides error checking mechanisms in each function to detect and report errors to the caller, ensuring robustness and reliability.

#### Memory Allocation Styles
- Supports different allocation strategies such as best-fit, worst-fit, and first-fit.
- Allows users to specify the desired allocation style when calling `mem_alloc()`, providing flexibility and customization options.

### How to Compile and Execute
1. **Compilation**:
   - Utilize the provided Makefile to compile the code:
     ```
     make
     ```
2. **Execution**:
   - Run the compiled executable:
     ```
     ./mem.out
     ```

### Files Included
- `mem.c`: Contains the implementation of the custom memory allocator, including memory allocation algorithms and supporting functions.
- `mem.h`: Header file with function prototypes, error code definitions, and data structure declarations.
- `Makefile`: Makefile for compiling the code and generating the executable.
- `README.md`: Documentation file providing comprehensive information about the project, including usage instructions and features.

### Usage
1. **Initialization**:
   - Call `mem_init()` to initialize the memory allocator with a specified size of memory region.
2. **Memory Allocation**:
   - Use `mem_alloc()` to allocate memory for data structures, objects, or arrays.
3. **Memory Deallocation**:
   - Release allocated memory using `mem_free()` when it is no longer needed to prevent memory leaks.
4. **Debugging**:
   - Utilize the `mem_dump()` function for debugging purposes, analyzing memory usage, and identifying potential issues such as memory leaks or fragmentation.


