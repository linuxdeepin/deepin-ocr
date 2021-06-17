#定义需要的cmake版本
cmake_minimum_required(VERSION 3.10)
set(CMD_NAME deepin-ocr)
project(deepin-ocr)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(DOTEST "option for test" OFF)
add_definitions(-DENABLE_ACCESSIBILITY=1)
if(DOTEST)
    set(QtModule Core Gui Widgets DBus LinguistTools Test)
    if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "mips64")
        set(CMAKE_CXX_FLAGS "-g -fprofile-arcs -ftest-coverage")
    else()
        set(CMAKE_CXX_FLAGS "-fsanitize=undefined,address,leak -fno-omit-frame-pointer -g -fprofile-arcs -ftest-coverage -lgcov")
    endif()
else()
    set(QtModule Core Gui Widgets DBus LinguistTools)
    # 加速编译优化参数
    if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "mips64")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wl,-O1 -Wl,--as-needed -Wl,-E -fPIE -ffunction-sections -fdata-sections -Wl,--gc-sections -ftree-vectorize -march=loongson3a -mhard-float -mno-micromips -mno-mips16 -flax-vector-conversions -mloongson-ext2 -mloongson-mmi -z noexecstack")
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wl,-O1 -Wl,--as-needed -Wl,-E -fPIE -ffunction-sections -fdata-sections -Wl,--gc-sections -ftree-vectorize -march=loongson3a -mhard-float -mno-micromips -mno-mips16 -flax-vector-conversions -mloongson-ext2 -mloongson-mmi -z noexecstack")
        SET(CMAKE_EXE_LINKER_FLAGS  "-pie")
    else()
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wl,-O1 -Wl,--as-needed -Wl,-E -fPIE -ffunction-sections -fdata-sections -Wl,--gc-sections")
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3  -Wl,-O1 -Wl,--as-needed -Wl,-E -fPIE -ffunction-sections -fdata-sections -Wl,--gc-sections")
        SET(CMAKE_EXE_LINKER_FLAGS  "-pie")
    endif()
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

find_package(PkgConfig REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

find_package(Qt5 REQUIRED ${QtModule})

#文件夹包含
include_directories(src)
include_directories(./src/service)
include_directories(./src/tessocrutils)
include_directories(./src/view)

aux_source_directory(src allSource)
aux_source_directory(src/service allSource)
aux_source_directory(src/tessocrutils allSource)
aux_source_directory(src/view allSource)

set(AllQRC
    ./resource.qrc
    )

# 翻译
file(GLOB TargetTsFiles LIST_DIRECTORIES false ./translations/${CMD_NAME}*.ts)
set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)

if (NOT (${CMAKE_BUILD_TYPE} MATCHES "Debug"))
    qt5_create_translation(LangSrcs ${allSource} ${TargetTsFiles}
        OPTIONS )
endif()
qt5_add_translation(${TargetTsFiles})
set_source_files_properties(${qmfiles} PROPERTIES OUTPUT_LOCATION "translations")

set(LINK_DIR lib)
link_directories(${LINK_DIR})

add_executable(${PROJECT_NAME} ${allSource} ${AllQRC} ${LangSrcs} )

pkg_check_modules(3rd_lib REQUIRED
        dtkwidget dtkgui
        )
target_include_directories(${PROJECT_NAME} PUBLIC ${3rd_lib_INCLUDE_DIRS})
if(DOTEST)
    target_include_directories(${PROJECT_NAME_TEST} PUBLIC ${3rd_lib_INCLUDE_DIRS})
endif()

target_link_libraries(${PROJECT_NAME} ${3rd_lib_LIBRARIES} lept tesseract)
if(DOTEST)
    target_link_libraries(${PROJECT_NAME_TEST} ${3rd_lib_LIBRARIES} lept tesseract)
endif()

qt5_use_modules(${PROJECT_NAME} ${QtModule})
if(DOTEST)
qt5_use_modules(${PROJECT_NAME_TEST} ${QtModule})
endif()

#将工程与这些模块链接起来
qt5_use_modules(${PROJECT_NAME} ${QtModule})

if(DOTEST)
qt5_use_modules(${PROJECT_NAME_TEST} ${QtModule})
endif()


# ------------------------------- install report script -------------------------------
set(PREFIX /usr)
set(TranslationDir ${PREFIX}/share/${PROJECT_NAME}/translations/)
set(AppIconDir ${PREFIX}/share/icons/hicolor/scalable/apps)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION /usr/bin
        INCLUDES DESTINATION allHeaders)

#install
install(FILES ${LangSrcs} DESTINATION ${TranslationDir})
install(FILES ./assets/appicon_dark.svg DESTINATION ${AppIconDir})

install(FILES ./com.deepin.Ocr.service DESTINATION ${PREFIX}/share/dbus-1/services)

install(FILES ./assets/tesslangs/chi_sim.traineddata
    ./assets/tesslangs/chi_tra.traineddata
    ./assets/tesslangs/eng.traineddata
    ./assets/tesslangs/chi_sim_vert.traineddata
    ./assets/tesslangs/chi_tra_vert.traineddata
    DESTINATION ${PREFIX}/share/deepin-ocr/tesslangs)

