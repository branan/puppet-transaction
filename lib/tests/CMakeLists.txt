# Setup compiling the test executable. C++ compile flags are inherited from the parent directory.
include_directories(${LEATHERMAN_CATCH_INCLUDE})

set(TEST_CASES puppet-transaction.cc)

add_executable(lib${PROJECT_NAME}_test $<TARGET_OBJECTS:libpuppet-transactionsrc> ${TEST_CASES} main.cc)
target_link_libraries(libpuppet-transaction_test
    ${Boost_LIBRARIES}
    ${LEATHERMAN_LIBRARIES}
)

add_test(NAME "unit_tests" COMMAND libpuppet-transaction_test)
