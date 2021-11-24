// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target test_fig

#include "fig_src.header"

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
