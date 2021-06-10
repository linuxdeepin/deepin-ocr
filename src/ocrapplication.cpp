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
#include "mainwindow.h"


OcrApplication::OcrApplication(QObject *parent) : QObject(parent)
{

}

bool OcrApplication::openFile(QString filePath)
{
    qDebug() << __FUNCTION__ << __LINE__ << filePath;
    MainWindow *win = new MainWindow();
    win->openFile(filePath);
    win->resize(800, 600);
    win->show();
    return true;
}

void OcrApplication::openImage(QImage image)
{
    qDebug() << __FUNCTION__ << __LINE__ << image.size();
    MainWindow *win = new MainWindow();
    win->openImage(image);
    win->resize(800, 600);
    win->show();
}
