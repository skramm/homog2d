/**
\file
\brief shows isInside() FRect vs CPolyline
*/

#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

using namespace h2d;

int main( int argc, const char** argv )
{
	auto n = 25; // nb images
	Circle cir( 100,100,15 );
	FRect rect(70,60,150,130);

	img::Image<cv::Mat> im( 320, 250 );

	auto col_outside = img::DrawParams().setColor(50,20,200);
	auto col_inside  = img::DrawParams().setColor(200,20,20).setThickness(2);


	for( int i=0; i<n; i++ )
	{
		im.clear();

		auto col_c = col_outside;
		if( cir.isInside( rect ) )
			col_c = col_inside;

		auto col_p = col_outside;
		if( rect.isInside( cir ) )
			col_p = col_inside;

		rect.draw( im, col_p );
		cir.draw(  im, col_c );

		auto inter =  cir.intersects(rect);
		if( inter() )
			for( const auto& pt: inter.get() )
				pt.draw( im );

		if( i < n/2 )
			cir.set( cir.radius() * 1.2 );
		else
			cir.set( cir.radius() * 1./1.2 );

		std::ostringstream oss;
		oss << "showcase7b_" << std::setfill('0') << std::setw(2) <<i << ".png";
		im.write( oss.str() );
	}
}

