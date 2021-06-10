#定义需要的cmake版本
cmake_minimum_required(VERSION 3.10)
project(deepin-ocr)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

#先查找到这些qt相关的模块以供链接使用
set(QtModule Core Gui Widgets DBus)
find_package(Qt5 REQUIRED ${QtModule})

#文件夹包含
include_directories(src)
aux_source_directory(./src allSource)


FILE (GLOB allHeaders
    ./src/mainwindow.h
    )

FILE (GLOB allSource
     ./src/mainwindow.cpp
    )

# 翻译



#file(GLOB TS LIST_DIRECTORIES false ./translations/${PROJECT_NAME}*.ts)
#set_source_files_properties(${TS}
    #PROPERTIES OUTPUT_LOCATION ${PROJECT_SOURCE_DIR}/translations)

#qt5_create_translation(QM ${allSource} ${TS})
#set(TS_FILES
#        ./translations/deepin-ocr_zh_CN.ts
##        ./translations/deepin-ocr_zh_TW.ts
#        ./translations/deepin-ocr.ts)
#find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} LinguistTools REQUIRED)
#qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES} OPTIONS)
#qt5_add_translation(qmFiles ${TargetTsFiles})
#set_source_files_properties(${qmfiles} PROPERTIES OUTPUT_LOCATION "translations")

set(LINK_DIR lib)
link_directories(${LINK_DIR})


add_executable(${PROJECT_NAME} ${allSource} ${allHeaders} "./src/main.cpp")
add_compile_definitions(LITE_DIV)

#------------------添加第三方库begins--------------------------#
#使用第三方库需要用到的一个包
#检查第三方库，然后取名3rd_lib
find_package(PkgConfig REQUIRED)
find_package(DtkCore REQUIRED)
find_package(DtkWidget REQUIRED)
find_package(DtkGui REQUIRED)

#添加第三方库的所有文件夹路径到工程中来(注意 *_INCLUDE_DIRS)
target_include_directories(${PROJECT_NAME} PUBLIC ${3rd_lib_INCLUDE_DIRS})

target_include_directories(${PROJECT_NAME} PUBLIC ${3rd_lib_INCLUDE_DIRS} ${DtkWidget_INCLUDE_DIRS})
if(DOTEST)
    target_include_directories(${PROJECT_NAME_TEST} PUBLIC ${3rd_lib_INCLUDE_DIRS} ${DtkWidget_INCLUDE_DIRS})
endif()

target_link_libraries(${PROJECT_NAME}
    ${DtkWidget_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
)




find_package(Qt5LinguistTools REQUIRED)
#find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} LinguistTools REQUIRED)
file(GLOB TargetTsFiles LIST_DIRECTORIES false ./translations/deepin-ocr.ts)
#set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)
qt5_create_translation(LangSrcs ${allSource} "ss.ts")
qt5_add_translation(${TargetTsFiles})
#set_source_files_properties(${qmfiles} PROPERTIES OUTPUT_LOCATION "translations")

#------------------添加第三方库end--------------------------#

#将工程与这些模块链接起来
qt5_use_modules(${PROJECT_NAME} ${QtModule})

if(DOTEST)
qt5_use_modules(${PROJECT_NAME_TEST} ${QtModule})
endif()


# ------------------------------- install report script -------------------------------
set(PREFIX /usr)
set(TranslationDir ${PREFIX}/share/${PROJECT_NAME}/translations/)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION /usr/bin
        INCLUDES DESTINATION allHeaders)

#install
install(FILES ${LangSrcs} DESTINATION ${TranslationDir})
