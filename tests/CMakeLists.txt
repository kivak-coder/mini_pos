add_executable(sale_tests
    SaleTests.cpp
)

target_link_libraries(sale_tests
    PRIVATE
        mini_pos_domain
)

target_include_directories(sale_tests
    PRIVATE
        ${BOOST_ROOT}
)

add_test(
    NAME sale_tests
    COMMAND sale_tests
        --log_level=test_suite
        --report_level=detailed
        --log_sink=stdout
)