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

#ifndef COMMON_MATH_DEFINES_H
#define COMMON_MATH_DEFINES_H

#define COMMON_MATH_NUMDIGITS_INT(X) (X == 0) ? 1 : (std::floor (std::log10 (X)) + 1)

#define COMMON_MATH_DEGREES_TO_RADIANS_D(A) A * M_PI / 180.0
#define COMMON_MATH_DEGREES_TO_RADIANS_F(A) A * static_cast<float> (M_PI) / 180.0f

#endif
