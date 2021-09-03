/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
 *   erik.sohns@web.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef COMMON_PRAGMAS_H
#define COMMON_PRAGMAS_H

// compile-time messages //

#define $Line COMMON_MAKESTRING(COMMON_XSTRINGIZE, __LINE__)

#if defined (_MSC_VER)
#define COMMON_PRAGMA(x) __pragma(x)
#define COMMON_PRAGMA_MESSAGE_PREFACE(type) __FILE__ "("$Line"): "type": "
#define COMMON_PRAGMA_MESSAGE(msg) COMMON_PRAGMA(message (msg))
#define COMMON_PRAGMA_MESSAGE_2(type,msg) COMMON_PRAGMA(message (COMMON_PRAGMA_MESSAGE_PREFACE(type) msg))
#elif defined (__GNUC__)
#define COMMON_PRAGMA(x) _Pragma(#x)
#define COMMON_PRAGMA_MESSAGE(msg) COMMON_PRAGMA(message (msg))
#define COMMON_PRAGMA_MESSAGE_2(type,msg) COMMON_PRAGMA(message (msg))
#else
#define COMMON_PRAGMA(x)
#define COMMON_PRAGMA_MESSAGE(msg)
#define COMMON_PRAGMA_MESSAGE_2(type,msg)
#endif
#define COMMON_PRAGMA_ERROR(x)   COMMON_PRAGMA_MESSAGE_2(*ERROR*,x)
#define COMMON_PRAGMA_NOTE(x)    COMMON_PRAGMA_MESSAGE_2(*NOTE*,x)
#define COMMON_PRAGMA_WARNING(x) COMMON_PRAGMA_MESSAGE_2(*WARNING*,x)

#endif
