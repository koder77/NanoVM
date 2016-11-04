/*
 * Copyright (c) 2011. Philipp Wagner <bytefish[at]gmx[dot]de>.
 * Released to public domain under terms of the BSD Simplified license.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the organization nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *   See <http://www.opensource.org/licenses/bsd-license>
 */

/* This is a shared library for face detection with open cv and Nano VM.
 * I changed the original code from a tutorial.
 * 
 * Stefan Pietzonke 2015
 */


//	gcc facerec_video.cpp -o facerec_video -lopencv_video -lopencv_core -lopencv_videostab -lopencv_contrib -lopencv_highgui


#include "../../include/host.h"

#undef JIT_CPP
#define JIT_CPP  1

#include "../../include/global_d.h"

extern "C" {
#include "../../dll_shared_libs/nanovmlib.h"
}

#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace cv;
using namespace std;

extern "C" int opencv_face_detection (U1 *pthreads_ptr, U1 *varlist_ptr, U1 *vm_mem_ptr, S4 threadnum, S8 stacksize)
{
    struct pthreads *pthreads;
	struct varlist *varlist;
	struct vm_mem *vm_mem;
	
	pthreads = (struct pthreads *) pthreads_ptr;
	varlist = (struct varlist *) varlist_ptr;
	vm_mem = (struct vm_mem *) vm_mem_ptr;
	
	/* get arguments from stack */
	
	U1 haar[256];
    S8 deviceId;
	S8 qvar_index;			/* qint var, return code variable index */
	
	if (stpull_l (threadnum, pthreads, stacksize, &qvar_index) != ERR_STACK_OK) printf ("opencv facerec: ERROR on stack qvar_index!\n");
	if (stpull_l (threadnum, pthreads, stacksize, &deviceId) != ERR_STACK_OK) printf ("opencv facerec: ERROR on stack deviceId!\n");
	if (stpull_s (threadnum, pthreads, stacksize, haar) != ERR_STACK_OK) printf ("opencv facerec: ERROR on stack haar!\n");
	
	// printf ("opencv facerec: csv: %s\n", csv);
	
	string fn_haar = (const char *) haar;
	
    CascadeClassifier haar_cascade;
    haar_cascade.load(fn_haar);
    // Get a handle to the Video device:
    VideoCapture cap(deviceId);
    // Check if we can use this device at all:
    if(!cap.isOpened()) {
        cerr << "Capture Device ID " << deviceId << "cannot be opened." << endl;
        return -1;
    }
    // Holds the current frame from the Video device:
    Mat frame;
    for(;;) {
        cap >> frame;
        // Clone the current frame:
        Mat original = frame.clone();
        // Convert the current frame to grayscale:
        Mat gray;
        cvtColor(original, gray, CV_BGR2GRAY);
        // Find the faces in the frame:
        vector< Rect_<int> > faces;
        haar_cascade.detectMultiScale(gray, faces);
        // At this point you have the position of the faces in
        // faces. Now we'll get the faces, make a prediction and
        // annotate it in the video. Cool or what?
		
		if (faces.size() == 0)
		{
			/* no faces detected write zero to array variable */
			var_lock (varlist, qvar_index);
			if (! let_2array_qint (varlist, vm_mem, 0, qvar_index, 0)) printf ("opencv facedet: return value array overflow!\n");
			var_unlock (varlist, qvar_index);
		}
		
        for(int i = 0; i < faces.size(); i++) {
            // Process face by face:
            Rect face_i = faces[i];
            // Crop the face from the image. So simple with OpenCV C++:
            Mat face = gray(face_i);
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
			
			rectangle(original, face_i, CV_RGB(0, 255, 0), 1);
			// box_text = format("detected");
			// And now put it into the image:
			// putText(original, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,255,0), 2.0);
			var_lock (varlist, qvar_index);
			if (! let_2array_qint (varlist, vm_mem, 1, qvar_index, 0)) printf ("opencv facedet: return value array overflow!\n");
			var_unlock (varlist, qvar_index);
        }
        // Show the result:
        imshow("face_detection", original);
        // And display it:
        char key = (char) waitKey(20);
        // Exit this loop on escape:
        if(key == 27)
            break;
    }
    return 0;
}
