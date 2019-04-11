/**
\file demo_opencv.cpp
\brief demo of interfacing with Opencv (PRELIMINAR)
*/

#define HOMOG2D_USE_OPENCV
#include "homog2d.hpp"

// additional Opencv header, needed for GUI stuff
#include "opencv2/highgui.hpp"

using namespace homog2d;

std::string g_wndname = "homog2d demo";
cv::Mat g_img;
int g_width = 600;
int g_height = 500;

void mouse_CB( int event, int x, int y, int flags, void* param )
{
	static int c;
	g_img = cv::Scalar(255,255,255);
	homog2d::Point2d pt1( x, y );
	homog2d::Point2d pt0( g_width/2, g_height/2 );
	homog2d::Line2d lA( pt1, pt0 );
	cv::line( g_img, pt0.getCvPt_i(), pt1.getCvPt_i(), cv::Scalar(0,50,200), 2 );

	homog2d::Line2d lB( 1, 1 ); // 45° line
	homog2d::Point2d pt3( lA, lB ); // intersection of lines
	homog2d::Point2d pt4(g_width, g_height );

	cv::line( g_img, pt3.getCvPt_i(), pt4.getCvPt_i(), cv::Scalar(0,250,50), 2 );


	cv::imshow( g_wndname, g_img );
}

int main()
{
	int w = 600;
	int h = 400;
	cv::namedWindow( g_wndname );
	cv::setMouseCallback( g_wndname, mouse_CB );

	g_img.create( g_width, g_height, CV_8UC3 );

	char key = cv::waitKey(0);
}
;
