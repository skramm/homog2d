/**
\file
\brief Generates moving polygons and shows their intersection points
*/
#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

using namespace h2d;

int main( int argc, const char** argv )
{
	auto nbim = 20; // nb images

//	auto nbpts_max = 20;
//	auto nbpts_min = 5;

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
	poly2.rotate( Rotate::CCW, Point2d(150, 150) );

	auto extr_col = img::DrawParams().setColor(100,250,0).setPointStyle( img::PtStyle::Dot );
	for( int i=0; i<nbim; i++ )
	{
		img::Image<cv::Mat> im( 300, 220 );
		poly1.draw( im, img::DrawParams().setColor(200,50,0) );
		poly2.draw( im, img::DrawParams().setColor(0,50,200) );
		auto res = poly1.intersects(poly2);
//		if( res() )
		{
			auto ptInt = res.get();
			for( const auto& pt: ptInt )
				pt.draw( im, img::DrawParams().setColor(0,250,0).setPointStyle( img::PtStyle::Dot ) );
		}
		poly1.translate( 10, 6 );
		poly2.translate( -7, -7 );
		std::ostringstream oss;
		oss << "showcase14_" << std::setfill('0') << std::setw(2) << i << ".png";
		im.write( oss.str() );
	}
}

