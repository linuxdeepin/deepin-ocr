#定义需要的cmake版本
cmake_minimum_required(VERSION 3.10)
set(CMD_NAME deepin-ocr)
project(deepin-ocr)

set(CMAKE_CXX_STANDARD 17)
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
#需要使用tesseract源码时添加tesseract三方库文件
#include_directories(/usr/include/leptonica)
#include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/include)
#include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/api)
#include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/arch)
#include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/ccmain)
#include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/ccstruct)
#include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/ccutil)
#include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/classify)
#include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/cutil)
#include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/dict)
#include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/lstm)
#include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/opencl)
#include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/textord)
#include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/viewer)
#include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/wordrec)

aux_source_directory(src allSource)
aux_source_directory(src/service allSource)
aux_source_directory(src/tessocrutils allSource)
aux_source_directory(src/view allSource)
#需要使用tesseract源码时添加tesseract三方库文件
#aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/api allSource)
##aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/arch allSource)
#aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/ccmain allSource)
#aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/ccstruct allSource)
#aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/ccutil allSource)
#aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/classify allSource)
#aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/cutil allSource)
#aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/dict allSource)
#aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/lstm allSource)
#aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/opencl allSource)
#aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/textord allSource)
#aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/viewer allSource)
#aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/wordrec allSource)
#FILE(GLOB COMMON_FILE
#    "./3rdparty/tesseract_ocr/tesseract/src/arch/dotproduct.cpp"
#    "./3rdparty/tesseract_ocr/tesseract/src/arch/intsimdmatrix.cpp"
#    "./3rdparty/tesseract_ocr/tesseract/src/arch/simddetect.cpp"
#    )
#FILE(GLOB ARCH_FILE
#    "./3rdparty/tesseract_ocr/tesseract/src/arch/dotproductavx.cpp"
#    "./3rdparty/tesseract_ocr/tesseract/src/arch/dotproductfma.cpp"
#    "./3rdparty/tesseract_ocr/tesseract/src/arch/dotproductsse.cpp"
#    "./3rdparty/tesseract_ocr/tesseract/src/arch/intsimdmatrixavx2.cpp"
#    "./3rdparty/tesseract_ocr/tesseract/src/arch/intsimdmatrixavx2.cpp"
#    "./3rdparty/tesseract_ocr/tesseract/src/arch/intsimdmatrixneon.cpp"
#    "./3rdparty/tesseract_ocr/tesseract/src/arch/intsimdmatrixsse.cpp"
#    )
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

#如果添加tesseract源码则需要arm平台需多添加几个文件
#message("The host system is " ${CMAKE_HOST_SYSTEM_PROCESSOR})
#if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64")
#    add_executable(${PROJECT_NAME} ${allSource} ${COMMON_FILE} ${ARCH_FILE} ${AllQRC} ${LangSrcs} )
#else()
#    add_executable(${PROJECT_NAME} ${allSource} ${COMMON_FILE}  ${AllQRC} ${LangSrcs} )
#endif()

add_executable(${PROJECT_NAME} ${allSource} ${AllQRC} ${LangSrcs} )

if(DOTEST)
    SET(PROJECT_NAME_TEST
        ${PROJECT_NAME}_test)

    include_directories(tests)
    include_directories(src)
    include_directories(./src/service)
    include_directories(./src/tessocrutils)
    include_directories(./src/view)
    include_directories(${PROJECT_SOURCE_DIR}/3rdparty/stub_linux/)
#需要使用tesseract源码时添加tesseract三方库文件
#    include_directories(/usr/include/leptonica)
#    include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/include)
#    include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/api)
#    include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/arch)
#    include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/ccmain)
#    include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/ccstruct)
#    include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/ccutil)
#    include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/classify)
#    include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/cutil)
#    include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/dict)
#    include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/lstm)
#    include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/opencl)
#    include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/textord)
#    include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/viewer)
#    include_directories(${PROJECT_SOURCE_DIR}/3rdparty/tesseract_ocr/tesseract/src/wordrec)

    aux_source_directory(src/service allTestSource)
    aux_source_directory(src/tessocrutils allTestSource)
    aux_source_directory(src/view allTestSource)
    aux_source_directory(tests allTestSource)
    aux_source_directory(tests/tessocrutils allTestSource)
    aux_source_directory(${PROJECT_SOURCE_DIR}/3rdparty/stub_linux/ allTestSource)
#需要使用tesseract源码时添加tesseract三方库文件
#    aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/api allTestSource)
#    #aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/arch allSource)
#    aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/ccmain allTestSource)
#    aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/ccstruct allTestSource)
#    aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/ccutil allTestSource)
#    aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/opencl allTestSource)
#    aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/classify allTestSource)
#    aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/cutil allTestSource)
#    aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/dict allTestSource)
#    aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/lstm allTestSource)
#    aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/textord allTestSource)
#    aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/viewer allTestSource)
#    aux_source_directory(3rdparty/tesseract_ocr/tesseract/src/wordrec allTestSource)

    FILE(GLOB allTestSource1
        "src/loadingwidget.cpp"
        "src/mainwidget.cpp"
        "src/mainwindow.cpp"
        "src/ocrapplication.cpp"
        "src/resulttextview.cpp"
        "src/textloadwidget.cpp")
#需要使用tesseract源码时添加tesseract三方库文件
#    if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64")
#        add_executable(${PROJECT_NAME_TEST} ${allTestSource} ${COMMON_FILE} ${ARCH_FILE} ${allTestSource1} )
#    else()
#        add_executable(${PROJECT_NAME_TEST} ${allTestSource} ${COMMON_FILE}  ${allTestSource1} )
#    endif()
        add_executable(${PROJECT_NAME_TEST} ${allTestSource}  ${allTestSource1} )

    #add_executable(${PROJECT_NAME_TEST} ${allTestSource} ${allTestSource1})
    target_link_libraries(${PROJECT_NAME_TEST} gmock gtest pthread)

    add_test(${PROJECT_NAME_TEST} COMMAND ${PROJECT_NAME_TEST})

#------------------------------ 创建'make test'指令---------------------------------------
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

#        COMMAND nohup x-www-browser ${CMAKE_BINARY_DIR}/coverageResult/report/index.html &

        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    add_dependencies(test ${PROJECT_NAME_TEST})
endif()

pkg_check_modules(3rd_lib REQUIRED
        dtkwidget dtkgui
        )
target_include_directories(${PROJECT_NAME} PUBLIC ${3rd_lib_INCLUDE_DIRS})
if(DOTEST)
    target_include_directories(${PROJECT_NAME_TEST} PUBLIC ${3rd_lib_INCLUDE_DIRS})
endif()
target_link_libraries(${PROJECT_NAME} ${3rd_lib_LIBRARIES} lept tesseract pthread)
if(DOTEST)
    target_link_libraries(${PROJECT_NAME_TEST} ${3rd_lib_LIBRARIES} lept tesseract pthread)
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
        RUNTIME DESTINATION /usr/bin)

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

