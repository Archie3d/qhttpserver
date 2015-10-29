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

#ifndef HTTPFILEREQUESTHANDLER_H
#define HTTPFILEREQUESTHANDLER_H

#include "HttpRequestHandler.h"

class HttpFileRequestHandler : public HttpRequestHandler
{
    Q_OBJECT
public:

    explicit HttpFileRequestHandler(QObject *pParent = nullptr);
    ~HttpFileRequestHandler();

    void setRootDirectory(const QString &path) { m_rootDirectory = path; }
    QString rootDirectory() const { return m_rootDirectory; }

    bool allowDirectoryListing() const { return m_allowDirectoryListing; }
    void setAllowDirectoryListng(bool v) { m_allowDirectoryListing = v; }

    quint64 bufferSize() const { return m_bufferSize; }
    void setBufferSize(quint64 s) { m_bufferSize = s; }

    QStringList& indexFiles() { return m_indexFiles; }

    void handleRequest(const HttpRequest &request, HttpResponse &response);

private:

    void serveFile(const QString &path,
                   const HttpRequest &request,
                   HttpResponse &response,
                   quint64 bufferSize = 1048576);

    void serveDirectory(const QString &path,
                        const HttpRequest &request,
                        HttpResponse &response);

private:

    QString m_rootDirectory;        ///< Root directory path.
    bool m_allowDirectoryListing;   ///< Directory content listing flag.
    QStringList m_indexFiles;       ///< List of index file names.
    quint64 m_bufferSize;           ///< File I/O buffer size.
};

#endif // HTTPFILEREQUESTHANDLER_H

