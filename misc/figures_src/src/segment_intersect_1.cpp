// this file is part of homog2d
// used to build a figure that is included in manual
// See makefile target doc_fig
// Requires OpenCv

#include "fig_src.header"

int main()
{
	Point2d pa1( 60,40 );
	Point2d pa2( 210,120 );
	Point2d pb1( 300,140 );
	Point2d pb2( 70,200 );
	
	Segment s1( pa1, pa2 );
	Segment s2( pb1, pb2 );

	img::Image<cv::Mat> img( 350,250 );
	s1.draw( img, DrawParams().setColor(250,50,80).setThickness(2) );
//	s2.draw( img, DrawParams().setColor(50,250,80).setThickness(2) );

	auto l1 = s1.getLine();
	auto l2 = s2.getLine();

	l1.draw( img );
	l2.draw( img );

	pa1.draw( img, DrawParams().setPointStyle(img::PtStyle::Dot).setColor(250,50,80).setThickness(4) );	
	pa2.draw( img, DrawParams().setPointStyle(img::PtStyle::Dot).setColor(250,50,80).setThickness(4) );	
	auto inter = l1 * l2;
	inter.draw( img, DrawParams().setPointStyle(img::PtStyle::Dot).setColor(50,50,250) );

	cv::putText( img.getReal(), "a", pa1.getCvPti(),   0, 1.0, cv::Scalar( 20,20,20 ), 2 );
	cv::putText( img.getReal(), "b", pa2.getCvPti(),   0, 1.0, cv::Scalar( 20,20,20 ), 2 );
	cv::putText( img.getReal(), "c", inter.getCvPti(), 0, 1.0, cv::Scalar( 20,20,20 ), 2 );
	
	img.write( "segment_intersect_1.png" );
}
