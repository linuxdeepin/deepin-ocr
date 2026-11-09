// SPDX-FileCopyrightText: 2023-2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "OCREngine.h"
#include <DOcr>
#include <QProcess>
#include <QFileInfo>
#include <QDebug>
#include <QLibrary>
#include <dconfigmanager.h>

namespace {
// ncnn::destroy_gpu_instance() 的导出符号（声明见 ncnn/gpu.h）。
// 这里通过符号解析调用，避免为维护分支新增 libncnn 的编译/链接依赖。
constexpr char kNcnnDestroyGpuInstance[] = "_ZN4ncnn20destroy_gpu_instanceEv";
}

OCREngine *OCREngine::s_instance = nullptr;

OCREngine *OCREngine::instance()
{
    if (s_instance == nullptr) {
        s_instance = new OCREngine;
    }
    return s_instance;
}

void OCREngine::shutdown()
{
    if (s_instance == nullptr)
        return;

    // 先释放 OCR 驱动，其内部 ncnn Net/Mat 持有 GPU 资源，必须在 GPU 实例销毁前释放
    delete s_instance->ocrDriver;
    s_instance->ocrDriver = nullptr;

    // 在进程退出前显式销毁 ncnn 全局 Vulkan GPU 实例。若留到进程退出阶段，
    // Vulkan ICD 可能已经先卸载，vkDestroyImageView() 会使用失效的 VkDevice 导致崩溃。
    QLibrary ncnnLib(QStringLiteral("libncnn.so.1"));
    using DestroyGpuInstance = void (*)();
    auto destroyGpuInstance = reinterpret_cast<DestroyGpuInstance>(
                ncnnLib.resolve(kNcnnDestroyGpuInstance));
    if (destroyGpuInstance != nullptr) {
        destroyGpuInstance();
    } else {
        qWarning() << "Failed to resolve ncnn::destroy_gpu_instance";
    }
}

OCREngine::OCREngine()
{
    //初始化变量
    m_isRunning = false;

    //初始化插件管理库
    //此处存在产品设计缺陷: 无法选择插件，无鉴权入口，无性能方面的高级设置入口
    //因此此处直接硬编码使用默认插件
    ocrDriver = new Dtk::Ocr::DOcr;
    ocrDriver->loadDefaultPlugin();
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
        ocrDriver->setUseHardware({{Dtk::Ocr::HardwareID::GPU_Vulkan, 0}});
    }
}

void OCREngine::setImage(const QImage &image)
{
    auto inputImage = image.convertToFormat(QImage::Format_RGB888);
    ocrDriver->setImage(image);
}

QString OCREngine::getRecogitionResult()
{
    m_isRunning = true;

    ocrDriver->analyze();

    m_isRunning = false;
    return ocrDriver->simpleResult();
}

bool OCREngine::setLanguage(const QString &language)
{
    if(ocrDriver->isRunning()) {
        ocrDriver->breakAnalyze();
    }

    return ocrDriver->setLanguage(language);
}

bool OCREngine::isGpuEnable()
{
    return DConfigManager::instance()->value(COMMON_GROUP, COMMON_ISGPUENABLE, true).toBool();
}
