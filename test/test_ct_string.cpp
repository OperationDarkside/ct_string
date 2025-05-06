// File: test_ct_string.cpp
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <string_view>
#include <cstring>   // For strcmp
#include <algorithm> // For std::equal with iterators
#include <array>     // For iterator test accumulation

// Ensure this import matches your module setup
import ct_string;

TEST_CASE("ct_string Construction", "[ct_string][construction]") {
    SECTION("Default constructor (empty string)") {
        constexpr ct_string<> empty_str;
        STATIC_REQUIRE(empty_str.size() == 0);
        STATIC_REQUIRE(empty_str.empty());
        STATIC_REQUIRE(empty_str.length() == 0);
        STATIC_REQUIRE(std::string_view(empty_str) == "");
        // empty_str[0] is accessing data[0] which is '\0'.
        // This is valid array access for std::array<char,1> but not a character of an empty string.
        // For N=0, begin() == end(), so no valid character access via operator[].
    }

    SECTION("Constructor from C-style string literal (explicit N)") {
        constexpr ct_string<5> hello_lit("Hello");
        STATIC_REQUIRE(hello_lit.size() == 5);
        STATIC_REQUIRE(!hello_lit.empty());
        STATIC_REQUIRE(std::string_view(hello_lit) == "Hello");
    }

    SECTION("Constructor using deduction guide") {
        constexpr ct_string world_deduced = "World";
        STATIC_REQUIRE(world_deduced.size() == 5);
        STATIC_REQUIRE(std::string_view(world_deduced) == "World");

        constexpr ct_string empty_deduced = "";
        STATIC_REQUIRE(empty_deduced.size() == 0);
        STATIC_REQUIRE(empty_deduced.empty());
        STATIC_REQUIRE(std::string_view(empty_deduced) == "");
    }
}

TEST_CASE("ct_string Accessors and Properties", "[ct_string][accessors]") {
    constexpr ct_string test_str = "Test";

    SECTION("size(), length(), empty()") {
        STATIC_REQUIRE(test_str.size() == 4);
        STATIC_REQUIRE(test_str.length() == 4);
        STATIC_REQUIRE(!test_str.empty());

        constexpr ct_string<> empty_s;
        STATIC_REQUIRE(empty_s.size() == 0);
        STATIC_REQUIRE(empty_s.length() == 0);
        STATIC_REQUIRE(empty_s.empty());
    }

    SECTION("c_str() and get_data()") {
        // Static check of content pointed to by c_str() via string_view conversion
        STATIC_REQUIRE(std::string_view(test_str.c_str()) == "Test");
        STATIC_REQUIRE(std::string_view(test_str.get_data()) == "Test");

        // Runtime check using strcmp
        REQUIRE(strcmp(test_str.c_str(), "Test") == 0);
        REQUIRE(strcmp(test_str.get_data(), "Test") == 0);
    }

    SECTION("operator[] (const access)") {
        STATIC_REQUIRE(test_str[0] == 'T');
        STATIC_REQUIRE(test_str[1] == 'e');
        STATIC_REQUIRE(test_str[2] == 's');
        STATIC_REQUIRE(test_str[3] == 't');
        // Accessing test_str[4] (i.e., test_str[test_str.size()]) would be an out-of-bounds
        // character access, though it might access the null terminator in `data`.
        // The contract for operator[] is for indices 0 to N-1.
    }
}

TEST_CASE("ct_string Conversions", "[ct_string][conversions]") {
    // cs is local, fine for std::string conversion, or direct use in STATIC_REQUIRE for string_view
    constexpr ct_string cs = "Convert";

    // For tests requiring a constexpr std::string_view VARIABLE, use a static ct_string
    static constexpr ct_string cs_static_for_sv = "StaticView";

    SECTION("operator std::string_view()") {
        // Valid: sv_static points to data with static storage duration
        constexpr std::string_view sv_static = cs_static_for_sv;
        STATIC_REQUIRE(sv_static.length() == 10);
        STATIC_REQUIRE(sv_static == "StaticView");

        // Valid: conversion result used directly in STATIC_REQUIRE
        STATIC_REQUIRE(std::string_view(cs).length() == 7);
        STATIC_REQUIRE(std::string_view(cs) == "Convert");

        // Example of passing to a constexpr function
        constexpr auto get_sv_len = [](std::string_view v_param) {
            return v_param.length();
        };
        constexpr size_t len_from_local_cs = get_sv_len(cs); // cs_local's data is valid for call
        STATIC_REQUIRE(len_from_local_cs == 7);
    }

    SECTION("operator const char*()") {
        constexpr const char* cptr = cs_static_for_sv; // Implicit conversion
        // Static check of content via string_view conversion
        STATIC_REQUIRE(std::string_view(cptr) == "StaticView");
        // Runtime check
        REQUIRE(strcmp(cptr, "StaticView") == 0);
    }

    SECTION("operator std::string()") {
        // This conversion is typically runtime due to std::string allocation
        std::string s = cs; // Implicit conversion
        REQUIRE(s.length() == 7);
        REQUIRE(s == "Convert");

        // Example of passing to a function expecting const std::string&
        auto func_taking_std_string = [&](const std::string& str_arg) {
            REQUIRE(str_arg == "Convert");
        };
        func_taking_std_string(cs);
    }
}

TEST_CASE("ct_string Concatenation (operator+)", "[ct_string][concatenation]") {
    constexpr ct_string s1 = "Hello";
    constexpr ct_string s2 = "World";
    constexpr ct_string empty_s = "";

    SECTION("Concatenating two non-empty strings") {
        constexpr auto result = s1 + s2;
        STATIC_REQUIRE(result.size() == 10);
        STATIC_REQUIRE(std::string_view(result) == "HelloWorld");
    }

    SECTION("Concatenating with an empty string (rhs)") {
        constexpr auto result = s1 + empty_s;
        STATIC_REQUIRE(result.size() == 5);
        STATIC_REQUIRE(std::string_view(result) == "Hello");
    }

    SECTION("Concatenating with an empty string (lhs)") {
        constexpr auto result = empty_s + s1;
        STATIC_REQUIRE(result.size() == 5);
        STATIC_REQUIRE(std::string_view(result) == "Hello");
    }

    SECTION("Concatenating two empty strings") {
        constexpr auto result = empty_s + empty_s;
        STATIC_REQUIRE(result.size() == 0);
        STATIC_REQUIRE(result.empty());
        STATIC_REQUIRE(std::string_view(result) == "");
    }

    SECTION("Chain concatenation") {
        constexpr ct_string s3 = "!";
        constexpr auto result = s1 + s2 + s3;
        STATIC_REQUIRE(result.size() == 11);
        STATIC_REQUIRE(std::string_view(result) == "HelloWorld!");
    }
}

TEST_CASE("ct_string Comparison Operators (==, !=)", "[ct_string][comparison][equality]") {
    constexpr ct_string cs_hello = "Hello";
    constexpr ct_string cs_world = "World";
    constexpr ct_string cs_hello_copy = "Hello"; // Same content, different object potentially
    constexpr ct_string cs_empty = "";

    // Test ct_string == ct_string
    STATIC_REQUIRE(cs_hello == cs_hello_copy);
    STATIC_REQUIRE(!(cs_hello == cs_world));
    STATIC_REQUIRE(cs_hello != cs_world);
    STATIC_REQUIRE(!(cs_hello != cs_hello_copy));
    STATIC_REQUIRE(cs_empty == ct_string(""));
    constexpr ct_string cs_diff_len = "HelloLonger";
    STATIC_REQUIRE(cs_hello != cs_diff_len);

    // Test ct_string == std::string_view (and symmetric)
    STATIC_REQUIRE(cs_hello == std::string_view("Hello"));
    STATIC_REQUIRE(std::string_view("Hello") == cs_hello);
    STATIC_REQUIRE(cs_hello != std::string_view("World"));
    STATIC_REQUIRE(std::string_view("World") != cs_hello);
    STATIC_REQUIRE(cs_empty == std::string_view(""));
    STATIC_REQUIRE(std::string_view("") == cs_empty);

    // Test ct_string == const char* (and symmetric)
    STATIC_REQUIRE(cs_hello == "Hello");
    STATIC_REQUIRE("Hello" == cs_hello);
    STATIC_REQUIRE(cs_hello != "World");
    STATIC_REQUIRE("World" != cs_hello);
    STATIC_REQUIRE(cs_empty == "");
    STATIC_REQUIRE("" == cs_empty);
    STATIC_REQUIRE(cs_hello != nullptr); // Comparison with nullptr
    STATIC_REQUIRE(nullptr != cs_hello);
    STATIC_REQUIRE_FALSE(cs_hello == nullptr);
    STATIC_REQUIRE_FALSE(nullptr == cs_hello);
}

#if defined(__cpp_impl_three_way_comparison) && __cpp_impl_three_way_comparison >= 201907L
TEST_CASE("ct_string Three-Way Comparison (operator<=>)", "[ct_string][comparison][ordering]") {
    constexpr ct_string cs_apple = "Apple";
    constexpr ct_string cs_apply = "Apply";
    constexpr ct_string cs_banana = "Banana";
    constexpr ct_string cs_apple_copy = "Apple";
    constexpr ct_string cs_empty = "";

    // ct_string <=> ct_string
    STATIC_REQUIRE((cs_apple <=> cs_apple_copy) == std::strong_ordering::equal);
    STATIC_REQUIRE((cs_apple <=> cs_apply) == std::strong_ordering::less);    // 'e' < 'y'
    STATIC_REQUIRE((cs_apply <=> cs_apple) == std::strong_ordering::greater);
    STATIC_REQUIRE((cs_banana <=> cs_apple) == std::strong_ordering::greater); // 'B' > 'A'
    STATIC_REQUIRE((cs_empty <=> cs_apple) == std::strong_ordering::less);
    STATIC_REQUIRE((cs_apple <=> cs_empty) == std::strong_ordering::greater);
    STATIC_REQUIRE((cs_empty <=> ct_string("")) == std::strong_ordering::equal);

    // Test different lengths
    constexpr ct_string cs_apples = "Apples";
    STATIC_REQUIRE((cs_apple <=> cs_apples) == std::strong_ordering::less);
    STATIC_REQUIRE((cs_apples <=> cs_apple) == std::strong_ordering::greater);

    // ct_string <=> std::string_view
    STATIC_REQUIRE((cs_apple <=> std::string_view("Apple")) == std::strong_ordering::equal);
    STATIC_REQUIRE((cs_apple <=> std::string_view("Apply")) == std::strong_ordering::less);
    STATIC_REQUIRE((std::string_view("Apply") <=> cs_apple) == std::strong_ordering::greater); // Symmetric check

    // ct_string <=> const char*
    STATIC_REQUIRE((cs_apple <=> "Apple") == std::strong_ordering::equal);
    STATIC_REQUIRE((cs_apple <=> "Apply") == std::strong_ordering::less);
    STATIC_REQUIRE(("Apply" <=> cs_apple) == std::strong_ordering::greater); // Symmetric check
    STATIC_REQUIRE((cs_apple <=> "App") == std::strong_ordering::greater); // Shorter
}
#endif // C++20 three-way comparison

TEST_CASE("ct_string Iterators", "[ct_string][iterators]") {
    constexpr ct_string str_iter = "Iter";

    SECTION("begin() and end()") {
        STATIC_REQUIRE(*str_iter.begin() == 'I');
        STATIC_REQUIRE(str_iter.begin() + str_iter.size() == str_iter.end());
        if constexpr (str_iter.size() > 0) { // Avoid dereferencing end() or end()-1 for empty string
            STATIC_REQUIRE(*(str_iter.end() - 1) == 'r');
        }

        // Check content via iterators at compile time
        constexpr bool content_match = std::equal(str_iter.begin(), str_iter.end(), "Iter");
        STATIC_REQUIRE(content_match);

        constexpr ct_string<> empty_iter_str = "";
        STATIC_REQUIRE(empty_iter_str.begin() == empty_iter_str.end());
    }

    SECTION("Range-based for loop") {
        // Runtime check of iteration
        std::string accumulated_str_runtime;
        for (char c : str_iter) {
            accumulated_str_runtime += c;
        }
        REQUIRE(accumulated_str_runtime == "Iter");

        // Compile-time accumulation check
        constexpr auto accumulate_chars_compile_time = []<std::size_t N>(const ct_string<N>& s) {
            std::array<char, N> arr{}; // NVRO should handle this well
            std::size_t i = 0;
            for (char c : s) { // This loop is constexpr
                arr[i++] = c;
            }
            return arr;
        };
        constexpr std::array<char, 4> iterated_chars = accumulate_chars_compile_time(str_iter);
        STATIC_REQUIRE(iterated_chars.size() == 4);
        STATIC_REQUIRE(iterated_chars[0] == 'I');
        STATIC_REQUIRE(iterated_chars[1] == 't');
        STATIC_REQUIRE(iterated_chars[2] == 'e');
        STATIC_REQUIRE(iterated_chars[3] == 'r');

        constexpr std::array<char, 0> empty_iterated_chars = accumulate_chars_compile_time(ct_string<>(""));
        STATIC_REQUIRE(empty_iterated_chars.empty());
    }
}