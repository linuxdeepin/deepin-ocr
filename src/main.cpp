// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ocrapplication.h"
#include "service/ocrinterface.h"
#include "service/dbusocr_adaptor.h"

#include <DWidget>
#include <DLog>
#include <DWindowManagerHelper>
#include <DWidgetUtil>
#include <DGuiApplicationHelper>
#include <DApplication>

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include "util/log.h"

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{

    if (argc < 2) {
        qDebug() << "Cant open a null file";
        return 0;
    }

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    DGuiApplicationHelper::setUseInactiveColorGroup(false);
#endif

#if(DTK_VERSION < DTK_VERSION_CHECK(5,4,0,0))
    DApplication::loadDXcbPlugin();
    QScopedPointer<DApplication> app(new DApplication(argc, argv));
#else
    QScopedPointer<DApplication> app(DApplication::globalApplication(argc, argv));
#endif
    app->setOrganizationName("deepin");
    app->setApplicationName("deepin-ocr");
    app->setProductName(QObject::tr("OCR Tool"));
    app->setApplicationVersion("1.0");

    qCInfo(dmOcr) << "Starting Deepin OCR Tool version 1.0";

    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();

    QCommandLineOption dbusOption(QStringList() << "u" << "dbus", "Start  from dbus.");
    QCommandLineParser cmdParser;
    cmdParser.setApplicationDescription("deepin-Ocr");
    cmdParser.addHelpOption();
    cmdParser.addVersionOption();
    cmdParser.addOption(dbusOption);
    cmdParser.process(*app);

    app->loadTranslator();

    OcrApplication instance;
    QDBusConnection dbus = QDBusConnection::sessionBus();

    if (dbus.registerService("com.deepin.Ocr")) {
        qCInfo(dmOcr) << "Registered as primary DBus service";
        // 第一次启动
        // 注册Dbus服务和对象
        dbus.registerObject("/com/deepin/Ocr", &instance);
        // 初始化适配器
        new DbusOcrAdaptor(&instance);

        if (cmdParser.isSet(dbusOption)) {
            // 第一调用已 --dbus参数启动
            qCInfo(dmOcr) << "Started in DBus mode, waiting for connections";
            return app->exec();
        }

        instance.openFile(QString(argv[1]));
    } else {
        qCInfo(dmOcr) << "Another instance is running, forwarding request";
        // 第二次运行此应用，
        // 调用DBus接口，处理交给第一次调用的进程
        // 本进程退退出
        OcrInterface *pOcr = new OcrInterface("com.deepin.Ocr", "/com/deepin/Ocr", QDBusConnection::sessionBus(), &instance);
        qDebug() << __FUNCTION__ << __LINE__;
        pOcr->openFile(QString(argv[1]));
        delete pOcr;
        return 0;
    }

    return app->exec();
}


