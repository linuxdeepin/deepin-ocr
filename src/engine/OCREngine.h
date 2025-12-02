// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <atomic>
#include <QImage>
#include <QString>

namespace Dtk {
namespace Ocr {
class DOcr;
}
}

class QSettings;

class OCREngine
{
public:
    static OCREngine *instance();

    bool isRunning() const
    {
        return m_isRunning;
    }

    bool setLanguage(const QString &language);
    void setImage(const QImage &image);
    QString getRecogitionResult();

private:
    OCREngine();
    ~OCREngine() = default;
    // 删除拷贝构造函数和赋值运算符，防止拷贝实例
    OCREngine(const OCREngine &) = delete;
    OCREngine &operator=(const OCREngine &) = delete;

    // 某些机型，使用GPU进行OCR识别，会导致OCR崩溃
    bool isGpuEnable();

    Dtk::Ocr::DOcr *ocrDriver;
    std::atomic_bool m_isRunning;
    QSettings *ocrSetting;
};
