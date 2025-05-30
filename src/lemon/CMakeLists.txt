cmake_minimum_required(VERSION 3.10)
project(lemon VERSION 1.0.0 LANGUAGES CXX C)

# 查找必要的包
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtkmm-3.0)


set(CORE_SOURCE

)

set(GUI_SOURCE
    ${CMAKE_CURRENT_LIST_DIR}/src/ui/main.cpp
        src/ui/LemonWindow.cpp
        src/ui/LemonWindow.h
        src/ui/LemonApplication.cpp
        src/ui/LemonApplication.h
        src/ui/LemonStatusBar.cpp
        src/ui/LemonStatusBar.h
        src/ui/LemonHeaderBar.cpp
        src/ui/LemonHeaderBar.h
        src/ui/LemonActivityBar.cpp
        src/ui/LemonActivityBar.h
        src/ui/LemonEditStack.cpp
        src/ui/LemonEditStack.h
        src/ui/LemonMenu.cpp
        src/ui/LemonMenu.h
)

# 创建lemon应用程序
add_executable(lemon
        ${CORE_SOURCE}
        ${GUI_SOURCE}
)

# 包含目录
target_include_directories(lemon
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src/scintilla/include
        ${CMAKE_SOURCE_DIR}/src/lexilla/include
        ${GTK3_INCLUDE_DIRS}
)

# 编译选项
target_compile_options(lemon
        PRIVATE
        -Wall
        -Wextra
        ${GTK3_CFLAGS}
)

# 链接库
target_link_libraries(lemon
        PRIVATE
        scintilla
        lexilla
        ${GTK3_LIBRARIES}
)

# 安装可执行文件
include(GNUInstallDirs)
install(TARGETS lemon
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)