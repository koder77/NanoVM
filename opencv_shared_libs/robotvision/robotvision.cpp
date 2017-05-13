/* This is a shared library for object detection with open cv and Nano VM.
 * I changed the original code from a tutorial.
 * 
 * uses two webcams for 3D points distance measure
 * 
 * Stefan Pietzonke 2017
 */


#include "../../include/host.h"

#undef JIT_CPP
#define JIT_CPP  1

#include "../../include/global_d.h"

extern "C" {
#include "../../dll_shared_libs/nanovmlib.h"
}

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace cv;
using namespace std;


#define DEBUG				0

#define CAMCENTER 			319		// half cam picture width - 1
#define MAX3DPOINTS			500
#define MAX3DPOINTS_LIMIT	512
struct points_3d
{
	S8 x;
	S8 y;
	S8 distance;
};


int get_distance (int diff)
{
	int distanceret;
	F8 sqrten, r2, distance;
	
	sqrten = sqrt (10.0);
	r2 = -11.0 * diff; r2 = r2 - 1200.0;
	r2 = sqrt (r2);
	
	distance = sqrten * r2 - 265.0;
	distance = distance / 11.0;
	distance = distance * 10.0;
	
	distanceret = distance;
	return (distanceret);
}

void opencv_detect_faces (Mat cam_left, Mat cam_right)
{
	CascadeClassifier haar_cascade;
    haar_cascade.load("/home/pi/nanovm/home/haarcascade_face.xml");
	CascadeClassifier eyes_cascade;
	eyes_cascade.load("/home/pi/nanovm/home/haarcascade_eye.xml");
	
	Mat gray_left, gray_right;
        cvtColor(cam_left, gray_left, CV_BGR2GRAY);
		cvtColor(cam_right, gray_right, CV_BGR2GRAY);
        // Find the faces in the frame:
        vector< Rect_<int> > faces_left;
        haar_cascade.detectMultiScale(gray_left, faces_left);
		
		vector< Rect_<int> > faces_right;
        haar_cascade.detectMultiScale(gray_right, faces_right);
        // At this point you have the position of the faces in
        // faces. Now we'll get the faces, make a prediction and
        // annotate it in the video. Cool or what?
		
        for(int i = 0; i < faces_left.size(); i++) {
            // Process face by face:
            Rect face_i = faces_left[i];
            // Crop the face from the image. So simple with OpenCV C++:
            Mat face = gray_left(face_i);
            // Resizing the face is necessary for Eigenfaces and Fisherfaces. You can easily
            // verify this, by reading through the face recognition tutorial coming with OpenCV.
            // Resizing IS NOT NEEDED for Local Binary Patterns Histograms, so preparing the
            // input data really depends on the algorithm used.
            //
            // I strongly encourage you to play around with the algorithms. See which work best
            // in your scenario, LBPH should always be a contender for robust face recognition.
            //
            // Since I am showing the Fisherfaces algorithm here, I also show how to resize the
            // face you have just found:

			string box_text;
			
			// Calculate the position for annotated text (make sure we don't
            // put illegal values in there):
            int pos_x = std::max(face_i.tl().x - 10, 0);
            int pos_y = std::max(face_i.tl().y - 10, 0);
			
            // Create the text we will annotate the box with:
			
			rectangle(cam_left, face_i, CV_RGB(0, 255, 0), 1);
			// box_text = format("detected");
			// And now put it into the image:
			// putText(original, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,255,0), 2.0);
			
			Mat faceROI = face;
			std::vector<Rect> eyes;

			//-- In each face, detect eyes
			eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );

			for( size_t j = 0; j < eyes.size(); j++ )
			{
				Point center( faces_left[i].x + eyes[j].x + eyes[j].width*0.5, faces_left[i].y + eyes[j].y + eyes[j].height*0.5 );
				int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
				circle( cam_left, center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
			}
        }
        // Show the result:
        imshow("face_detection: left", cam_left);
        // And display it:
}

// Store the formatted string of time in the output
void format_time (char *output)
{
    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    sprintf (output, "[%d-%d-%d-%d:%d:%d]", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

extern "C" int opencv_detect_object (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	struct points_3d points_3d[MAX3DPOINTS_LIMIT];
	
	S8 max3dpoints = 0;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
	
	/* get arguments from stack */
	
    S8 deviceId = 0;
	S8 qvar_index;			/* qint var, return code variable index */
	S8 points_index;		/* qint var, number of 3D points */
	S8 matches;
	
	// for blackbox picture save
	S8 picture_num = 1, picture_nummax = 4, numlen, zero_fill;
	int save_skip = 3;
	
	if (stpull_l (threadnum, pthreads, stacksize, &points_index) != ERR_STACK_OK) printf ("opencv detect object: ERROR on stack points_index!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &qvar_index) != ERR_STACK_OK) printf ("opencv detect object: ERROR on stack qvar_index!\n");
	// if (stpull_l (threadnum, pthreads, stacksize, &deviceId) != ERR_STACK_OK) printf ("opencv detect object: ERROR on stack deviceId!\n");
	
	// Get a handle to the Video device:
    VideoCapture cap_left(deviceId);
	VideoCapture cap_right(deviceId + 1);
	
    // Check if we can use this device at all:
    if(!cap_left.isOpened()) {
        cerr << "Capture Device ID " << deviceId << "cannot be opened." << endl;
        return -1;
    }
    // Check if we can use this device at all:
    if(!cap_right.isOpened()) {
        cerr << "Capture Device ID " << deviceId + 1 << "cannot be opened." << endl;
        return -1;
    }
    
	Mat cam_left_out;
	Mat cam_right_out;
	
	Mat cam_left_grey;
	Mat cam_right_grey;
	
	int framewidth, frameheight, x, y, outline;
	int xscan, yscan, yscanmin, yscanmax, outlinescan, xscan_thresh = 5, xscanmin, xscanmax;
	
	// for edge detect save obstacles
	int left_obstacles[640];
	int right_obstacles[640];
	
	int drop;
	
	int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
	double fontScale = 0.5;
	int thickness = 1;   
	
for(;;) {
	
	// drop frames to get real time frames, was 10
	for (drop = 5; drop > 1; drop--)
	{
		cap_left >> cam_left_out;
		cap_right >> cam_right_out;
	}
	
	
	// find 3D points ---------------------------------------------------------
	
	//-- Step 1: Detect the keypoints using SURF Detector
  int minHessian = 400;

  SurfFeatureDetector detector( minHessian );

  std::vector<KeyPoint> keypoints_left, keypoints_right;

  detector.detect( cam_left_out, keypoints_left );
  detector.detect( cam_right_out, keypoints_right );

  //-- Step 2: Calculate descriptors (feature vectors)
  SurfDescriptorExtractor extractor;

  Mat descriptors_left, descriptors_right;

  extractor.compute( cam_left_out, keypoints_left, descriptors_left );
  extractor.compute( cam_right_out, keypoints_right, descriptors_right );
		
if ( descriptors_left.empty() )
{	
   // cvError(0,"MatchFinder","1st descriptor empty",__FILE__,__LINE__);
   continue;
}   
    
if ( descriptors_right.empty() )
{
   // cvError(0,"MatchFinder","2nd descriptor empty",__FILE__,__LINE__);
   continue;
}

		//-- Step 3: Matching descriptor vectors using FLANN matcher
  FlannBasedMatcher matcher;
  std::vector< DMatch > matches;
  matcher.match( descriptors_left, descriptors_right, matches );
	// min_dist = 100
  double max_dist = 0; double min_dist = 100;

  //-- Quick calculation of max and min distances between keypoints
  for( int i = 0; i < descriptors_left.rows; i++ )
  { double dist = matches[i].distance;
    if( dist < min_dist ) min_dist = dist;
    if( dist > max_dist ) max_dist = dist;
  }

  printf("-- Max dist : %f \n", max_dist );
  printf("-- Min dist : %f \n", min_dist );

  //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
  //-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
  //-- small)
  //-- PS.- radiusMatch can also be used here.
  std::vector< DMatch > good_matches;

	for( int i = 0; i < descriptors_left.rows; i++ )
	{ 
		if( matches[i].distance <= max(2.2 * min_dist, 0.25) )  // 2 * min_dist, 0.25
		{ 
			Point2f point1 = keypoints_left[matches[i].queryIdx].pt;
			Point2f point2 = keypoints_right[matches[i].trainIdx].pt;
		  
			int y1, y2, x1, x2, ydiff, distance;
			y1 = (int) point1.y;
			y2 = (int) point2.y;
			
			x1 = (int) point1.x;
			x2 = (int) point2.x;

			distance = x2 - x1;

			if (y1 > y2)
			{
				ydiff = y1 - y2;
			}
			else 
			{
				ydiff = y2 - y1;
			}
			
			if (ydiff < 10)
			{
				// points match on about same height, avoid diagonal matches!!
				good_matches.push_back( matches[i]);
			}
		}
	}

  //-- Draw only "good" matches
  Mat img_matches;
  drawMatches( cam_left_out, keypoints_left, cam_right_out, keypoints_right,
               good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

  if (good_matches.size () < 10)
  {
	  
	// convert frames to grey, for edge detection
	cvtColor(cam_left_out, cam_left_grey, CV_BGR2GRAY);
	// Perform canny edge detection
	Canny(cam_left_grey, cam_left_grey, 10, 100, 3 );
	
	cvtColor(cam_right_out, cam_right_grey, CV_BGR2GRAY);
	// Perform canny edge detection
	Canny(cam_right_grey, cam_right_grey, 10, 100, 3 );
	
	// scan left frame from bottom to top for obstacles
	
	cv::Size s = cam_left_grey.size();
	frameheight = s.height;
	framewidth = s.width;
	
	for (x = 0; x < framewidth; x += 8)
	{
		left_obstacles[x] = 0;
	}
	
	for (x = 0; x < framewidth; x += 8)
	{
		outline = 0;
		for (y = 320; y >= 40; y--)	// 320 = horizon line -x
		{
			if (cam_left_grey.at<char>(y,x) == 255)
			{
				outline++;
				if (outline == 1) yscanmax = 40;
				if (left_obstacles[x] == 0 && outline == 2) 
				{	
					// check left and right edges
					yscanmin = y;
					outlinescan = 0;
					for (yscan = yscanmin; yscan >= yscanmax; yscan--)
					{
						xscanmin = x - xscan_thresh;
						if (xscanmin < 0) xscanmin = 0;
						
						xscanmax = x + xscan_thresh;
						if (xscanmax > framewidth) xscanmax = framewidth - 1;
						
						for (xscan = xscanmin; xscan < xscanmax; xscan++)
						{
							if (cam_left_grey.at<char>(y,x) == 255)
							{
								outlinescan++;
								if (outlinescan == 50)
								{
									left_obstacles[x] = y; // found outline pixel, store as obstacle
									break;
								}
							}
							else
							{
								left_obstacles[x] = 0;
								break;
							}
						}
					}
				}
			}
		}
	}
	
	
	for (x = 0; x < framewidth; x += 8)
	{
		right_obstacles[x] = 0;
	}
	
	for (x = 0; x < framewidth; x += 8)
	{
		outline = 0;
		for (y = 320; y >= 40; y--)
		{
			if (cam_right_grey.at<char>(y,x) == 255)
			{
				outline++;
				if (outline == 1) yscanmax = 40;
				if (right_obstacles[x] == 0 && outline == 2) 
				{	
					// check left and right edges
					yscanmin = y;
					outlinescan = 0;
					for (yscan = yscanmin; yscan >= yscanmax; yscan--)
					{
						xscanmin = x - xscan_thresh;
						if (xscanmin < 0) xscanmin = 0;
						
						xscanmax = x + xscan_thresh;
						if (xscanmax > framewidth) xscanmax = framewidth - 1;
						
						for (xscan = xscanmin; xscan < xscanmax; xscan++)
						{
							if (cam_right_grey.at<char>(y,x) == 255)
							{
								outlinescan++;
								if (outlinescan == 50)
								{
									right_obstacles[x] = y; // found outline pixel, store as obstacle
								}
							}
							else
							{
								right_obstacles[x] = 0;
								break;
							}
						}
					}
				}
			}
		}
	}
	
	
	
	// check left edge detection and convert to "3D points"
	for (x = 0; x < framewidth; x += 8)
	{
		int position_real;
		
		if (left_obstacles[x] > 0)
		{
			if (x > 319)
			{
				// object is ahead
				
				position_real = 0;
			}
			else
			{
				position_real = -1;
			}
			
			points_3d[max3dpoints].x = position_real;
			points_3d[max3dpoints].y = left_obstacles[x];
			points_3d[max3dpoints].distance = 500;	// object distance in millimeters, always 500 for not measurable points
	
			max3dpoints++;
			
			if (max3dpoints == MAX3DPOINTS)
			{	
				goto endleftedge;
			}
		}
	}
endleftedge:
	max3dpoints--;
	
	if (max3dpoints == MAX3DPOINTS - 1) goto return_points;
	if (max3dpoints < 0)
	{
		max3dpoints = 0;
		goto return_points;
	}
	// check right edge detection and convert to "3D points"
	for (x = 0; x < framewidth; x += 8)
	{
		int position_real;
		
		if (right_obstacles[x] > 0)
		{
			if (x < 319)
			{
				// object is ahead
				
				position_real = 0;
			}
			else
			{
				position_real = 1;
			}
			
			points_3d[max3dpoints].x = position_real;
			points_3d[max3dpoints].y = right_obstacles[x];
			points_3d[max3dpoints].distance = 500;	// object distance in millimeters, always 500 for not measurable points
	
			max3dpoints++;
			
			if (max3dpoints == MAX3DPOINTS)
			{	
				goto endrightedge;
			}
		}
	}
endrightedge:
	max3dpoints--;
  }
  
  
	// get 3dpoints data
	max3dpoints = 0;
	for (size_t i = 0; i < good_matches.size (); i++)
	{
		Point2f point1 = keypoints_left[good_matches[i].queryIdx].pt;
		Point2f point2 = keypoints_right[good_matches[i].trainIdx].pt;
		// do something with the best points...
	
		int x1, y1, x2, y2;
		
		int distance, distance_real;
		int position_real;
			
		x1 = (int) point1.x; y1 = (int) point1.y;
		x2 = (int) point2.x; y2 = (int) point2.y;
	
		// check if edge point is close
		for (x = 0; x < framewidth; x += 8)
		{
			if ((x1 >= x - 16) && (x1 <= x + 16))
			{
				// x coordinate in range of edge x
				
				// if ((y1 >= left_obstacles[x] - 20) && (y1 <= left_obstacles[x] + 20))
				
					// filter out!!
					
				left_obstacles[x] = 0;
				break;
			}
		}
		
		// check if edge point is close
		for (x = 0; x < framewidth; x += 8)
		{
			if ((x2 >= x - 16) && (x2 <= x + 16))
			{
				// x coordinate in range of edge x
				
					// filter out!!
					
				right_obstacles[x] = 0;
				break;
			}
		}
		
		
		
			// get object distance for near objects ( <= 250 mm)
			// all objects farer away get -1
			distance = (x2 - x1) * -1;
		
			if (distance > 510) distance_real = 50;
			if (distance >= 280 && distance < 510) distance_real = 100;
			if (distance >= 200 && distance < 280) distance_real = 150;
			if (distance >= 145 && distance < 200) distance_real = 200;
			if (distance >= 110 && distance < 145) distance_real = 250;
			if (distance < 110) distance_real = -1;
		
		
		if (x1 > 319 && x2 < 270)
		{
		    // object is ahead
		    position_real = 0;
		}
		if (x1 < 319 && x2 < 270)
		{
		  // object is on the left
		  position_real = -1;
		}
		if (x1 > 319 && x2 > 270)
		{
		  // object is on the right
		  position_real = 1;
		}
		
		if (distance_real != -1)
		{
			points_3d[max3dpoints].x = position_real;
			points_3d[max3dpoints].y = y1;
			points_3d[max3dpoints].distance = distance_real;	// object distance in millimeters
		
			max3dpoints++;
			if (max3dpoints == MAX3DPOINTS)
			{
				goto end3dpoints;
			}
		}
	}
end3dpoints:
	printf ("END3DPOINTS: max3dpoints: %lli\n", max3dpoints);
	max3dpoints--;
	
	if (max3dpoints == MAX3DPOINTS - 1) goto return_points;
	if (max3dpoints < 0)
	{
		max3dpoints = 0;
		goto return_points;
	}
	
return_points:	
	// sort 3d points
	
	/*
	int sorted = 0;
	S8 savex, savey, savedistance;
	
	while (sorted == 0)
	{
		int j; int modified = 0;
		for (size_t i = 0; i < max3dpoints; i++)
		{
			j = i + 1;
			if (points_3d[j].distance < points_3d[i].distance)
			{
				// swap
				savex = points_3d[i].x;
				savey = points_3d[i].y;
				savedistance = points_3d[i].distance;
	
				points_3d[i].x = points_3d[j].x;
				points_3d[i].y = points_3d[j].y;
				points_3d[i].distance = points_3d[j].distance;
				
				points_3d[j].x = savex;
				points_3d[j].y = savey;
				points_3d[j].distance = savedistance;
				
				modified = 1;
			}
		}
		if (modified == 0) sorted = 1;
	}
	*/
	
	printf ("max3dpoints: %lli\n", max3dpoints);
	
	int qvar_i = 0;
	
	var_lock (varlist, points_index);
	if (! let_2array_qint (varlist, vm_mem, max3dpoints, points_index, 0))
	{
		printf ("robotvision: points_index overflow!\n");
		return (1);
	}
	var_unlock (varlist, points_index);
	
	var_lock (varlist, qvar_index);
	for (size_t i = 0; i <= max3dpoints; i++)
	{
		if (! let_2array_qint (varlist, vm_mem, points_3d[i].x, qvar_index, qvar_i))
		{
			printf ("robotvision: [%i] return value array overflow: x!\n", qvar_i);
			return (1);
		}
		qvar_i++;
		if (! let_2array_qint (varlist, vm_mem, points_3d[i].y, qvar_index, qvar_i))
		{
			printf ("robotvision: [%i] return value array overflow: y!\n", qvar_i);
			return (1);
		}
		qvar_i++;
		if (! let_2array_qint (varlist, vm_mem, points_3d[i].distance, qvar_index, qvar_i))
		{
			printf ("robotvision: [%i] return value array overflow: distance!\n", qvar_i);
			return (1);
		}
		qvar_i++;
	}
	var_unlock (varlist, qvar_index);
		
#if DEBUG
	// debug print sorted 3d points
	for (size_t i = 0; i <= max3dpoints; i++)
	{
		if (points_3d[i].distance)
		{
			printf ("3d points sorted: %i: x: %lli, y: %lli, distance: %lli\n", i, points_3d[i].x, points_3d[i].y, points_3d[i].distance);
		}
	}
#endif
  
	// display matches in window
	goto showoutput;
	
	for(size_t i = 0; i < good_matches.size(); i++)
	{
		Point2f point1 = keypoints_left[good_matches[i].queryIdx].pt;
		Point2f point2 = keypoints_right[good_matches[i].trainIdx].pt;
		// do something with the best points...
		
		int x1, y1, x2, y2, distance;
		int distance_real;
		
		x1 = (int) point1.x; y1 = (int) point1.y;
		x2 = (int) point2.x; y2 = (int) point2.y;
		distance = (x2 - x1) * -1;
		
		if (distance > 510) distance_real = 50;
		if (distance >= 280 && distance < 510) distance_real = 100;
		if (distance >= 200 && distance < 280) distance_real = 150;
		if (distance >= 145 && distance < 200) distance_real = 200;
		if (distance >= 110 && distance < 145) distance_real = 250;
		if (distance < 110) distance_real = -1; 
		
		// show distances value
		std::ostringstream ss;
		ss << good_matches[i].distance;
		std::string text(ss.str());
		
#if DEBUG
		printf ("point %i: distance %f, x left: %i, x right: %i, distance (px): %i\n", i, good_matches[i].distance, x1, x2, x2 - x1);
#endif
		
		putText(img_matches, text, cvPoint (x1 - 40, y1), fontFace, fontScale, Scalar::all(255), thickness, 1);
		
		// show real distance
		std::ostringstream ss2;
		ss2 << distance_real;
		std::string text2(ss2.str());
		
		putText(img_matches, text2, cvPoint (x1 - 40, y1 + 15), fontFace, fontScale, Scalar::all(255), thickness, 1);
	}
	
	// draw detected obstacles
	/*
	for (x = 0; x < framewidth; x += 8)
	{
		if (left_obstacles[x] > 0)
		{
			cv::line(cam_left_out, cv::Point2i(x, frameheight), cv::Point2i(x, left_obstacles[x]), cv::Scalar(0, 255, 0), 1, 1);
		}
	}
	*/
	
showoutput:
  // for( int i = 0; i < (int)good_matches.size(); i++ )
  // { printf( "-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx ); }
  
         // imshow("edge_detection_left", cam_left_out);
	//	imshow("edge_detection_right", cam_right_out);
		 // imshow("matches", img_matches);
		 imshow("left", cam_left_out);
		 imshow("right", cam_right_out);
        // And display it:
        // Exit this loop on escape:
	char key = (char) waitKey(2);
        if(key == 27)
            break;
	
		
	// opencv_detect_faces (cam_left_out, cam_right_out);	
		
	// save pictures, aka blackbox
	
	if (save_skip == 0)
	{
	char picture_matches[256] = "/home/pi/nanovm/home/robot/match/match-";
	char picture_left[256] = "/home/pi/nanovm/home/robot/left/left-";
	char picture_right[256] ="/home/pi/nanovm/home/robot/right/right-";
	char zeroes[5];
	char num[256];
	char time[256];

	format_time (time);
	strcat (picture_matches, time);
	strcat (picture_left, time);
	strcat (picture_right, time);
	
	strcat (picture_matches, "-");
	strcat (picture_left, "-");
	strcat (picture_right, "-");
	
	sprintf (num, "%d", picture_num);
	numlen = strlen (num);
	if (numlen < picture_nummax)
	{
		for (zero_fill = picture_nummax - numlen; zero_fill > 0; zero_fill--)
		{
			strcat (picture_matches, "0");
			strcat (picture_left, "0");
			strcat (picture_right, "0");
		}
	}
	
	strcat (picture_matches, num);
	strcat (picture_matches, ".jpg");
		
	strcat (picture_left, num);
	strcat (picture_left, ".jpg");
	
	strcat (picture_right, num);
	strcat (picture_right, ".jpg");
		
	// imwrite (picture_matches, img_matches);
	imwrite (picture_left, cam_left_out);
	imwrite (picture_right, cam_right_out);

	picture_num++;
	save_skip = 3;
	}

	save_skip--;
    }
       
    return 0;
}

