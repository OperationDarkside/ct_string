module;

#include <array>
#include <string>
#include <string_view>
#include <cstddef>
#include <stdexcept> // For optional bounds checking
#include <compare>   // For <=>
#include <algorithm>   // For std::copy, std::equal
#include <iterator>    // For std::begin, std::end

export module ct_string;

export template<std::size_t N = 0> // N is the number of characters (excluding null terminator)
struct ct_string {
    // Store N characters + 1 null terminator
    std::array<char, N + 1> data;

    // Default constructor: Creates an empty string
    constexpr ct_string() : data{} {
        // Ensure null termination even for default constructor
        if constexpr (N == 0) {
            data[0] = '\0';
        }
        // For N > 0, zero-initialization already sets null terminators
    }

    // Constructor from C-style string literal
    // Requires literal of size N+1 (N chars + null terminator)
    constexpr ct_string(const char (&str)[N + 1]) {
        // Use std::copy for efficient and correct copying.
        // It copies N+1 elements, including the null terminator.
        std::copy(std::begin(str), std::end(str), data.begin());
        // Ensure the last element is null, even if the literal wasn't proper
        // (though the array size constraint should guarantee it)
        data[N] = '\0';
    }

    // Return size excluding null terminator (standard behavior)
    constexpr std::size_t size() const { return N; }
    constexpr std::size_t length() const { return N; } // Alias
    constexpr bool empty() const { return N == 0; }

    // Access as const char* (null-terminated)
    constexpr const char* c_str() const { return data.data(); }
    constexpr const char* get_data() const { return data.data(); } // Alias

    // Conversion to string_view (efficient, constexpr-friendly)
    constexpr operator std::string_view() const {
        return std::string_view(data.data(), N); // Use N, not N+1
    }

    // Conversion to std::string (potentially allocates, less constexpr-friendly)
    // This provides seamless conversion where std::string is required at runtime.
    /*constexpr*/ // std::string construction might not be fully constexpr pre-C++20/23
    operator std::string() const {
        return std::string(data.data(), N); // Use N, not N+1
    }

    // Conversion to const char*
    constexpr operator const char*() const {
        return data.data();
    }

    // Const access operator. Add bounds checking if desired.
    constexpr const char& operator[](std::size_t index) const {
        // Optional: Add bounds check
        // if (index >= N) throw std::out_of_range("ct_string index out of range");
        return data[index];
    }

    // Iterators (optional but good for range-based for loops)
    constexpr const char* begin() const { return data.data(); }
    constexpr const char* end() const { return data.data() + N; } // Points one past the last char
};

// Deduction guide to automatically deduce N from a string literal
// Example: ct_string myStr = "Hello"; // Deduces N=5
template<std::size_t N_with_null>
ct_string(const char (&str)[N_with_null]) -> ct_string<N_with_null - 1>;

// --- Operators ---

// Concatenation operator
export template<std::size_t N1, std::size_t N2>
constexpr auto operator+ (const ct_string<N1>& lhs, const ct_string<N2>& rhs) {
    ct_string<N1 + N2> result{}; // Result has N1+N2 chars + null

    // Copy characters from lhs (N1 chars)
    std::copy(lhs.data.begin(), lhs.data.begin() + N1, result.data.begin());
    // Copy characters from rhs (N2 chars) and its null terminator
    // std::copy copies [first, last), so rhs.data.begin() + N2 + 1 is correct for N2 chars + null
    std::copy(rhs.data.begin(), rhs.data.begin() + N2 + 1, result.data.begin() + N1);
    // The null terminator from rhs is now at result.data[N1 + N2]

    return result;
}

// Comparison operators (Leverage string_view conversion for efficiency and constexpr)

// ct_string == ct_string
export template<std::size_t N1, std::size_t N2>
constexpr bool operator==(const ct_string<N1>& lhs, const ct_string<N2>& rhs) {
    return std::string_view(lhs) == std::string_view(rhs);
}

// ct_string == string_view
export template<std::size_t N>
constexpr bool operator==(const ct_string<N>& lhs, std::string_view rhs) {
    return std::string_view(lhs) == rhs;
}
// string_view == ct_string (symmetric)
export template<std::size_t N>
constexpr bool operator==(std::string_view lhs, const ct_string<N>& rhs) {
    return lhs == std::string_view(rhs);
}

export template<std::size_t N>
constexpr bool operator==(const ct_string<N>& lhs, const char* rhs) {
    return rhs ? (std::string_view(lhs) == rhs) : (N == 0 && *lhs.c_str()=='\0'); // Handle nullptr rhs
}
export template<std::size_t N>
constexpr bool operator==(const char* lhs, const ct_string<N>& rhs) {
    return lhs ? (lhs == std::string_view(rhs)) : (N == 0 && *rhs.c_str()=='\0'); // Handle nullptr lhs
}


// Optional: Add != operators (usually defaulted in C++20, but explicit here for clarity)
export template<std::size_t N1, std::size_t N2>
constexpr bool operator!=(const ct_string<N1>& lhs, const ct_string<N2>& rhs) {
    return !(lhs == rhs);
}
export template<std::size_t N>
constexpr bool operator!=(const ct_string<N>& lhs, std::string_view rhs) {
    return !(lhs == rhs);
}
export template<std::size_t N>
constexpr bool operator!=(std::string_view lhs, const ct_string<N>& rhs) {
    return !(lhs == rhs);
}
export template<std::size_t N>
constexpr bool operator!=(const ct_string<N>& lhs, const char* rhs) {
    return !(lhs == rhs);
}
export template<std::size_t N>
constexpr bool operator!=(const char* lhs, const ct_string<N>& rhs) {
    return !(lhs == rhs);
}

// C++20 Three-Way Comparison (operator<=>)
// This will also generate <, <=, >, >= operators.
export template<std::size_t N1, std::size_t N2>
constexpr auto operator<=>(const ct_string<N1>& lhs, const ct_string<N2>& rhs) {
    return std::string_view(lhs) <=> std::string_view(rhs);
}
export template<std::size_t N>
constexpr auto operator<=>(const ct_string<N>& lhs, std::string_view rhs) {
    return std::string_view(lhs) <=> rhs;
}
export template<std::size_t N>
constexpr auto operator<=>(const ct_string<N>& lhs, const char* rhs) {
    return std::string_view(lhs) <=> (rhs ? rhs : ""); // Handle nullptr rhs
}