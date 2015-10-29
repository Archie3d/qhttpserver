#ifndef HTTPSERVERAPI_H
#define HTTPSERVERAPI_H

#ifdef HTTPSERVERLIB_BUILD
#   define HTTP_API  __declspec(dllexport)
#else
#   define HTTP_API  __declspec(dllimport)
#endif

#endif // HTTPSERVERAPI_H
