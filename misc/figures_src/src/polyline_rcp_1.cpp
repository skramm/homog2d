// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

int main()
{
	img::Image<img::SvgImage> im( 450,200 );

	Point2d p1(100,100);
	Point2d p2(280,100 );
	p1.draw( im );
	p2.draw( im );
	CPolyline pol1, pol2;
	auto values1 = pol1.set(80,5);
	auto values2 = pol2.set(100,7);
	pol1.moveTo( p1 );
	pol2.moveTo( p2 );
	Circle c1( p1, values1.second );
	Circle c2( p2, values2.second );

	c1.draw( im, img::DrawParams().setColor(50,150,50) );
	c2.draw( im, img::DrawParams().setColor(50,150,50) );

	pol1.draw( im, img::DrawParams().setColor(250,0,0) );
	pol2.draw( im, img::DrawParams().setColor(250,0,0) );

	im.write( "polyline_rcp_1.svg" );
}
