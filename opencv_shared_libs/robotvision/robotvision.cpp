/* This is a shared library for object detection with open cv and Nano VM.
 * I changed the original code from a tutorial.
 * 
 * uses two webcams for 3D points distance measure
 * 
 * Stefan Pietzonke 2016
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


#include <iostream>
#include <fstream>
#include <sstream>

using namespace cv;
using namespace std;


#define DEBUG				0

#define CAMCENTER 			319		// half cam picture width - 1
#define MAX3DPOINTS			256

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


extern "C" int opencv_detect_object (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	struct points_3d points_3d[MAX3DPOINTS];
	
	S8 max3dpoints = 0;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
	
	/* get arguments from stack */
	
    S8 deviceId;
	S8 qvar_index;			/* qint var, return code variable index */
	
	if (stpull_l (threadnum, pthreads, stacksize, &qvar_index) != ERR_STACK_OK) printf ("opencv facerec: ERROR on stack qvar_index!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &deviceId) != ERR_STACK_OK) printf ("opencv facerec: ERROR on stack deviceId!\n");
	
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
    
	Mat frame_left;
	Mat frame_right;
for(;;) {
	cap_left >> frame_left;
	cap_right >> frame_right;
        // Clone the current frame:
        Mat original_left = frame_left.clone();
		Mat original_right = frame_right.clone();
		Mat gray_left_out, gray_right_out;
		
		gray_left_out = frame_left;
		gray_right_out = frame_right;
		
		// cvtColor(original_left, gray_left_out, CV_BGR2GRAY);
		// cvtColor(original_right, gray_right_out, CV_BGR2GRAY);
		
		// GaussianBlur (gray_left_out, gray_left_out, Size (5, 5), 0, 0);
		// GaussianBlur (gray_right_out, gray_right_out, Size (5, 5), 0, 0);
		
		// Perform canny edge detection
		// Canny( gray_left_out, gray_left_out, 10, 100, 3 );
		// Canny( gray_right_out, gray_right_out, 10, 100, 3 );
	
    int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
	double fontScale = 0.5;
	int thickness = 1;   

		
	//-- Step 1: Detect the keypoints using SURF Detector
  int minHessian = 400;

  SurfFeatureDetector detector( minHessian );

  std::vector<KeyPoint> keypoints_left, keypoints_right;

  detector.detect( gray_left_out, keypoints_left );
  detector.detect( gray_right_out, keypoints_right );

  //-- Step 2: Calculate descriptors (feature vectors)
  SurfDescriptorExtractor extractor;

  Mat descriptors_left, descriptors_right;

  extractor.compute( gray_left_out, keypoints_left, descriptors_left );
  extractor.compute( gray_right_out, keypoints_right, descriptors_right );
		
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
		if( matches[i].distance <= max(2*min_dist, 0.25) )  // 0.2
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
  drawMatches( gray_left_out, keypoints_left, gray_right_out, keypoints_right,
               good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

	// get 3dpoints data
	max3dpoints = 0;
	for (size_t i = 0; i < good_matches.size(); i++)
	{
		Point2f point1 = keypoints_left[good_matches[i].queryIdx].pt;
		Point2f point2 = keypoints_right[good_matches[i].trainIdx].pt;
		// do something with the best points...
	
		int x1, y1, x2, y2;
		
		int distance, distance_real;
		int position_real;
			
		x1 = (int) point1.x; y1 = (int) point1.y;
		x2 = (int) point2.x; y2 = (int) point2.y;
	
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
		  // object is o the right
		  position_real = 1;
		}
		
		points_3d[max3dpoints].x = position_real;
		points_3d[max3dpoints].y = y1;
		points_3d[max3dpoints].distance = distance_real;	// object distance in millimeters
		
		max3dpoints++;
		if (max3dpoints == MAX3DPOINTS)
		{
			break;
		}
	}
	max3dpoints--;
	
	// sort 3d points
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
	
	int qvar_i = 0;
	var_lock (varlist, qvar_index);
	for (size_t i = 0; i <= max3dpoints; i++)
	{
		if (! let_2array_qint (varlist, vm_mem, points_3d[i].x, qvar_index, qvar_i)) printf ("robotvision: return value array overflow!\n");
		qvar_i++;
		if (! let_2array_qint (varlist, vm_mem, points_3d[i].y, qvar_index, qvar_i)) printf ("robotvision: return value array overflow!\n");
		qvar_i++;
		if (! let_2array_qint (varlist, vm_mem, points_3d[i].distance, qvar_index, qvar_i)) printf ("robotvision: return value array overflow!\n");
		qvar_i++;
	}
	
	if (max3dpoints < MAX3DPOINTS - 1)
	{
		// fill rest with zeros
		
		for (size_t i = max3dpoints + 1; i < MAX3DPOINTS; i++)
		{
			if (! let_2array_qint (varlist, vm_mem, 0, qvar_index, qvar_i)) printf ("robotvision: return value array overflow!\n");
			qvar_i++;
			if (! let_2array_qint (varlist, vm_mem, -1, qvar_index, qvar_i)) printf ("robotvision: return value array overflow!\n");
			qvar_i++;
			if (! let_2array_qint (varlist, vm_mem, 0, qvar_index, qvar_i)) printf ("robotvision: return value array overflow!\n");
			qvar_i++;
		}
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
  
  
  // for( int i = 0; i < (int)good_matches.size(); i++ )
  // { printf( "-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx ); }
  
         // imshow("edge_detection_left", gray_left_out);
	//	imshow("edge_detection_right", gray_right_out);
		imshow("matches", img_matches);
        // And display it:
        // Exit this loop on escape:
	char key = (char) waitKey(20);
        if(key == 27)
            break;


    }
       
    return 0;
}



