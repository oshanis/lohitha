/* This is a standalone program. Pass an image name as a first parameter of the program.
Switch between standard and probabilistic Hough transform by changing "#if 1" to "#if 0" and back */

#include <cv.h>
#include <highgui.h>
#include <math.h>
#include <iostream>
#include <ostream>

int main()
{
	IplImage* src;

	char* name = "std_count.jpg";
    src = cvLoadImage(name, 0);
    
    IplImage* dst = cvCreateImage( cvGetSize(src), 8, 1 );
    IplImage* color_dst = cvCreateImage( cvGetSize(src), 8, 3 );
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* lines = 0;
    CvSeq* lines2 = 0;

    int i;
    cvCanny( src, dst, 50, 200, 3 );
    cvCvtColor( dst, color_dst, CV_GRAY2BGR );

    //Gets all the lines
	lines = cvHoughLines2( dst, storage, CV_HOUGH_STANDARD, 1, CV_PI/180. * 2, 200);
	
    for( i = 0; i < lines->total; i++ )
    {
        float* line = (float*)cvGetSeqElem(lines,i);
        float rho = line[0];
        float theta = line[1];
		std::cout <<  i << "\t Rho=" << rho << "\t Theta=" << theta <<std::endl;
        CvPoint pt1, pt2;
        double a = cos(theta), b = sin(theta);

		/*For vertical lines*/
		if( fabs(a) < 0.001 )
        {
			pt1.x = pt2.x = cvRound(rho);
            pt1.y = 0;
            pt2.y = color_dst->height;
			
        }
		/*For horizontal lines*/
        else if( fabs(b) < 0.001 )
        {
            pt1.y = pt2.y = cvRound(rho);
            pt1.x = 0;
            pt2.x = color_dst->width;
        }
        /* I have commented this out, because, otherwise we get diagonal lines
			which are not necessarily the line demarcations in the counting chamber
			- it will not be a problem since we will be compensating for rotation*/
/*		else
        {
            pt1.x = 0;
            pt1.y = cvRound(rho/b);
            pt2.x = cvRound(rho/a);
            pt2.y = 0;
        }
*/
        cvLine( color_dst, pt1, pt2, CV_RGB(255,0,0) );
    }
    cvNamedWindow( "Source", 1 );
    cvShowImage( "Source", src );
	
    cvNamedWindow( "Hough", 1 );
    cvShowImage( "Hough", color_dst );
	
    cvWaitKey(0);
    
	return 0;
}