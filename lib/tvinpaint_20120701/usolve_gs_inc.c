/**
 * @file usolve_gs_inc.c
 * @brief u-subproblem Gauss-Seidel solvers for denoising and inpainting
 * @author Pascal Getreuer <getreuer@gmail.com>
 * 
 * Copyright (c) 2010-2012, Pascal Getreuer
 * All rights reserved.
 * 
 * This program is free software: you can use, modify and/or 
 * redistribute it under the terms of the simplified BSD License. You 
 * should have received a copy of this license along this program. If 
 * not, see <http://www.opensource.org/licenses/bsd-license.html>.
 */

/** 
 * @brief Approximately solve the u-subproblem for constant lambda
 * @param S tvreg solver state
 * 
 * Performs one Gauss-Seidel iteration on u to improve the solution in the
 * u-subproblem for denoising problems with constant lambda,
 * \f[ \alpha u-\Delta u=\alpha\tilde{z}-\operatorname{div}\tilde{d}, \f]
 * where \f$ \alpha = \lambda/\gamma \f$, \f$ \Delta u \f$ is the discrete
 * Laplacian of u, and \f$ \operatorname{div}\tilde{d} \f$ is the discrete
 * divergence of dtilde.
 * 
 * In the interior domain, the Gauss-Seidel update is
 * \f[ u_{i,j}=\frac{\alpha\tilde{z}_{i,j}-\tilde{d}^x_{i,j}+
 * \tilde{d}^x_{i-1,j}-\tilde{d}^y_{i,j} + \tilde{d}^y_{i,j-1}+u_{i-1,j}
 * +u_{i+1,j}+u_{i,j-1}+u_{i,j+1}}{\alpha+4}. \f]
 */
static num UGaussSeidelConstantLambda(tvregsolver *S);
/** 
 * @brief Approximately solve the u-subproblem for varying lambda
 * @param S tvreg solver state
 * 
 * This function is used for problems where there is no deconvolution and 
 * spatially-varying lambda, e.g., pure inpainting problems.  It is the same 
 * as UGaussSeidelConstantLambda() except that lambda is spatially varying.  
 */
static num UGaussSeidelVaryingLambda(tvregsolver *S);

#ifndef DOXYGEN
#ifndef _VARYINGLAMBDA
/* Recursively include file twice to define both versions of UGaussSeidel */
#define _VARYINGLAMBDA  0
#include __FILE__           /* Define UGaussSeidelConstantLambda */
#define _VARYINGLAMBDA  1
#include __FILE__           /* Define UGaussSeidelVaryingLambda */
#else   /* if _VARYINGLAMBDA is defined */

#include "tvregopt.h"

#if !_VARYINGLAMBDA
static num UGaussSeidelConstantLambda(tvregsolver *S)
{
#define LAMBDA_INIT     
#define LAMBDA_STEP         
#define ALPHA(i)            (ConstantAlpha)
#define DENOM_INTERIOR      (DenomInterior)
    const num ConstantAlpha = S->Alpha;
    const num DenomInterior = ConstantAlpha + 4;

#else
static num UGaussSeidelVaryingLambda(tvregsolver *S)
{
#define LAMBDA_INIT         Lambda = VaryingLambda
#define LAMBDA_STEP         Lambda += Width
#define ALPHA(i)            (Lambda[i] / Gamma)
#define DENOM_INTERIOR      (4 + ALPHA(x))
    const num *VaryingLambda = S->Opt.VaryingLambda;    
    const num *Lambda;
    const num Gamma = S->Opt.Gamma1;
#endif
    num *u = S->u;
#ifndef TVREG_USEZ    
    const num *ztilde = S->f;
#else
    const num *ztilde = (S->UseZ) ? S->ztilde : S->f;
#endif
    const numvec2 *dtilde = S->dtilde;
    const int Width = S->Width;
    const int Height = S->Height;
    const int NumChannels = S->NumChannels;
    num unew, Norm = 0;   
    int x, y, k;
    
    for(k = 0; k < NumChannels; k++)
    {
        LAMBDA_INIT;
        
        /* Top-left corner */
        unew = (ALPHA(0)*ztilde[0] - dtilde[0].x - dtilde[0].y 
            + u[1] + u[Width]) / (2 + ALPHA(0));
        Norm += (unew - u[0]) * (unew - u[0]);
        u[0] = unew;
        
        /* Top row, x = 1, ..., Width - 2 */
        for(x = 1; x < Width - 1; x++)
        {
            unew = (ALPHA(x)*ztilde[x] - dtilde[x].x + dtilde[x - 1].x
                - dtilde[x].y + u[x - 1] + u[x + 1] + u[x + Width]) 
                / (3 + ALPHA(x));
            Norm += (unew - u[x]) * (unew - u[x]);
            u[x] = unew;
        }
        
        /* Top-right corner */
        unew = (ALPHA(x)*ztilde[x] - dtilde[x].y 
            + u[x - 1] + u[x + Width]) / (2 + ALPHA(x));
        Norm += (unew - u[x]) * (unew - u[x]);
        u[x] = unew;
        
        u += Width;
        ztilde += Width;
        dtilde += Width;
        LAMBDA_STEP;
        
        /* Rows y = 1, ..., Height - 2 */        
        for(y = 1; y < Height - 1; y++, 
            u += Width, ztilde += Width, dtilde += Width)
        {
            /* Left edge */
            unew = (ALPHA(0)*ztilde[0] - dtilde[0].x - dtilde[0].y
                + dtilde[-Width].y + u[1] + u[-Width] + u[Width]) 
                / (3 + ALPHA(0));
            Norm += (unew - u[0]) * (unew - u[0]);
            u[0] = unew;
            
            /* Interior */
            for(x = 1; x < Width - 1; x++)
            {
                unew = (ALPHA(x)*ztilde[x] - dtilde[x].x + dtilde[x - 1].x
                    - dtilde[x].y + dtilde[x - Width].y
                    + u[x - 1] + u[x + 1] + u[x - Width] + u[x + Width])
                    / DENOM_INTERIOR;
                
                Norm += (unew - u[x]) * (unew - u[x]);
                u[x] = unew;
            }
            
            /* Right edge */
            unew = (ALPHA(x)*ztilde[x] - dtilde[x].y + dtilde[x - Width].y 
                + u[x - 1] + u[x - Width] + u[x + Width]) / (3 + ALPHA(x));
            Norm += (unew - u[x]) * (unew - u[x]);
            u[x] = unew;            
            LAMBDA_STEP;
        }
        
        /* Bottom-left corner */
        unew = (ALPHA(0)*ztilde[0] - dtilde[0].x
            + u[1] + u[-Width]) / (2 + ALPHA(0));
        Norm += (unew - u[0]) * (unew - u[0]);
        u[0] = unew;
        
        /* Bottom row, x = 1, ..., Width - 2 */
        for(x = 1; x < Width - 1; x++)
        {
            unew = (ALPHA(x)*ztilde[x] - dtilde[x].x + dtilde[x - 1].x 
                + u[x - 1] + u[x + 1] + u[x - Width]) / (3 + ALPHA(x));
            Norm += (unew - u[x]) * (unew - u[x]);
            u[x] = unew;
        }
        
        /* Bottom-right corner */
        unew = (ALPHA(x)*ztilde[x] + u[x - 1] + u[x - Width]) 
            / (2 + ALPHA(x));
        Norm += (unew - u[x]) * (unew - u[x]);
        u[x] = unew;
        
        u += Width;
        ztilde += Width;
        dtilde += Width;
    }
    
    return (num)sqrt(Norm) / S->fNorm;
#undef DENOM_INTERIOR
#undef LAMBDA_INIT
#undef LAMBDA_STEP
#undef ALPHA
#undef _VARYINGLAMBDA    
}

#endif /* _VARYINGLAMBDA */
#endif /* DOXYGEN */
