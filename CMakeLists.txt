# SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: CC0-1.0

cmake_minimum_required(VERSION 3.10)
project(deepin-ocr)

option(DOTEST "option for test" OFF)

# 启用位置无关代码，提高安全性（ASLR）
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#deepin-ocr
add_subdirectory(src)
