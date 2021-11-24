// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target test_fig

#include "fig_src.header"

int main()
{
	FRect r1( 50,50, 250,200 );
	FRect r2( 150,50, 300,200 );
	cv::Mat img;
	img.create( 250, 450, CV_8UC3 );
	img = cv::Scalar(255,255,255);
	img::Image<cv::Mat> img2( img );

	r1.draw( img2, DrawParams().setColor(250,0,20) );
	r2.draw( img2, DrawParams().setColor(20,0,250) );
	cv::imwrite( "frect1.png", img );
}
