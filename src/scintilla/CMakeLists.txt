cmake_minimum_required(VERSION 3.10)
project(Scintilla VERSION 5.3.2 LANGUAGES CXX C)

# 是否构建为共享库
option(SCINTILLA_BUILD_SHARED "Build Scintilla as shared library" ON)

# 查找GTK（如果需要GTK支持）
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Scintilla核心源文件
set(SCINTILLA_CORE_SOURCES
        src/AutoComplete.cxx
        src/CallTip.cxx
        src/CaseConvert.cxx
        src/CaseFolder.cxx
        src/CellBuffer.cxx
        src/ChangeHistory.cxx
        src/CharacterCategoryMap.cxx
        src/CharacterType.cxx
        src/CharClassify.cxx
        src/ContractionState.cxx
        src/DBCS.cxx
        src/Decoration.cxx
        src/Document.cxx
        src/EditModel.cxx
        src/Editor.cxx
        src/EditView.cxx
        src/Geometry.cxx
        src/Indicator.cxx
        src/KeyMap.cxx
        src/LineMarker.cxx
        src/MarginView.cxx
        src/PerLine.cxx
        src/PositionCache.cxx
        src/RESearch.cxx
        src/RunStyles.cxx
        src/ScintillaBase.cxx
        src/Selection.cxx
        src/Style.cxx
        src/UndoHistory.cxx
        src/UniConversion.cxx
        src/UniqueString.cxx
        src/ViewStyle.cxx
        src/XPM.cxx
)

# GTK平台相关源文件
set(SCINTILLA_GTK_SOURCES
        gtk/PlatGTK.cxx
        gtk/ScintillaGTK.cxx
        gtk/scintilla-marshal.c
        gtk/ScintillaGTKAccessible.cxx
)

# 创建库目标
if(SCINTILLA_BUILD_SHARED)
    add_library(scintilla SHARED ${SCINTILLA_CORE_SOURCES} ${SCINTILLA_GTK_SOURCES})
    set_target_properties(scintilla PROPERTIES
            VERSION ${PROJECT_VERSION}
            SOVERSION ${PROJECT_VERSION_MAJOR}
    )
else()
    add_library(scintilla STATIC ${SCINTILLA_CORE_SOURCES} ${SCINTILLA_GTK_SOURCES})
endif()

# 包含目录
target_include_directories(scintilla
        PUBLIC
        $<INSTALL_INTERFACE:include/scintilla>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/gtk
        ${GTK3_INCLUDE_DIRS}
)

# 编译定义
target_compile_definitions(scintilla
        PRIVATE
        GTK
        SCI_LEXER
        UNICODE
        _UNICODE
)

# 编译选项
target_compile_options(scintilla
        PRIVATE
        -Wall
        -Wno-deprecated-declarations
        $<$<CONFIG:Release>:-O2>
        $<$<CONFIG:Debug>:-g>
        ${GTK3_CFLAGS}
)

# 链接库
target_link_libraries(scintilla
        PRIVATE
        ${GTK3_LIBRARIES}
)

# 安装目标
include(GNUInstallDirs)

# 安装库
install(TARGETS scintilla
        EXPORT ScintillaTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# 安装头文件
install(FILES
        include/ILexer.h
        include/ILoader.h
        include/Sci_Position.h
        include/Scintilla.h
        include/ScintillaWidget.h
        include/ScintillaMessages.h
        include/ScintillaStructures.h
        include/ScintillaTypes.h
        include/ScintillaCall.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/scintilla
)

# 创建并安装CMake配置文件
include(CMakePackageConfigHelpers)

configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ScintillaConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/ScintillaConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Scintilla
)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/ScintillaConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/ScintillaConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/ScintillaConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Scintilla
)

install(EXPORT ScintillaTargets
        FILE ScintillaTargets.cmake
        NAMESPACE Scintilla::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Scintilla
)

# 创建cmake目录（如果不存在）
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# 创建ScintillaConfig.cmake.in文件
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ScintillaConfig.cmake.in [=[
@PACKAGE_INIT@

include("${CMAKE_CURRENT_LIST_DIR}/ScintillaTargets.cmake")

if(NOT TARGET Scintilla::scintilla)
    add_library(Scintilla::scintilla ALIAS scintilla)
endif()

check_required_components(Scintilla)
]=])