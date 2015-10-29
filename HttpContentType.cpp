/*
                          qhttpserver

    Copyright (C) 2015 Arthur Benilov,
    arthur.benilov@gmail.com

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Lesser General Public License for more details.
*/

#include <QMap>
#include "HttpContentType.h"

const HttpContentType HttpContentType::ApplicationJson("application", "json");
const HttpContentType HttpContentType::ApplicationJavaScript("application", "javascript");
const HttpContentType HttpContentType::ApplicationOctetStream("application", "octet-stream");
const HttpContentType HttpContentType::ApplicationOgg("application", "ogg");
const HttpContentType HttpContentType::ApplicationPdf("application", "pdf");
const HttpContentType HttpContentType::ApplicationPostscript("application", "postscript");
const HttpContentType HttpContentType::ApplicationSoapXml("application", "soap+xml");
const HttpContentType HttpContentType::ApplicationXhtmlXml("application", "xhtml+xml");
const HttpContentType HttpContentType::ApplicationZip("application", "zip");
const HttpContentType HttpContentType::ApplicationGzip("application", "x-gzip");
const HttpContentType HttpContentType::ApplicationDicom("application", "dicom");
const HttpContentType HttpContentType::ApplicationFromUrlEncoded("application", "x-www-from-urlencoded");

const HttpContentType HttpContentType::ImageGif("image", "gif");
const HttpContentType HttpContentType::ImageJpeg("image", "jpeg");
const HttpContentType HttpContentType::ImagePjpeg("image", "pjpeg");
const HttpContentType HttpContentType::ImagePng("image", "png");
const HttpContentType HttpContentType::ImageSvgXml("image", "svg+xml");
const HttpContentType HttpContentType::ImageTiff("image", "tiff");
const HttpContentType HttpContentType::ImageIcon("image", "vnd.microsoft.icon");

const HttpContentType HttpContentType::TextCmd("text", "cmd");
const HttpContentType HttpContentType::TextCss("text", "css");
const HttpContentType HttpContentType::TextCsv("text", "csv");
const HttpContentType HttpContentType::TextHtml("text", "html");
const HttpContentType HttpContentType::TextJavaScript("text", "javascript");
const HttpContentType HttpContentType::TextPlain("text", "plain");
const HttpContentType HttpContentType::TextXml("text", "xml");

QMap<QString, HttpContentType> cExtToContentMap {
    {"htm", HttpContentType::TextHtml},
    {"html", HttpContentType::TextHtml},
    {"css", HttpContentType::TextCss},
    {"csv", HttpContentType::TextCsv},
    {"cmd", HttpContentType::TextCmd},
    {"js", HttpContentType::TextJavaScript},
    {"xml", HttpContentType::TextXml},
    {"txt", HttpContentType::TextPlain},

    {"gif", HttpContentType::ImageGif},
    {"ico", HttpContentType::ImageIcon},
    {"jpg", HttpContentType::ImageJpeg},
    {"jpeg", HttpContentType::ImageJpeg},
    {"png", HttpContentType::ImagePng},
    {"tiff", HttpContentType::ImageTiff},

    {"dcm", HttpContentType::ApplicationDicom},
    {"zip", HttpContentType::ApplicationZip},
    {"gz", HttpContentType::ApplicationGzip},
    {"pdf", HttpContentType::ApplicationPdf},
    {"ogg", HttpContentType::ApplicationOgg}
};

HttpContentType::HttpContentType()
    : m_type(),
      m_media()
{
}

HttpContentType::HttpContentType(const QString &type, const QString &media)
    : m_type(type),
      m_media(media)
{
}

HttpContentType::HttpContentType(const QString &str)
{
    int pos = str.indexOf("/");
    m_type = str.left(pos);
    m_media = str.mid(pos + 1);
}

HttpContentType::HttpContentType(const HttpContentType &ct)
    : m_type(ct.m_type),
      m_media(ct.m_media)
{
}

HttpContentType& HttpContentType::operator =(const HttpContentType &ct)
{
    if (this != &ct) {
        m_type = ct.m_type;
        m_media = ct.m_media;
    }
    return *this;
}

bool HttpContentType::operator ==(const HttpContentType &ct) const
{
    return m_type == ct.m_type && m_media == ct.m_media;
}

bool HttpContentType::operator !=(const HttpContentType &ct) const
{
    return m_type != ct.m_type || m_media != ct.m_media;
}

QString HttpContentType::toString() const
{
    return QString("%1/%2").arg(m_type).arg(m_media);
}

HttpContentType HttpContentType::fromFileExtension(const QString &ext)
{
    return cExtToContentMap.value(ext.toLower(), HttpContentType::ApplicationOctetStream);
}
