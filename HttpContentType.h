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

#ifndef HTTPCONTENTTYPE_H
#define HTTPCONTENTTYPE_H

#include <QString>

/**
 * HTTP content type encoded
 * as type/media pair.
 */
class HttpContentType
{
public:

    const static HttpContentType ApplicationJson;
    const static HttpContentType ApplicationJavaScript;
    const static HttpContentType ApplicationOctetStream;
    const static HttpContentType ApplicationOgg;
    const static HttpContentType ApplicationPdf;
    const static HttpContentType ApplicationPostscript;
    const static HttpContentType ApplicationSoapXml;
    const static HttpContentType ApplicationXhtmlXml;
    const static HttpContentType ApplicationZip;
    const static HttpContentType ApplicationGzip;
    const static HttpContentType ApplicationDicom;
    const static HttpContentType ApplicationFromUrlEncoded;

    const static HttpContentType ImageGif;
    const static HttpContentType ImageJpeg;
    const static HttpContentType ImagePjpeg;
    const static HttpContentType ImagePng;
    const static HttpContentType ImageSvgXml;
    const static HttpContentType ImageTiff;
    const static HttpContentType ImageIcon;

    const static HttpContentType TextCmd;
    const static HttpContentType TextCss;
    const static HttpContentType TextCsv;
    const static HttpContentType TextHtml;
    const static HttpContentType TextJavaScript;
    const static HttpContentType TextPlain;
    const static HttpContentType TextXml;

    HttpContentType();
    HttpContentType(const QString &str);
    HttpContentType(const QString &type, const QString &media);
    HttpContentType(const HttpContentType &ct);
    HttpContentType& operator =(const HttpContentType &ct);
    bool operator ==(const HttpContentType &ct) const;
    bool operator !=(const HttpContentType &ct) const;

    QString type() const { return m_type; }
    void setType(const QString &s) { m_type = s; }

    QString media() const { return m_media; }
    void setMedia(const QString &s) { m_media = s; }

    QString toString() const;

    /**
     * Guess content type from file extension.
     * For unknown extensions application/octet-stream is assumed.
     * @param ext
     */
    static HttpContentType fromFileExtension(const QString &ext);

private:
    QString m_type;     ///< Content type.
    QString m_media;    ///< Content media.
};

#endif // HTTPCONTENTTYPE_H
