/**
\file
\brief Generates moving polygons and shows their intersection points
*/
#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

using namespace h2d;

int main( int, const char** )
{
	auto nbim = 20; // nb images

	std::vector<Point2d> vpts1{
		{ 0, 0 },
		{ 140,30 },
		{ 80,110},
		{ 60,50 },
		{ 20,80 }
	};
	std::vector<Point2d> vpts2{
		{ 0, 0 },
		{ 65, -30 },
		{ 78, -10 },
		{ 120,-20 },
		{ 140,30 },
		{ 80,110},
		{ 60,50 },
		{ 20,80 }
	};

	CPolyline poly1( vpts1 );
	CPolyline poly2( vpts2 );
	poly2.translate(120,80);
	poly1.translate(20,30);
	poly2.rotate( Rotate::CCW, Point2d(150, 150) );

	auto extr_col = img::DrawParams().setColor(100,250,0).setPointStyle( img::PtStyle::Dot );
	for( int i=0; i<nbim; i++ )
	{
		img::Image<img::SvgImage> im( 360, 280 );
//		poly1.draw( im, img::DrawParams().setColor(250,128,0) );
//		poly2.draw( im, img::DrawParams().setColor(250,0,128) );
		im.draw( poly1, img::DrawParams().setColor(250,128,0) );
		im.draw( poly2, img::DrawParams().setColor(250,0,128) );

		auto res = poly1.intersects(poly2);  // intersection points
		auto ptInt = res.get();
		for( const auto& pt: ptInt )
			pt.draw( im, img::DrawParams().setColor(0,250,0).setPointStyle( img::PtStyle::Dot ) );

		auto closest = getClosestPoints( poly1, poly2 );
		auto ppts = closest.getPoints();
		draw( im, ppts );
		Segment( closest.getPoints() ).draw( im, img::DrawParams().setColor(0,0,250) );
		getBB( poly1, poly2 ).draw( im, img::DrawParams().setColor(220,200,220) );
		im.drawText( "MinDist=" + std::to_string( closest.getMinDist() ), Point2d(20,20) );

		poly1.translate( 10, 7 );
		poly2.translate( -7, -7 );

		auto c2 = poly2.centroid();
		poly2 = Homogr().addTranslation( -c2.getX(), -c2.getY() ).addRotation(10.*M_PI/180.).addTranslation( c2.getX(), c2.getY() ) * poly2;

		std::ostringstream oss;
		oss << "showcase14_" << std::setfill('0') << std::setw(2) << i << ".svg";
		im.write( oss.str() );
	}
}

