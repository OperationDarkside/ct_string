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