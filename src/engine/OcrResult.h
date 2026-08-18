// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <docr.h>

#include <QChar>
#include <QList>
#include <QPointF>
#include <QRectF>
#include <QString>

namespace Dtk {
namespace Ocr {
class DOcr;
}
}

struct OcrCharItem
{
    Dtk::Ocr::TextBox box;
    QRectF rect;                 // Cached bounding rect of #box (V-2325 v8 perf).
    int lineIndex = -1;
    int charIndexInLine = -1;
    QChar character;
    int plainTextStart = -1;
    int plainTextEnd = -1;
};

struct OcrLineItem
{
    Dtk::Ocr::TextBox box;
    QRectF rect;                 // Cached bounding rect of #box (V-2325 v8 perf).
    QString text;
    int plainTextStart = -1;
    int plainTextEnd = -1;
    QList<OcrCharItem> chars;
};

struct OcrResult
{
    QString plainText;
    QList<OcrLineItem> lines;
    QList<OcrCharItem> allChars;

    bool isEmpty() const
    {
        return lines.isEmpty();
    }

    static OcrResult fromDOcr(Dtk::Ocr::DOcr *ocr);

    int charIndexAt(const QPointF &imagePos) const;
    QString textInRange(int start, int end) const;
    QString textInRanges(const QList<QPair<int, int>> &ranges) const;
    QList<QPair<int, int>> plainTextRangesForRect(const QRectF &rect) const;
    QList<int> charIndicesInRange(int start, int end) const;
    QList<QRectF> highlightRects(int start, int end) const;
    QList<QRectF> highlightRectsForRanges(const QList<QPair<int, int>> &ranges) const;

    static QRectF textBoxRect(const Dtk::Ocr::TextBox &box);
    static bool textBoxContains(const Dtk::Ocr::TextBox &box, const QPointF &point);
};
