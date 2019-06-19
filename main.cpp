#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace std;
using namespace cv;

/* NOTES
This programme detects the lanes on which a vehicle is being driven from the camera input in real time.
It uses mainly the followin three steps to achieve it target:
Step 1: Blurring the frame to keep sharp lines in focus
Step 2: Using canny edge detection to identify the sharp lines in frame
Step 3: Masking the screen to keep suitable area, i.e. the road ahead, in focus
Step 4: Using hough line transformation detect edges (rho and theta are detremined by trail and error so that only the wanted lines are selected)
Step 5: Display the transformed frame
*/

#include <string>
#include <vector>
#include "opencv2/opencv.hpp"


// IMAGE BLURRING
Mat noise(Mat input)
{
    Mat output;
    GaussianBlur(input, output, Size(3,3), 0, 0);
    return output;
}

// EDGE DETECTION

 Mat edge ( Mat imd_denoised)
 {
    Mat output, kernel;
    Point anchor;

    cvtColor(img_denoised, out, cv::COLOR_RGB2GRAY);
    threshold(output, output, 140, 255, THRESH_BINARY);

    //create kernel [-1 0 1]
    anchor = cv::Point(-1, -1);
    kernel = cv::Mat(1, 3, CV_32F);
    kernel.at<float>(0, 0) = -1;
    kernel.at<float>(0, 1) = 0;
    kernel.at<float>(0, 2) = 1;

    filter2D(output, output, -1, kernel, anchor, 0, BORDER_DEFAULT);

    return output;

 }



// Select suitable area
Mat choose (Mat imgedge)
{
    Mat output;
    Mat chosen=Mat::zeros(imgedge.size(), imgedge.type());
    Point pts[4] = {
      cv::Point(210, 720),
      cv::Point(550, 450),
      cv::Point(717, 450),
      cv::Point(1280, 720)
    };

    fillConvexPoly(chosen, pts, 4, cv::Scalar(255, 0, 0));
    bitwise_and(img_edges, mask, output);

    return output;

}

// APPLYING HOUGH TRANSFORM

std::vector<cv::Vec4i> houghLines(Mat img_mask) {
  std::vector<cv::Vec4i> line;
  // rho and theta are selected by trial and error
  HoughLinesP(img_mask, line, 1, CV_PI/180, 20, 20, 30);

  return line;
}


int main()
{

   VideoCapture vid("testvid.mp4"); //remember to name the video test vid


   if (!vid.isOpened())
        return -1;

    Mat frame;
    Mat img_denoise;
    Mat img_edges;
    Mat img_mask;
    Mat img_lines;
    vector<cv::Vec4i> lines;

    while(1)
    {
        if(!vid.read(frame)) break;

        img_denoise= noise(frame);
        img_edges = edge(img_denoise);
        img_mask= choose(img_edges);
        lines = houghLines(img_mask);

   //draw our lines

        for( size_t i = 0; i < lines.size(); i++ )
        {
            Vec4i l = lines[i];  // we have 4 elements p1=x1,y1  p2= x2,y2
            Scalar redColor= Scalar(0,30,250);  // B=0 G=250 R=30
            line( ROI, Point(l[0], l[1]), Point(l[2], l[3]), redColor, 3, CV_AA);

        }

         namedWindow("frame");
         moveWindow("frame",275,30);
         imshow("frame",frame);


     if(waitKey(30)==27)
      {
       cout<<"esc";
         break;
      }
    }

    return 0;
    }
}
