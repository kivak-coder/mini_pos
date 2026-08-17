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


add_executable(payment_tests
    PaymentTests.cpp
)

target_link_libraries(payment_tests
    PRIVATE
        mini_pos_domain
        mini_pos_application
        Qt5::Core
)

target_include_directories(payment_tests
    PRIVATE
        ${BOOST_ROOT}
)

add_test(
    NAME payment_tests
    COMMAND payment_tests
        --log_level=test_suite
        --report_level=detailed
        --log_sink=stdout
)


add_executable(sqlite_receipt_repository_tests
    RepositoryTests.cpp
)

target_link_libraries(sqlite_receipt_repository_tests
    PRIVATE
        mini_pos_domain
        mini_pos_application
        mini_pos_infrastructure
        Qt5::Core
        Qt5::Sql
)

target_include_directories(sqlite_receipt_repository_tests
    PRIVATE
        ${BOOST_ROOT}
)

target_compile_features(sqlite_receipt_repository_tests
    PRIVATE
        cxx_std_17
)

add_test(
    NAME sqlite_receipt_repository_tests
    COMMAND sqlite_receipt_repository_tests
        --log_level=test_suite
        --report_level=detailed
        --catch_system_errors=yes
        --log_sink=stdout
)