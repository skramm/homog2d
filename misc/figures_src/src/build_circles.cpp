/**
\file
\brief random points and circles build from nearest points

!!! WIP !!!
*/

#include "fig_src.header"
using namespace h2d;



int main( int, const char** )
{
#if 0
	auto nbim = 20; // nb images

	auto cmax = 300;
	auto cmin = 20;

	int nbPts = 80;
	std::vector<Point2d> src(nbPts);
	auto it = std::begin(src);
	for( auto i=0; i<nbPts; i++ )
		*it++ = Point2d(
			1.0*rand() / RAND_MAX * (cmax-cmin) + cmin,
			1.0*rand() / RAND_MAX * (cmax-cmin) + cmin
	);
	std::vector<Point2d> remaining(nbPts);
	auto k = 70.;
	auto x0 = 150;
	auto y0 = 150;


	std::vector<bool> isUsed(nbPts,false);
	isUsed[0] = true;
	for( int i=0; i<nbim; i++ )
	{
		img::Image<cv::Mat> im( cmax+30, cmax+30 );

//		draw( im, vec );

		size_t current = 0;
		auto finished = false;
		im.write( oss.str() );
//		do
		{
		findPoint( current, )

			auto np1 = findNearestPoint( vec[current] , vec );
			auto np2 = findNearestPoint( vec[np1] , vec );
			Circle cir( vec[current], vec[np1], vec[np2] );
		}
//		while( finished == false );


		std::ostringstream oss;
		oss << "showcase21_" << std::setfill('0') << std::setw(2) << 1 << ".png";
		im.write( oss.str() );

/*		auto angle = i*360./nbim;
		auto angle_r = angle * M_PI / 180.;
		auto x1 = std::cos(angle_r)*k;
		auto y1 = std::sin(angle_r)*k;
		Point2d pt( x1+x0,y1+y0);
		pt.draw( im, img::DrawParams().setColor(250,0,0).setPointStyle( img::PtStyle::Dot ) );
		auto pidx = findNearestFarthestPoint( pt, vec );
		Segment(vec[pidx.first],  pt).draw( im, img::DrawParams().setColor( 250,0,0) );
		Segment(vec[pidx.second], pt).draw( im, img::DrawParams().setColor( 0,250,0) );

		std::ostringstream oss;
		oss << "showcase15_" << std::setfill('0') << std::setw(2) << i << ".png";
		im.write( oss.str() );
*/
	}
*/
#endif
}

