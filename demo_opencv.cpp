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

homog2d::Point2d g_pt[4];
homog2d::Point2d g_pt_mouse;

void Draw()
{
	g_img = cv::Scalar(255,255,255);

// line from origin to mouse pos
	cv::line( g_img, cv::Point2d(), g_pt_mouse.getCvPtd(), cv::Scalar(0,50,20), 2 );

	for( int i=0; i<4; i++ )
	{
		std::cout << "i=" << i << " pt=" << g_pt[i] << '\n';
		g_pt[i].drawCvMat( g_img, cv::Scalar(50,50,250) );
	}

	Line2d lA( g_pt[0], g_pt[2] );
	Line2d lB( g_pt[0], g_pt[3] );
	Line2d lC( g_pt[1], g_pt[2] );
	Line2d lD( g_pt[1], g_pt[3] );

	std::cout << "lC: x=0: y=" << lC.getValue(GC_X, 0 ) << ", x=600 y=" << lC.getValue(GC_X, g_width ) << '\n';
	std::cout << "lC: y=0: x=" << lC.getValue(GC_Y, 0 ) << ", y=500 x=" << lC.getValue(GC_Y, g_height ) << "\n";

	std::cout << "lD: x=0: y=" << lD.getValue(GC_X, 0 ) << ", x=600 y=" << lD.getValue(GC_X, g_width ) << '\n';
	std::cout << "lD: y=0: x=" << lD.getValue(GC_Y, 0 ) << ", y=500 x=" << lD.getValue(GC_Y, g_height ) << "\n";

//	std::cout << "g_p00=" <<  g_p00 << " g_p01=" << g_p01 << " lA=" << lA << '\n';
//	cv::line( g_img, g_p00.getCvPtd(), g_p01.getCvPtd(), cv::Scalar(10,50,20), 2 );

//	lA.drawCvMat( g_img, cv::Scalar(   50, 150,   0 ) );
//	lB.drawCvMat( g_img, cv::Scalar(   0,  150,  50 ) );
	lC.drawCvMat( g_img, cv::Scalar( 150,   0,   50 ) );
	lD.drawCvMat( g_img, cv::Scalar(   0,   50, 150 ) );

	cv::imshow( g_wndname, g_img );
}

/// Mouse callback
void mouse_CB( int event, int x, int y, int flags, void* param )
{
	static int c;
	std::cout << "count:" << c++ << '\n';
	Draw();
	g_pt_mouse.set( x, y );
}

int main()
{
	int w = 600;
	int h = 400;
	cv::namedWindow( g_wndname );
	cv::setMouseCallback( g_wndname, mouse_CB );

	int n=5;
	g_pt[0].set( g_width/2,       g_height/n );
	g_pt[1].set( g_width/2,       g_height*(n-1)/n );
	g_pt[2].set( g_width/n,       g_height/2 );
	g_pt[3].set( g_width*(n-1)/n, g_height/2 );

	g_img.create( g_height, g_width, CV_8UC3 );
	Draw();
	char key = cv::waitKey(0);
}


