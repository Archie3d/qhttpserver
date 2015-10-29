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
#include "HttpRequest.h"

const QMap<HttpRequest::Method, QString> sMethodToStringMap {
    {HttpRequest::Method_Options, "OPTIONS"},
    {HttpRequest::Method_Get, "GET"},
    {HttpRequest::Method_Head, "HEAD"},
    {HttpRequest::Method_Post, "POST"},
    {HttpRequest::Method_Put, "PUT"},
    {HttpRequest::Method_Delete, "DELETE"},
    {HttpRequest::Method_Trace, "TRACE"},
    {HttpRequest::Method_Connect, "CONNECT"},

    // Non-http standard methods
    {HttpRequest::Method_Unknown, "UNKNOWN"},
    {HttpRequest::Method_Invalid, "INVALID"}
};

const QMap<QString, HttpRequest::Method> sStringToMethodMap {
    {"OPTIONS", HttpRequest::Method_Options},
    {"GET", HttpRequest::Method_Get},
    {"HEAD", HttpRequest::Method_Head},
    {"POST", HttpRequest::Method_Post},
    {"PUT", HttpRequest::Method_Put},
    {"DELETE", HttpRequest::Method_Delete},
    {"TRACE", HttpRequest::Method_Trace},
    {"CONNECT", HttpRequest::Method_Connect},

    // Non-http standard methods
    {"UNKNOWN", HttpRequest::Method_Unknown},
    {"INVALID", HttpRequest::Method_Invalid}
};

HttpRequest::HttpRequest()
    : m_method(Method_Invalid),
      m_uri(),
      m_majorVersion(1),
      m_minorVersion(1),
      m_contentType(),
      m_arguments(),
      m_options(),
      m_cookies(),
      m_data()
{
}

HttpRequest::HttpRequest(Method method, const QString &uri, int major, int minor)
    : m_method(method),
      m_uri(uri),
      m_majorVersion(major),
      m_minorVersion(minor),
      m_contentType(),
      m_arguments(),
      m_options(),
      m_cookies(),
      m_data()
{
}

HttpRequest::HttpRequest(const HttpRequest &req)
    : m_method(req.m_method),
      m_uri(req.m_uri),
      m_majorVersion(req.m_majorVersion),
      m_minorVersion(req.m_minorVersion),
      m_contentType(req.m_contentType),
      m_arguments(req.m_arguments),
      m_options(req.m_options),
      m_cookies(req.m_cookies),
      m_data(req.m_data)
{
}

HttpRequest& HttpRequest::operator =(const HttpRequest &req)
{
    if (this != & req) {
        m_method = req.m_method;
        m_uri = req.m_uri;
        m_majorVersion = req.m_majorVersion;
        m_minorVersion = req.m_minorVersion;
        m_contentType = req.m_contentType;
        m_arguments = req.m_arguments;
        m_options = req.m_options;
        m_cookies = req.m_cookies;
        m_data = req.m_data;
    }
    return *this;
}

void HttpRequest::addArgument(const QString &name, const QVariant &value)
{
    m_arguments[name] = value;
}

void HttpRequest::parseArguments(const QString &str)
{
    QStringList args = str.split("&", QString::SkipEmptyParts);
    foreach (const QString &arg, args) {
        QStringList keyValue = arg.split("=", QString::KeepEmptyParts);
        if (keyValue.count() == 2) {
            QString key = keyValue.at(0);
            QString value = QString(QByteArray::fromPercentEncoding(keyValue.at(1).toUtf8()));
            addArgument(key, value);
        }
    }
}

void HttpRequest::addCookie(const QNetworkCookie &cookie)
{
    // TODO: Check coockie name corresponds to session holder
    if (cookie.name() == "HttpSession::cookieName") {
        //setSessionUid(QString(QByteArray::fromPersentEncoding(cookie.value())));
    } else {
        m_cookies.insert(cookie.name(), cookie);
    }
}

void HttpRequest::parseCookie(const QString &cookieString)
{
    QList<QNetworkCookie> cookies = QNetworkCookie::parseCookies(cookieString.toLatin1());
    foreach (const QNetworkCookie &cookie, cookies) {
        addCookie(cookie);
    }
}

void HttpRequest::addOption(const QString &name, const QVariant &value)
{
    m_options.insertMulti(name, value);
}

void HttpRequest::appendData(const QByteArray &d)
{
    m_data.append(d);
}

QString HttpRequest::toString() const
{
    QString str = QString("%1 %2")
            .arg(methodToString(m_method))
            .arg(m_uri);
    return str;
}

QString HttpRequest::methodToString(Method method)
{
    return sMethodToStringMap.value(method);
}

HttpRequest::Method HttpRequest::stringToMethod(const QString &s)
{
    return sStringToMethodMap.value(s.toUpper(), HttpRequest::Method_Unknown);
}
