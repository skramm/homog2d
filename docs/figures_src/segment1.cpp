#define HOMOG2D_USE_OPENCV

#include "homog2d.hpp"
#include "opencv2/highgui.hpp"

using namespace h2d;
int main()
{
	Segment s1(50,50, 250,50 );
	Segment s2(150,150, 150,50 );

	cv::Mat img;
	img.create( 200, 350, CV_8UC3 );
	img = cv::Scalar(255,255,255);
	img::Image<cv::Mat> img2( img );

	s1.draw( img2, DrawParams().setColor(250,0,20) );
	s2.draw( img2, DrawParams().setColor(20,0,250) );
	cv::imwrite( "segment1.png", img );
}
