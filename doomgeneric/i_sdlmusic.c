//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//	System interface for music.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <SDL2/SDL.h>
//#include <SDL2_Mixer/SDL_Mixer.h>

#include "config.h"
#include "doomtype.h"
#include "memio.h"
#include "mus2mid.h"

#include "deh_str.h"
#include "gusconf.h"
#include "i_sound.h"
#include "i_system.h"
#include "i_swap.h"
#include "m_argv.h"
#include "m_config.h"
#include "m_misc.h"
#include "sha1.h"
#include "w_wad.h"
#include "z_zone.h"

#define MAXMIDLENGTH (96 * 1024)
#define MID_HEADER_MAGIC "MThd"
#define MUS_HEADER_MAGIC "MUS\x1a"

#define FLAC_HEADER "fLaC"
#define OGG_HEADER "OggS"

// Looping Vorbis metadata tag names. These have been defined by ZDoom
// for specifying the start and end positions for looping music tracks
// in .ogg and .flac files.
// More information is here: http://zdoom.org/wiki/Audio_loop
#define LOOP_START_TAG "LOOP_START"
#define LOOP_END_TAG   "LOOP_END"

// FLAC metadata headers that we care about.
#define FLAC_STREAMINFO      0
#define FLAC_VORBIS_COMMENT  4

// Ogg metadata headers that we care about.
#define OGG_ID_HEADER        1
#define OGG_COMMENT_HEADER   3

// Structure for music substitution.
// We store a mapping based on SHA1 checksum -> filename of substitute music
// file to play, so that substitution occurs based on content rather than
// lump name. This has some inherent advantages:
//  * Music for Plutonia (reused from Doom 1) works automatically.
//  * If a PWAD replaces music, the replacement music is used rather than
//    the substitute music for the IWAD.
//  * If a PWAD reuses music from an IWAD (even from a different game), we get
//    the high quality version of the music automatically (neat!)
