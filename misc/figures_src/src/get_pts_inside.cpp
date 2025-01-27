// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

template<typename T,typename PRIM>
void process( img::Image<img::SvgImage>& im, const T& input, const PRIM& prim, std::string name )
{
	im.clear();

	auto col_1 = DrawParams().setColor(250,0,20);
	auto col_2 = DrawParams().setColor(20,0,250);
	auto col_p = DrawParams().setColor(0,250,0);

	draw( im, input, col_1 );
	draw( im, prim, col_p );

	auto vec_c = getPtsInside( input, prim );
	draw( im, vec_c, col_2 );

	im.write( "pts_inside_" + name + ".svg" );
}

int main()
{
	std::vector<Point2d>   v_pts;

	int xmax = 300;
	int ymax = 250;
	int xmin = 20;
	int ymin = 20;
	std::srand( std::time(nullptr) );
	int nbPts = 200;

// generate random points
	for( int i=0; i<nbPts; i++ )
	{
		auto x1 = 1.*std::rand()/RAND_MAX * xmax + xmin;
		auto y1 = 1.*std::rand()/RAND_MAX * ymax + xmin;
		v_pts.emplace_back( Point2d(x1,y1) );
	}

	img::Image<img::SvgImage> im( xmax, ymax );

	FRect rect(xmin*2.4,ymin*4., xmax*.7, ymax*.8 );
	process( im, v_pts, rect, "rect");

	Circle cir( xmax/2,ymax/2, 70 );
	process( im, v_pts, cir, "circle" );

	CPolyline pol{
		std::vector<Point2d>{
			{40,60},
			{55,70},
			{200,50},
			{230,170},
			{160,220},
			{110,100},
			{80,120}
		}
	};
	process( im, v_pts, pol, "pol" );

	Ellipse ell( Point2d(xmax/2,ymax/2), 85, 45, M_PI/6. );
	process( im, v_pts, ell, "ell" );
}

