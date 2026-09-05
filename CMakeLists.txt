cmake_minimum_required(VERSION 3.16)

project(Gomoku LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 入口为标准 main()，沿用 EasyX 常见的控制台工程配置
add_executable(Gomoku
    src/main.cpp
    src/GameState.cpp
    src/Rules.cpp
    src/AiPlayer.cpp
    src/GameUi.cpp
    src/MainMenu.cpp
    src/GameSession.cpp
)

# EasyXw.lib 依赖的 Windows 系统库（IDE 默认自带，这里显式声明以兼容命令行构建）
target_link_libraries(Gomoku PRIVATE
    kernel32
    user32
    gdi32
    winspool
    comdlg32
    advapi32
    shell32
    ole32
    oleaut32
    uuid
    odbc32
    odbccp32
)

# EasyX 根据 UNICODE 宏自动选择 EasyXw.lib（Unicode 版）
target_compile_definitions(Gomoku PRIVATE
    UNICODE
    _UNICODE
    _CRT_SECURE_NO_WARNINGS
)

if(MSVC)
    # 源码统一按 UTF-8 处理，保证宽字符中文与注释不乱码
    target_compile_options(Gomoku PRIVATE /utf-8 /W3)

    # EasyX 安装器通常会把库放到 VS 的 Auxiliary\VS\lib\<arch> 下。
    # Visual Studio 图形界面会自动包含该目录；命令行构建时需要手动补上。
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(EASYX_ARCH_DIR x64)
    else()
        set(EASYX_ARCH_DIR x86)
    endif()
    set(EASYX_LIB_DIR "${EASYX_LIB_DIR}" CACHE PATH "EasyX 库文件所在目录")
    if(NOT EASYX_LIB_DIR AND DEFINED ENV{VCINSTALLDIR})
        set(EASYX_LIB_DIR "$ENV{VCINSTALLDIR}/Auxiliary/VS/lib/${EASYX_ARCH_DIR}")
    endif()
    if(EASYX_LIB_DIR AND EXISTS "${EASYX_LIB_DIR}")
        target_link_directories(Gomoku PRIVATE "${EASYX_LIB_DIR}")
    endif()
endif()

# 把背景图复制到 exe 所在目录，保证菜单和对局画面都能找到背景图
add_custom_command(TARGET Gomoku POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/ChessUnderground.bmp"
        "$<TARGET_FILE_DIR:Gomoku>/ChessUnderground.bmp"
    COMMENT "Copying ChessUnderground.bmp to executable directory"
)

# EasyX 官方仅支持 MSVC。若手动安装到非标准目录，请在这里补充：
# include_directories(<EasyX include 目录>)
# link_directories(<EasyX lib 目录>)
