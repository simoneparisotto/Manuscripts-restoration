/**
 * @file applymask.c
 * @brief Apply an inpainting mask
 * @author Pascal Getreuer <getreuer@gmail.com>
 * 
 * Copyright (c) 2012, Pascal Getreuer
 * All rights reserved.
 * 
 * This program is free software: you can use, modify and/or 
 * redistribute it under the terms of the simplified BSD License. You 
 * should have received a copy of this license along this program. If 
 * not, see <http://www.opensource.org/licenses/bsd-license.html>.
 */

#include <stdio.h>
#include <string.h>
#include "imageio.h"

/** @brief Quality for writing JPEG images */
#define JPEGQUALITY                 95


void PrintHelpMessage()
{
    puts("Apply an inpainting mask, P. Getreuer, 2012\n\n"
        "Syntax: applymask <input> <mask> <output>\n");
        puts("where <input>, <mask>, and <output> are " 
    READIMAGE_FORMATS_SUPPORTED " images.\n");
        puts("Example:\n"
            "  applymask input.bmp mask.bmp output.bmp\n");
}


void ApplyMask(unsigned char *Image, int Width, int Height, int NumChannels,
    const unsigned char *Mask, int MaskWidth, int MaskHeight);
int IsGrayscale(const unsigned char *Image, int Width, int Height);


int main(int argc, char **argv)
{
    const char *InputFile, *MaskFile, *OutputFile;
    unsigned char *Image = NULL, *Mask = NULL;
    int Width, Height, NumChannels, MaskWidth, MaskHeight, Status = 1;
    
    if(argc != 4)
    {
        PrintHelpMessage();
        return 0;
    }
    
    InputFile = argv[1];
    MaskFile = argv[2];
    OutputFile = argv[3];
    
    /* Read the input image */
    if(!(Image = (unsigned char *)ReadImage(&Width, &Height,
        InputFile, IMAGEIO_RGB | IMAGEIO_PLANAR | IMAGEIO_U8))
        || !(Mask = (unsigned char *)ReadImage(&MaskWidth, &MaskHeight,
        MaskFile, IMAGEIO_GRAYSCALE | IMAGEIO_U8)))
        goto Catch;
    
    NumChannels = IsGrayscale(Image, Width, Height) ? 1 : 3;
    
    /* Apply the mask */
    ApplyMask(Image, Width, Height, NumChannels, Mask, MaskWidth, MaskHeight);
    
    /* Write the mask image */
    if(!WriteImage(Image, Width, Height, OutputFile, 
        ((NumChannels == 1) ? IMAGEIO_GRAYSCALE : IMAGEIO_RGB) 
        | IMAGEIO_PLANAR | IMAGEIO_U8, JPEGQUALITY))
    {
        fprintf(stderr, "Error writing to \"%s\".\n", OutputFile);
        goto Catch;
    }
    
    Status = 0;
Catch:
    if(Mask)
        Free(Mask);
    if(Image)
        Free(Image);
    return Status;
}


/**
 * @brief Apply a mask to the image
 * @param Image the destination image
 * @param Width, Height, NumChannels the image dimensions
 * @param Mask the mask
 * @param MaskWidth, MaskHeight the mask dimensions
 * 
 * This routine sets Image pixels to gray where the mask has value >= 128.
 */
void ApplyMask(unsigned char *Image, int Width, int Height, int NumChannels,
    const unsigned char *Mask, int MaskWidth, int MaskHeight)
{
    const long NumPixels = ((long)Width) * ((long)Height);
    long x, y, MinWidth, MinHeight, Channel;
    
    MinWidth = (MaskWidth < Width) ? MaskWidth : Width;
    MinHeight = (MaskHeight < Height) ? MaskHeight : Height;
    
    for(Channel = 0; Channel < NumChannels; Channel++, Image += NumPixels)
        for(y = 0; y < MinHeight; y++)
            for(x = 0; x < MinWidth; x++)
                if(Mask[x + MaskWidth*y] >= 128)
                    Image[x + Width*y] = 128;
}


/** @brief Test whether image is grayscale */
int IsGrayscale(const unsigned char *Image, int Width, int Height)
{
    const long NumPixels = ((long)Width) * ((long)Height);
    const unsigned char *Red = Image;
    const unsigned char *Green = Image + NumPixels;
    const unsigned char *Blue = Image + 2*NumPixels;
    long n;
    
    for(n = 0; n < NumPixels; n++)
        if(Red[n] != Green[n] || Red[n] != Blue[n])
            return 0;
    
    return 1;
}
