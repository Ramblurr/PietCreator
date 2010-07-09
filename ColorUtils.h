/* This class is an almagamation of several KDE color classes:
 * KColorSpaces, KColorHelpers, KColorUtils
 * Copyright (C) 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
 * Copyright (C) 2007 Olaf Schmidt <ojschmidt@kde.org>
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2007 Zack Rusin <zack@kde.org>
 * Copyright (C) 2010 Casey Link <unnamedrambler@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */


#ifndef COLORUTILS_H
#define COLORUTILS_H

#include <QtGui/QColor>

#include <math.h>

namespace ColorUtils {

  ///////////////////////////////////////////////////////////////////////////////
  // HCY color space
  // from kcolorspaces.cpp
  #define HCY_REC 709 // use 709 for now
  #if   HCY_REC == 601
  static const qreal yc[3] = { 0.299, 0.587, 0.114 };
  #elif HCY_REC == 709
  static const qreal yc[3] = {0.2126, 0.7152, 0.0722};
  #else // use Qt values
  static const qreal yc[3] = { 0.34375, 0.5, 0.15625 };
  #endif

  // from kcolorhelpers_p.h
  inline qreal normalize(qreal a)
  {
      return (a < 1.0 ? (a > 0.0 ? a : 0.0) : 1.0);
  }

  // from kcolorspaces.cpp
  inline qreal gamma(qreal n)
  {
      return pow(normalize(n), 2.2);
  }

  // from kcolorspaces.cpp
  inline qreal lumag(qreal r, qreal g, qreal b)
  {
      return r*yc[0] + g*yc[1] + b*yc[2];
  }

  // from kcolorspaces.cpp
  inline qreal luma(const QColor& color)
  {
      return lumag(gamma(color.redF()),
                  gamma(color.greenF()),
                  gamma(color.blueF()));
  }

  // from kcolorutils.cpp
  inline qreal contrastRatio(const QColor &c1, const QColor &c2)
  {
      qreal y1 = luma(c1), y2 = luma(c2);
      if (y1 > y2)
          return (y1 + 0.05) / (y2 + 0.05);
      else
          return (y2 + 0.05) / (y1 + 0.05);
  }
}

#endif