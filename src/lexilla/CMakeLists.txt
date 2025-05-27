cmake_minimum_required(VERSION 3.10)
project(Lexilla VERSION 5.3.2 LANGUAGES CXX C)

# 是否构建为共享库
option(LEXILLA_BUILD_SHARED "Build Lexilla as shared library" ON)

# 需要scintilla
find_package(Scintilla QUIET)
if(NOT Scintilla_FOUND)
    # 如果找不到已安装的Scintilla，使用本地路径
    if(NOT TARGET scintilla)
        message(STATUS "Scintilla package not found, using local build")
    endif()
endif()

# Lexilla核心源文件
set(LEXILLA_CORE_SOURCES
        src/Lexilla.cxx
        lexlib/LexerModule.cxx
        lexlib/DefaultLexer.cxx
        lexlib/Accessor.cxx
        lexlib/CharacterCategory.cxx
        lexlib/CharacterSet.cxx
        lexlib/DefaultLexer.cxx
        lexlib/InList.cxx
        lexlib/LexAccessor.cxx
        lexlib/LexerBase.cxx
        lexlib/LexerModule.cxx
        lexlib/LexerSimple.cxx
        lexlib/PropSetSimple.cxx
        lexlib/StyleContext.cxx
        lexlib/WordList.cxx
)

# Lexilla词法分析器源文件
set(LEXILLA_LEXER_SOURCES
        lexers/LexA68k.cxx
        lexers/LexAbaqus.cxx
        lexers/LexAda.cxx
        lexers/LexAPDL.cxx
        lexers/LexAsciidoc.cxx
        lexers/LexAsm.cxx
        lexers/LexAsn1.cxx
        lexers/LexASY.cxx
        lexers/LexAU3.cxx
        lexers/LexAVE.cxx
        lexers/LexAVS.cxx
        lexers/LexBaan.cxx
        lexers/LexBash.cxx
        lexers/LexBasic.cxx
        lexers/LexBatch.cxx
        lexers/LexBibTeX.cxx
        lexers/LexBullant.cxx
        lexers/LexCaml.cxx
        lexers/LexCIL.cxx
        lexers/LexCLW.cxx
        lexers/LexCmake.cxx
        lexers/LexCOBOL.cxx
        lexers/LexCoffeeScript.cxx
        lexers/LexConf.cxx
        lexers/LexCPP.cxx
        lexers/LexCrontab.cxx
        lexers/LexCsound.cxx
        lexers/LexCSS.cxx
        lexers/LexD.cxx
        lexers/LexDart.cxx
        lexers/LexDataflex.cxx
        lexers/LexDiff.cxx
        lexers/LexDMAP.cxx
        lexers/LexDMIS.cxx
        lexers/LexECL.cxx
        lexers/LexEDIFACT.cxx
        lexers/LexEiffel.cxx
        lexers/LexErlang.cxx
        lexers/LexErrorList.cxx
        lexers/LexEScript.cxx
        lexers/LexFlagship.cxx
        lexers/LexForth.cxx
        lexers/LexFortran.cxx
        lexers/LexFSharp.cxx
        lexers/LexGAP.cxx
        lexers/LexGDScript.cxx
        lexers/LexGui4Cli.cxx
        lexers/LexHaskell.cxx
        lexers/LexHex.cxx
        lexers/LexHollywood.cxx
        lexers/LexHTML.cxx
        lexers/LexIndent.cxx
        lexers/LexInno.cxx
        lexers/LexJulia.cxx
        lexers/LexJSON.cxx
        lexers/LexKix.cxx
        lexers/LexKVIrc.cxx
        lexers/LexLaTeX.cxx
        lexers/LexLisp.cxx
        lexers/LexLout.cxx
        lexers/LexLua.cxx
        lexers/LexMagik.cxx
        lexers/LexMake.cxx
        lexers/LexMarkdown.cxx
        lexers/LexMatlab.cxx
        lexers/LexMaxima.cxx
        lexers/LexMetapost.cxx
        lexers/LexMMIXAL.cxx
        lexers/LexModula.cxx
        lexers/LexMPT.cxx
        lexers/LexMSSQL.cxx
        lexers/LexMySQL.cxx
        lexers/LexNimrod.cxx
        lexers/LexNsis.cxx
        lexers/LexNull.cxx
        lexers/LexOpal.cxx
        lexers/LexOScript.cxx
        lexers/LexPascal.cxx
        lexers/LexPB.cxx
        lexers/LexPerl.cxx
        lexers/LexPLM.cxx
        lexers/LexPO.cxx
        lexers/LexPOV.cxx
        lexers/LexPowerPro.cxx
        lexers/LexPowerShell.cxx
        lexers/LexProgress.cxx
        lexers/LexProps.cxx
        lexers/LexPS.cxx
        lexers/LexPython.cxx
        lexers/LexR.cxx
        lexers/LexRebol.cxx
        lexers/LexRegistry.cxx
        lexers/LexRuby.cxx
        lexers/LexRust.cxx
        lexers/LexSAS.cxx
        lexers/LexScriptol.cxx
        lexers/LexSmalltalk.cxx
        lexers/LexSML.cxx
        lexers/LexSorcus.cxx
        lexers/LexSpecman.cxx
        lexers/LexSpice.cxx
        lexers/LexSQL.cxx
        lexers/LexSTTXT.cxx
        lexers/LexTACL.cxx
        lexers/LexTADS3.cxx
        lexers/LexTAL.cxx
        lexers/LexTCL.cxx
        lexers/LexTCMD.cxx
        lexers/LexTeX.cxx
        lexers/LexTxt2tags.cxx
        lexers/LexVB.cxx
        lexers/LexVerilog.cxx
        lexers/LexVHDL.cxx
        lexers/LexVisualProlog.cxx
        lexers/LexYAML.cxx
        lexers/LexZig.cxx
        lexers/LexNim.cxx
        lexers/LexNix.cxx
        lexers/LexRaku.cxx
        lexers/LexStata.cxx
        lexers/LexTOML.cxx
        lexers/LexTroff.cxx
        lexers/LexX12.cxx

)

# 创建库目标
if(LEXILLA_BUILD_SHARED)
    add_library(lexilla SHARED ${LEXILLA_CORE_SOURCES} ${LEXILLA_LEXER_SOURCES})
    set_target_properties(lexilla PROPERTIES
            VERSION ${PROJECT_VERSION}
            SOVERSION ${PROJECT_VERSION_MAJOR}
    )
else()
    add_library(lexilla STATIC ${LEXILLA_CORE_SOURCES} ${LEXILLA_LEXER_SOURCES})
endif()

# 包含目录
target_include_directories(lexilla
        PUBLIC
        $<INSTALL_INTERFACE:include/lexilla>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/lexlib
        ${CMAKE_CURRENT_SOURCE_DIR}/../scintilla/include
)

# 编译定义
target_compile_definitions(lexilla
        PRIVATE
        UNICODE
        _UNICODE
        SCI_LEXER
)

# 编译选项
target_compile_options(lexilla
        PRIVATE
        -Wall
        -Wno-deprecated-declarations
        $<$<CONFIG:Release>:-O2>
        $<$<CONFIG:Debug>:-g>
)

# 链接库
target_link_libraries(lexilla
        PRIVATE
        scintilla
)

# 安装目标
include(GNUInstallDirs)

# 安装库
install(TARGETS lexilla
        EXPORT LexillaTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# 安装头文件
install(FILES
        include/Lexilla.h
        include/SciLexer.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/lexilla
)

# 创建并安装CMake配置文件
include(CMakePackageConfigHelpers)

configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/LexillaConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/LexillaConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Lexilla
)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/LexillaConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/LexillaConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/LexillaConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Lexilla
)

install(EXPORT LexillaTargets
        FILE LexillaTargets.cmake
        NAMESPACE Lexilla::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Lexilla
)

# 创建cmake目录（如果不存在）
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# 创建LexillaConfig.cmake.in文件
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/LexillaConfig.cmake.in
        "@PACKAGE_INIT@

include(\${CMAKE_CURRENT_LIST_DIR}/LexillaTargets.cmake)

# 为了向后兼容定义导入的目标
if(NOT TARGET Lexilla::lexilla)
    add_library(Lexilla::lexilla ALIAS lexilla)
endif()

# 查找scintilla依赖
find_package(Scintilla REQUIRED)

check_required_components(Lexilla)
")