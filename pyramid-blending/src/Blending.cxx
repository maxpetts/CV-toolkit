/**
 ********************************************************************************
 *
 *   @file       Blending.cxx
 *
 *   @brief      Handle command args for pyramid blending two images together
 *
 *   @date       11/02/21
 *
 *   @author     Max Petts / eeub35
 *
 *   @todo      Accept pyramid level, reconstructed save location, testing as arguments
 *
 ********************************************************************************
 */

#include "Pyramid.h"

int main (int argc, char** argv) {
    try {
        bool display_img_1 = false;
        bool display_img_2 = false;
        string filename_1;
        string filename_2;
        Mat image_1;
        Mat image_2;
        string window_title; // the generated window title
        bool testing = true;
        
        if (argc == 3 || argc == 4) {
            filename_1 = argv[1];
            filename_2 = argv[2];
            for (int i = 0; i < argc; i++) {
                string temp = argv[i];
                
                if (temp == "-display") {
                    switch (i) {
                        case 1:
                            display_img_1 = true;
                            display_img_2 = true;
                            filename_1 = argv[2];
                            filename_2 = argv[3];
                            break;
                        case 2:
                            display_img_1 = true;
                            filename_2 = argv[3];
                            break;
                        case 3:
                            display_img_2 = true;
                            break;
                    }
                }
            }
            
            if (filename_1 == "-display" || filename_2 == "-display") {
                string error_message;
                error_message  = "usage: ";
                error_message += argv[0];
                error_message += " [-display] <image_1> [-display]";
                error_message += " <image_2> [-display]\n";
                
                throw error_message;
            }
            
            // Load images
            image_1 = imread(filename_1, IMREAD_COLOR);
            image_2 = imread(filename_2, IMREAD_COLOR);
            
            // Check images loaded
            if (!image_1.data) {
                string error_message;
                error_message  = "Could not open or find the first image \"";
                error_message += filename_1;
                error_message += "\".";
                
                // Throw an error
                throw error_message;
            }
            if (!image_2.data) {
                string error_message;
                error_message  = "Could not open or find the second image \"";
                error_message += filename_2;
                error_message += "\".";
                
                // Throw an error
                throw error_message;
            }
            
            // Are the image sizes powers of two?
            if (!isPowerOfTwo((int)image_1.rows) || !isPowerOfTwo((int)image_1.cols)) {
                int row_delta = image_2.rows - image_1.rows;
                int col_delta = image_2.cols - image_1.cols;
                
                // Add a border on both edges - keeps image central.
                // May make image +/- 1px due to using integers, which screws up pyramid creation(?or something else).
                // copyMakeBorder(image_1, image_1, row_delta / 2, row_delta / 2, col_delta / 2, col_delta / 2, BORDER_REPLICATE);
                copyMakeBorder(image_1, image_1, row_delta, 0, col_delta, 0, BORDER_DEFAULT);
                
            }
            if (!isPowerOfTwo((int)image_2.rows) || !isPowerOfTwo((int)image_2.cols)) {
                int row_delta = image_1.rows - image_2.rows;
                int col_delta = image_1.cols - image_2.cols;
                
                copyMakeBorder(image_2, image_2, row_delta, 0, col_delta, 0, BORDER_DEFAULT);
            }
            
            if (display_img_1) {
                window_title = "Displaying: " +  filename_1 + "\"";
                namedWindow(window_title, WINDOW_AUTOSIZE);
                imshow(window_title, image_1);
                waitKey(0);
            }
            if (display_img_2) {
                window_title = "Displaying: " + filename_2 + "\"";
                namedWindow(window_title, WINDOW_AUTOSIZE);
                imshow(window_title, image_2);
                waitKey(0);
            }
            //            destroyAllWindows();
            
            // convert both images to use 3 channel 32bit floats
            image_1.convertTo(image_1, CV_32FC3);
            image_2.convertTo(image_2, CV_32FC3);
            
            if (image_1.rows == image_2.rows || image_1.cols == image_2.cols) {
                size_t levels = 8;
                vector<Mat> gauss_pyramid_1;
                vector<Mat> gauss_pyramid_2;
                vector<Mat> lapl_pyramid_1;
                vector<Mat> lapl_pyramid_2;
                createGaussianPyramid(image_1, gauss_pyramid_1, levels);
                createGaussianPyramid(image_2, gauss_pyramid_2, levels);
                createLaplacianPyramid(gauss_pyramid_1, lapl_pyramid_1);
                createLaplacianPyramid(gauss_pyramid_2, lapl_pyramid_2);
                
                
                
                if (testing) {
                    // display gaus pyramids
                    window_title = "Displaying: " +  filename_1 + "\" as gaussian pyramid";
                    namedWindow(window_title, WINDOW_AUTOSIZE);
                    imshow(window_title, visualisePyramid(gauss_pyramid_1));
                    imwrite("../pyramid1-gauss.png", visualisePyramid(gauss_pyramid_1));
                    
                    window_title = "Displaying: " +  filename_2 + "\" as gaussian pyramid";
                    namedWindow(window_title, WINDOW_AUTOSIZE);
                    imshow(window_title, visualisePyramid(gauss_pyramid_2));
                    imwrite("../pyramid2-gauss.png", visualisePyramid(gauss_pyramid_2));
                    waitKey(0);
                    
                    // display lapl pyramids
                    window_title = "Displaying: " +  filename_1 + "\" as laplacian pyramid";
                    namedWindow(window_title, WINDOW_AUTOSIZE);
                    imshow(window_title, visualisePyramid(lapl_pyramid_1));
                    imwrite("../pyramid1-lapl.png", visualisePyramid(lapl_pyramid_1));
                    
                    window_title = "Displaying: " +  filename_2 + "\" as laplacian pyramid";
                    namedWindow(window_title, WINDOW_AUTOSIZE);
                    imshow(window_title, visualisePyramid(lapl_pyramid_2));
                    imwrite("../pyramid2-lapl.png", visualisePyramid(lapl_pyramid_2));
                    waitKey(0);
                }
                
                for (size_t i = 0; i < lapl_pyramid_1.size(); ++i) {
                    swapHalves(lapl_pyramid_1[i], lapl_pyramid_2[i]);
                }
                
                Mat lapl_pyramid_1_2 = visualisePyramid(lapl_pyramid_1);
                Mat lapl_pyramid_2_1 = visualisePyramid(lapl_pyramid_2);
                Mat recon_1 = reconstruct(lapl_pyramid_1, 1);
                Mat recon_2 = reconstruct(lapl_pyramid_2, 1);
                recon_1.convertTo(recon_1, CV_8UC3);
                recon_2.convertTo(recon_2, CV_8UC3);
                
                // save & display for testing
                if (testing) {
                    for (size_t i = 0; i < levels; ++i) {
                        Mat reconstruct_1 = reconstruct(lapl_pyramid_1, i);
                        Mat reconstruct_2 = reconstruct(lapl_pyramid_2, i);
                        reconstruct_1.convertTo(reconstruct_1, CV_8UC3);
                        reconstruct_2.convertTo(reconstruct_2, CV_8UC3);
                        
                        stringstream filename;
                        filename << "../image1-recon" << i + 1 << ".png";
                        imwrite(filename.str(), reconstruct_1);
                        filename.str(string()); // clear stringstream
                        filename << "../image2-recon" << i + 1 << ".png";
                        imwrite(filename.str(), reconstruct_2);
                    }
                    
                    //display stitched lapl pyramid
                    window_title = "Displaying: \"" + filename_1 + "\" stitched lapl pyramid";
                    namedWindow(window_title, WINDOW_AUTOSIZE);
                    imshow(window_title, lapl_pyramid_1_2);
                    imwrite("../stitch-lapl-1-2.png", lapl_pyramid_1_2);
                    
                    if (!imwrite("../gauss1.png", gauss_pyramid_1))
                        cout <<"gauss 1 no" <<endl;
                    
                    if (!imwrite("../gauss2.png", gauss_pyramid_2))
                        cout <<"gauss 2 no" <<endl;
                    
                    window_title = "Displaying: \"" + filename_2 + "\" stitched lapl pyramid";
                    namedWindow(window_title, WINDOW_AUTOSIZE);
                    imshow(window_title, lapl_pyramid_2_1);
                    imwrite("../stitch-lapl-2-1.png", lapl_pyramid_2_1);
                    waitKey(0);
                }
                
                // display stitched images
                window_title = "Displaying: \"" + filename_1 + "\" split with \"" + filename_2 + "\"";
                namedWindow(window_title, WINDOW_AUTOSIZE);
                imshow(window_title, recon_1);
                imwrite("../stitch-1-2.png", recon_1);
                
                window_title = "Displaying: \"" + filename_2 + "\" split with \"" + filename_1 + "\"";
                namedWindow(window_title, WINDOW_AUTOSIZE);
                imshow(window_title, recon_2);
                imwrite("../stitch-2-1.png", recon_2);
                waitKey(0);
            } else {
                string error_message;
                error_message = "Images are not the same size. Resizing failed\nAborting";
                
                throw error_message;
            }
        } else {
            string error_message;
            error_message  = "usage: ";
            error_message += argv[0];
            error_message += " [-display] <image_1> [-display]";
            error_message += " <image_2> [-display]";
            
            throw error_message;
        }
    } catch (const exception& error) {
        // Display an error message in the console
        cerr << error.what() << endl;
    } catch (const string& error) {
        // Display an error message in the console
        cerr << error << endl;
    } catch (const char* error) {
        // Display an error message in the console
        cerr << error << endl;
    } catch (...) {
        // Display an error message in the console
        cerr << "Unnown error caught" << endl;
    }
    
    return 0;
}
