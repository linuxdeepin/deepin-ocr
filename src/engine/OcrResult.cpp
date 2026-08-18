// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "OcrResult.h"

#include <DOcr>
#include <docr.h>

#include <QtMath>
#include <QLineF>
#include <QVector>
#include <algorithm>

namespace {

QRectF boxRect(const Dtk::Ocr::TextBox &box)
{
    if (box.points.size() < 4) {
        return {};
    }

    qreal minX = box.points.first().x();
    qreal maxX = minX;
    qreal minY = box.points.first().y();
    qreal maxY = minY;
    for (const auto &point : box.points) {
        minX = qMin(minX, point.x());
        maxX = qMax(maxX, point.x());
        minY = qMin(minY, point.y());
        maxY = qMax(maxY, point.y());
    }
    return QRectF(minX, minY, maxX - minX, maxY - minY);
}

qreal centerY(const QRectF &rect)
{
    return rect.center().y();
}

qreal centerX(const QRectF &rect)
{
    return rect.center().x();
}

qreal characterDisplayWeight(QChar ch)
{
    const ushort unicode = ch.unicode();
    if (unicode <= 0x007F) {
        return 1.0;
    }
    if (unicode >= 0xFF01 && unicode <= 0xFF60) {
        return 1.0;
    }
    return 2.0;
}

QList<OcrCharItem> buildCharItems(const OcrLineItem &line, int lineIndex, const QList<Dtk::Ocr::TextBox> &charBoxes)
{
    QList<OcrCharItem> chars;
    if (line.text.isEmpty()) {
        return chars;
    }

    const QRectF lineRect = line.rect;
    const int textLen = line.text.size();
    chars.reserve(textLen);

    const bool useRealBoxes = (charBoxes.size() == textLen);

    // Equal-length fast path: each recognized char already has its real
    // polygon from the engine — use it verbatim so highlight/click positions
    // align with the actual glyphs in the image (V-2325 v7 problem 1). Only
    // when the counts mismatch do we fall back to weight-based synthesis,
    // distributing the line rect across the chars proportionally. Each char's
    // bounding rect is precomputed and cached in OcrCharItem::rect so the hot
    // paths (draw/lasso/click) never recompute it (V-2325 v8 perf).
    if (useRealBoxes) {
        for (int charIndex = 0; charIndex < textLen; ++charIndex) {
            OcrCharItem item;
            item.box = charBoxes.at(charIndex);
            item.rect = OcrResult::textBoxRect(item.box);
            item.lineIndex = lineIndex;
            item.charIndexInLine = charIndex;
            item.character = line.text.at(charIndex);
            chars.append(item);
        }
        return chars;
    }

    QVector<qreal> weights(textLen);
    qreal totalWeight = 0.0;
    for (int charIndex = 0; charIndex < textLen; ++charIndex) {
        weights[charIndex] = characterDisplayWeight(line.text.at(charIndex));
        totalWeight += weights[charIndex];
    }
    if (totalWeight <= 0.0) {
        totalWeight = textLen;
        weights.fill(1.0, textLen);
    }

    qreal x = lineRect.left();
    for (int charIndex = 0; charIndex < textLen; ++charIndex) {
        const qreal charWidth = lineRect.width() * weights.at(charIndex) / totalWeight;
        OcrCharItem item;
        Dtk::Ocr::TextBox box;
        box.angle = 0;
        box.points.push_back(QPointF(x, lineRect.top()));
        box.points.push_back(QPointF(x + charWidth, lineRect.top()));
        box.points.push_back(QPointF(x + charWidth, lineRect.bottom()));
        box.points.push_back(QPointF(x, lineRect.bottom()));
        item.box = box;
        item.rect = QRectF(x, lineRect.top(), charWidth, lineRect.height());
        item.lineIndex = lineIndex;
        item.charIndexInLine = charIndex;
        item.character = line.text.at(charIndex);
        chars.append(item);
        x += charWidth;
    }
    return chars;
}

QList<QList<int>> groupIntoVisualLines(QList<OcrLineItem> &rawLines)
{
    QList<int> order;
    order.reserve(rawLines.size());
    for (int i = 0; i < rawLines.size(); ++i) {
        order.append(i);
    }

    std::sort(order.begin(), order.end(), [&rawLines](int a, int b) {
        const QRectF rectA = rawLines[a].rect;
        const QRectF rectB = rawLines[b].rect;
        const qreal ya = centerY(rectA);
        const qreal yb = centerY(rectB);
        if (qAbs(ya - yb) > 4.0) {
            return ya < yb;
        }
        return centerX(rectA) < centerX(rectB);
    });

    QList<QList<int>> groups;
    for (int index : order) {
        const QRectF rect = rawLines[index].rect;
        const qreal cy = centerY(rect);
        const qreal height = rect.height();

        int targetGroup = -1;
        for (int groupIndex = 0; groupIndex < groups.size(); ++groupIndex) {
            const int rep = groups[groupIndex].constFirst();
            const QRectF repRect = rawLines[rep].rect;
            if (qAbs(cy - centerY(repRect)) < qMin(height, repRect.height()) * 0.55) {
                targetGroup = groupIndex;
                break;
            }
        }

        if (targetGroup < 0) {
            groups.append(QList<int>{index});
        } else {
            groups[targetGroup].append(index);
        }
    }

    for (auto &group : groups) {
        std::sort(group.begin(), group.end(), [&rawLines](int a, int b) {
            return centerX(rawLines[a].rect) < centerX(rawLines[b].rect);
        });
    }

    std::sort(groups.begin(), groups.end(), [&rawLines](const QList<int> &a, const QList<int> &b) {
        return centerY(rawLines[a.constFirst()].rect) < centerY(rawLines[b.constFirst()].rect);
    });

    return groups;
}

bool needSpaceBetweenBoxes(const OcrLineItem &previous, const OcrLineItem &current)
{
    const QRectF prevRect = previous.rect;
    const QRectF currRect = current.rect;
    const qreal gap = currRect.left() - prevRect.right();
    const qreal threshold = qMin(prevRect.height(), currRect.height()) * 0.35;
    return gap > threshold;
}

} // namespace

QRectF OcrResult::textBoxRect(const Dtk::Ocr::TextBox &box)
{
    return boxRect(box);
}

bool OcrResult::textBoxContains(const Dtk::Ocr::TextBox &box, const QPointF &point)
{
    return boxRect(box).contains(point);
}

OcrResult OcrResult::fromDOcr(Dtk::Ocr::DOcr *ocr)
{
    OcrResult result;
    if (ocr == nullptr) {
        return result;
    }

    const auto textBoxes = ocr->textBoxes();
    if (textBoxes.isEmpty()) {
        return result;
    }

    QList<OcrLineItem> rawLines;
    rawLines.reserve(textBoxes.size());
    for (int i = 0; i < textBoxes.size(); ++i) {
        OcrLineItem line;
        line.box = textBoxes.at(i);
        line.rect = textBoxRect(line.box);  // precompute once (V-2325 v8 perf)
        line.text = ocr->resultFromBox(i);
        if (line.text.isEmpty()) {
            continue;
        }
        line.chars = buildCharItems(line, rawLines.size(), ocr->charBoxes(i));
        rawLines.append(line);
    }

    const auto visualLines = groupIntoVisualLines(rawLines);
    int offset = 0;
    for (int visualIndex = 0; visualIndex < visualLines.size(); ++visualIndex) {
        const auto &group = visualLines.at(visualIndex);
        if (visualIndex > 0) {
            result.plainText.append('\n');
            ++offset;
        }

        for (int boxIndex = 0; boxIndex < group.size(); ++boxIndex) {
            const int rawIndex = group.at(boxIndex);
            auto &line = rawLines[rawIndex];

            if (boxIndex > 0) {
                const int prevRawIndex = group.at(boxIndex - 1);
                if (needSpaceBetweenBoxes(rawLines[prevRawIndex], line)) {
                    result.plainText.append(' ');
                    ++offset;
                }
            }

            line.plainTextStart = offset;
            result.plainText.append(line.text);
            offset += line.text.size();
            line.plainTextEnd = offset;

            for (auto &item : line.chars) {
                item.plainTextStart = line.plainTextStart + item.charIndexInLine;
                item.plainTextEnd = item.plainTextStart + 1;
                item.lineIndex = result.lines.size();
                result.allChars.append(item);
            }
            result.lines.append(line);
        }
    }

    return result;
}

int OcrResult::charIndexAt(const QPointF &imagePos) const
{
    // Strict hit-only (V-2325 v5/v7): a char is selected only when the click
    // point truly falls inside its box. Space/tab synthesized boxes are never
    // click targets (isSpace() skip), so clicking inter-word gaps returns -1
    // and the view clears the selection — no nearest-line/nearest-char snapping.
    for (int i = 0; i < allChars.size(); ++i) {
        const OcrCharItem &ch = allChars.at(i);
        if (ch.character.isSpace()) {
            continue;
        }
        if (ch.rect.contains(imagePos)) {
            return i;
        }
    }
    return -1;
}

QList<QPair<int, int>> OcrResult::plainTextRangesForRect(const QRectF &rect) const
{
    if (rect.isNull() || rect.isEmpty()) {
        return {};
    }

    QList<int> hitIndices;
    hitIndices.reserve(allChars.size());
    for (int i = 0; i < allChars.size(); ++i) {
        if (rect.intersects(allChars.at(i).rect)) {
            hitIndices.append(i);
        }
    }

    if (hitIndices.isEmpty()) {
        return {};
    }

    std::sort(hitIndices.begin(), hitIndices.end());

    QList<QPair<int, int>> ranges;
    int rangeStart = allChars.at(hitIndices.first()).plainTextStart;
    int rangeEnd = allChars.at(hitIndices.first()).plainTextEnd;
    int prevCharIndex = hitIndices.first();

    for (int k = 1; k < hitIndices.size(); ++k) {
        const int idx = hitIndices.at(k);
        const auto &item = allChars.at(idx);
        if (idx == prevCharIndex + 1) {
            rangeEnd = item.plainTextEnd;
        } else {
            ranges.append(qMakePair(rangeStart, rangeEnd));
            rangeStart = item.plainTextStart;
            rangeEnd = item.plainTextEnd;
        }
        prevCharIndex = idx;
    }
    ranges.append(qMakePair(rangeStart, rangeEnd));
    return ranges;
}

QString OcrResult::textInRanges(const QList<QPair<int, int>> &ranges) const
{
    QString result;
    for (const auto &range : ranges) {
        result.append(textInRange(range.first, range.second));
    }
    return result;
}

QString OcrResult::textInRange(int start, int end) const
{
    if (start < 0 || end <= start || start >= plainText.size()) {
        return {};
    }
    return plainText.mid(start, qMin(end, plainText.size()) - start);
}

QList<int> OcrResult::charIndicesInRange(int start, int end) const
{
    QList<int> indices;
    for (int i = 0; i < allChars.size(); ++i) {
        const auto &item = allChars.at(i);
        if (item.plainTextEnd <= start || item.plainTextStart >= end) {
            continue;
        }
        indices.append(i);
    }
    return indices;
}

QList<QRectF> OcrResult::highlightRects(int start, int end) const
{
    QList<QRectF> rects;
    if (start < 0 || end <= start) {
        return rects;
    }

    for (const auto &line : lines) {
        if (line.plainTextEnd <= start || line.plainTextStart >= end) {
            continue;
        }

        QRectF united;
        for (const auto &item : line.chars) {
            if (item.plainTextEnd <= start || item.plainTextStart >= end) {
                continue;
            }
            united = united.isNull() ? item.rect : united.united(item.rect);
        }

        if (united.isNull()) {
            united = line.rect;
        }
        if (!united.isNull()) {
            rects.append(united);
        }
    }
    return rects;
}

QList<QRectF> OcrResult::highlightRectsForRanges(const QList<QPair<int, int>> &ranges) const
{
    QList<QRectF> rects;
    for (const auto &range : ranges) {
        rects.append(highlightRects(range.first, range.second));
    }
    return rects;
}
