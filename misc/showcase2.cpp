/**
\file showcase1.cpp
\brief Generates showcase images
*/
#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "../homog2d.hpp"

#include "opencv2/highgui.hpp"

using namespace h2d;

int main( int argc, const char** argv )
{
	int n=10;
	cv::Mat img;
	auto im_w = 350;
	auto im_h = 180;
	img.create( im_h, im_w, CV_8UC3 );

	FRect r1(40,30, 130,90 );
	FRect r2(160,45, 210,125 );

	auto color_red = CvDrawParams().setColor(200,20,20);
	auto color_green = CvDrawParams().setColor(20,220,20);
	for( int i=0; i<n; i++ )
	{
		img = cv::Scalar(255,255,255);
		r1.draw( img, color_green );
		r2.draw( img, color_green );

		auto a = r1.intersection(r2);
		if( a() )
			a.get().draw( img, color_red );

		std::ostringstream oss;
		oss << "BUILD/showcase2_" << std::setfill('0') << std::setw(2) <<i << ".png";
		cv::imwrite( oss.str(), img );

		r1.translate( 20,0);
	}

}


