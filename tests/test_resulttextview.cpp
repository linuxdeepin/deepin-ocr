/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co., Ltd.
 *
 * Author:     ZhangYong <zhangyong@uniontech.com>
 *
 * Maintainer: ZhangYong <ZhangYong@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QTestEventList>
#include <QObject>
#include <QStandardPaths>
#include <QResizeEvent>
#include <QSize>
#include <QGesture>
#include <QTapAndHoldGesture>

#define private public
#define protected public

#include "ocrapplication.h"
#include "mainwidget.h"
#include "view/imageview.h"
#include "resulttextview.h"
//初始拉起主界面
TEST(ResultTextView, mainwindow)
{
    ResultTextView* reTextView = new ResultTextView(nullptr);
    QResizeEvent resizeEvent(QSize(500, 200), QSize(500, 400));
    reTextView->resizeEvent(&resizeEvent);

    //QMouseEvent mouseMoveEvent;

    //e->type() == QEvent::MouseMove && e->source() == Qt::MouseEventSynthesizedByQt


    QMouseEvent *ev = new QMouseEvent(QEvent::MouseMove, QPoint(15,36), QPoint(25,40), QPoint(60,80), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier, Qt::MouseEventSynthesizedByQt);
    reTextView->mouseMoveEvent(ev);
    delete  ev;


    //reTextView->contextMenuEvent(nullptr);
    //QTest::mouseClick(reTextView, Qt::MouseButton::LeftButton);
    //reTextView->m_Menu->activeAction()->trigger();

    QMouseEvent *evPress = new QMouseEvent(QEvent::MouseButtonPress, QPoint(15,36), QPoint(25,40), QPoint(60,80), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier, Qt::MouseEventSynthesizedByQt);
    reTextView->mousePressEvent(evPress);
    delete evPress;


    QMouseEvent *evRele = new QMouseEvent(QEvent::MouseButtonRelease, QPoint(15,36), QPoint(25,40), QPoint(60,80), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier, Qt::MouseEventSynthesizedByQt);
    reTextView->mousePressEvent(evRele);
    delete evRele;


    QList<QGesture*> list;
    QTapGesture* tap = new QTapGesture();
    list.append(tap);
    QGestureEvent *gestureEvent = new QGestureEvent(list);
    reTextView->event(gestureEvent);
    delete gestureEvent;
    delete tap;


    QList<QGesture*> list1;
    QTapAndHoldGesture* tapAndHold = new QTapAndHoldGesture();
    list1.append(tapAndHold);
    QGestureEvent *gestureEvent1 = new QGestureEvent(list1);
    reTextView->event(gestureEvent1);
    delete gestureEvent1;
    delete tapAndHold;

    QList<QGesture*> list2;
    QPanGesture* pan = new QPanGesture();
    list2.append(pan);
    QGestureEvent *gestureEvent2 = new QGestureEvent(list2);
    reTextView->event(gestureEvent2);
    delete gestureEvent2;
    delete pan;

    QList<QGesture*> list3;
    QPinchGesture* pinch = new QPinchGesture();
    list3.append(pinch);
    QGestureEvent *gestureEvent3 = new QGestureEvent(list3);
    reTextView->event(gestureEvent3);
    delete gestureEvent3;
    delete pinch;


    reTextView->slideGestureY(2.5);
    reTextView->slideGestureX(2.5);

    delete reTextView;
}


