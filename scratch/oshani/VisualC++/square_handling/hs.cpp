/*Test code for the standard count*/

#include <cv.h>
#include <highgui.h>
#include <math.h>
#include <iostream>
#include <ostream>
#include <algorithm>
#include <vector>

//The maximum number of pixels between the double lines that demarcate bigger
//counting squares
#define MAX_PIXELS 15		

//Default number of small squares in a big square
#define DEFAULT_SQUARES 4		

using namespace std;

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

    /*Gets all the lines*/
	lines = cvHoughLines2( dst, storage, CV_HOUGH_STANDARD, 1, CV_PI/180. * 2, 200);

	/*An estimate for the positions in either the horizontal or vertical directions*/
	int positions = (int)((lines->total)/2) + 5;
	
	/*Arrays to hold the positions of the horizontal and vertical lines detected*/
	int* vertical_pos = new int[positions];
	int* horizontal_pos = new int[positions];

	int counter_x = 0;
	int counter_y = 0;
	

	/*Iterate over the lines*/
    for( i = 0; i < lines->total; i++ )
    {
        float* line = (float*)cvGetSeqElem(lines,i);
        float rho = line[0];
        float theta = line[1];
		//std::cout <<  i << "\t Rho=" << rho << "\t Theta=" << theta <<std::endl;
        CvPoint pt1, pt2;
        double a = cos(theta), b = sin(theta);

		/*For vertical lines*/
		if( fabs(a) < 0.001 )
        {
			pt1.x = pt2.x = vertical_pos[counter_x++] = cvRound(rho);
            pt1.y = 0;
            pt2.y = color_dst->height;
			
        }
		/*For horizontal lines*/
        else if( fabs(b) < 0.001 )
        {
            pt1.y = pt2.y = horizontal_pos[counter_y++] = cvRound(rho);
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
		/*Draw all the lines detected in RED*/
        cvLine( color_dst, pt1, pt2, CV_RGB(255,0,0) );
    }

	/*Heap Sort the 2 arrays*/

	vector<int> vertical_pos_vector (vertical_pos, vertical_pos+counter_x);               
	make_heap (vertical_pos_vector.begin(),vertical_pos_vector.end());
	sort_heap (vertical_pos_vector.begin(), vertical_pos_vector.end());
	
	vector<int> horizontal_pos_vector (horizontal_pos, horizontal_pos+counter_y);               
	make_heap (horizontal_pos_vector.begin(),horizontal_pos_vector.end());
	sort_heap (horizontal_pos_vector.begin(), horizontal_pos_vector.end());
	
	//To count how many small squares that we have met so far
	int square_counter = 0; 
	
	/*If it is a double line demarcating a bigger square*/

	/*For vertical lines in the bigger squares*/
	
	//FIXME Hack used! - vertical lines will start drawing from the first 
	//encountered line, and not from the actual demarcation for the big square
	//May need some sort of heuristics here!
	
	for (i=0; i<vertical_pos_vector.size(); i++) 
	{
		CvPoint pt1, pt2;
		int line_diff = vertical_pos_vector[i+1] - vertical_pos_vector[i];
		
//		printf("%d : v_pos = %d\tsquare_counter = %d\n",i,vertical_pos_vector[i],square_counter);
		
		if ((square_counter==0 && line_diff > MAX_PIXELS) ||
			i==0 || 
			i == vertical_pos_vector.size()-1)
		{
			pt1.x = pt2.x = vertical_pos_vector[i];
			pt1.y = 0;
			pt2.y = color_dst->height;
			square_counter = (square_counter+1) % DEFAULT_SQUARES;					
		}
		else if (line_diff > MAX_PIXELS)
		{
			square_counter = (square_counter+1) % DEFAULT_SQUARES;					
		}	
		
		
		//Draw a thick vertical line in GREEN
		cvLine( color_dst, pt1, pt2, CV_RGB(0,255,0), 3 );	
	}
	

	/*Detect and draw the horizontal lines*/
		
	for (i=0; i<horizontal_pos_vector.size(); i++) 
	{
		CvPoint pt1, pt2;
		int line_diff = horizontal_pos_vector[i+1] - horizontal_pos_vector[i];
		if (line_diff < MAX_PIXELS && square_counter++ == 0)
		{
			pt1.y = pt2.y = (int)((horizontal_pos_vector[i] + horizontal_pos_vector[i])/2);
			pt1.x = 0;
			pt2.x = color_dst->width;
			
		}
		else if (line_diff < MAX_PIXELS && square_counter > 0)
		{
			square_counter = (square_counter-1)%(DEFAULT_SQUARES+1);
		}
		else
		{
			square_counter = (square_counter+1)%(DEFAULT_SQUARES+1);
		}
		
		//Draw a thick horizontal line in GREEN
		cvLine( color_dst, pt1, pt2, CV_RGB(0,255,0), 3 );	
	}
	

	cvNamedWindow( "Source", 1 );
    cvShowImage( "Source", src );
	
    cvNamedWindow( "Squares Identified", 1 );
    cvShowImage( "Squares Identified", color_dst );
	
    cvWaitKey(0);
    
	return 0;
}