// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "mainwindow.h"
#include "mainwidget.h"
#include "service/dbusocr_adaptor.h"
#include "util/log.h"

#include <QLabel>
#include <QDBusConnection>

#include <DTitlebar>

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    if (!m_mainWidget) {
        m_mainWidget = new MainWidget(this);
    }
    this->setCentralWidget(m_mainWidget);
    this->resize(840, 550);
    this->setMinimumSize(840, 550);
}

MainWindow::~MainWindow()
{

}

bool MainWindow::openFile(const QString &filePaths)
{
    qCInfo(dmOcr) << "Opening file in main window:" << filePaths;
    //更改打开判断文件是否是图片文件
    bool success = m_mainWidget->openImage(filePaths);
    if (!success) {
        qCWarning(dmOcr) << "Failed to open file in main window:" << filePaths;
    }
    return success;
}

bool MainWindow::openImage(const QImage &image, const QString &name)
{
    qCInfo(dmOcr) << "Opening image in main window, size:" << image.size() << "name:" << name;
    m_mainWidget->openImage(image, name);
    return true;
}
