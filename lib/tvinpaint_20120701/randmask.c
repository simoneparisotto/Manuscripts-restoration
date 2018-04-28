/**
 * @file randmask.c
 * @brief Generate a random inpainting mask
 * @author Pascal Getreuer <getreuer@gmail.com>
 * @author Yiqing Wang <yiqing.wang@polytechnique.edu>
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
#include "randmt.h"
#include "drawtext.h"
#include "imageio.h"

/** @brief Quality for writing JPEG images */
#define JPEGQUALITY                 95

#define CLAMP(X,A,B)    (((X) <= (A)) ? (A) : (((X) >= (B)) ? (B) : (X)))


void PrintHelpMessage()
{
    puts("Generate a random inpainting mask, P. Getreuer, Y. Wang, 2012\n\n"
        "Syntax: randmask <type> <input> <output>\n");
    puts("where <input> and <output> are " 
    READIMAGE_FORMATS_SUPPORTED " images.\n");
    puts("Choices for <type>\n");
    puts(
    "    dots:<r>          Random dots of radius <r>\n"
    "    scribble:<r>      Random scribble with pen of radius <r>\n"
    "    Bernoulli:<p>     Random pixel mask with masking ratio <p>\n"
    "    text              Random text\n");
    puts("Example:\n"
        "  randmask scribble:3 input.bmp mask.bmp\n");
}


int Dots(unsigned char *Mask, int Width, int Height, int PenRadius);
int Scribble(unsigned char *Mask, int Width, int Height, int PenRadius);
void RandomText(unsigned char *Mask, int Width, int Height);
int Bernoulli(unsigned char *Mask, int Width, int Height, double Ratio);

int main(int argc, char **argv)
{
    const char *Type, *InputFile, *OutputFile;
    char *ParamString;
    unsigned char *Image = NULL;
    double Param = 1;
    int Width, Height, Status = 1;
    
    if(argc != 4)
    {
        PrintHelpMessage();
        return 0;
    }
    
    /* Read command line arguments */
    if((ParamString = strchr(argv[1], ':')))
    {
        Param = atof(ParamString + 1);
	    *ParamString = '\0';		
    }
    
    if(Param < 0)
        Param = 0;
    
    Type = argv[1]; 
    InputFile = argv[2];
    OutputFile = argv[3];
    
    /* Read the input image */
    if(!(Image = (unsigned char *)ReadImage(&Width, &Height, InputFile, 
        IMAGEIO_GRAYSCALE | IMAGEIO_U8)))
        goto Catch;
    
    memset(Image, 0, ((long)Width) * ((long)Height));
    
    /* Initialize random number generator */
    init_randmt_auto();
    
    if(!strcmp(Type, "dots"))
    {
        if(!Dots(Image, Width, Height, (int)Param))
            goto Catch;
    }
    else if(!strcmp(Type, "scribble"))
    {
        if(!Scribble(Image, Width, Height, (int)Param))
            goto Catch;
    }
    else if(!strcmp(Type, "Bernoulli"))
    {   
        if(!Bernoulli(Image, Width, Height, Param))
            goto Catch;
    }
    else if(!strcmp(Type, "text"))
        RandomText(Image, Width, Height);
    else
    {
        fprintf(stderr, "Unknown mask type, \"%s\".\n", Type);
        goto Catch;
    }
    
    /* Write the mask image */
    if(!WriteImage(Image, Width, Height, OutputFile, 
        IMAGEIO_GRAYSCALE | IMAGEIO_U8, JPEGQUALITY))
    {
        fprintf(stderr, "Error writing to \"%s\".\n", OutputFile);
        goto Catch;
    }
    
    Status = 0;
Catch:
    Free(Image);
    return Status;
}


unsigned char *MakePen(int PenRadius)
{
    const int PenRadiusSquared = (int)((PenRadius + 0.5f)*(PenRadius + 0.5f));
    const int PenWidth = 2*PenRadius + 1;    
    unsigned char *Pen = NULL;    
    int x, y, i;
    
    if((Pen = calloc(PenWidth*PenWidth, sizeof(unsigned char))))
        for(y = -PenRadius, i = 0; y <= PenRadius; y++)
            for(x = -PenRadius; x <= PenRadius; x++, i++)
                if(x*x + y*y <= PenRadiusSquared)
                    Pen[i] = 1;
    
    return Pen;
}


void PutPixel(unsigned char *Image, int Width, int Height, 
    int x, int y)
{
    if(0 <= x && x < Width && 0 <= y && y < Height)
        Image[x + Width*y] = 255;
}


void PutPen(unsigned char *Image, int Width, int Height, 
    unsigned char *Pen, int PenRadius, int x0, int y0)
{
    int x, y, i;
    
    for(y = -PenRadius, i = 0; y <= PenRadius; y++)
        for(x = -PenRadius; x <= PenRadius; x++, i++)
            if(Pen[i])
                PutPixel(Image, Width, Height, x0 + x, y0 + y);
}


unsigned char GetPixel(unsigned char *Image, 
    int Width, int Height, int x, int y)
{
    if(0 <= x && x < Width && 0 <= y && y < Height)
        return Image[x + Width*y];
    else
        return 0;
}


/**
 * @brief Fill inpainting mask with random dots
 * @param Mask destination
 * @param Width, Height the mask dimensions
 * @param PenRadius the radius of the dots
 * @return 1 on success, 0 on failure
 */
int Dots(unsigned char *Mask, int Width, int Height, int PenRadius)
{
    const long NumPoints = (long)
        (2 * sqrt(((float)Width) * ((float)Height))/PenRadius + 0.5);
    unsigned char *Pen = NULL;
    long i;
    
    if(!(Pen = MakePen(PenRadius)))
        return 0;
    
    for(i = 0; i < NumPoints; i++)
        PutPen(Mask, Width, Height, Pen, PenRadius,
            (int)(Width * rand_unif()), (int)(Height * rand_unif()));
    
    Free(Pen);
    return 1;
}

/**
 * @brief Fill inpainting mask with Bernoulli distributed pixel-wise mask
 * @param Mask destination
 * @param Width, Height the mask dimensions
 * @param Ratio masking ratio: probability that a pixel is part of the mask
 */
int Bernoulli(unsigned char *Image, int Width, int Height, double Ratio)
{
    const long NumPixels = ((long)Width) * ((long)Height);
    long i;

    if(Ratio < 0 || Ratio > 1)
    {
        fprintf(stderr, "Invalid ratio %1.1f "
            "(should be in the interval [0,1]).\n", Ratio);
        return 0;
    }

    for(i = 0; i < NumPixels; i++) 
        if(rand_unif() < Ratio)
            Image[i] = 255;

    return 1;
}

/**
 * @brief Fill inpainting mask with a random scribble
 * @param Mask destination
 * @param Width, Height the mask dimensions
 * @param PenRadius the radius of the scribble line
 * @return 1 on success, 0 on failure
 * 
 * This routine generates a random path and draws it with radius PenRadius.
 * The path is generated several times and the path with the greatest variance
 * is selected.  
 */
int Scribble(unsigned char *Mask, int Width, int Height, int PenRadius)
{
    const float Accel = 0.15f;
    const float Force = 0.1f;
    const int NumPaths = 20;
    const long NumPoints = (long)
        (5 * sqrt(((float)Width) * ((float)Height)) + 0.5);
    unsigned char *Pen = NULL;
    float (*Path)[2] = NULL, (*BestPath)[2] = NULL;
    float MeanX, MeanY, Variance, BestVariance = 0;
    float PosX, PosY, VelocityX, VelocityY, VelocityMag;
    long i;
    int j, Success = 0;
    
    if(!(Pen = MakePen(PenRadius))
        || !(Path = Malloc(sizeof(float)*2*NumPoints))
        || !(BestPath = Malloc(sizeof(float)*2*NumPoints)))
        goto Catch;
    
    for(j = 0; j < NumPaths; j++)
    {
        /* Generate a random path */
        PosX = (float)(rand_unif() * Width);
        PosY = (float)(rand_unif() * Height);
        VelocityX = (float)rand_normal();
        VelocityY = (float)rand_normal();
        VelocityMag = (float)sqrt(VelocityX*VelocityX + VelocityY*VelocityY);
        VelocityX /= VelocityMag;
        VelocityY /= VelocityMag;
        
        for(i = 0; i < NumPoints; i++)
        {
            Path[i][0] = PosX;
            Path[i][1] = PosY;
            
            VelocityX += 
                Accel*((float)rand_normal() + (Width/2 - PosX)*Force/Width);
            VelocityY +=
                Accel*((float)rand_normal() + (Height/2 - PosY)*Force/Height);

            VelocityMag = (float)sqrt(VelocityX*VelocityX 
                + VelocityY*VelocityY);

            if(VelocityMag > 1)
            {
                VelocityX /= VelocityMag;
                VelocityY /= VelocityMag;
            }
            
            PosX += VelocityX;
            PosY += VelocityY;
            PosX = CLAMP(PosX, 0, Width - 1);
            PosY = CLAMP(PosY, 0, Height - 1);
            
            if((PosX <= 0 && VelocityX < 0)
                || (PosX >= Width - 1 && VelocityX > 0))
                VelocityX = 0;
            
            if((PosY <= 0 && VelocityY < 0)
                || (PosY >= Height - 1 && VelocityY > 0))
                VelocityY = 0;
        }
        
        /* Compute the variance of the path */        
        for(i = 0, MeanX = MeanY = 0; i < NumPoints; i++)
        {
            MeanX += Path[i][0];
            MeanY += Path[i][1];
        }
        
        MeanX /= NumPoints;
        MeanY /= NumPoints;
        
        for(i = 0, Variance = 0; i < NumPoints; i++)
            Variance += (Path[i][0] - MeanX)*(Path[i][0] - MeanX)
                + (Path[i][1] - MeanY)*(Path[i][1] - MeanY);
        
        if(Variance > BestVariance)
        {
            BestVariance = Variance;
            memcpy(BestPath, Path, sizeof(float)*2*NumPoints);
        }
    }
    
    for(i = 0; i < NumPoints; i++)
        PutPen(Mask, Width, Height, Pen, PenRadius,
            (int)BestPath[i][0], (int)BestPath[i][1]);
    
    Success = 1;
Catch:
    if(BestPath)
        Free(BestPath);
    if(Path)
        Free(Path);
    if(Pen)
        Free(Pen);
    return Success;
}


/**
 * @brief Fill inpainting mask with random text about TV inpainting
 * @param Mask destination
 * @param Width, Height the mask dimensions
 *
 * This routine generates a random text from several samples about TV 
 * regularized inpainting.
 */
void RandomText(unsigned char *Mask, int Width, int Height)
{
    /* Several text samples about TV inpainting */
    static const char Text0[] =
    "In the context of digital images, inpainting is used to restore regions "
    "of an image that are corrupted by noise or where the data is missing.  "
    "Inpainting is also used to solve disocclusion, to estimate the scene "
    "behind an obscuring foreground object. Inpainting is an interpolation "
    "problem, filling the unknown region with a condition to agree with the "
    "known image on the boundary.  ";
    static const char Text1[] = 
    "However, Laplace's equation is usually unsatisfactory for images since "
    "it is overly smooth.  It cannot recover a step edge passing through the "
    "region.  Total variation (TV) regularization is an effective inpainting "
    "technique which is capable of recovering sharp edges under some "
    "conditions (these conditions will be explained).  The use of TV "
    "regularization was originally developed for image denoising by Rudin, "
    "Osher, and Fatemi [3] and then applied to inpainting by Chan and Shen.  ";
    static const char Text2[] = 
    "The first variational approach to the image inpainting problem was "
    "Nitzberg and Mumford's 2.1-D sketch [2], based on a variant of the "
    "Mumford-Shah functional, and the second variational approach was the "
    "work of Masnou and Morel [6], based on solving for level lines with "
    "minimal curvature.  Bertalmio, Sapiro, Caselles, and Ballester [8] "
    "introduced the term \"image inpainting\" in analogy to artistic "
    "inpainting and proposed an anisotropic diffusion PDE model.  ";
    static const char Text3[] =
    "TV inpainting prefers straight contours as they have minimal TV, but "
    "this is less successful for recovering curved boundaries.  TV "
    "inpainting can reconstruct a stripe passing through the inpainting "
    "domain, but only if the length to be bridged is less than the stripe "
    "thickness. TV inpainting breaks the stripe if the length is greater.  "
    "TV inpainting prefers straight contours as they have minimal TV, but "
    "this is less successful for recovering curved boundaries.  ";
    const char *Text;
    int TextLength;
    char LineBuffer[1024];
    int i, j, y;
    
    for(y = -4; y < Height; y += 19)
    {
        i = (int)(4*rand_unif());
        
        if(i == 0)
            Text = Text0;
        else if(i == 1)
            Text = Text1;
        else if(i == 2)
            Text = Text2;
        else
            Text = Text3;
        
        TextLength = strlen(Text);
        i = (int)(rand_unif()*TextLength);
        j = 0;
        
        do
        {
            LineBuffer[j] = Text[i % TextLength];
            LineBuffer[j + 1] = '\0';
            i++;
            j++;
        }while(j < 1023 && TextWidth(LineBuffer) < Width + 5);
        
        /* Draw the text with an 18-point sans serif font */
        DrawText(Mask, Width, Height, -2, y, 255, LineBuffer);
    }
}

