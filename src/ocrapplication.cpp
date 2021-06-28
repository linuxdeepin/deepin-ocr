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
#include "tessocrutils/tessocrutils.h"
#include <DWidgetUtil>

OcrApplication::OcrApplication(QObject *parent) : QObject(parent)
{

}

bool OcrApplication::openFile(QString filePath)
{
    qDebug() << __FUNCTION__ << __LINE__ << filePath;
    bool bRet = false;
    if(!TessOcrUtils::instance()->isRunning())
    {
        MainWindow *win = new MainWindow();
        //增加判断，空图片不会启动
        bRet = win->openFile(filePath);
        if (bRet) {
            win->show();
            //第一次启动才居中
            if (m_loadingCount == 0) {
                Dtk::Widget::moveToCenter(win);
                m_loadingCount++;
            }
        }
    }else {
        qDebug() << "正在识别中！";
    }

    return bRet;
}

void OcrApplication::openImage(QImage image)
{
    //增加判断，空图片不会启动
    if (!image.isNull() && image.width() >= 1) {
        qDebug() << __FUNCTION__ << __LINE__ << image.size();
        if(!TessOcrUtils::instance()->isRunning())
        {
            MainWindow *win = new MainWindow();
            win->openImage(image);
            win->show();
            //第一次启动才居中
            if (m_loadingCount == 0) {
                Dtk::Widget::moveToCenter(win);
                m_loadingCount++;
            }
        }else {
            qDebug() << "正在识别中！";
        }
    }
}

void OcrApplication::openImageAndName(QImage image, QString imageName)
{
    //增加判断，空图片不会启动
    if (!image.isNull() && image.width() >= 1) {
        if(!TessOcrUtils::instance()->isRunning()){
            MainWindow *win = new MainWindow();
            win->openImage(image, imageName);
            win->show();
            //第一次启动才居中
            if (m_loadingCount == 0) {
                Dtk::Widget::moveToCenter(win);
                m_loadingCount++;
            }
        }else {
            qDebug() << "正在识别中！";
        }
    }
}

