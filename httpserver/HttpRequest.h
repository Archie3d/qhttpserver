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

#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QString>
#include <QVariantMap>
#include <QMultiMap>
#include <QNetworkCookie>
#include "HttpServerApi.h"
#include "HttpContentType.h"

class HTTP_API HttpRequest
{
public:

    /// Request method
    enum Method {
        Method_Options,
        Method_Get,
        Method_Head,
        Method_Post,
        Method_Put,
        Method_Delete,
        Method_Trace,
        Method_Connect,

        // Non-HTTP methods
        Method_Unknown,    ///< Unknown access method.
        Method_Invalid     ///< Request is invalid.
    };

    /**
     * Construct an empty (invalid) request.
     */
    HttpRequest();
    HttpRequest(Method method,
                const QString &uri,
                int major = 1, int minor = 1);
    HttpRequest(const HttpRequest& req);
    HttpRequest& operator =(const HttpRequest &req);

    Method method() const { return m_method; }
    void setMethod(Method m) { m_method = m; }

    QString uri() const { return m_uri; }
    void setUri(const QString &s) { m_uri = s; }

    int majorVersion() const { return m_majorVersion; }
    void setMajorVersion(int v) { m_majorVersion = v; }

    int minorVersion() const { return m_minorVersion; }
    void setMinorVersion(int v) { m_minorVersion = v; }

    bool isValid() const { return m_method != Method_Invalid && m_method != Method_Unknown; }

    const HttpContentType& contentType() const { return m_contentType; }
    void setContentType(const HttpContentType &ct) { m_contentType = ct; }

    void addArgument(const QString &name, const QVariant &value);
    void parseArguments(const QString &str);
    const QVariantMap& arguments() const { return m_arguments; }
    QVariant argument(const QString &name) { return m_arguments.value(name); }

    void addCookie(const QNetworkCookie &cookie);
    void parseCookie(const QString &cookieString);

    void addOption(const QString &name, const QVariant &value);
    QVariant option(const QString &name) const { return m_options.value(name); }

    const QByteArray& constData() const { return m_data; }
    void appendData(const QByteArray &d);
    void setData(const QByteArray &d) { m_data = d; }

    QString toString() const;

    static QString methodToString(Method method);
    static Method stringToMethod(const QString &s);

private:

    Method m_method;
    QString m_uri;
    int m_majorVersion;
    int m_minorVersion;
    HttpContentType m_contentType;
    QVariantMap m_arguments;    ///< Arguments passed in request (POST or GET).
    QMultiMap<QString, QVariant> m_options;     ///< Options.
    QMap<QString, QNetworkCookie> m_cookies;    ///< Cookies.
    QByteArray m_data;  ///< Data sent.
};

#endif // HTTPREQUEST_H
