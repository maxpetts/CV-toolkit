/**
 ********************************************************************************
 *
 *   @file       MotionDetection.cxx
 *
 *   @brief      Takes a
 *
 *   @date       20.03.20
 *
 *   @author     Max Petts
 *
 *   @todo       Camera pose estimation - for non-static shots?
 *               Auto background detection with createBackgroundSubtractorMOG2()
 *               OR
 *               Track location of contours: if no movement; remove
 *               Add moments for direction estimation
 *
 ********************************************************************************
 */

#include <iostream>
#include <exception>
#include <stdexcept>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat cleanBinaryImage(const Mat& aBinaryImage, int elementSize = 5)
{
    Mat output;

    Mat element = getStructuringElement(MORPH_ELLIPSE,
                        Size(elementSize, elementSize));

    morphologyEx(aBinaryImage, output, MORPH_CLOSE, element + 5);
    morphologyEx(output, output, MORPH_OPEN, element);

    return output;
}

Mat getForegroundMask(const Mat& background, const Mat& next_frame, int aThreshold = 128) {
    Mat current_frame;
    cvtColor(next_frame, current_frame, COLOR_BGR2GRAY);
    
    medianBlur(current_frame, current_frame, 5);
    
    current_frame.convertTo(current_frame, CV_32F);
    
    Mat foreground;
    foreground = background - current_frame;
    foreground = abs(foreground);
    
    normalize(foreground, foreground, 0, 255, NORM_MINMAX, CV_8UC1);
    
    Mat mask;
    threshold(foreground, mask, aThreshold, 255, THRESH_BINARY);
    
    mask = cleanBinaryImage(mask);
    
    return mask;
}

int main (int argc, char** argv) {
    try {
        string filename_1;
        string filename_2;
        VideoCapture video_1;
        VideoWriter video_output;
        bool display_vid_1 = false;
        int key = -1;
        
        // ======= POSSIBLE ARGUMENT COMBOS ========
        // MotionDetection invideo outvideo
        // MotionDetection -display invideo outvideo
        // MotionDetection invideo -display outvideo
        // MotionDetection invideo outvideo -display
        
        if (argc > 2 && argc < 5) {
            
            filename_2 = argv[argc-1];
            
            if ((string)argv[1] == "webcam") {
                cout << "using webcam instead of input file" << endl;
                video_1.open(0);
            } else {
                video_1.open(argv[1]);
            }
            for (int i = 0; i < argc; i++) {
                string temp = argv[i];
                
                if (temp == "-display") {
                    switch (i) {
                        case 1:
                            display_vid_1 = true;
                            video_1.open(argv[2]);
                            break;
                        case 2:
                            display_vid_1 = true;
                            break;
                        case 3:
                            filename_2 = argv[argc-2];
                            break;
                    }
                }
            }
        } else {
            string error_message;
            error_message  = "usage: ";
            error_message += argv[0];
            error_message += " [-display] <input_video/webcam> [-display]";
            error_message += " <output_image> [-display] [radius]";
            error_message += "\n Ensure your -display flag is in the correct location.";
            error_message += "\n <x/y> : both x and y are interchangable";
            error_message += "\n [x] : x is singular and optional";
            
            throw error_message;
        }
        
        // is video empty?
        if (!video_1.isOpened()) {
            string error_message;
            error_message  = "Could not open or find the first video \"";
            error_message += filename_1;
            error_message += "\".";
            
            // Throw an error
            throw error_message;
        }
        
        Mat background;
        
        video_1 >> background; // take mean of first 3 or 5 frames?
        cvtColor(background, background, COLOR_RGB2GRAY);
        imshow("Background", background);
        medianBlur(background, background, 3);
        background.convertTo(background, CV_32F);
        
        while(key != 27 && key != 113){ // esc or q
            Mat frame;
            video_1 >> frame;
            
            if (!frame.empty()) {
                imshow("Input Video", frame);
                
                if (!background.empty()) {
                    int area_thresh = 256;
                    int fore_thresh = 64;
                    
                    Mat foreground_mask = getForegroundMask(background, frame, fore_thresh);
                    Mat clean;
                    vector<vector<Point> > contours;
                    vector<Vec4i> hierarchy;
                    
                    frame.copyTo(clean, foreground_mask);

                    findContours(foreground_mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
                                        
                    for (size_t i = 0; i < contours.size(); i++) { // int or size_t?
                        if (contourArea(contours[i]) > area_thresh) {
                            drawContours(clean, contours, (int)i, Scalar(0,255,0));
                        }
                    }

                    imshow("Foreground", clean);

                    cv::TrackbarCallback cont_callback = [](int pos, void* userdata)  {
                        int& area_thresh = *(int*) userdata;
                    };

                    cv::TrackbarCallback fore_callback = [](int pos, void* userdata) {
                        int& fore_thresh = *(int*) userdata;
                    };
                    
                    cv::createTrackbar("Contour Threshold", "Foreground", NULL, 528, cont_callback);
                    cv::createTrackbar("Foreground Threshold", "Foreground", NULL, 255, fore_callback);
                    
                    // Save video
                    int fps = video_1.get(CAP_PROP_FPS);
                    VideoWriter vid_output(filename_2, VideoWriter::fourcc('M', 'P', 'E', 'G'), fps, Size(frame.cols, frame.rows));
                }
                
            } else {
                video_1.release();
                video_output.release();
                throw runtime_error("Video finished or OpenCV cannot read the video file");
            }
            waitKey(1);
        }
        
        video_1.release();
        video_output.release();
        destroyAllWindows();
        
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
