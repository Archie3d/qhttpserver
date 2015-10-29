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

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QDirIterator>
#include "HttpOption.h"
#include "HttpFileRequestHandler.h"

HttpFileRequestHandler::HttpFileRequestHandler(QObject *pParent)
    : HttpRequestHandler(pParent),
      m_rootDirectory(qApp->applicationDirPath()),
      m_allowDirectoryListing(true),
      m_bufferSize(1024*1024)
{
}

HttpFileRequestHandler::~HttpFileRequestHandler()
{
}

void HttpFileRequestHandler::handleRequest(const HttpRequest &request, HttpResponse &response)
{
    QString url(request.uri());
    QString fileName(QDir::cleanPath(m_rootDirectory + QDir::toNativeSeparators(url)));

    QFileInfo fileInfo(fileName);
    if (fileInfo.exists()) {
        if (fileInfo.isFile()) {
            // Serve a file
            serveFile(fileName, request, response, m_bufferSize);
        } else if (fileInfo.isDir()) {
            // Looking for index file
            QDir dir(fileName);
            foreach (const QString &indexFile, m_indexFiles) {
                QString indexFileName = dir.absoluteFilePath(indexFile);
                QFileInfo indexFileInfo(indexFileName);
                if (indexFileInfo.exists() && indexFileInfo.isFile()) {
                    serveFile(indexFileName, request, response, m_bufferSize);
                    response.finalize();
                    return;
                }
            }

            // Serve directory content
            if (m_allowDirectoryListing) {
                serveDirectory(fileName, request, response);
            }
        }
    } else {
        response.setStatus(HttpResponse::NotFound);
    }

    response.finalize();
}

void HttpFileRequestHandler::serveFile(const QString &path,
                                       const HttpRequest &request,
                                       HttpResponse &response,
                                       quint64 bufferSize)
{
    if (request.method() != HttpRequest::Method_Get) {
        response.setStatus(HttpResponse::MethodNotAllowed);
        return;
    }

    QFileInfo fileInfo(path);
    if (!fileInfo.exists()) {
        response.setStatus(HttpResponse::NotFound);
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        response.setStatus(HttpResponse::Forbidden);
        return;
    }

    // Handle If-Modified-Since
    {
        QVariant opt = request.option(HttpOption::IfModifiedSince);
        if (!opt.isNull()) {
            QDateTime dt = HttpOption::stringToDateTime(opt.toString());
            if (dt.isValid()) {
                if (fileInfo.lastModified() <= dt) {
                    response.setStatus(HttpResponse::NotModified);
                    return;
                }
            }
        }
    }

    // Handle If-Unmodified-Since
    {
        QVariant opt = request.option(HttpOption::IfUnmodifiedSince);
        if (!opt.isNull()) {
            QDateTime dt = HttpOption::stringToDateTime(opt.toString());
            if (dt.isValid()) {
                if (fileInfo.lastModified() > dt) {
                    response.setStatus(HttpResponse::PreconditionFailed);
                    return;
                }
            }
        }
    }

    // Guess content type
    response.setContentType(HttpContentType::fromFileExtension(fileInfo.suffix()));

    response.setStatus(HttpResponse::Ok);
    response.setOption(HttpOption::ContentLength, QString::number(fileInfo.size()));
    response.send();

    while (!file.atEnd()) {
        response.sendData(file.read(bufferSize));
        response.socket()->flush();
    }

    file.close();
}

void HttpFileRequestHandler::serveDirectory(const QString &path,
                                            const HttpRequest &request,
                                            HttpResponse &response)
{
    if (request.method() != HttpRequest::Method_Get) {
        response.setStatus(HttpResponse::MethodNotAllowed);
        return;
    }

    response.setContentType(HttpContentType::TextHtml);
    response.setStatus(HttpResponse::Ok);

    QDir rootDirectory(m_rootDirectory);
    QDir refDirectory(path);
    QString contentLocation = rootDirectory.relativeFilePath(path);
    if (contentLocation.right(1) != "/") {
        contentLocation.append("/");
    }

    response.setOption(HttpOption::ContentLocation, contentLocation);

    QByteArray ba;
    ba.append(QString("<html><head><title>Content of %1</title><head>").arg(contentLocation));
    ba.append("<body>\n");

    QDirIterator iterator(refDirectory, QDirIterator::FollowSymlinks);
    while (iterator.hasNext()) {
        iterator.next();

        QFileInfo fileInfo = iterator.fileInfo();
        QString name = fileInfo.fileName();
        QString path = rootDirectory.relativeFilePath(fileInfo.absoluteFilePath());
        if (fileInfo.isDir()) {
            name.append("/");
        }
        path.prepend("/");

        ba.append(QString("<a href=\"%1\">%2</a><br>\n")
                  .arg(path)
                  .arg(name));
    }

    ba.append("\n</body></html>");

    response.setData(ba);
}
