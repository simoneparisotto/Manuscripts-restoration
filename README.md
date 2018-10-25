# Digital restoration of Illuminated Manuscripts

<img src="https://raw.githubusercontent.com/simoneparisotto/Manuscripts-restoration/master/results/paper_results/test203/input_orig203.png" width=16%> <img src="https://raw.githubusercontent.com/simoneparisotto/Manuscripts-restoration/master/results/paper_results/test203/overlap_SUPER203.png" width=16%>  <img src="https://raw.githubusercontent.com/simoneparisotto/Manuscripts-restoration/master/results/paper_results/test203/overlap_CV203.png" width=16%> <img src="https://raw.githubusercontent.com/simoneparisotto/Manuscripts-restoration/master/results/paper_results/test203/overlap203.png" width=16%>  <img src="https://raw.githubusercontent.com/simoneparisotto/Manuscripts-restoration/master/results/paper_results/test203/TVinpainted203.png" width=16%>  <img src="https://raw.githubusercontent.com/simoneparisotto/Manuscripts-restoration/master/results/paper_results/test203/PATCHinpainted203_5x5.png" width=16%> 

**Authors of this software**: Simone Parisotto and Luca Calatroni

**Other authors and collaborators**: Carola-Bibiane Schönlieb, Stella Panayotova, Paola Ricciardi

**Version 1.0**

**Date**: 22/02/2018

This is a companion software for the [journal article](https://doi.org/10.1186/s40494-018-0216-z):
```
L. Calatroni, M. D’Autume, R. Hocking, S. Panayotova, S. Parisotto, P. Ricciardi, C.-B. Schönlieb, 
"Unveiling the invisible - mathematical methods for restoring and interpreting illuminated manuscripts"
Heritage Science, Springer International Publishing (2018) 6: 56. DOI: 10.1186/s40494-018-0216-z.
```

Please use the following entry to cite the article
```
@Article{CalAutHocPanParRicSch2018,
author="Calatroni, Luca and d'Autume, Marie and Hocking, Rob 
and Panayotova, Stella and Parisotto, Simone 
and Ricciardi, Paola and Sch{\"o}nlieb, Carola-Bibiane",
title="Unveiling the invisible: mathematical methods for restoring and interpreting illuminated manuscripts",
journal="Heritage Science",
year="2018",
volume="6",
number="1",
pages="56",
publisher="Springer International Publishing"
doi="10.1186/s40494-018-0216-z",
url="https://doi.org/10.1186/s40494-018-0216-z"
}
```

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
The experiment with number XXX is stored in the folder “./results/paper_results/testXXX” folder, where
```
- 101,102,... refers to images from Manuscript 1;
- 201,202,... refers to images from Manuscript 2.
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
  
### Dataset Copyright
The content and images of this work are part of the **ILLUMINATED: Manuscripts in the making** project.
We refers to the [project website](http://www.fitzmuseum.cam.ac.uk/illuminated) for more information and the related [copyright](http://www.fitzmuseum.cam.ac.uk/illuminated/footer/copyright?back=section/undefined) notice.
  
### License
[BSD 3-Clause License](https://opensource.org/licenses/BSD-3-Clause).
