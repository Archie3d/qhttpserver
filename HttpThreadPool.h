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

#ifndef HTTPTHREADPOOL_H
#define HTTPTHREADPOOL_H

#include <QList>
#include <QQueue>
#include <QThread>

/**
 * This class handles a collection of threads.
 */
class HttpThreadPool : public QObject
{
    Q_OBJECT
public:

    HttpThreadPool(int minThreads = 5,
                   int maxThreads = 10,
                   QObject *pParent = nullptr);
    ~HttpThreadPool();

    /**
     * Allocate a thread.
     * If there are no threads available, nullptr is returned.
     * @return
     */
    QThread* allocate();

    /**
     * Put a thread back to the list of available threads.
     * @param pThread
     */
    void putBack(QThread *pThread);

private:

    void dumpStatistics();

    /**
     * Initial threads preallocation.
     */
    void initThreads();

    /// All threads managed by this pool.
    QList<QThread*> m_threads;

    int m_minThreads;   ///< Minimal number of reserved threads.
    int m_maxThreads;   ///< Maximal number of threads.

    QQueue<QThread*> m_freeThreads;
    QQueue<QThread*> m_busyThreads;
};

#endif // HTTPTHREADPOOL_H
