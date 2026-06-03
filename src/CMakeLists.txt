# SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: CC0-1.0

cmake_minimum_required(VERSION 3.10)
project(deepin-ocr)

#玲珑构建依赖目录
include_directories(${CMAKE_INSTALL_PREFIX}/include)

# Auto-detect Qt version (tries Qt6 first, falls back to Qt5)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)

# Map to DTK version (Qt6→DTK6, Qt5→DTK5)
if (QT_VERSION_MAJOR MATCHES 6)
    set(DTK_VERSION_MAJOR 6)
    set(SUPPORT_QT6 TRUE)
else()
    set(DTK_VERSION_MAJOR "")
    set(SUPPORT_QT6 FALSE)
endif()

message(STATUS "Found Qt version: ${QT_VERSION_MAJOR}")
message(STATUS "Build with DTK: ${DTK_VERSION_MAJOR}")

# 玲珑构建依赖目录
include_directories(${CMAKE_INSTALL_PREFIX}/include)

# for Qt
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(QtModule Core Gui Widgets DBus LinguistTools)

if(DOTEST)
    set(QtModule ${QtModule} Test)

    if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "mips64")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fprofile-arcs -ftest-coverage")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined,address,leak -fno-omit-frame-pointer -g -fprofile-arcs -ftest-coverage -lgcov -fno-rtti")
    endif()
endif()

# sw_64 上运行时有崩溃, 所以暂时排除该架构
if (NOT ${CMAKE_SYSTEM_PROCESSOR} MATCHES "sw_64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wl,--as-need -fPIE")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=incompatible-pointer-types -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wl,--as-need -fPIE -Wno-error=incompatible-pointer-types")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie")
endif()

# 启用只读重定位/立即绑定，以提高安全性
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,relro -Wl,-z,now")

find_package(Qt${QT_VERSION_MAJOR} REQUIRED ${QtModule})

add_definitions(${QT_DEFINITIONS})

include(CMakePackageConfigHelpers)

# for ocr plugin manager
# find_package(dtkocr)
# include_directories(${dtkocr_INCLUDE_DIRS})

# for OpenMP
find_package(OpenMP REQUIRED)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

# for project files
include_directories(.)
include_directories(view)
include_directories(service)
include_directories(engine)
include_directories(util)
include_directories(utils)

aux_source_directory(. allSource)
aux_source_directory(./view allSource)
aux_source_directory(./service allSource)
aux_source_directory(./engine allSource)
aux_source_directory(./util allSource)
aux_source_directory(./utils allSource)

# translation
file(GLOB TargetTsFiles LIST_DIRECTORIES false ../translations/${PROJECT_NAME}*.ts)
set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)

if(NOT(${CMAKE_BUILD_TYPE} MATCHES "Debug"))
    if(SUPPORT_QT6)
        qt_create_translation(LangSrcs ${allSource} ${TargetTsFiles} OPTIONS)
        qt_add_translation(${TargetTsFiles})
    else()
        qt5_create_translation(LangSrcs ${allSource} ${TargetTsFiles} OPTIONS)
        qt5_add_translation(${TargetTsFiles})
    endif()
endif()

set_source_files_properties(${qmfiles} PROPERTIES OUTPUT_LOCATION "translations")

# QRC file
set(AllQRC ../resource.qrc)

add_executable(${PROJECT_NAME} ${allSource} ${LangSrcs} ${AllQRC})

# for test
if(DOTEST)
    SET(PROJECT_NAME_TEST ${PROJECT_NAME}_test)

    include_directories(./../tests/)
    include_directories(./../3rdparty/stub_linux/)

    aux_source_directory(./../tests/ allTestSource)
    aux_source_directory(./view allTestSource)
    aux_source_directory(./service allTestSource)
    aux_source_directory(./engine allTestSource)
    aux_source_directory(./util allTestSource)
    aux_source_directory(./utils allTestSource)

    FILE(GLOB allTestSource1
        "./frame.cpp"
        "./loadingwidget.cpp"
        "./ocrapplication.cpp"
        "./mainwidget.cpp"
        "./mainwindow.cpp"
        "./resulttextview.cpp"
        "./textloadwidget.cpp"
    )

    add_executable(${PROJECT_NAME_TEST} ${allHeaders} ${allTestSource} ${allTestSource1})
    target_link_libraries(${PROJECT_NAME_TEST} gmock gtest pthread)

    add_test(${PROJECT_NAME_TEST} COMMAND ${PROJECT_NAME_TEST})

    # ------------------------------ 创建'make test'指令---------------------------------------
    add_custom_target(test
        COMMAND mkdir -p coverageResult
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    add_custom_command(TARGET test
        COMMAND echo " =================== CREAT LCOV REPROT BEGIN ==================== "

        COMMAND lcov --directory ./CMakeFiles/${PROJECT_NAME_TEST}.dir --zerocounters
        COMMAND lcov --directory ./CMakeFiles/${PROJECT_NAME}.dir --zerocounters
        COMMAND ./${PROJECT_NAME}/${PROJECT_NAME_TEST}

        COMMAND lcov --directory . --capture --output-file ./coverageResult/${PROJECT_NAME}_Coverage.info

        COMMAND echo " =================== do filter begin ==================== "
        COMMAND lcov --remove ./coverageResult/${PROJECT_NAME}_Coverage.info
        '*/${PROJECT_NAME_TEST}_autogen/*' '*/${PROJECT_NAME}_autogen/*' '*/usr/include/*' '*/tests/*' '*/googletest/*'
        -o ./coverageResult/${PROJECT_NAME}_Coverage.info
        COMMAND echo " =================== do filter end ==================== "

        COMMAND genhtml -o ./coverageResult/report ./coverageResult/${PROJECT_NAME}_Coverage.info
        COMMAND echo " =================== CREAT LCOV REPROT END ==================== "

        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    add_dependencies(test ${PROJECT_NAME_TEST})
endif()

# for dtk
find_package(PkgConfig REQUIRED)
if(SUPPORT_QT6)
    pkg_check_modules(ocr_lib REQUIRED dtk6ocr)
else()
    pkg_check_modules(ocr_lib REQUIRED dtkocr)
endif()
pkg_check_modules(InferenceEngine REQUIRED ncnn opencv_mobile)
target_include_directories(${PROJECT_NAME} PUBLIC ${ocr_lib_INCLUDE_DIRS})
find_package(Dtk${DTK_VERSION_MAJOR} REQUIRED COMPONENTS Core Widget)

target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Dtk${DTK_VERSION_MAJOR}::Core
    Dtk${DTK_VERSION_MAJOR}::Widget
    ${InferenceEngine_LIBRARIES}
    ${ocr_lib_LIBRARIES}
    pthread
)

if(DOTEST)
    target_include_directories(${PROJECT_NAME_TEST} PUBLIC ${ocr_lib_INCLUDE_DIRS})
    target_include_directories(${PROJECT_NAME_TEST} PUBLIC ${InferenceEngine_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME_TEST}
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::DBus
        Dtk${DTK_VERSION_MAJOR}::Core
        Dtk${DTK_VERSION_MAJOR}::Widget
        ${ocr_lib_LIBRARIES}
        ${InferenceEngine_LIBRARIES}
        pthread
    )
endif()

# 启用位置无关可执行文件，提高安全性
target_link_options(${PROJECT_NAME} PRIVATE -pie)

# dde项目，期望CMAKE_INSTALL_PREFIX为/usr
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr)
endif()

#dde项目，期望CMAKE_INSTALL_PREFIX为/usr
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr)
endif ()

# install
set(PREFIX ${CMAKE_INSTALL_PREFIX})
set(TranslationDir ${PREFIX}/share/${PROJECT_NAME}/translations)
set(DesktopDir ${PREFIX}/share/applications/)
set(AppIconDir ${PREFIX}/share/icons/hicolor/scalable/apps)
set(DBusServiceDir ${PREFIX}/share/dbus-1/services)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${PREFIX}/lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION ${PREFIX}/bin
    INCLUDES DESTINATION allHeaders)

install(FILES ${LangSrcs} DESTINATION ${TranslationDir})
install(FILES ../deepin-ocr.desktop DESTINATION ${DesktopDir})
install(FILES ../assets/deepin-ocr.svg DESTINATION ${AppIconDir})
install(FILES ../com.deepin.Ocr.service DESTINATION ${DBusServiceDir})

include(${CMAKE_SOURCE_DIR}/install_dconfig.cmake)
INSTALL_DCONFIG("deepin-ocr.common.json")