// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

int main()
{
	img::Image<img::SvgImage> im( 550,250 );

	float radius = 100;
	Point2d p1(110,radius+20);
	Point2d p2(360,radius+20 );
	p1.draw( im );
	p2.draw( im );
	CPolyline pol1, pol2;
	auto values1 = pol1.set( radius, 5 );
	auto values2 = pol2.set( radius, 7 );
	pol1.translate( p1.getX(), p1.getY() );
	pol2.translate( p2.getX(), p2.getY() );

	Circle c1( p1, values1.second );
	Circle c2( p2, values2.second );

	c1.draw( im, img::DrawParams().setColor(50,150,50) );
	c2.draw( im, img::DrawParams().setColor(50,150,50) );

	pol1.draw( im, img::DrawParams().setColor(250,0,0) );
	pol2.draw( im, img::DrawParams().setColor(250,0,0) );

	draw( im, pol1.getPts().front(), img::DrawParams().setPointStyle( img::PtStyle::Dot ).setColor(0,0,200) );
	draw( im, pol2.getPts().front(), img::DrawParams().setPointStyle( img::PtStyle::Dot ).setColor(0,0,200) );

	{
		Segment s1( p1, pol1.getPts().front() );
		Segment s2( p2, pol2.getPts().front() );
		s1.draw( im ) ;
		s2.draw( im ) ;

		drawText( im, std::to_string(int(radius)), s1.getCenter() );
		drawText( im, std::to_string(int(radius)), s2.getCenter() );
	}
	{
		auto s1 = pol1.getSegs().at(0);
		auto s2 = pol2.getSegs().at(0);
		auto spara1 = s1.getParallelSegs(25).second;
		auto spara2 = s2.getParallelSegs(25).second;
		spara1.draw( im );
		spara2.draw( im );

		std::ostringstream oss1, oss2;
		oss1 << std::fixed << std::setprecision(1) << values1.first;
		oss2 << std::fixed << std::setprecision(1) << values2.first;
		drawText( im, oss1.str(), spara1.getCenter() );
		drawText( im, oss2.str(), spara2.getCenter() );
	}
	{
		Segment s1( p1, pol1.getPts().at(3) );
		Segment s2( p2, pol2.getPts().at(3) );

		auto it1 = c1.intersects(s1);
		auto it2 = c2.intersects(s2);

		Segment ss1( p1, it1.get()[0] );
		Segment ss2( p2, it2.get()[0] );
		ss1.draw( im ) ;
		ss2.draw( im ) ;

		std::ostringstream oss1, oss2;
		oss1 << std::fixed << std::setprecision(1) << values1.second;
		oss2 << std::fixed << std::setprecision(1) << values2.second;
		drawText( im, oss1.str(), ss1.getCenter() );
		drawText( im, oss2.str(), ss2.getCenter() );
	}
	im.write( "polyline_rcp_1.svg" );
}
