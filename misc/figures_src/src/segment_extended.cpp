// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

int main()
{
	img::Image<cv::Mat> img( 400,200 );

	Point2d p1(130,120);
	Point2d p2(210,90);
	Segment s1(p1,p2 );
	s1.getLine().draw( img, DrawParams().setColor(200,200,200) );
//	p1.draw( img );
//	p2.draw( img );
	auto segext = s1.getExtended();
	segext.draw( img, DrawParams().setColor(250,0,20) );
	auto ppts = segext.getPts();
	draw( img, ppts, DrawParams().setColor(250,20,20) ); //.setPointStyle(PtStyle::Dot) );
	s1.draw( img, DrawParams().setColor(20,0,250).showPoints(true) );

//	s1.getExtended().draw( img, DrawParams().setColor(250,0,20) );
//	s1.draw( img, DrawParams().setColor(250,0,20) );

//	s2.draw( img, DrawParams().setColor(20,0,250) );

	img.write( "segment_extended1.png" );
}
