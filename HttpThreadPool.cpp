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

#if _DEBUG
#   include <QDebug>
#endif
#include "HttpThreadPool.h"

HttpThreadPool::HttpThreadPool(int minThreads, int maxThreads, QObject *pParent)
    : QObject(pParent),
      m_threads(),
      m_minThreads(minThreads),
      m_maxThreads(maxThreads),
      m_freeThreads(),
      m_busyThreads()
{
    Q_ASSERT(m_minThreads > 0);
    Q_ASSERT(m_maxThreads >= m_minThreads);

    initThreads();
}

HttpThreadPool::~HttpThreadPool()
{
    foreach (QThread *pThread, m_threads) {
        pThread->quit();
    }

    // Wait for threads to terminate
    foreach (QThread *pThread, m_threads) {
        pThread->wait();
    }

    qDeleteAll(m_threads);
}

QThread* HttpThreadPool::allocate()
{
    QThread *pThread = nullptr;

    if (m_freeThreads.count() == 0) {
        // No more free threads
        if (m_threads.count() < m_maxThreads) {
            // Can still allocate threads
            pThread = new QThread(this);
            m_threads.append(pThread);
        } else {
            // No more thread can be allocated
            return nullptr;
        }
    } else {
        pThread = m_freeThreads.dequeue();
    }

    Q_ASSERT(pThread != nullptr);

    m_busyThreads.enqueue(pThread);

    if (!pThread->isRunning()) {
        pThread->start();
    }

#ifdef _DEBUG
    dumpStatistics();
#endif

    return pThread;
}

void HttpThreadPool::putBack(QThread *pThread)
{
    Q_ASSERT(pThread != nullptr);
    Q_ASSERT(m_threads.contains(pThread));

    if (m_busyThreads.removeOne(pThread)) {
        m_freeThreads.enqueue(pThread);
    }

#ifdef _DEBUG
    dumpStatistics();
#endif
}

void HttpThreadPool::initThreads()
{
    for (int i = 0; i < m_minThreads; i++) {
        QThread *pThread = new QThread(this);
        m_threads.append(pThread);
        m_freeThreads.enqueue(pThread);
    }

#ifdef _DEBUG
    dumpStatistics();
#endif
}

void HttpThreadPool::dumpStatistics()
{
    qDebug() << m_threads.count() << "threads:"
             << m_busyThreads.count() << "busy,"
             << m_freeThreads.count() << "free";
}
