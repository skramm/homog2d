/**
\file
\brief shows Segment isInside() FRect , Circle, CPolyline
*/

#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

using namespace h2d;

img::Image<cv::Mat> im( 380, 160 );

template<typename T>
void draw_intersect( const Segment& seg, const T& p )
{
	auto inter =  seg.intersects(p);
	if( inter() )
		for( const auto& pt: inter.get() )
			pt.draw( im, img::DrawParams().setPointStyle( img::PtStyle::Dot ) );
}

int main( int argc, const char** argv )
{
	auto n = 35; // nb images
	FRect rect(35,30,110,90);
	Circle cir( 160,60, 30 );

	std::vector<Point2d> vpts{
		{ 0, 0 },
		{ 140,30 },
		{ 80,110},
		{ 60,50 },
		{ 20,80 }
	};
	CPolyline pol( vpts );
	pol.translate( 220, 20 );

	auto col = img::DrawParams().setColor(50,250,20);
	auto col_outside = img::DrawParams().setColor(50,20,200);
	auto col_inside  = img::DrawParams().setColor(200,20,20).setThickness(2);

	Point2d pt1( 20,55 );
	Point2d pt2( 32,38 );
	for( int i=0; i<n; i++ )
	{
		im.clear();
		Segment seg(pt1,pt2);

		auto cols = col_outside;
		if( seg.isInside( rect ) || seg.isInside( cir )  || seg.isInside( pol ) )
			cols = col_inside;

		rect.draw( im, col );
		cir.draw(  im, col );
		pol.draw(  im, col );
		seg.draw(  im, cols );

		draw_intersect( seg, rect );
		draw_intersect( seg, cir );
		draw_intersect( seg, pol );

		pt1.translate( 10, 0);
		pt2.translate( 8, 2) ;
		std::ostringstream oss;
		oss << "showcase7c_" << std::setfill('0') << std::setw(2) <<i << ".png";
		im.write( oss.str() );
	}
}

