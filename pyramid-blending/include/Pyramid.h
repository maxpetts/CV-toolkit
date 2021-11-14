#ifndef __Pyramid_h
#define __Pyramid_h

/**
 ********************************************************************************
 *
 *   @file       Pyramid.h
 *
 *   @brief      Header file for Pyramid.cxx to declare functions needed for pyramid blending
 *
 *   @date       11/02/21
 *
 *   @author     Max Petts
 *
 *   @todo      create gauss & laplac pyramid constructors that return <vector>Mat
 *
 ********************************************************************************
 */

#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

/**
 * Generates a gaussian pyramid from an image
 *
 * @param input_image: The image that will make up the pyramid.
 * @param gauss_pyramid: The gaussian pyramid the images will be placed into.
 * @param level_number: The number of levels the pyramid will have.
 */
void createGaussianPyramid(const Mat& input_image, vector<Mat>& gauss_pyramid, size_t level_number);

/**
 * Generates a laplacian pyramid from a given gaussian pyramid
 *
 * @param gauss_pyramid: The input pyramid to be turned into laplacian pyramid.
 * @param laplac_pyramid: The output pyramid.
 *
 */
void createLaplacianPyramid(const vector<Mat>& gauss_pyramid, vector<Mat>& laplac_pyramid);

/**
 * Reconstruct a laplacian pyramid at any level within the pyramid.
 *
 * @param laplac_pyramid: The pyramid with the desired level to reconstruct.
 * @param aLevel: The level within the pyramid to reconstruct.
 */
Mat reconstruct(const vector<Mat>& laplac_pyramid, int aLevel);

/**
 * Create an image thats a visual representation of the pyramid.
 *
 * @param pyramid: The pyramid to visualise.
 */
Mat visualisePyramid(const vector<Mat>& pyramid);

/**
 * Swap the halves of two images
 *
 * @param image_1: the first image to swap.
 * @param image_2: the second image to swap.
 */
void swapHalves(Mat& image_1, Mat& image_2);

/**
 * Check if an integer is power of two
 *
 * @param i: Integer to check is power of two.
 */
bool isPowerOfTwo(int i);

#endif // __Pyramid_h

