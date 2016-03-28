//
//  fstream.h   File I/O streams
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
#ifndef _FSTREAM_H_INCLUDED
#define _FSTREAM_H_INCLUDED
#if !defined(_ENABLE_AUTODEPEND)
  #pragma read_only_file;
#endif


#ifndef __cplusplus
#error fstream.h is for use with C++
#endif

#ifndef _COMDEF_H_INCLUDED
 #include <_comdef.h>
#endif
#ifndef _IOSTREAM_H_INCLUDED
 #include <iostream.h>
#endif

// POSIX file handle:
typedef int filedesc;

// **************************** FILEBUF **************************************
#if defined(_M_IX86)
  #pragma pack(__push,1);
#else
  #pragma pack(__push,8);
#endif
class _WPRTLINK filebuf : public streambuf {
public:
    static int const openprot;  // default file protection
    enum sh_mode {              // sharing mode
        sh_compat = 0x0000,     // - compatibility mode
        sh_read   = 0x1000,     // - allow others to read
        sh_write  = 0x2000,     // - allow others to write
        sh_none   = 0x4000      // - do not allow others to read or write
    };
    typedef int shmode;

    filebuf();
    filebuf( filedesc __fd );
    filebuf( filedesc __fd, char *__buf, int __len );
    ~filebuf();

    int       is_open() const;
    filedesc  fd() const;
    filebuf  *attach( filedesc __fd );
    filebuf  *open( char const    *__name,
                    ios::openmode  __mode,
                    int            __prot = openprot );
    filebuf  *close();

    virtual int        pbackfail( int __c );
    virtual int        overflow( int = EOF );
    virtual int        underflow();
    virtual streambuf *setbuf( char *__buf, int __len );
    virtual streampos  seekoff( streamoff     __offset,
                                ios::seekdir  __direction,
                                ios::openmode __ignored );
    virtual int        sync();

private:
    filedesc      __file_handle;
    ios::openmode __file_mode;
    char          __unbuffered_get_area[ DEFAULT_PUTBACK_SIZE+1 ];
    char          __attached : 1;
    int           : 0;
};
#pragma pack(__pop);

inline filedesc filebuf::fd() const {
    return( __file_handle );
}

inline int filebuf::is_open() const {
    return( __file_handle != EOF );
}

// **************************** FSTREAMBASE **********************************
#if defined(_M_IX86)
  #pragma pack(__push,1);
#else
  #pragma pack(__push,8);
#endif
class _WPRTLINK fstreambase : virtual public ios {
public:
    int       is_open() const;
    filedesc  fd() const;
    void      attach( filedesc __fd );
    void      open( char const    *__name,
                    ios::openmode  __mode,
                    int            __prot = filebuf::openprot );
    void      close();
    filebuf  *rdbuf() const;
    void      setbuf( char *__buf, int __len );

protected:
    fstreambase();
    fstreambase( char const    *__name,
                 ios::openmode  __mode,
                 int            __prot = filebuf::openprot );
    fstreambase( filedesc __fd );
    fstreambase( filedesc __fd, char *__buf, int __len );
    ~fstreambase();

private:
    filebuf   __flbuf;
};
#pragma pack(__pop);

inline filedesc fstreambase::fd() const {
    __lock_it( __i_lock );
    filebuf *__fb = rdbuf();
    return( (__fb == NULL) ? EOF : __fb->fd() );
}

inline int fstreambase::is_open() const {
    __lock_it( __i_lock );
    filebuf *__fb = rdbuf();
    return( (__fb == NULL) ? 0 : __fb->is_open() );
}

inline filebuf *fstreambase::rdbuf() const {
    return( (filebuf *) ios::rdbuf() );
}

// **************************** IFSTREAM *************************************
#if defined(_M_IX86)
  #pragma pack(__push,1);
#else
  #pragma pack(__push,8);
#endif
class _WPRTLINK ifstream : public fstreambase, public istream {
public:
    ifstream();
    ifstream( char const    *__name,
              ios::openmode  __mode = ios::in,
              int            __prot = filebuf::openprot );
    ifstream( filedesc __fd );
    ifstream( filedesc __fd, char *__buf, int __len );
    ~ifstream();

    void open( char const    *__name,
               ios::openmode  __mode = ios::in,
               int            __prot = filebuf::openprot );
};
#pragma pack(__pop);

inline void ifstream::open( char const *__n, ios::openmode __m, int __p ) {
    fstreambase::open( __n, __m, __p );
}

// **************************** OFSTREAM *************************************
#if defined(_M_IX86)
  #pragma pack(__push,1);
#else
  #pragma pack(__push,8);
#endif
class _WPRTLINK ofstream : public fstreambase, public ostream {
public:
    ofstream();
    ofstream( char const    *__name,
              ios::openmode  __mode = ios::out,
              int            __prot = filebuf::openprot );
    ofstream( filedesc __fd );
    ofstream( filedesc __fd, char *__buf, int __len );
    ~ofstream();

    void open( char const    *__name,
               ios::openmode  __mode = ios::out,
               int            __prot = filebuf::openprot );
};
#pragma pack(__pop);

inline void ofstream::open( char const *__n, ios::openmode __m, int __p ) {
    fstreambase::open( __n, __m, __p );
}

// **************************** FSTREAM **************************************
#if defined(_M_IX86)
  #pragma pack(__push,1);
#else
  #pragma pack(__push,8);
#endif
class _WPRTLINK fstream : public fstreambase, public iostream {
public:
    fstream();
    fstream( char const    *__name,
             ios::openmode  __mode = ios::in|ios::out,
             int            __prot = filebuf::openprot );
    fstream( filedesc __fd );
    fstream( filedesc __fd, char *__buf, int __len );
    ~fstream();

    void open( char const    *__name,
               ios::openmode  __mode = ios::in|ios::out,
               int            __prot = filebuf::openprot );
};
#pragma pack(__pop);

inline void fstream::open( char const *__n, ios::openmode __m, int __p ) {
    fstreambase::open( __n, __m, __p );
}

#endif
