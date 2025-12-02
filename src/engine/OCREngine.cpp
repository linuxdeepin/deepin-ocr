// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "OCREngine.h"
#include <DOcr>
#include <QProcess>
#include <QFileInfo>
#include <QDebug>
#include <dconfigmanager.h>
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
    const QString ocrV5 = "PPOCR_V5";
    bool load = false;

    auto plugins = ocrDriver->installedPluginNames();
    if (plugins.contains(ocrV5, Qt::CaseInsensitive)) {
        if (ocrDriver->loadPlugin(ocrV5)) {
            load = true;
            m_isV5 = true;
            qCInfo(dmOcr) << "OCR V5 plugin loaded";
        } else {
            qCWarning(dmOcr) << "Failed to load OCR V5 plugin";
        }
    }

    if(!load) {
        ocrDriver->loadDefaultPlugin();
        qCInfo(dmOcr) << "Default OCR plugin loaded";
    }
 
    ocrDriver->setUseMaxThreadsCount(2);
    if (!isGpuEnable()) {
        qWarning() << "GPU is not enabled";
        return;
    }

    QProcess kxProc;
    kxProc.setProgram("bash");
    kxProc.setArguments({"-c", "lscpu | grep name"});
    kxProc.start();
    kxProc.waitForFinished();
    QString cpuInfo = kxProc.readAllStandardOutput();
    QFileInfo mtfi("/dev/mtgpu.0");
    if (mtfi.exists() | !cpuInfo.contains("KX-7000")) {
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
    QString result = ocrDriver->simpleResult();
    m_isRunning = false;
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

bool OCREngine::isGpuEnable()
{
    return DConfigManager::instance()->value(COMMON_GROUP, COMMON_ISGPUENABLE, true).toBool();
}
