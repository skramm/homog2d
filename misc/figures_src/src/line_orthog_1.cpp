// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc-fig

#include "fig_src.header"

int main()
{
	img::Image<img::SvgImage> im( 250,180 );
	Line2d li( 50,50,300,180 );

	li.draw( im, DrawParams().setColor(250,0,20) );

	Point2d pt1( 80,96 );
	Point2d pt2( 166,80 );

	auto li2 = li.getOrthogLine( pt1 );
	auto li1 = li.getOrthogLine( pt2 );

	li1.draw( im, DrawParams().setColor(20,0,250) );
	li2.draw( im, DrawParams().setColor(20,0,250) );

	pt1.draw( im, DrawParams().setColor(20,0,200).setPointStyle( img::PtStyle::Dot) );
	pt2.draw( im, DrawParams().setColor(20,0,200).setPointStyle( img::PtStyle::Dot) );

	im.write( "line_orthog_1.svg" );
}
