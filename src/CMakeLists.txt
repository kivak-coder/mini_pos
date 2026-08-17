add_library(mini_pos_domain
    domain/Money.h
    domain/Product.h
    domain/Product.cpp
    domain/ProductCatalog.h
    domain/ProductCatalog.cpp
    domain/CartItem.h
    domain/CartItem.cpp
    domain/Sale.h
    domain/Sale.cpp
    domain/Receipt.h
    domain/Receipt.cpp
)

target_include_directories(mini_pos_domain
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(mini_pos_application
    application/CashPaymentService.h
    application/CashPaymentService.cpp
    application/ReceiptRepository.h
    application/CheckoutService.h
    application/CheckoutService.cpp
    application/ReceiptReader.h
)

target_include_directories(mini_pos_application
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(mini_pos_application
    PUBLIC
        mini_pos_domain
)

if(MINGW)
    target_compile_options(mini_pos_application PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

endif()

add_library(mini_pos_infrastructure
    infrastructure/ProductCatalogDemo.h
    infrastructure/ProductCatalogDemo.cpp
    infrastructure/SqliteDatabase.h
    infrastructure/SqliteDatabase.cpp
    infrastructure/SqliteReceiptRepository.h
    infrastructure/SqliteReceiptRepository.cpp
)

target_include_directories(mini_pos_infrastructure
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(mini_pos_infrastructure
    PUBLIC
        mini_pos_domain
        mini_pos_application
        Qt5::Core
        Qt5::Sql
)


add_executable(mini_pos
    main.cpp

    ui/MoneyFormatter.h
    ui/MoneyFormatter.cpp
    ui/ProductTableModel.h
    ui/ProductTableModel.cpp
    ui/CartTableModel.h
    ui/CartTableModel.cpp
    ui/MainWindow.h
    ui/MainWindow.cpp
    ui/PaymentDialog.h
    ui/PaymentDialog.cpp
    ui/ReceiptHistoryTableModel.h
    ui/ReceiptHistoryTableModel.cpp
    ui/ReceiptHistoryDialog.h
    ui/ReceiptHistoryDialog.cpp

)

target_link_libraries(mini_pos
    PRIVATE
        mini_pos_domain
        mini_pos_application
        mini_pos_infrastructure
        Qt5::Widgets
        Qt5::Sql
)

target_include_directories(mini_pos
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if(MINGW)
    target_compile_options(mini_pos_domain PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

    target_compile_options(mini_pos PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

