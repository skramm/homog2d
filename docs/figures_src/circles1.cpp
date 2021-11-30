// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

int main()
{
	Circle c1( Point2d(120,100), 80 );
	Circle c2( Point2d(300,200), 60 );
	auto seg = getSegment( c1, c2 );
	auto pseg = getTanSegs( c1, c2 );

	cv::Mat img;
	img.create( 300, 450, CV_8UC3 );
	img = cv::Scalar(255,255,255);

	img::Image<cv::Mat> img2( img );

	c1.draw( img2, DrawParams().setColor(0,0,100) );
	c2.draw( img2, DrawParams().setColor(0,0,100) );
	seg.draw( img2, DrawParams().setColor(250,0,0) );
	pseg.first.draw( img2,  DrawParams().setColor(0,200,100) );
	pseg.second.draw( img2, DrawParams().setColor(0,100,200) );
	cv::imwrite( "circles1.png", img );
}
