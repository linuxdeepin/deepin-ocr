/*
* Copyright (C) 2020 ~ 2022 Deepin Technology Co., Ltd.
*
* Author: WangZhengYang<wangzhengyang@uniontech.com>
*
* Maintainer: WangZhengYang<wangzhengyang@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "OCREngine.h"
#include <deepin-ocr-plugin-manager/deepinocrplugin.h>

OCREngine *OCREngine::instance()
{
    static OCREngine *ocr_detail = nullptr;
    if (ocr_detail == nullptr) {
        ocr_detail = new OCREngine;
    }
    return ocr_detail;
}

OCREngine::OCREngine()
{
    //初始化变量
    m_isRunning = false;

    //初始化插件管理库
    //此处存在产品设计缺陷: 无法选择插件，无鉴权入口，无性能方面的高级设置入口
    //因此此处直接硬编码使用默认插件
    ocrDriver = new DeepinOCRPlugin::DeepinOCRDriver;
    ocrDriver->loadDefaultPlugin();
    ocrDriver->setUseMaxThreadsCount(2);
    ocrDriver->setUseHardware({{DeepinOCRPlugin::HardwareID::GPU_Vulkan, 0}});
}

void OCREngine::setImage(const QImage &image)
{
    auto inputImage = image.convertToFormat(QImage::Format_RGB888);
    ocrDriver->setMatrix(inputImage.height(),
                         inputImage.width(),
                         inputImage.bits(),
                         static_cast<size_t>(inputImage.bytesPerLine()),
                         DeepinOCRPlugin::PixelType::Pixel_RGB);
}

QString OCREngine::getRecogitionResult()
{
    m_isRunning = true;

    ocrDriver->analyze();

    m_isRunning = false;
    return ocrDriver->getAllResult().c_str();
}

bool OCREngine::setLanguage(const QString &language)
{
    if(ocrDriver->isRunning()) {
        ocrDriver->breakAnalyze();
    }

    return ocrDriver->setLanguage(language.toStdString());
}
