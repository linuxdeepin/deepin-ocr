// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DEEPIN_OCR_LOGGER_H
#define DEEPIN_OCR_LOGGER_H

#include <QObject>
#include <dtkcore_global.h>

#ifdef DTKCORE_CLASS_DConfig
DCORE_BEGIN_NAMESPACE
class DConfig;
DCORE_END_NAMESPACE
#endif

// 动态日志规则监听器：从 DConfig(org.deepin.ocr) 读取 log_rules 并应用，
// 监听运行中的配置变更，实现调试模式动态调整日志级别（参考 deepin-reader 的 MLogger）。
// 必须在 Application 实例化之前创建，且实例需存活整个进程，否则 valueChanged 监听失效。
class MLogger : public QObject
{
    Q_OBJECT
public:
    explicit MLogger(QObject *parent = nullptr);
    ~MLogger() override;

    inline QString rules() const { return m_rules; }
    void setRules(const QString &rules);

private:
    void appendRules(const QString &rules);

private:
    QString m_rules;
#ifdef DTKCORE_CLASS_DConfig
    Dtk::Core::DConfig *m_config;
#endif
};

#endif   // LOGGER_H
