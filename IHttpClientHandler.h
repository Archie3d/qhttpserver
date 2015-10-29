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

#ifndef IHTTPCLIENTHANDLER_H
#define IHTTPCLIENTHANDLER_H

/**
 * Interface to HTTP client handler.
 * This interface is used as a 'call-back' for HTTP responses
 * to notify the client handler to proceed with response
 * finalization (e.g. sending it to remote client).
 */
class IHttpClientHandler
{
public:

    virtual void finalizeResponse() = 0;

    virtual ~IHttpClientHandler() {}
};

#endif // IHTTPCLIENTHANDLER_H

