/**
 ********************************************************************************
 *
 *   @file       Pyramid.cxx
 *
 *   @brief      Handle the functions and logic required for the pyramid blending of two images
 *
 *   @date       11/02/21
 *
 *   @author     Max Petts / eeub35
 *
 *   @todo      create gauss & laplac pyramid constructors that return <vector>Mat
 *
 ********************************************************************************
 */

#include "Pyramid.h"

void createGaussianPyramid(const Mat& input_image, vector<Mat>& gauss_pyramid, size_t level_number) {
    gauss_pyramid.clear();
    
    Mat source = input_image;
    gauss_pyramid.push_back(source);
    
    for (unsigned int i = 0; i < level_number; ++i) {
        Mat dst;
        pyrDown(source, dst, Size(source.cols / 2, source.rows / 2));
        
        gauss_pyramid.push_back(dst);
        
        source = dst;
    }
}

void createLaplacianPyramid(const vector<Mat>& gauss_pyramid, vector<Mat>& laplac_pyramid) {
    laplac_pyramid.clear();

    unsigned int last_id(gauss_pyramid.size());

    Mat source = gauss_pyramid[--last_id];
    laplac_pyramid.push_back(source);

    for (unsigned int i = 0; i < gauss_pyramid.size() - 1; ++i) {
        Mat dst;
        pyrUp(source, dst, Size(source.cols*2, source.rows*2));

        source = gauss_pyramid[--last_id];
        Mat pushtest = source - dst;
        laplac_pyramid.push_back(pushtest);
    }
}

Mat reconstruct(const vector<Mat>& laplac_pyramid, int aLevel) {
    Mat reconstruction;
    
    if(laplac_pyramid.size()) {
        for (int i = 0; i < laplac_pyramid.size() - aLevel; ++i) {
            if (i == 0) {
                reconstruction = laplac_pyramid[i];
            } else {
                Mat dst;
                pyrUp(reconstruction, dst, Size(reconstruction.cols * 2, reconstruction.rows * 2));
                
                reconstruction = dst + laplac_pyramid[i];
            }
        }
    }
    
    return reconstruction;
}


Mat visualisePyramid(const vector<Mat>& pyramid) {
    cv::Scalar background_colour(51, 184, 34, 255);
    cv::Mat window_data(256, 256, CV_8UC3, background_colour);
    
    if (pyramid.size()) {
        int EDGE = 2;
        bool ascending(true);
        
        Mat tmp;
        
        if (pyramid.front().rows > pyramid.back().rows) {
            tmp = pyramid.front();
            ascending = true;
        } else {
            tmp = pyramid.back();
            ascending = false;
        }
        
        window_data = Mat(tmp.rows * 1.5 + EDGE * 2.0,
                          tmp.cols  + EDGE * (pyramid.size() + 1),
                          CV_8UC3,
                          background_colour);
        
        int x_offset(EDGE);
        int y_offset(EDGE);
        
        for (unsigned int i = 0; i < pyramid.size(); ++i) {
            if (ascending) {
                tmp = pyramid[i];
            } else {
                tmp = pyramid[pyramid.size() - 1 - i];
            }
            
            normalize(tmp, tmp, 0, 255, NORM_MINMAX, CV_8UC3);
            Mat targetROI = window_data(Rect(x_offset, y_offset, tmp.cols, tmp.rows));
            
            tmp.copyTo(targetROI);
            
            if (i == 0) {
                y_offset += tmp.rows + EDGE;
            } else {
                x_offset += tmp.cols + EDGE;
            }
        }
    }
    
    return window_data;
}

void swapHalves(Mat& image_1, Mat& image_2) {
    if (image_1.rows != image_2.rows || image_1.cols != image_2.cols) {
        throw runtime_error("Images do not have the same size");
    }
    
    int half_width = image_1.cols / 2;
    
    Rect rectangle;
    rectangle.x = 0;
    rectangle.y = 0;
    rectangle.width = image_1.cols / 2;
    rectangle.height = image_1.rows;
    
    Mat ROI1 = image_1(rectangle);
    Mat ROI2 = image_2(rectangle);
    
    Mat temp = ROI1.clone();
    
    ROI2.copyTo(ROI1);
    temp.copyTo(ROI2);
}

bool isPowerOfTwo(int i) {
    if (i % 2 == 0) {
        return true;
    } else {
        return false;
    }
}

