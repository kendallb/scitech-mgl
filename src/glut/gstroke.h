#ifndef __glutstroke_h__
#define __glutstroke_h__

#include "scitech.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  float x;
  float y;
} CoordRec, *CoordPtr;

typedef struct {
  int num_coords;
  CoordPtr coord;
} StrokeRec, *StrokePtr;

typedef struct {
  int num_strokes;
  StrokePtr stroke;
  float center;
  float right;
} StrokeCharRec, *StrokeCharPtr;

typedef struct {
  char *name;
  int num_chars;
  StrokeCharPtr ch;
  float top;
  float bottom;
} StrokeFontRec, *StrokeFontPtr;

typedef void *GLUTstrokeFont;

#ifdef __cplusplus
}
#endif

#endif /* __glutstroke_h__ */

