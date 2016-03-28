//
//  stdiobuf.h  Standard I/O streams
//
//                          Open Watcom Project
//
//    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
//
//  ========================================================================
//
//    This file contains Original Code and/or Modifications of Original
//    Code as defined in and that are subject to the Sybase Open Watcom
//    Public License version 1.0 (the 'License'). You may not use this file
//    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
//    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
//    provided with the Original Code and Modifications, and is also
//    available at www.sybase.com/developer/opensource.
//
//    The Original Code and all software distributed under the License are
//    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
//    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
//    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
//    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
//    NON-INFRINGEMENT. Please see the License for the specific language
//    governing rights and limitations under the License.
//
//  ========================================================================
//
#ifndef _STDIOBUF_H_INCLUDED
#define _STDIOBUF_H_INCLUDED
#if !defined(_ENABLE_AUTODEPEND)
  #pragma read_only_file;
#endif


#ifndef __cplusplus
#error stdiobuf.h is for use with C++
#endif

#ifndef _COMDEF_H_INCLUDED
 #include <_comdef.h>
#endif
#ifndef _STDIO_H_INCLUDED
 #include <stdio.h>
#endif
#ifndef _IOSTREAM_H_INCLUDED
 #include <iostream.h>
#endif

// **************************** STDIOBUF *************************************
#if defined(_M_IX86)
  #pragma pack(__push,1);
#else
  #pragma pack(__push,8);
#endif
class _WPRTLINK stdiobuf : public streambuf {
public:
    stdiobuf();
    stdiobuf( FILE *__fptr );
    ~stdiobuf();

    FILE *stdiofile();

    virtual int overflow( int = EOF );
    virtual int underflow();
    virtual int sync();

private:
    FILE *__file_pointer;
    char __unbuffered_get_area[ DEFAULT_PUTBACK_SIZE+1 ];
    int : 0;
};
#pragma pack(__pop);

inline FILE *stdiobuf::stdiofile() {
    return __file_pointer;
}

#endif
