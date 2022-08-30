// Copyright (C) 2020 ~ 2022 Deepin Technology Co., Ltd.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <atomic>
#include <QImage>
#include <QString>

class Details;

class PaddleOCRApp
{
public:
    enum Languages {
        UNKNOWN = -1, //未知语言
        CHI_SIM,      //简体中文
        CHI_TRA,      //繁体中文
        ENG           //英文
    };

    static PaddleOCRApp *instance();

    bool isRunning() const
    {
        return m_isRunning;
    }

    QString getRecogitionResult(const QImage &image);

private:
    PaddleOCRApp();
    ~PaddleOCRApp();

    PaddleOCRApp::Languages getSystemLang();
    std::vector<std::string> loadDict(const QString &dictPath);

    Details *ocrDetails;

    std::atomic_bool m_isRunning;
};
