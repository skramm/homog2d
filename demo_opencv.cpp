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

homog2d::Point2d g_p00( g_width/2,   g_height/4 );
homog2d::Point2d g_p01( g_width/4,   g_height*3/4 );
homog2d::Point2d g_p10( g_width*3/4, g_height/4 );
homog2d::Point2d g_p11( g_width/2,   g_height*3/4 );

homog2d::Point2d g_pt_mouse;

void Draw()
{
	g_img = cv::Scalar(255,255,255);

// line from origin to mouse pos
	cv::line( g_img, cv::Point2d(), g_pt_mouse.getCvPtd(), cv::Scalar(0,50,200), 2 );

	homog2d::Line2d lA( g_p00, g_p01 );
	std::cout << "g_p00=" <<  g_p00 << " g_p01=" << g_p01 << '\n';
	lA.drawCvMat( g_img, cv::Scalar(200,50,20), 2 );
	cv::imshow( g_wndname, g_img );
}

/// Mouse callback
void mouse_CB( int event, int x, int y, int flags, void* param )
{
	static int c;
	std::cout << "count:" << c++ << '\n';

	Draw();

	g_pt_mouse.set( x, y );
/*
	homog2d::Point2d pt0( g_width/2, g_height/2 );
	homog2d::Line2d lA( pt1, pt0 );
	cv::line( g_img, pt0.getCvPtd(), pt1.getCvPtd(), cv::Scalar(0,50,200), 2 );

	homog2d::Line2d lB( 2, 1 ); // 45° line
	lB.drawCvMat( g_img, cv::Scalar(200,50,20), 2 );

	homog2d::Point2d pt3( lA, lB ); // intersection of lines
	homog2d::Point2d pt4(g_width, g_height);

	cv::line( g_img, pt3.getCvPtd(), pt4.getCvPtd(), cv::Scalar(0,250,50), 2 );


	cv::imshow( g_wndname, g_img );*/
}

int main()
{
	int w = 600;
	int h = 400;
	cv::namedWindow( g_wndname );
	cv::setMouseCallback( g_wndname, mouse_CB );

	g_img.create( g_width, g_height, CV_8UC3 );
	Draw();
	char key = cv::waitKey(0);
}
;
