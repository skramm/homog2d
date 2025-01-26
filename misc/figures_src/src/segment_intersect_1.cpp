// this file is part of homog2d
// used to build a figure that is included in manual
// See makefile target doc-fig
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

	img::Image<img::SvgImage> im( 350,250 );
	s1.draw( im, DrawParams().setColor(250,50,80).setThickness(2) );
//	s2.draw( img, DrawParams().setColor(50,250,80).setThickness(2) );

	auto l1 = s1.getLine();
	auto l2 = s2.getLine();

	l1.draw( im );
	l2.draw( im );

	pa1.draw( im, DrawParams().setPointStyle(img::PtStyle::Dot).setColor(250,50,80).setThickness(4) );
	pa2.draw( im, DrawParams().setPointStyle(img::PtStyle::Dot).setColor(250,50,80).setThickness(4) );
	auto inter = l1 * l2;
	inter.draw( im, DrawParams().setPointStyle(img::PtStyle::Dot).setColor(50,50,250) );

	auto dp = DrawParams().setFontSize(40);
	dp.setDefault();

	drawText( im, "a", pa1   );
	drawText( im, "b", pa2   );
	drawText( im, "c", inter );
	im.write( "segment_intersect_1.svg" );
}
