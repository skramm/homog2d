// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

int main()
{
	Segment s1(50,50, 250,50 );
	Segment s2(150,150, 150,50 );
	img::Image<cv::Mat> img2( 400,300 );

	s1.draw( img2, DrawParams().setColor(250,0,20) );
	s2.draw( img2, DrawParams().setColor(20,0,250) );

	img2.write( "segment1.png" );
}
