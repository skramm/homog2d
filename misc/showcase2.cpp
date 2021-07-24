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
	int n=20;
	cv::Mat cvmat;
	auto im_w = 350;
	auto im_h = 180;
	cvmat.create( im_h, im_w, CV_8UC3 );
	img::Image<cv::Mat> img( cvmat );

	FRect r1(40,30, 130,90 );
	FRect r2(160,45, 210,130 );

	auto color_red = DrawParams().setColor(200,20,20);
	auto color_green = DrawParams().setColor(20,220,20);
	int mul = 1;
	for( int i=0; i<n; i++ )
	{
		cvmat = cv::Scalar(255,255,255);
		r1.draw( img, color_green );
		r2.draw( img, color_green );

		auto a = r1.intersection(r2);
		if( a() )
			a.get().draw( img, color_red );
		if( (i+1)%10 == 0 )
		{
			mul = -1;
			r1.translate(0,40);
		}
		std::ostringstream oss;
		oss << "BUILD/showcase2_" << std::setfill('0') << std::setw(2) <<i << ".png";
		cv::imwrite( oss.str(), cvmat );

		r1.translate( mul*20,0);
	}

}


