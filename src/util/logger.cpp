// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logger.h"

#include <QLoggingCategory>
#include <QDebug>

#include <DConfig>

#include "log.h"

DCORE_USE_NAMESPACE

MLogger::MLogger(QObject *parent)
    : QObject(parent), m_rules("")
#ifdef DTKCORE_CLASS_DConfig
    , m_config(nullptr)
#endif
{
    QByteArray logRules = qgetenv("QT_LOGGING_RULES");
    // 在 Qt 日志系统初始化前置空 QT_LOGGING_RULES，避免环境变量规则
    // 与 DConfig 规则叠加；此时一定不能有任何日志打印，否则置空不会生效
    qputenv("QT_LOGGING_RULES", "");

    // set env
    m_rules = logRules;

#ifdef DTKCORE_CLASS_DConfig
    // set dconfig
    m_config = DConfig::create("org.deepin.ocr", "org.deepin.ocr");
    if (!m_config) {
        // meta 缺失时兑底：仅沿用环境变量规则，避免空指针崩溃
        setRules(m_rules);
        return;
    }
    logRules = m_config->value("log_rules").toByteArray();
    appendRules(logRules);
    setRules(m_rules);

    // watch dconfig
    connect(m_config, &DConfig::valueChanged, this, [this](const QString &key) {
        if (key == "log_rules") {
            setRules(m_config->value(key).toByteArray());
        }
    });
#else
    setRules(m_rules);
#endif
}

MLogger::~MLogger()
{
#ifdef DTKCORE_CLASS_DConfig
    if (m_config) {
        m_config->deleteLater();
    }
#endif
}

void MLogger::setRules(const QString &rules)
{
    auto tmpRules = rules;
    // Qt 限制：setFilterRules 的规则分隔符必须为 \n，不能是 ;
    m_rules = tmpRules.replace(";", "\n");
    QLoggingCategory::setFilterRules(m_rules);
}

void MLogger::appendRules(const QString &rules)
{
    QString tmpRules = rules;
    tmpRules = tmpRules.replace(";", "\n");
    auto tmplist = tmpRules.split('\n');
    for (int i = 0; i < tmplist.count(); i++)
        if (m_rules.contains(tmplist.at(i))) {
            tmplist.removeAt(i);
            i--;
        }
    if (tmplist.isEmpty())
        return;
    m_rules.isEmpty() ? m_rules = tmplist.join("\n")
                      : m_rules += "\n" + tmplist.join("\n");
}
