/**
\file
\brief shows isInside() for Ellipse
*/

#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

using namespace h2d;

img::Image<cv::Mat> im( 280, 220 );

template<typename T>
void draw_object( const Ellipse& ell, const T& p, img::DrawParams& dp )
{
/*	auto inter =  seg.intersects(p);
	if( inter() )
		for( const auto& pt: inter.get() )
			pt.draw( im, img::DrawParams().setPointStyle( img::PtStyle::Dot ) );
*/
//		auto col = col_outside;
		if( p.isInside( ell ) )
			dp = dp.setThickness(2);
		p.draw( im, dp );
}

int main( int argc, const char** argv )
{
	auto n = 25; // nb images

	Ellipse ell( 100,70,50,90, 35 * M_PI/180. );

	FRect rect(15,110,40,150);
	Circle cir( 170,120, 22 );
	Segment seg( 50,10, 85,7 );
	std::vector<Point2d> vpts{
		{ 0, 0 },
		{ 40,12 },
		{ 32,38},
		{ 28,14},
		{ 17,50 },
		{ 9,20 }
	};
	CPolyline pol( vpts );
	pol.translate( 3, 5 );

	auto col_ell     = img::DrawParams().setColor(250,20,20);

	for( int i=0; i<n; i++ )
	{
		im.clear();

		ell.draw( im, col_ell );
/*		rect.draw( im, img::DrawParams().setColor(50,250,120) );
		cir.draw(  im, img::DrawParams().setColor(50,120,250) );
		pol.draw(  im, img::DrawParams().setColor(50,180,180) );
	*/
		draw_object( ell, rect, img::DrawParams().setColor(10,250,120) );
//		draw_object( ell, cir,  img::DrawParams().setColor(10,120,250) );
		draw_object( ell, pol,  img::DrawParams().setColor(10,180,180) );
		draw_object( ell, seg,  img::DrawParams().setColor(180,0,180) );

		pol.translate( 8, 3) ;
		cir.translate( -5, -1) ;
		rect.translate( 9, -6) ;
		seg.translate( 2, 7) ;

		std::ostringstream oss;
		oss << "showcase7d_" << std::setfill('0') << std::setw(2) <<i << ".png";
		im.write( oss.str() );
	}
}

