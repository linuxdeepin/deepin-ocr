// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dbusocr_adaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QWidget>
#include <QDebug>
#include "util/log.h"

DbusOcrAdaptor::DbusOcrAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

DbusOcrAdaptor::~DbusOcrAdaptor()
{
    // destructor
}

bool DbusOcrAdaptor::openFile(QString filePath)
{
    qCInfo(dmOcr) << "Opening file via DBus:" << filePath;
    QMetaObject::invokeMethod(parent(), "openFile", Q_ARG(QString, filePath));
    return true;
}

void DbusOcrAdaptor::openImageAndName(QByteArray images, QString imageName)
{
    qCInfo(dmOcr) << __FUNCTION__ << __LINE__;
    QByteArray data = images;
    QString tmp_data = QString::fromLatin1(data.data(), data.size());
    QByteArray srcData = QByteArray::fromBase64(tmp_data.toLatin1());
    data = qUncompress(srcData);
    QImage image;
    if (!image.loadFromData(data)) {
        qCWarning(dmOcr) << "Failed to load image data for:" << imageName;
        return;
    }
    qCDebug(dmOcr) << "Image loaded successfully, size:" << image.size();
    QMetaObject::invokeMethod(parent(), "openImageAndName", Q_ARG(QImage, image), Q_ARG(QString, imageName));
}

void DbusOcrAdaptor::openImage(QByteArray images)
{
    qCInfo(dmOcr) << "Opening image via DBus";
    QByteArray data = images;
    QString tmp_data = QString::fromLatin1(data.data(), data.size());
    QByteArray srcData = QByteArray::fromBase64(tmp_data.toLatin1());
    data = qUncompress(srcData);
    QImage image;
    if (!image.loadFromData(data)) {
        qCWarning(dmOcr) << "Failed to load image data";
        return;
    }
    qCDebug(dmOcr) << "Image loaded successfully, size:" << image.size();
    QMetaObject::invokeMethod(parent(), "openImage", Q_ARG(QImage, image));
}

