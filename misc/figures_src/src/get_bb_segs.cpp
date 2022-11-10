// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

int main()
{
	std::vector<Segment> vsegs;
	int xmax = 300;
	int xmin = 20;
	int seg_max = 40;
	int seg_min = 10;
	srand( time(0) );
	int n = 40;
	for( int i=0; i<n; i++ )
	{
		auto x1 = 1.*std::rand()/RAND_MAX * xmax + xmin;
		auto y1 = 1.*std::rand()/RAND_MAX * xmax + xmin;

		auto x2 = x1+1.*std::rand()/RAND_MAX * seg_max + seg_min;
		auto y2 = y1+1.*std::rand()/RAND_MAX * seg_max + seg_min;
		vsegs.push_back( Segment( x1,y1,x2,y2) );
	}
	img::Image<cv::Mat>       img1( 1.5*xmax,1.5*xmax );
	img::Image<img::SvgImage> img2( 1.5*xmax,1.5*xmax );


	img1.clear();
	img2.clear();
	auto bbpts = getBB( vsegs );
	bbpts.draw( img1, DrawParams().setColor(250,50,20) );
	bbpts.draw( img2, DrawParams().setColor(250,50,20) );
	draw( img1, vsegs );
	draw( img2, vsegs );
	img1.write( "bbSegs.png" );
	img2.write( "bbSegs.svg" );
}
