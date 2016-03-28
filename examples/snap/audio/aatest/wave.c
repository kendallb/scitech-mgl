/****************************************************************************
*
*                      SciTech SNAP Audio Architecture
*
*  ========================================================================
*
*   Copyright (C) 1991-2005 SciTech Software, Inc. All rights reserved.
*
*   This file may be distributed and/or modified under the terms of the
*   GNU General Public License version 2.0 as published by the Free
*   Software Foundation and appearing in the file LICENSE.GPL included
*   in the packaging of this file.
*
*   Licensees holding a valid Commercial License for this product from
*   SciTech Software, Inc. may use this file in accordance with the
*   Commercial License Agreement provided with the Software.
*
*   This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
*   THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
*   PURPOSE.
*
*   See http://www.scitechsoft.com/license/ for information about
*   the licensing options available and how to purchase a Commercial
*   License Agreement.
*
*   Contact license@scitechsoft.com if any conditions of this licensing
*   are not clear to you, or you have questions about licensing options.
*
*  ========================================================================
*
* Language:     ANSI C
* Environment:  Any 32-bit protected mode environment
*
* Description:  Simple functions to load and entire .WAV file from disk.
*
****************************************************************************/

#include "aatest.h"

/*--------------------------- Global variables ----------------------------*/

#pragma pack(1)

#define  WAV_PCM      1                  // PCM (uncompressed) wave file
#define  WAV_RIFF     0x46464952         // "RIFF"
#define  WAV_WAVE     0x45564157         // "WAVE"
#define  WAV_FMT      0x20746D66         // " fmt"
#define  WAV_DATA     0x61746164         // "data"

// Wave chunk header
typedef struct {
    ulong   id;
    ulong   size;
    } WAV_chunk;

// Wave format structure
typedef struct {
    ushort  format;
    ushort  channel;
    ulong   rate;
    ulong   avgBytes;
    ushort  blockAlign;
    ushort  bit;
    } WAV_format;

#pragma pack()

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
REMARKS:
Searches for a chunk in the file with the specified ID.
****************************************************************************/
static ibool FindChunk(
    FILE *f,
    ulong id,
    WAV_chunk *chunk)
{
    while (!feof(f)) {
        if ((fread(chunk, 1, sizeof(WAV_chunk), f)) != sizeof(WAV_chunk))
            return false;
        if (chunk->id == id)
            return true;
        if (fseek(f, chunk->size, SEEK_CUR) != 0)
            return false;
        }
    return false;
}

/****************************************************************************
REMARKS:
Open a wave file on disk and load it into memory.
****************************************************************************/
AA_waveFile *WAV_load(
    const char *filename)
{
    FILE        *f;
    AA_waveFile *wave = NULL;
    WAV_chunk   chunk;
    WAV_format  fmt;
    ulong       id,pos;

    if ((f = fopen(filename,"rb")) != NULL) {
        if (fread(&chunk,1,sizeof(chunk),f) != sizeof(chunk))
            goto Error;
        if (fread(&id,1,sizeof(id),f) != sizeof(id))
            goto Error;
        if (id != WAV_WAVE || chunk.id != WAV_RIFF)
            goto Error;
        if (!FindChunk(f,WAV_FMT,&chunk))
            goto Error;
        pos = ftell(f);
        if (fread(&fmt,1,sizeof(fmt),f) != sizeof(fmt))
            goto Error;
        if (fmt.format != WAV_PCM)
            goto Error;
        if (fseek(f,pos + chunk.size,SEEK_SET) != 0)
            goto Error;
        if (!FindChunk(f,WAV_DATA,&chunk))
            goto Error;
        if ((wave = malloc(sizeof(*wave))) == NULL)
            goto Error;
        wave->buffer = malloc(chunk.size);
        if (wave->buffer == NULL)
            goto Error;

        wave->length = chunk.size;
        wave->rate = fmt.rate;
        wave->bit = fmt.bit;
        wave->channel = fmt.channel;
        if (fread(wave->buffer,1,wave->length,f) != wave->length)
            goto Error;
        fclose(f);
        return wave;
        }
Error:
    if (wave) {
        if (wave->buffer)
            free(wave->buffer);
        free(wave);
        }
    fclose(f);
    return NULL;
}

/****************************************************************************
REMARKS:
Unload a WAV file loaded from disk.
****************************************************************************/
void WAV_unload(
    AA_waveFile *wave)
{
    if (wave) {
        free(wave->buffer);
        free(wave);
        }
}
