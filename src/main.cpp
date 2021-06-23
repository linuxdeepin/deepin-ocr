/*
* Copyright (C) 2020 ~ 2021 Deepin Technology Co., Ltd.
*
* Author: He MingYang Hao<hemingyang@uniontech.com>
*
* Maintainer: He MingYang <hemingyang@uniontech.com>
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

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{

    if (argc < 2) {
        qDebug() << "Cant open a null file";
        return 0;
    }


    DGuiApplicationHelper::setUseInactiveColorGroup(false);
#if(DTK_VERSION < DTK_VERSION_CHECK(5,4,0,0))
    DApplication::loadDXcbPlugin();
    QScopedPointer<DApplication> app(new DApplication(argc, argv));
#else
    QScopedPointer<DApplication> app(DApplication::globalApplication(argc, argv));
#endif
    app->setOrganizationName("deepin");
    app->setApplicationName("deepin-ocr");
    app->setApplicationVersion("1.0");

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
        // 第一次启动
        // 注册Dbus服务和对象
        dbus.registerObject("/com/deepin/Ocr", &instance);
        // 初始化适配器
        new DbusOcrAdaptor(&instance);

        if (cmdParser.isSet(dbusOption)) {
            // 第一调用已 --dbus参数启动
            qDebug() << "dbus register waiting!";
            return app->exec();
        }

        instance.openFile(QString(argv[1]));
    } else {
        // 第二次运行此应用，
        // 调用DBus接口，处理交给第一次调用的进程
        // 本进程退退出
        OcrInterface *pOcr = new OcrInterface("com.deepin.Ocr", "/com/deepin/Ocr", QDBusConnection::sessionBus(), &instance);
        qDebug() << __FUNCTION__ << __LINE__;
        pOcr->openFile(QString(argv[1]));
        //pOcr->openImage(QImage(QSize(500, 145), QImage::Format_RGB32));
        //pOcr->openImageAndName(QImage(QSize(500, 145), QImage::Format_RGB32),"test");
        return 0;
    }

    return app->exec();
}


