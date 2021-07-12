#define HOMOG2D_USE_OPENCV

#include "homog2d.hpp"
#include "opencv2/highgui.hpp"

using namespace h2d;
int main()
{
	FRect r1( 50,50, 200,200 );
	FRect r2( 200,50, 300,200 );
	cv::Mat img;
	img.create( 250, 450, CV_8UC3 );
	img = cv::Scalar(255,255,255);
	r1.draw( img, CvDrawParams().setColor(250,0,20) );
	r2.draw( img, CvDrawParams().setColor(20,0,250) );
	cv::imwrite( "frect2.png", img );
}
