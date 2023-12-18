// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

int main()
{
	img::Image<img::SvgImage> im( 550,200 );

	Point2d p1(100,100);
	Point2d p2(360,100 );
	p1.draw( im );
	p2.draw( im );
	CPolyline pol1, pol2;
	auto values1 = pol1.set(80,5);
	auto values2 = pol2.set(100,7);
	pol1.translate( p1.getX(), p1.getY() );
	pol2.translate( p2.getX(), p2.getY() );

	Circle c1( p1, values1.second );
	Circle c2( p2, values2.second );

	c1.draw( im, img::DrawParams().setColor(50,150,50) );
	c2.draw( im, img::DrawParams().setColor(50,150,50) );

	pol1.draw( im, img::DrawParams().setColor(250,0,0) );
	pol2.draw( im, img::DrawParams().setColor(250,0,0) );

	auto s1 = pol1.getSegs().at(0);
	auto s2 = pol2.getSegs().at(0);

	auto spara1 = s1.getParallelSegs(25).second;
	auto spara2 = s2.getParallelSegs(25).second;
	spara1.draw( im );
	spara2.draw( im );
	std::ostringstream oss1, oss2;
	oss1 << values1.first;
	oss2 << values2.first;

	drawText( im, oss1.str(), spara1.getCenter() );
	drawText( im, oss2.str(), spara2.getCenter() );

	im.write( "polyline_rcp_1.svg" );
}
