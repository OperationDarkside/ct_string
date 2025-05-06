# ct_string: A C++20 Compile-Time String Library

`ct_string` is a lightweight, header-only (via C++20 module interface) library that provides a `constexpr`-friendly string type. It's designed to allow for string manipulations, particularly concatenation, at compile time, addressing limitations of `std::string_view` (which cannot own concatenated data) and `std::string` (which typically involves heap allocations unsuitable for `constexpr` contexts before C++20/23's fuller support).

This library is ideal for scenarios like constructing file paths, generating lookup keys, or any situation where string content can be determined entirely at compile time.

## Features

*   **Compile-Time Operations:** Construct, concatenate, and compare strings entirely at compile time.
*   **Deduction Guides:** Easy construction from string literals (e.g., `ct_string s = "Hello";`).
*   **Seamless Conversions:**
    *   Implicit `constexpr` conversion to `std::string_view`.
    *   Implicit `constexpr` conversion to `const char*`.
    *   Implicit (runtime) conversion to `std::string`.
*   **Standard-like API:** Provides `size()`, `empty()`, `c_str()`, `operator[]`, iterators.
*   **Comparison Operators:** Full set of `constexpr` comparison operators (`==`, `!=`, C++20 `operator<=>`).
*   **C++20 Module:** Packaged as a C++20 module (`ct_string.ixx`) for cleaner integration.
*   **No Heap Allocations:** All string data is stored within the `ct_string` object itself (on the stack or in static storage).

## Requirements

*   A C++20 compliant compiler that supports C++20 modules. Tested/Known to work with:
    *   MSVC (Visual Studio 2022 v17.13.19+ recommended)
    *   (TODO) Clang (15+ with appropriate flags, support is rapidly improving)
    *   (TODO) GCC (12+ with appropriate flags, support is rapidly improving)
*   CMake (version 3.25+ recommended for smoother module support, 3.28+ is even better).

## Installation / Integration

Since `ct_string` is provided as a single C++20 module interface file (`ct_string.ixx`), integration is straightforward.

### Using CMake

1.  Copy `ct_string.ixx` into your project (e.g., in a `modules/` or `include/` directory).
2.  Add the module to your CMake target:

    ```cmake
    # In your CMakeLists.txt

    # If ct_string.ixx is providing an interface for other libraries/executables
    add_library(my_awesome_lib ...) # Or add_executable(my_app ...)

    # Create a FILE_SET for your module interface units
    # Adjust the path to where you placed ct_string.ixx
    target_sources(my_awesome_lib PRIVATE
        FILE_SET CXX_MODULES FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/ct_string.ixx
        # Add other .ixx files here if you have more
    )

    # If you have an executable that directly uses ct_string:
    # add_executable(my_app main.cpp)
    # target_sources(my_app PRIVATE
    #     FILE_SET CXX_MODULES FILES
    #         ${CMAKE_CURRENT_SOURCE_DIR}/path/to/ct_string.ixx
    # )
    #
    # If my_app links against my_awesome_lib which exports ct_string,
    # then my_app might not need to list ct_string.ixx directly in its
    # sources if the dependency is handled correctly.
    #
    # For CMake 3.28+ consumers of a library exporting a module:
    # target_sources(my_consumer_app PRIVATE
    #    FILE_SET CXX_MODULE_IMPORTS FILES
    #        ${CMAKE_CURRENT_SOURCE_DIR}/path/to/ct_string.ixx
    # )
    # The exact CMake incantation can vary based on project structure and
    # whether ct_string is part of a compiled library or used directly.
    ```

3.  Ensure your compiler flags are set for C++20 and module support. CMake often handles this based on `CMAKE_CXX_STANDARD`. For MSVC, `/std:c++20` (or `/std:c++latest`) and `/experimental:module` (if needed, though often implied by `.ixx` files) are key.

### Manual Integration (Simpler Projects)

For very simple projects or those not using CMake, you can:
1.  Copy `ct_string.ixx` into your project.
2.  Ensure your compiler is invoked with C++20 and module support enabled, and that it knows how to find and compile `ct_string.ixx` before compiling files that import it. The exact commands depend on your compiler.

## Usage

```cpp
// main.cpp
#include <iostream>
#include <string>
#include <string_view>

import ct_string; // Import the module

// For compile-time assertions in examples (like Catch2's STATIC_REQUIRE)
#define STATIC_ASSERT_MSG(cond, msg) static_assert(cond, msg)

void print_std_string(const std::string& s) {
    std::cout << "std::string: " << s << std::endl;
}

void print_string_view(std::string_view sv) {
    std::cout << "string_view: " << sv << std::endl;
}

int main() {
    // Construction
    constexpr ct_string hello = "Hello"; // Deduction guide
    constexpr ct_string world_suffix = " World!";
    constexpr ct_string empty_str;

    STATIC_ASSERT_MSG(hello.size() == 5, "Size check");
    STATIC_ASSERT_MSG(empty_str.empty(), "Empty check");

    // Concatenation
    constexpr auto greeting = hello + world_suffix;
    STATIC_ASSERT_MSG(greeting.size() == 12, "Concatenation size");
    STATIC_ASSERT_MSG(std::string_view(greeting) == "Hello World!", "Concatenation content");

    // Using in a constexpr context
    constexpr ct_string base_path = "/usr/local";
    constexpr ct_string app_folder = "/my_app";
    constexpr ct_string config_file = "/config.json";
    constexpr auto full_config_path = base_path + app_folder + config_file;

    std::cout << "Config path: " << full_config_path << std::endl; // Implicit conversion to const char*

    if constexpr (full_config_path == "/usr/local/my_app/config.json") {
        std::cout << "Compile-time path verification successful!" << std::endl;
    }

    // Conversions
    const char* cstr_from_ct = greeting; // Implicit to const char*
    std::cout << "C-string: " << cstr_from_ct << std::endl;

    std::string std_str_from_ct = greeting; // Implicit to std::string (runtime)
    std::cout << "std::string: " << std_str_from_ct << std::endl;

    // Passing to functions
    print_string_view(full_config_path);
    print_std_string(full_config_path);

    // Comparisons
    constexpr ct_string s1 = "test";
    constexpr ct_string s2 = "test";
    constexpr ct_string s3 = "Test";

    STATIC_ASSERT_MSG(s1 == s2, "Equality check");
    STATIC_ASSERT_MSG(s1 != s3, "Inequality check");
    // 'Test' (s3) is lexicographically less than 'test' (s1)
    STATIC_ASSERT_MSG(s3 < s1, "Less than check: 'Test' < 'test'");
    // 'test' (s1) is lexicographically greater than 'Test' (s3)
    STATIC_ASSERT_MSG(s1 > s3, "Greater than check: 'test' > 'Test'");

    return 0;
}
```

## API Overview

*   `ct_string<std::size_t N = 0>`: The main class template. `N` is the number of characters excluding the null terminator.
    *   `constexpr ct_string()`: Default constructor (empty string).
    *   `constexpr ct_string(const char (&str)[N + 1])`: Constructor from C-style string literal.
    *   `constexpr std::size_t size() const`: Returns the number of characters.
    *   `constexpr bool empty() const`: Checks if the string is empty.
    *   `constexpr const char* c_str() const`: Returns a null-terminated C-style string.
    *   `constexpr operator std::string_view() const`: Converts to `std::string_view`.
    *   `operator std::string() const`: Converts to `std::string`.
    *   `constexpr operator const char*() const`: Converts to `const char*`.
    *   `constexpr const char& operator[](std::size_t index) const`: Accesses character at index.
    *   `constexpr const char* begin() const`, `constexpr const char* end() const`: Iterators.
*   `template<std::size_t N_with_null> ct_string(const char (&str)[N_with_null]) -> ct_string<N_with_null - 1>;`: Deduction guide.
*   `operator+`: Concatenates two `ct_string` objects.
*   `operator==`, `operator!=`, `operator<=>`: Comparison operators for `ct_string` with `ct_string`, `std::string_view`, and `const char*`.

## Building and Running Tests

The repository includes tests written using [Catch2](https://github.com/catchorg/Catch2). To build and run them:
1.  Ensure you have CMake and a C++20 compiler installed.
2.  Clone the repository: `git clone https://github.com/OperationDarkside/ct_string.git`
3.  Configure and build:
    ```bash
    cd ct_string
    cmake -B build -S .
    cmake --build build
    ```
4.  Run tests:
    ```bash
    cd build
    ctest # Or run the test executable directly, e.g., ./run_tests
    ```
    (The test executable name might vary based on your `CMakeLists.txt`).

## Motivation

I needed/wanted a compile-time string type in my game engine that could be used in `constexpr` contexts and could be concatenated with other `constexpr` strings. I also wanted to avoid heap allocations and the overhead of `std::string`. My example use case was to combine file paths in a `constexpr` context. While `std::string_view` is excellent for non-owning string views, it lacks the ability to own data resulting from operations like concatenation. `std::string` can do this but often involves heap allocations, making it less suitable for extensive `constexpr` usage, especially in older C++ standards or when strict compile-time guarantees are needed. `ct_string` fills this gap by providing a string type that stores its data directly and performs all its core operations at compile time.

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests. If submitting a PR, please ensure existing tests pass and add new tests for your changes.

## License

This project is licensed under the [MIT License](LICENSE).

## DISCLAIMER

This project was mostly generated using the LLM Gemini 2.5 Pro Preview 03-25 in Google AI Studio. The code was then reviewed and modified by a human to ensure correctness and readability. This README was also mostly generated using the same LLM. The code is provided as-is, and the author is not responsible for any issues or bugs that may arise.
