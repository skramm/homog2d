/**
\file
\brief shows isInside circles vs polyline
*/

#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

using namespace h2d;

int main( int argc, const char** argv )
{
	auto n = 25; // nb images
	Circle cir( 3. );

	std::vector<Point2d> vpts{
		{ -1.0,  1.0 },
		{ -1.2, -0.7 },
		{  1.8, -0.5 },
		{  1.6,  1.1 },
		{  0.5,  0.8 }
	};
	CPolyline pl( vpts );

	auto Hdraw = Homogr().addTranslation(7,6).addScale(15);

	img::Image<cv::Mat> im( 300, 200 );

	auto col_outside = img::DrawParams().setColor(50,20,200);
	auto col_inside  = img::DrawParams().setColor(200,20,20).setThickness(2);
	auto coeff_x = 1.12;
	auto coeff_y = 1.16;
	Homogr H1 = Homogr().addScale(    coeff_x,    coeff_y );
	Homogr H2 = Homogr().addScale( 1./coeff_x, 1./coeff_y );

	for( int i=0; i<n; i++ )
	{
		im.clear();
		auto pl2  = Hdraw * pl;
		auto cir2 = Hdraw * cir;

		auto col_c = col_outside;
		if( cir.isInside( pl ) )
			col_c = col_inside;

		auto col_p = col_outside;
		if( pl.isInside( cir ) )
			col_p = col_inside;

		pl2.draw( im, col_p );
		cir2.draw( im, col_c );

		auto inter =  cir.intersects(pl);
		if( inter() )
		{
			auto inter_pts = inter.get();
			for( const auto& pt: inter.get() )
			{
				auto pt2 = Hdraw * pt;
				pt2.draw( im );
			}
		}
		if( i<n/2 )
			pl = H1*pl;
		else
			pl = H2*pl;

		std::ostringstream oss;
		oss << "showcase7_" << std::setfill('0') << std::setw(2) <<i << ".png";
		im.write( oss.str() );
	}
}

