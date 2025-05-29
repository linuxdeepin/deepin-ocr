// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "OCREngine.h"
#include <QFileInfo>
#include <DOcr>
#include "util/log.h"

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
    qCInfo(dmOcr) << "Initializing OCR driver";
    ocrDriver = new Dtk::Ocr::DOcr;
    ocrDriver->loadDefaultPlugin();
    ocrDriver->setUseMaxThreadsCount(2);
    QFileInfo mtfi("/dev/mtgpu.0");
    if (mtfi.exists()) {
        qCInfo(dmOcr) << "GPU device found, enabling Vulkan hardware acceleration";
        ocrDriver->setUseHardware({{Dtk::Ocr::HardwareID::GPU_Vulkan, 0}});
    }
    qCInfo(dmOcr) << "OCR driver initialization completed";
}

void OCREngine::setImage(const QImage &image)
{
    auto inputImage = image.convertToFormat(QImage::Format_RGB888);
    ocrDriver->setImage(image);
}

QString OCREngine::getRecogitionResult()
{
    qCInfo(dmOcr) << "Starting OCR recognition";
    m_isRunning = true;

    ocrDriver->analyze();

    m_isRunning = false;
    QString result = ocrDriver->simpleResult();
    qCInfo(dmOcr) << "OCR recognition completed";
    return result;
}

bool OCREngine::setLanguage(const QString &language)
{
    qCInfo(dmOcr) << "Setting OCR language to:" << language;
    if(ocrDriver->isRunning()) {
        qCInfo(dmOcr) << "Breaking current analysis for language change";
        ocrDriver->breakAnalyze();
    }

    bool success = ocrDriver->setLanguage(language);
    if (!success) {
        qCWarning(dmOcr) << "Failed to set language:" << language;
    }
    return success;
}
