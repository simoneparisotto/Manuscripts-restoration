%%% SEGMENTATION OF INPAINTING DOMAIN FOR MANUSCRIPTS
%
%   Description:
%       Combination Chan-Vese + K-Means
%
%   Authors:
%       Simone Parisotto  sp751@cam.ac.uk
%       Dr Luca Calatroni luca.calatroni@polytechnique.edu
%       Dr Carola-Bibiane Sch?nlieb cbs31@cam.ac.uk
%
%   Date: 20-02-2018
%
%%%

clc
clear
close all

addpath ./dataset
addpath ./results/points


% CROP A DETAIL
LOAD_CROP = [101 106 201 203 204 110];

for LC = 1:numel(LOAD_CROP)
    
    %% LOAD CROP
    load_crop = LOAD_CROP(LC);
    flag_crop = 0;
    
    if ~exist(['./results/paper_results/test',num2str(load_crop)],'dir')
        mkdir('./results/paper_results/',['test',num2str(load_crop)]);
    end
    
    if load_crop<200
        imagename = 'ms 330_f5_201101_mfj22_mas.tif';
        im_orig    = im2double(imread(imagename));
    else
        imagename = 'ms 330_f3_201101_mfj22_mas.tif';
        im_orig    = im2double(imread(imagename));
    end
    
    switch load_crop
        case 101
            im_crop = im_orig(2756:3716,910:1600,:);
            %         case 102
            %             im_crop = im_orig(500:1100,950:1500,:);
            %         case 103
            %             im_crop = im_orig(3900:5000,2345:3120,:);
            %         case 104
            %             im_crop = im_orig(2930:3620,1550:2070,:);
            %         case 105
            %             im_crop = im_orig(470:1100,1215:1950,:);
        case 106
            im_crop = im_orig(1270:1830,430:850,:);
            %         case 107
            %             im_crop = im_orig(3328:3900,1730:2300,:);
            %         case 108
            %             im_crop = im_orig(500:1100,950:1950,:);
            %         case 109
            %             im_crop = im_orig(445:2270,1050:2425,:);
        case 110
            im_crop = im_orig(2950:4630,1040:2611,:);
        case 201
            im_crop = im_orig(1500:2400,2340:3000,:);
            %         case 202
            %             im_crop = im_orig(1290:2200,1260:1900,:);
        case 203
            im_crop = im_orig(3000:3800,425:1080,:);
        case 204
            im_crop = im_orig(2890:3620,2465:3010,:);
            %         case 205
            %             im_crop = im_orig(1400:2800,320:3150,:);
        otherwise
            flag_crop = 1;
            figure(1),
            imshow(im_orig)
            im_crop = imcrop;
    end
    
    nrows = size(im_crop,1);
    ncols = size(im_crop,2);
    nchan = size(im_crop,3);
    
    %% PREPROCESSING FOR A SMOOTHER INSPECTION
    addpath ./lib/RollingGuidanceFilter_Matlab/
    im           = RollingGuidanceFilter(im_crop,1,0.1,4);
    
    %% SUPERVISED DEFECT SEGMENTATION
    fprintf('1) Selecting defects... ')
    
    % LOAD (already saved) LOCATIONS or MANUALLY SELECT THE DAMAGES
    if flag_crop || ~exist(['./results/points/',num2str(load_crop),'.mat'],'file')
        figure
        imshow(im,[])
        title('Select damages (press ENTER to continue)')
        [jj, ii, button] = ginput;
        N = numel(button);
        jj = uint64(jj);
        ii = uint64(ii);
        save(['./results/points/',num2str(load_crop),'.mat'],'ii','jj')
    else
        load(['./results/points/',num2str(load_crop),'.mat'])
    end
    fprintf('loaded\n')
    
    %% MASK FROM SUPERVISED SEGMENTATION (external input)
    SUPERVISED_mask = zeros(nrows,ncols);
    SUPERVISED_mask(sub2ind([nrows,ncols],ii,jj)) = 1;
    
    %% MASK FROM CHAN-VESE SEGMENTATION
    fprintf('2) Segmenting with CHAN-VESE\n')
    ACTIVE_mask     = imdilate(SUPERVISED_mask,ones(5));
    imgray          = rgb2gray(im);
    CV_mask         = activecontour(imgray,ACTIVE_mask,1000,'Chan-Vese');
    CV_mask         = imerode(CV_mask,ones(2));
    
    % DICTIONARY MASK: SUPERVISED + CHAN-VESE
    DICTIONARY_MASK = SUPERVISED_mask | CV_mask;
    
    % DISPLAY RESULT
    figure(100)
    subplot(2,3,1)
    imshow(im_crop,[])
    title('Crop')
    subplot(2,3,2)
    imshow(DICTIONARY_MASK,[])
    title('Chan-Vese mask')
    subplot(2,3,3)
    imshow(imoverlay(im_crop,DICTIONARY_MASK),[])
    title('Chan-Vese overlap')
    pause(0.1)
    
    %% K-MEANS SEGMENTATION
    fprintf('3) Segmenting with KMEANS\n\n')
    imk = im;
    
    % COMPUTE MULTIPLE FEATURES
    
    % srgb2lab
    lab_im = rgb2lab(imk);
    %lab_im = lab_im(:,:,2:3);
    Input_lab = reshape(lab_im,nrows*ncols,size(lab_im,3));
    
    % srgb2cmyk
    cform = makecform('srgb2cmyk');
    cmyk_im = applycform(imk,cform);
    Input_cmyk = reshape(cmyk_im,nrows*ncols,size(cmyk_im,3));
    
    % rgb2hsv
    hsv_im = rgb2hsv(imk);
    %hsv_im = hsv_im(:,:,1:2);
    Input_hsv = reshape(hsv_im,nrows*ncols,size(hsv_im,3));
    
    % 'chroma'
    norm_fact = repmat(prod(imk,3).^(1/3),1,1,3);
    chrom_im = imk./norm_fact;
    Input_chroma = reshape(chrom_im,nrows*ncols,size(chrom_im,3));
    
    Input = cat(2,Input_lab,Input_cmyk,Input_hsv,Input_chroma);
    
    % K-MEANS SEGMENTATION
    nColors                       = 35; %25
    [cluster_idx, cluster_center] = kmeans(Input,nColors,'distance','sqEuclidean', 'Replicates',5,'EmptyAction','drop','MaxIter',10000);
    
    KMEANS_mask                   = reshape(cluster_idx,nrows,ncols);
    
    clear imk;
    
    %% SELECT ONLY RELEVANT INDICES IN THE DICTIONARY
    % accept only if the indices labels given by the joint use of
    % CHAN-VESE segmentation and KMEANS have more than % of data
    % in the dictionary (this is to prevent tiny classes)
    values = KMEANS_mask(DICTIONARY_MASK);
    v      = unique(KMEANS_mask(DICTIONARY_MASK));
    Val    = [];
    percentage = 0.1;
    accept_val = @(val) sum(values==val) > percentage*numel(values);
    for vv = 1:numel(v)
        if accept_val(v(vv))
            Val(end+1) = v(vv);
        end
    end
    Val = unique(cat(2,Val,unique(KMEANS_mask(SUPERVISED_mask>0)).'));
    
    %% INSPECT THE WHOLE IMAGE DOMAIN TO FIND THE INDICES LEARNED
    LEARNED_mask = zeros(size(DICTIONARY_MASK));
    for j= 1:length(Val)
        idx = KMEANS_mask == Val(j);
        LEARNED_mask(idx) = 1;
    end
    
    %% COMBINE CHAN-VESE and K-MEANS
    FINAL_mask = LEARNED_mask | DICTIONARY_MASK;
    % dilate mask for boundary conditions
    FINAL_mask = imdilate(FINAL_mask,ones(3));
    
    %% CREATE OVERLAPPED MASKS TO BE SAVED
    overlap_CV = imoverlay(im_crop,DICTIONARY_MASK);
    overlap    = imoverlay(im_crop,FINAL_mask);
    SUPER_dilated_mask   = 255*imdilate(SUPERVISED_mask,ones(20));
    SUPER_overlayed_mask = repmat(SUPER_dilated_mask,1,1,3);
    SUPER_overlayed_RGB  = cat(3,zeros(size(SUPERVISED_mask)),zeros(size(SUPERVISED_mask)),SUPER_dilated_mask);
    overlap_SUPER       = im_crop;
    overlap_SUPERwithCV = overlap_CV;
    overlap_SUPER(SUPER_overlayed_mask>0)       = SUPER_overlayed_RGB(SUPER_overlayed_mask>0);
    overlap_SUPERwithCV(SUPER_overlayed_mask>0) = SUPER_overlayed_RGB(SUPER_overlayed_mask>0);
    
    %% SAVE RESULTS
    imwrite(im_crop,            ['./results/paper_results/test',num2str(load_crop),'/input_orig',num2str(load_crop),'.png'])
    imwrite(im,                 ['./results/paper_results/test',num2str(load_crop),'/input',num2str(load_crop),'.png'])
    imwrite(FINAL_mask,         ['./results/paper_results/test',num2str(load_crop),'/mask',num2str(load_crop),'.png'])
    imwrite(overlap,            ['./results/paper_results/test',num2str(load_crop),'/overlap',num2str(load_crop),'.png'])
    imwrite(overlap_SUPER,      ['./results/paper_results/test',num2str(load_crop),'/overlap_SUPER',num2str(load_crop),'.png'])
    imwrite(overlap_SUPERwithCV,['./results/paper_results/test',num2str(load_crop),'/overlap_SUPERwithCV',num2str(load_crop),'.png'])
    imwrite(overlap_CV,         ['./results/paper_results/test',num2str(load_crop),'/overlap_CV',num2str(load_crop),'.png'])
    
    %% FIGURE
    figure(100)
    subplot(2,3,5)
    imshow(LEARNED_mask,[])
    title('K-Means mask')
    subplot(2,3,6)
    imshow(overlap,[])
    title('Combination')
    
    %% SAVE
    close all
    clear im_orig
    save(['./results/result_full',num2str(load_crop),'.mat'])
    
end