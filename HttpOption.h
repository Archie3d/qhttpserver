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

#ifndef HTTPOPTION_H
#define HTTPOPTION_H

#include <QString>
#include <QDateTime>

/**
 * This class holds known HTTP option names.
 */
class HttpOption
{
public:

    const static QString Accept;
    const static QString AcceptCharset;
    const static QString AcceptEncoding;
    const static QString AcceptLanguage;
    const static QString Authorization;
    const static QString CacheControl;
    const static QString Connection;
    const static QString Cookie;
    const static QString ContentLength;
    const static QString ContentLocation;
    const static QString ContentMd5;
    const static QString ContentType;
    const static QString Date;
    const static QString Expect;
    const static QString From;
    const static QString Host;
    const static QString IfMatch;
    const static QString IfModifiedSince;
    const static QString IfUnmodifiedSince;
    const static QString IfRange;
    const static QString Range;
    const static QString UserAgent;

    static QDateTime asctimeToDateTime(const QString &str);
    static QDateTime rfc1036ToDateTime(const QString &str);
    static QDateTime rfc1123ToDateTime(const QString &str);

    /**
     * Try to convert a string into a valid date/time
     * applying rfc1123, rfc1036 and asctime patterns.
     * @param str
     * @return
     */
    static QDateTime stringToDateTime(const QString &str);
};

#endif // HTTPOPTION_H
