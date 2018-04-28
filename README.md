# MANUSCRIPTS RESTORATION
**Version 1.0**

**Date**: 22/02/2018

**Authors**: Simone Parisotto, Luca Calatroni, Carola-Bibiane Schönlieb

This is a companion software for the [submission](https://arxiv.org/abs/1803.07187)
```
L. Calatroni, M. D’Autume, R. Hocking, S. Panayotova, S. Parisotto, P. Ricciardi, C.-B. Schönlieb, 
"Unveiling the invisible - mathematical methods for restoring and interpreting illuminated manuscripts"
arXiv:1803.07187, 2018.
```
Please use the following entry to cite this code:
```
@Misc{ManuscriptRestorationGitHub2018,
 author       = {Parisotto, S Calatroni, L. \and Sch\"{o}nlieb, C.-B.},
 title        = {Manuscripts restoration},
 howpublished = {GitHub repository},
 month        = {April},
 year         = {2018}
  }
```

#### Dataset Copyright
The content and images of this work are part of the **ILLUMINATED: Manuscripts in the making** project.
We refers to the [project website](http://www.fitzmuseum.cam.ac.uk/illuminated) for more information and the related [copyright]9http://www.fitzmuseum.cam.ac.uk/illuminated/footer/copyright?back=section/undefined) notice.


### Code description: segmentation and Inpainting step
The workflow consists in two steps: the recognition of damaged areas in manuscripts and the inpainting restoration.
- 1st step: run Matlab file for segmentation combining active contour + kmeans 
```
./manuscript_segmentation.m
```
- 2nd step: run bash script for the inpaint the damaged areas detected in step 1
```
./manuscript_inpainting.sh
```

#### Results:
Each experiment is stored in a “./results/paper_results/testXXX” folder where XXX is the number of the 
experiments, named as 
```
- 101,102 etc if from Manuscript 1;
- 201,202 etc if from Manuscript 2.
```

#### Example
In folder ./results/paper_resultS/test101 we store the results of experiment 01 for Manuscript 1:
- input_orig101.png: the real crop of Manuscript 1;
- input101.png: the preprocessing (smoothing/texture removal);
- overlap_SUPER101.png: overlap between image crop and supervised pixel inputs in blue 
  squares (just one pixel, dilated for display purposes);
- overlap_SUPERwithCV101.png: overlap between active contour regions in yellow and 
  supervised pixel inputs in blue squares (just one pixel, dilated for display purposes);
- overlap_CV101png: overlap between image crop and active contour regions in yellow;
- overlap101.png: final overlap between image and segmentation from kmeans+activecontour
- mask101.png: final segmentation mask
- masked101.png: input for TV inpainting
- TVinpainted101.png: TV inpainting result
- PATCHinpainted101_PxP: results of nonlocal exemplar-based inpainting result with patch 
  of size P and TV inpainting as initialization
  
#### LICENSE
BSD 3-Clause License,
