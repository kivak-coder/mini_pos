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
)

target_include_directories(mini_pos_domain
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(mini_pos
    main.cpp

    infrastructure/ProductCatalogDemo.h
    infrastructure/ProductCatalogDemo.cpp

    ui/MoneyFormatter.h
    ui/MoneyFormatter.cpp
    ui/ProductTableModel.h
    ui/ProductTableModel.cpp
    ui/CartTableModel.h
    ui/CartTableModel.cpp
    ui/MainWindow.h
    ui/MainWindow.cpp
)

target_link_libraries(mini_pos
    PRIVATE
        mini_pos_domain
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