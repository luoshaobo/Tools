/**
* @file
*          operatingsystem.h
* @brief
*          Header file
* @author  (last changes):
*          - Elmar Weber
*          - elmar.weber@continental-corporation.com
*          - Continental AG
* @par Project:
* @par SW-Package:
*
* @par SW-Module:
*
* @note
*
* @par Module-History:
*  Date        Author                   Reason
*
* @par Copyright Notice:
* Copyright (C) Continental AG 2016
* Alle Rechte vorbehalten. All Rights Reserved.
* The reproduction, transmission or use of this document or its contents is
* not permitted without express written authority.
* Offenders will be liable for damages. All rights, including rights created
* by patent grant or registration of a utility model or design, are reserved.
*/


#ifndef OPERATING_SYSTEM_H__
#define OPERATING_SYSTEM_H__

#if defined(_WIN32)
  #define OS_WINDOWS
#else //if defined(__linux__)
  #define OS_LINUX
#endif

#ifndef THREAD_STACK_SIZE
#define THREAD_STACK_SIZE 0
#endif

#define UNUSED(x) (void)(x)

#if defined(OS_WINDOWS)

#if _MSC_VER >= 1200
#include "vs_bugfix.h"
#endif

//warning C4290: C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
//https://www.codeproject.com/Articles/6641/Standard-Features-Missing-From-VC-Part-I-Excep
#pragma warning( disable : 4290 )
#pragma warning( disable : 4251 )
#pragma warning( disable : 4275 )
//#pragma warning( disable : 4005 )

//#define snprintf _snprintf

typedef signed __int64         int64_t;
typedef unsigned __int64       uint64_t;

#define os_putenv _putenv
#define os_getenv _getenv
#define os_itoa   _itoa

//#include <winsock2.h>
//#include <ws2tcpip.h>
#define OS_INVALID_SOCKET   INVALID_SOCKET
#define os_socket_t        SOCKET
#define os_socklen_t       int
#define os_ioctl_request_t int
#define os_closesocket(s)  closesocket(s)
#define os_ioctl           ioctlsocket
#define OS_EINTR           WSAEINTR
#define OS_EACCES          WSAEACCES
#define OS_EFAULT          WSAEFAULT
#define OS_EINVAL          WSAEINVAL
#define OS_EMFILE          WSAEMFILE
#define OS_EAGAIN          WSAEWOULDBLOCK
#define OS_EWOULDBLOCK     WSAEWOULDBLOCK
#define OS_EINPROGRESS     WSAEINPROGRESS
#define OS_EALREADY        WSAEALREADY
#define OS_ENOTSOCK        WSAENOTSOCK
#define OS_EDESTADDRREQ    WSAEDESTADDRREQ
#define OS_EMSGSIZE        WSAEMSGSIZE
#define OS_EPROTOTYPE      WSAEPROTOTYPE
#define OS_ENOPROTOOPT     WSAENOPROTOOPT
#define OS_EPROTONOSUPPORT WSAEPROTONOSUPPORT
#define OS_ESOCKTNOSUPPORT WSAESOCKTNOSUPPORT
#define OS_ENOTSUP         WSAEOPNOTSUPP
#define OS_EPFNOSUPPORT    WSAEPFNOSUPPORT
#define OS_EAFNOSUPPORT    WSAEAFNOSUPPORT
#define OS_EADDRINUSE      WSAEADDRINUSE
#define OS_EADDRNOTAVAIL   WSAEADDRNOTAVAIL
#define OS_ENETDOWN        WSAENETDOWN
#define OS_ENETUNREACH     WSAENETUNREACH
#define OS_ENETRESET       WSAENETRESET
#define OS_ECONNABORTED    WSAECONNABORTED
#define OS_ECONNRESET      WSAECONNRESET
#define OS_ENOBUFS         WSAENOBUFS
#define OS_EISCONN         WSAEISCONN
#define OS_ENOTCONN        WSAENOTCONN
#define OS_ESHUTDOWN       WSAESHUTDOWN
#define OS_ETIMEDOUT       WSAETIMEDOUT
#define OS_ECONNREFUSED    WSAECONNREFUSED
#define OS_EHOSTDOWN       WSAEHOSTDOWN
#define OS_EHOSTUNREACH    WSAEHOSTUNREACH
#define OS_ESYSNOTREADY    WSASYSNOTREADY
#define OS_ENOTINIT        WSANOTINITIALISED
#define OS_HOST_NOT_FOUND  WSAHOST_NOT_FOUND
#define OS_TRY_AGAIN       WSATRY_AGAIN
#define OS_NO_RECOVERY     WSANO_RECOVERY
#define OS_NO_DATA         WSANO_DATA
#define OS_INFINITE        INFINITE
#define OS_IN_ADDR         IN_ADDR
#define OS_SHUT_RD         1
#define OS_SHUT_WD         2
#define OS_SHUT_BOTH       3
#ifndef ADDRESS_FAMILY
#define ADDRESS_FAMILY USHORT
#endif


#elif defined(OS_LINUX)

#if (__WORDSIZE == 64)
//unittest for 64-bit host
typedef long int int64_t;
typedef unsigned long int uint64_t;
#else
//TODO: make sure this fits...
typedef signed long long int   int64_t;
typedef unsigned long long int uint64_t;
#endif


#define os_putenv putenv
#define os_getenv getenv
#define os_itoa   itoa


#define OS_INVALID_SOCKET  -1
#define os_socket_t        int
#define os_socklen_t       socklen_t
#define os_fcntl_request_t int
#define os_ioctl            ioctl

#include <unistd.h>
#define os_closesocket(s)  ::close(s)

#define OS_EINTR           EINTR
#define OS_EACCES          EACCES
#define OS_EFAULT          EFAULT
#define OS_EINVAL          EINVAL
#define OS_EMFILE          EMFILE
#define OS_EAGAIN          EAGAIN
#define OS_EWOULDBLOCK     EWOULDBLOCK
#define OS_EINPROGRESS     EINPROGRESS
#define OS_EALREADY        EALREADY
#define OS_ENOTSOCK        ENOTSOCK
#define OS_EDESTADDRREQ    EDESTADDRREQ
#define OS_EMSGSIZE        EMSGSIZE
#define OS_EPROTOTYPE      EPROTOTYPE
#define OS_ENOPROTOOPT     ENOPROTOOPT
#define OS_EPROTONOSUPPORT EPROTONOSUPPORT
#define OS_ENOTSUP         ENOTSUP
#define OS_EPFNOSUPPORT    EPFNOSUPPORT
#define OS_EAFNOSUPPORT    EAFNOSUPPORT
#define OS_EADDRINUSE      EADDRINUSE
#define OS_EADDRNOTAVAIL   EADDRNOTAVAIL
#define OS_ENETDOWN        ENETDOWN
#define OS_ENETUNREACH     ENETUNREACH
#define OS_ENETRESET       ENETRESET
#define OS_ECONNABORTED    ECONNABORTED
#define OS_ECONNRESET      ECONNRESET
#define OS_ENOBUFS         ENOBUFS
#define OS_EISCONN         EISCONN
#define OS_ENOTCONN        ENOTCONN
#define OS_ETIMEDOUT       ETIMEDOUT
#define OS_ECONNREFUSED    ECONNREFUSED
#define OS_EHOSTUNREACH    EHOSTUNREACH
#define OS_ESYSNOTREADY    -4
#define OS_ENOTINIT        -5
#define OS_HOST_NOT_FOUND  HOST_NOT_FOUND
#define OS_TRY_AGAIN       TRY_AGAIN
#define OS_NO_RECOVERY     NO_RECOVERY
#define OS_NO_DATA         NO_DATA
#define OS_INFINITE        2147483647
#define OS_IN_ADDR         in_addr
#define s6_words           s6_addr16
#define OS_SHUT_RD         SHUT_RD
#define OS_SHUT_WD         SHUT_WR
#define OS_SHUT_BOTH       SHUT_RDWR
#endif

//not needed yet
//#define OS_STATIC

#if defined(DLL) || defined(_DLL)
#if !defined(OS_STATIC)
  #define OS_DLL
#endif
#endif

#if defined(OS_WINDOWS)  && defined(OS_DLL)
#if defined(SYMBOLS_EXPORTS)
#define Lib_API __declspec(dllexport)
#else
#define Lib_API __declspec(dllimport)
#endif

#if defined(PROTO_EXPORTS)
#define Lib_Prototbuf __declspec(dllexport)
#else
#define Lib_Prototbuf __declspec(dllimport)
#endif

#endif

#if !defined(Lib_API)
#if defined (__GNUC__) && (__GNUC__ >= 4)
#define Lib_API __attribute__ ((visibility ("default")))
#else
#define Lib_API
#endif
#endif

#if !defined(Lib_Prototbuf)
#if defined (__GNUC__) && (__GNUC__ >= 4)
#define Lib_Prototbuf __attribute__ ((visibility ("default")))
#else
#define Lib_Prototbuf
#endif
#endif


#endif  //OPERATING_SYSTEM_H__
