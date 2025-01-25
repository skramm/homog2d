// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc-fig
/**
\file
\brief Demo of bounding box for a set of variant-based graphical primitives
*/

#define HOMOG2D_ENABLE_VRTP
#include "fig_src.header"

int main()
{
	std::vector<CommonType_<double>> vec;

	auto nb_pts_poly_max = 5;
	auto nb_pts_poly_min = 2;
//	size_t polySize = 1.*std::rand()/RAND_MAX * nb_pts_poly_max + nb_pts_poly_min;

	int xmax = 350;
	int ymax = 280;
	int xmin = 50;
	int seg_max = 70;
	int seg_min = 10;
	std::srand( std::time(nullptr) );

	int nb = 40;

//	v_colors = genRandomColors( 2 * nbSegs );
	int r_max = 30;
	int r_min = 8;

	for( int i=0; i<nb; i++ )
	{
		auto elemType = (int)(1.*std::rand()/RAND_MAX * 4);
		auto x1 = 1.*std::rand()/RAND_MAX * xmax + xmin;
		auto y1 = 1.*std::rand()/RAND_MAX * ymax + xmin;
		auto x2 = x1 + (1.*std::rand()/RAND_MAX-.5) * seg_max + seg_min;
		auto y2 = y1 + (1.*std::rand()/RAND_MAX-.5) * seg_max + seg_min;
		switch( elemType )
		{
			case 0:
				vec.emplace_back( Point2d(x1,y1) );
			break;
			case 1:
				vec.emplace_back( Segment(x1,y1,x2,y2) );
			break;
			case 2:
				vec.emplace_back( FRect(x1,y1,x2,y2) );
			break;
			case 3:
				vec.emplace_back( Circle(x1,y1,1.*std::rand()/RAND_MAX * r_max + r_min ) );
			break;
			default:
				std::cout <<"elemType=" << elemType << '\n';
				assert(0);
		}
	}
	auto k = 1.4;
	img::Image<img::SvgImage> im( k*xmax, k*ymax );

	fct::DrawFunct vde( im /*, dp */ );
	for( auto& e: vec )
		std::visit( vde, e );

	auto bb = getBB( vec );
	bb.draw( im, DrawParams().setColor(250,20,20) );
	im.write( "bb_variant.svg" );
}

