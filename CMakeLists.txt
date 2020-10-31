cmake_minimum_required(VERSION 3.2)

project(polinom)

find_package(Threads REQUIRED)
#find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(helloworld_cli "${PROJECT_SOURCE_DIR}/Polynom/Polynom/Polynom.cpp")

#add_library(helloworld lib.cpp)
#add_executable(test_version test_version.cpp)

set_target_properties(helloworld_cli PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)


target_link_libraries(helloworld_cli
    Threads::Threads
)

install(TARGETS helloworld_cli RUNTIME DESTINATION bin)

set(CPACK_GENERATOR ZIP)

set(CPACK_PACKAGE_CONTACT cheshirwr@mail.com)

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)

#enable_testing()
#add_test(helloworld_tests test_version)
