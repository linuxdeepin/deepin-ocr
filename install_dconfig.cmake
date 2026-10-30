# SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: CC0-1.0

# expect that all dconfigs of plugin is saved in ./assets/configs
# usage: INSTALL_DCONFIG(<config.json> [APPID <appId>])
#        不传 APPID 时默认使用 deepin-ocr
function(INSTALL_DCONFIG CONFIG_NAME)
    cmake_parse_arguments(DCFG "" "APPID" "" ${ARGN})
    set(DConfigPath ${CMAKE_SOURCE_DIR}/assets/configs)
    message("DConfigPath: ${DConfigPath}")
    if(DCFG_APPID)
        set(AppId ${DCFG_APPID})
    else()
        set(AppId "deepin-ocr")
    endif()
    set(ConfigName ${CONFIG_NAME})

    if (DEFINED DSG_DATA_DIR)
        message("-- DConfig is supported by DTK")
        message("---- AppId: ${AppId}")
        message("---- Base: ${DConfigPath}")
        message("---- Files: ${DConfigPath}/${ConfigName}")
        if(COMMAND dtk_add_config_meta_files)
            message(STATUS "Using dtk_add_config_meta_files")
            dtk_add_config_meta_files(APPID ${AppId}
                BASE ${DConfigPath}
                FILES ${DConfigPath}/${ConfigName})
        else()
            message(STATUS "Using dconfig_meta_files")
            dconfig_meta_files(APPID ${AppId}
                BASE ${DConfigPath}
                FILES ${DConfigPath}/${ConfigName})
        endif()
    else()
        set(DSG_DATA_DIR ${CMAKE_INSTALL_PREFIX}/share/dsg)
        message("-- DConfig is NOT supported by DTK, install files into target path")
        message("---- InstallTargetDir: ${DSG_DATA_DIR}/configs")
        install(FILES ${DConfigPath}/${ConfigName} DESTINATION ${DSG_DATA_DIR}/configs/${AppId})
    endif()
endfunction()
