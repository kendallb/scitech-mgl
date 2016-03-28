
/* Copyright (c) Mark J. Kilgard, 1994. */

/* This program is freely distributable without licensing fees
   and is provided without guarantee or warrantee expressed or
   implied. This program is -not- in the public domain. */

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "glutint.h"

/* strdup is actually not a standard ANSI C or POSIX routine
   so implement a private one for GLUT.  OpenVMS does not have a
   strdup; Linux's standard libc doesn't declare strdup by default
   (unless BSD or SVID interfaces are requested). */
char *
__glutStrdup(const char *string)
{
  char *copy;

  copy = (char*) malloc(strlen(string) + 1);
  if (copy == NULL)
    return NULL;
  strcpy(copy, string);
  return copy;
}

void
__glutWarning(char *format,...)
{
  va_list args;

#if 1  /* Assemble message into strings for both console and GUI output */
  char str1[255],str2[255];

  va_start(args, format);
  sprintf(str1, "GLUT: Warning in %s: ",
    __glutProgramName ? __glutProgramName : "(unamed)");
  vsprintf(str2, format, args);
  va_end(args);
  fprintf(stderr, "%s%s\n", str1, str2);
# ifdef _WIN32
  strcat(str1, str2);
  MessageBox(NULL, str1, "GLUT Warning", MB_OK | MB_ICONWARNING);
# endif

#else
  va_start(args, format);
  fprintf(stderr, "GLUT: Warning in %s: ",
    __glutProgramName ? __glutProgramName : "(unamed)");
  vfprintf(stderr, format, args);
  va_end(args);
  putc('\n', stderr);
#endif
}

/* CENTRY */
void APIENTRY
glutReportErrors(void)
{
  GLenum error;

  while ((error = glGetError()) != GL_NO_ERROR)
    __glutWarning("GL error: %s", gluErrorString(error));
}
/* ENDCENTRY */

void
__glutFatalError(char *format,...)
{
  va_list args;

#if 1  /* Assemble message into strings for both console and GUI output */
  char str1[255],str2[255];

  va_start(args, format);
  sprintf(str1, "GLUT: Fatal Error in %s: ",
    __glutProgramName ? __glutProgramName : "(unamed)");
  vsprintf(str2, format, args);
  va_end(args);
  fprintf(stderr, "%s%s\n", str1, str2);
# ifdef _WIN32
  strcat(str1, str2);
  MessageBox(NULL, str1, "GLUT Fatal Error", MB_OK | MB_ICONERROR);
# endif

#else
  va_start(args, format);
  fprintf(stderr, "GLUT: Fatal Error in %s: ",
    __glutProgramName ? __glutProgramName : "(unamed)");
  vfprintf(stderr, format, args);
  va_end(args);
  putc('\n', stderr);
#endif

#ifdef _WIN32
  if (__glutExitFunc) {
    __glutExitFunc(1);
  }
#endif
  exit(1);
}

void
__glutFatalUsage(char *format,...)
{
  va_list args;

  va_start(args, format);
  fprintf(stderr, "GLUT: Fatal API Usage in %s: ",
    __glutProgramName ? __glutProgramName : "(unamed)");
  vfprintf(stderr, format, args);
  va_end(args);
  putc('\n', stderr);
  abort();
}
