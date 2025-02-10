/**
\file
\brief offset polygon
*/

#define HOMOG2D_USE_OPENCV
#include "../../homog2d.hpp"

using namespace h2d;

int main( int, const char** )
{
	auto n = 15; // nb images
	std::vector<Point2d> vpts{
		{ -1.0,  1.0 },
		{ -1.2, -0.7 },
		{  1.9, -0.5 },
		{  1.6,  1.6 },
		{  0.5,  0.8 }
	};
	CPolyline p0( vpts );

	auto H1 = Homogr().addTranslation(7,6).addScale(15);
	auto H2 = Homogr().addRotation(0.8).addScale(60).addTranslation(280,100);
	auto pol1 = H1 * p0;
	auto pol2 = H2 * p0;

	auto delta = 5.6;
//	img::Image<img::SvgImage> im( 500, 250 );
	img::Image<img::SvgImage> im( 500, 250 );

	std::vector<CPolyline> v_poly1, v_poly2;
	v_poly1.push_back( pol1 );
	v_poly2.push_back( pol2 );

	im.draw( pol1, img::DrawParams().setColor(250,0,0) );
	im.draw( pol2, img::DrawParams().setColor(0,0,250) );
	im.write( "showcase22_00.svg" );

	for( int i=1; i<n; i++ )
	{
		im.clear();
		int c = 30;
		for( int j=0; j<(int)v_poly1.size(); j++ ) // draw previous
		{
			im.draw( v_poly1.at(j), img::DrawParams().setColor(c,0,250-c) );
			im.draw( v_poly2.at(j), img::DrawParams().setColor(250-c,c,0 ) );
			c += 10;
		}
		v_poly1.push_back( v_poly1.at(i-1).getOffsetPoly( delta ) );
		v_poly2.push_back( v_poly2.at(i-1).getOffsetPoly( -delta ) );
		im.draw( v_poly1.back(), img::DrawParams().setColor(0,250,0) );
		im.draw( v_poly2.back(), img::DrawParams().setColor(0,0,250) );
#if 0
		auto segs1= v_poly1.back().getSegs();
		auto segs2= v_poly2.back().getSegs();
		for ( const auto& seg: segs2 )
			im.draw( seg.getLine(), img::DrawParams().setColor(200,200,200) );
#endif

		getBB( pol2, v_poly1.back() ).draw( im, img::DrawParams().setColor(150,150,200) );
		std::ostringstream oss;
		oss << "showcase22_" << std::setfill('0') << std::setw(2) << i << ".svg";
		im.write( oss.str() );
	}
}

