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

#pragma once

#include <atomic>
#include <QImage>
#include <QString>

namespace DeepinOCRPlugin {
class DeepinOCRDriver;
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

    DeepinOCRPlugin::DeepinOCRDriver *ocrDriver;
    std::atomic_bool m_isRunning;
    QSettings *ocrSetting;
};
