# Download Catch2 using FetchContent (simple and effective)
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.0 # or latest stable
)
FetchContent_MakeAvailable(Catch2)

add_executable(test_ct_string test_ct_string.cpp)
target_link_libraries(test_ct_string PRIVATE Catch2::Catch2WithMain ct_string)
target_compile_features(test_ct_string PRIVATE cxx_std_20)

include(CTest)
include(Catch)
catch_discover_tests(test_ct_string)
