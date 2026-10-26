// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include <QTestEventList>
#include <QObject>
#include <QStandardPaths>

#define private public
#define protected public

#include "ocrapplication.h"
#include "engine/OCREngine.h"
#include "mainwidget.h"
#include "view/imageview.h"
//初始拉起主界面
TEST(MainWindow, mainwindow)
{
    OcrApplication instance;
    instance.openFile("");
    QTest::qWait(2000);
}

//其余case
//初始拉起主界面,带有参数的
TEST(MainWindow, mainwindow_openFile)
{
    QString picPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/ocr_test/testResource/test.png";
    OcrApplication instance;
    instance.openFile(picPath);
    bool bRet = false;
    while (!bRet) {
        if (!OCREngine::instance()->isRunning()) {
            bRet = true;
            QTest::qWait(1000);
        }
    }

    instance.openImage(QImage(picPath).scaled(200, 200));

    bRet = false;
    while (!bRet) {
        if (!OCREngine::instance()->isRunning()) {
            bRet = true;
            QTest::qWait(1000);
        }
    }

    instance.openImageAndName(QImage(picPath).scaled(10, 10), picPath);
    QTest::qWait(2000);
}

//初始拉起主界面,带有参数的
TEST(MainWidget, MainWidget_show)
{
    MainWidget *a = new MainWidget();
    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::LightType;
    a->setIcons(themeType);
    a->loadHtml("");
    a->loadHtml("test");
    a->slotCopy();
    a->resultEmpty();
    a->deleteLater();
    a = nullptr;
    QTest::qWait(2000);
}

TEST(ImageView, ImageView_show)
{
    QString picPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/Wallpapers/luca-micheli-ruWkmt3nU58-unsplash.jpg";
    ImageView *imageView = new ImageView();
    imageView->openImage(picPath);
    imageView->fitImage();
    imageView->fitWindow();
    imageView->autoFit();
    imageView->imageRelativeScale();
    imageView->setScaleValue(1.1);
    imageView->setScaleValue(0.9);
    imageView->RotateImage(90);

    imageView->scaleAtPoint(QPoint(10, 10), 1.2);
    QPinchGesture *gesture = new QPinchGesture;
    imageView->pinchTriggered(gesture);
    delete gesture;

    QTest::qWait(2000);
    delete imageView;
}

TEST(ImageView, LinkedSelectionReplacesPreviousRange)
{
    ImageView imageView;
    OcrResult result;
    result.plainText = QStringLiteral("first line\nsecond line");
    imageView.setOcrResult(result);

    // Simulate a selection made on the image, then right-to-left linkage.
    imageView.setSelectionRanges({qMakePair(0, 5)});
    imageView.selectRange(11, 17);

    ASSERT_EQ(imageView.selectionRanges().size(), 1);
    EXPECT_EQ(imageView.selectionRanges().first().first, 11);
    EXPECT_EQ(imageView.selectionRanges().first().second, 17);
    EXPECT_EQ(imageView.selectedText(), QStringLiteral("second"));

    imageView.selectRange(-1, -1);
    EXPECT_TRUE(imageView.selectionRanges().isEmpty());
    EXPECT_FALSE(imageView.hasSelection());
}

TEST(OcrResult, CharacterBoxesAreNormalizedToLineBox)
{
    OcrLineItem line;
    line.text = QStringLiteral("abcd");
    line.rect = QRectF(0, 10, 100, 20);

    QList<Dtk::Ocr::TextBox> charBoxes;
    for (int i = 0; i < 4; ++i) {
        Dtk::Ocr::TextBox box;
        const qreal left = i * 12.5;
        box.points = {QPointF(left, 10), QPointF(left + 12.5, 10),
                      QPointF(left + 12.5, 30), QPointF(left, 30)};
        charBoxes.append(box);
    }

    const auto chars = OcrResult::buildCharItemsForLine(line, 0, charBoxes);

    ASSERT_EQ(chars.size(), 4);
    EXPECT_DOUBLE_EQ(chars.first().rect.left(), 0.0);
    EXPECT_DOUBLE_EQ(chars.last().rect.right(), 100.0);
    EXPECT_DOUBLE_EQ(chars.at(1).rect.left(), 25.0);
}

TEST(ResultTextView, LinkedRangesUseNativeSelectionColors)
{
    ResultTextView view;
    view.setPlainTextResult(QStringLiteral("first line\nsecond line"));

    const QPalette appPalette = DGuiApplicationHelper::instance()->applicationPalette();
    QColor expectedBackground = appPalette.color(QPalette::Highlight);
    expectedBackground.setAlpha(255);
    const QColor expectedForeground = appPalette.color(QPalette::HighlightedText);

    view.selectPlainTextRanges({qMakePair(0, 5), qMakePair(11, 17)});
    auto extras = view.extraSelections();
    ASSERT_EQ(extras.size(), 2);
    EXPECT_TRUE(view.m_linkedSelectionRenderingActive);
    EXPECT_EQ(view.palette().color(QPalette::Highlight).alpha(), 0);
    QTest::qWait(0);
    EXPECT_EQ(view.extraSelections().size(), 2);
    for (const auto &extra : extras) {
        EXPECT_EQ(extra.format.background().color(), expectedBackground);
        EXPECT_EQ(extra.format.foreground().color(), expectedForeground);
    }

    // A manual selection replaces all linked multi-range highlights.
    QTextCursor cursor = view.textCursor();
    cursor.setPosition(0);
    cursor.setPosition(3, QTextCursor::KeepAnchor);
    view.setTextCursor(cursor);
    EXPECT_TRUE(view.extraSelections().isEmpty());
    EXPECT_FALSE(view.m_linkedSelectionRenderingActive);
    EXPECT_EQ(view.palette().color(QPalette::Highlight), expectedBackground);

    view.selectPlainTextRanges({qMakePair(11, 17)});
    extras = view.extraSelections();
    ASSERT_EQ(extras.size(), 1);
    EXPECT_TRUE(view.m_linkedSelectionRenderingActive);
    EXPECT_EQ(view.palette().color(QPalette::Highlight).alpha(), 0);
    EXPECT_EQ(extras.first().format.background().color(), expectedBackground);
    EXPECT_EQ(extras.first().format.foreground().color(), expectedForeground);
}
