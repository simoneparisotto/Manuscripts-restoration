/**
 * @file drawtext.h
 * @brief Draw text on an image
 * @author Pascal Getreuer <getreuer@gmail.com>
 * 
 * This file contains a simple implementation for drawing text onto an image.
 * Data for one 18-point sans serif font is included.
 * 
 * 
 * Copyright (c) 2012, Pascal Getreuer
 * All rights reserved.
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under, at your option, the terms of the GNU General Public License as 
 * published by the Free Software Foundation, either version 3 of the 
 * License, or (at your option) any later version, or the terms of the 
 * simplified BSD license.
 *
 * You should have received a copy of these licenses along with this program.
 * If not, see <http://www.gnu.org/licenses/> and
 * <http://www.opensource.org/licenses/bsd-license.html>.
 */

#ifndef _DRAWTEXT_H_
#define _DRAWTEXT_H_

int TextWidth(const char *Text);

void DrawText(unsigned char *Image, int Width, int Height,
    int x0, int y0, unsigned char Value, const char *Text);

#endif /* _DRAWTEXT_H_ */
