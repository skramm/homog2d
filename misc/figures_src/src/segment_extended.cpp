// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

int main()
{
//	img::Image<cv::Mat>  img( 400,200 );
	img::Image<SvgImage> im( 400,200 );

	Point2d p1(130,120);
	Point2d p2(210,90);
	Segment s1(p1,p2 );
	s1.getLine().draw( im, DrawParams().setColor(200,200,200) );
	auto segext = s1.getExtended();
	segext.draw( im, DrawParams().setColor(250,0,20) );
	auto ppts = segext.getPts();
	draw( im, ppts, DrawParams().setColor(250,20,20) ); //.setPointStyle(PtStyle::Dot) );
	s1.draw( im, DrawParams().setColor(20,0,250).showPoints(true) );

//	im.write( "segment_extended1.png" );
	im.write( "segment_extended1.svg" );
}
