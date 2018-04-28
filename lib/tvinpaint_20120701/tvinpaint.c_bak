/**
 * @file tvinpaint.c
 * @brief Total variation regularized inpainting demo for IPOL
 * @author Pascal Getreuer <getreuer@gmail.com>
 * 
 * 
 * Copyright (c) 2011-2012, Pascal Getreuer
 * All rights reserved.
 * 
 * This program is free software: you can use, modify and/or 
 * redistribute it under the terms of the simplified BSD License. You 
 * should have received a copy of this license along this program. If 
 * not, see <http://www.opensource.org/licenses/bsd-license.html>.
 */

/**
 * @mainpage
 * @verbinclude readme.txt
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "num.h"
#include "tvreg.h"
#include "imageio.h"

/** @brief Display intensities in the range [0,DISPLAY_SCALING] */
#define DISPLAY_SCALING             255

/** @brief Quality for writing JPEG images */
#define JPEGQUALITY                 95

#ifdef NUM_SINGLE
#define IMAGEIO_NUM           (IMAGEIO_SINGLE)
#else
#define IMAGEIO_NUM           (IMAGEIO_DOUBLE)
#endif

/** @brief struct representing an image */
typedef struct
{
    /** @brief Float image data */
    num *Data;
    /** @brief Image width */
    int Width;
    /** @brief Image height */
    int Height;
    /** @brief Number of channels */
    int NumChannels;
} image;


/** @brief Print program explanation and usage */
void PrintHelpMessage()
{
    puts(
    "Total variation regularized inpainting IPOL demo, P. Getreuer, 2012\n\n"
    "Syntax: tvinpaint <D> <lambda> <input> <inpainted>\n");
        puts("where <D>, <input>, and <inpainted> are " 
    READIMAGE_FORMATS_SUPPORTED " images.\n");        
        puts("Example:\n"
    "  tvinpaint mountains-D.bmp 1e3 mountains-f.bmp inpainted.bmp\n");
}

int Inpaint(image u, image f, image D, num Lambda);
num ComputeRmse(image f, image u);
void ThresholdD(image D, num Lambda);
int IsGrayscale(image f);


int main(int argc, char **argv)
{
    const char *InputFile, *DomainFile, *OutputFile;
    image f = {NULL, 0, 0, 0}, u = {NULL, 0, 0, 0}, D = {NULL, 0, 0, 0};
    num Lambda;
    int Status = 1;
    
    if(argc != 5)
    {
        PrintHelpMessage();
        return 0;
    }
    
    /* Read command line arguments */    
    DomainFile = argv[1];
    Lambda = (num)atof(argv[2]);
    InputFile = argv[3];    
    OutputFile = argv[4];    
    
    /* Read the input images */
    if(!(f.Data = (num *)ReadImage(&f.Width, &f.Height, InputFile,
        IMAGEIO_RGB | IMAGEIO_PLANAR | IMAGEIO_NUM)) || 
        !(D.Data = (num *)ReadImage(&D.Width, &D.Height, DomainFile,
        IMAGEIO_RGB | IMAGEIO_PLANAR | IMAGEIO_NUM)))
        goto Catch;
    else if(f.Width != D.Width || f.Height != D.Height)
    {
        fprintf(stderr, "Size mismatch: D is %dx%d but f is %dx%d\n",
            D.Width, D.Height, f.Width, f.Height);
        goto Catch;
    }
    
    f.NumChannels = IsGrayscale(f) ? 1 : 3;
    u = f;
    
    /* Allocate space for the inpainted image */
    if(!(u.Data = (num *)Malloc(sizeof(num) * ((size_t)f.Width)
        * ((size_t)f.Height) * f.NumChannels)))
    {
        fprintf(stderr, "Memory allocation failed.\n");
        goto Catch;
    }
    
    if(!Inpaint(u, f, D, Lambda))
    {
        fprintf(stderr, "Failure!\n");
        goto Catch;
    }
    
    /* Write inpainted image */
    if(!WriteImage(u.Data, u.Width, u.Height, OutputFile, 
        ((u.NumChannels == 1) ? IMAGEIO_GRAYSCALE : IMAGEIO_RGB)
        | IMAGEIO_PLANAR | IMAGEIO_NUM, JPEGQUALITY))    
        fprintf(stderr, "Error writing to \"%s\".\n", OutputFile);
    
    Status = 0;
Catch:
    if(u.Data)
        Free(u.Data);
    if(D.Data)
        Free(D.Data);
    if(f.Data)
        Free(f.Data);
    return Status;
}


/** 
 * @brief TV regularized inpainting
 * @param u denoised image
 * @param f given noisy image
 * @param D the inpainting domain
 * @param Lambda the fidelity weight
 * @return 1 on success, 0 on failure
 * 
 * This wrapper routine sets up the inpainting problem.  The actual 
 * split Bregman computation is performed in TvRestore().
 */
int Inpaint(image u, image f, image D, num Lambda)
{
    tvregopt *Opt = NULL;
    const long NumPixels = ((long)f.Width) * ((long)f.Height);
    num *Red = D.Data;
    num *Green = D.Data + NumPixels;
    num *Blue = D.Data + 2*NumPixels;
    long n, k;
    int Success = 0;
    
    if(!(Opt = TvRegNewOpt()))
    {
        fprintf(stderr, "Memory allocation failed\n");
        return 0;
    }
    
    memcpy(u.Data, f.Data, sizeof(num)*f.Width*f.Height*f.NumChannels);
    
    /* Convert the mask into spatially-varing lambda */       
    for(n = 0; n < NumPixels; n++)
        if(0.299*Red[n] + 0.587*Green[n] + 0.114*Blue[n] > 0.5)
        {
            D.Data[n] = 0;          /* Inside of the inpainting domain */
            
            /* Set u = 0.5 within D */
            for(k = 0; k < u.NumChannels; k++)
                u.Data[n + k*NumPixels] = 0.5;
        }
        else
            D.Data[n] = Lambda;    /* Outside of the inpainting domain */
    
    TvRegSetVaryingLambda(Opt, D.Data, D.Width, D.Height);
    TvRegSetMaxIter(Opt, 250);
    TvRegSetTol(Opt, (num)1e-5);
    
    /* TvRestore performs the split Bregman inpainting */
    if(!TvRestore(u.Data, f.Data, f.Width, f.Height, f.NumChannels, Opt))
    {
        fprintf(stderr, "Error in computation.\n");
        goto Catch;
    }    
    
    Success = 1;
Catch:
    TvRegFreeOpt(Opt);
    return Success;
}


/** @brief Test whether image is grayscale */
int IsGrayscale(image f)
{
    const long NumPixels = ((long)f.Width) * ((long)f.Height);
    const num *Red = f.Data;
    const num *Green = f.Data + NumPixels;
    const num *Blue = f.Data + 2*NumPixels;
    long n;
    
    for(n = 0; n < NumPixels; n++)
        if(Red[n] != Green[n] || Red[n] != Blue[n])
            return 0;
    
    return 1;
}
