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
#include <QRegExp>
#include "HttpOption.h"

const QString HttpOption::Accept("Accept");
const QString HttpOption::AcceptCharset("Accept-Charset");
const QString HttpOption::AcceptEncoding("Accept-Encoding");
const QString HttpOption::AcceptLanguage("Accept-Language");
const QString HttpOption::Authorization("Authorization");
const QString HttpOption::CacheControl("Cache-Control");
const QString HttpOption::Connection("Connection");
const QString HttpOption::Cookie("Cookie");
const QString HttpOption::ContentLength("Content-Length");
const QString HttpOption::ContentLocation("Content-Location");
const QString HttpOption::ContentMd5("Content-MD5");
const QString HttpOption::ContentType("Content-Type");
const QString HttpOption::Date("Date");
const QString HttpOption::Expect("Expect");
const QString HttpOption::From("From");
const QString HttpOption::Host("Host");
const QString HttpOption::IfMatch("If-Match");
const QString HttpOption::IfModifiedSince("If-Modified-Since");
const QString HttpOption::IfUnmodifiedSince("If-Unmodified-Since");
const QString HttpOption::IfRange("If-Range");
const QString HttpOption::Range("Range");
const QString HttpOption::UserAgent("UserAgent");

const QRegExp cRegExpAscTime("(?:\\w{3})\\s+"   // Day of week (not captured)
                             "(\\w{3})\\s+"     // Month
                             "(\\d{1,2})\\s+"   // Day
                             "(\\d{2}):"        // Hours
                             "(\\d{2}):"        // Minutes
                             "(\\d{2})\\s+"     // Seconds
                             "(\\d{4})"         // Year
                            );

const QRegExp cRegExpRfc1036Time("(?:\\w{3},\\s*)"      // Day of week (not captured)
                                 "(\\d{1,2})-"          // Day
                                 "(\\w{3})-"            // Month
                                 "(\\d{2})\\s+"         // Year
                                 "(\\d{2}):"            // Hours
                                 "(\\d{2}):"            // Minutes
                                 "(\\d{2})\\s+"         // Seconds
                                 "GMT"
                                );

const QRegExp cRegExpRfc1123Time("(?:\\w{3},\\s*)?"         // Day of the week (not captured)
                                 "(\\d{1,2})\\s+"           // Day
                                 "(\\w{3})\\s+"             // Month
                                 "(\\d{2}(?:\\d{2})?)\\s+"  // Year
                                 "(\\d{2}):"                // Hours
                                 "(\\d{2})"                 // Minutes
                                 "(?::(\\d{2}))?\\s*"       // Seconds
                                 "GMT"
                                );

QMap<QString, int> cMonthNameToNumberMap {
    {"Jan", 1},
    {"Feb", 2},
    {"Mar", 3},
    {"Apr", 4},
    {"May", 5},
    {"Jun", 6},
    {"Jul", 7},
    {"Aug", 8},
    {"Sep", 9},
    {"Oct", 10},
    {"Nov", 11},
    {"Dec", 12}
};

QDateTime HttpOption::asctimeToDateTime(const QString &str)
{
    QRegExp regexp(cRegExpAscTime);

    if (regexp.indexIn(str) < 0) {
        // No match to asctime format
        return QDateTime();
    }

    if (!regexp.captureCount()) {
        return QDateTime();
    }

    // E.g. "Sun Nov  6 08:49:37 1994"

    int year = regexp.cap(6).toInt();
    int day = regexp.cap(2).toInt();

    QString strMonth(regexp.cap(1));
    int month = cMonthNameToNumberMap.value(strMonth, 0);
    if (month < 1 || month > 12) {
        // Invalid month name
        return QDateTime();
    }

    int hours = regexp.cap(3).toInt();
    int minutes = regexp.cap(4).toInt();
    int seconds = regexp.cap(5).toInt();

    return QDateTime(QDate(year, month, day),
                     QTime(hours, minutes, seconds), Qt::UTC);
}

QDateTime HttpOption::rfc1036ToDateTime(const QString &str)
{
    QRegExp regexp(cRegExpRfc1036Time);

    if (regexp.indexIn(str) < 0) {
        // No match to RFC-1036 format
        return QDateTime();
    }

    if (!regexp.captureCount()) {
        return QDateTime();
    }

    // E.g. "Sunday, 06-Nov-94 08:49:37 GMT"

    int year = regexp.cap(3).toInt() + 1900;
    int day = regexp.cap(1).toInt();

    QString strMonth(regexp.cap(2));
    int month = cMonthNameToNumberMap.value(strMonth, 0);
    if (month < 1 || month > 12) {
        // Invalid month
        return QDateTime();
    }

    int hours = regexp.cap(4).toInt();
    int minutes = regexp.cap(5).toInt();
    int seconds = regexp.cap(6).toInt();

    return QDateTime(QDate(year, month, day),
                     QTime(hours, minutes, seconds), Qt::UTC);
}

QDateTime HttpOption::rfc1123ToDateTime(const QString &str)
{
    QRegExp regexp(cRegExpRfc1123Time);

    if (regexp.indexIn(str) < 0) {
        // No match to RFC-1123 format
        return QDateTime();
    }

    if (!regexp.captureCount()) {
        return QDateTime();
    }

    // E.g. Sun, 06 Nov 1994 08:49:37 GMT

    int year;
    int day = regexp.cap(1).toInt();

    QString strMonth(regexp.cap(2));
    int month = cMonthNameToNumberMap.value(strMonth, 0);
    if (month < 1 || month > 12) {
        // Invalid month
        return QDateTime();
    }

    QString strYear(regexp.cap(3));
    if (strYear.length() == 2) {
        year = strYear.toInt() + 1900;
    } else {
        year = strYear.toInt();
    }

    int hours = regexp.cap(4).toInt();
    int minutes = regexp.cap(5).toInt();
    int seconds = 0;

    QString strSeconds(regexp.cap(6));
    if (!strSeconds.isEmpty()) {
        seconds = strSeconds.toInt();
    }

    return QDateTime(QDate(year, month, day),
                     QTime(hours, minutes, seconds), Qt::UTC);
}

QDateTime HttpOption::stringToDateTime(const QString &str)
{
    QDateTime dt = rfc1123ToDateTime(str);
    if (!dt.isValid()) {
        dt = rfc1036ToDateTime(str);
        if (!dt.isValid()) {
            dt = asctimeToDateTime(str);
        }
    }
    return dt;
}
