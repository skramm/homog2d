/**
\file
\brief Demo of point inside polygon (PIP) algorithm
*/
//#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "fig_src.header"

using namespace h2d;

template<typename P,typename BB, typename PT, typename IMG>
void drawBase( const P& poly, const BB& bb, const PT& pt, const Segment* seg, IMG& im, int n )
{
	im.clear();
	poly.draw( im, DrawParams().setColor(250,0,20)  );
	bb.draw( im, DrawParams().setColor(0,0,250)  );
	pt.draw( im, DrawParams().setColor(0,250,0)  );
	if( seg )
	{
		std::cout << *seg << "\n";
		seg->draw( im, DrawParams().setColor(0,0,250)  );
		auto inter = seg->intersects( poly );
		if( inter() )
			inter.get()[0].draw( im, DrawParams().setColor(0,250,0).setPointStyle(PtStyle::Dot).setThickness(3).setPointSize(15) );
	}
	std::ostringstream oss;
	oss << "demo_pip_" << n << ".svg";
	im.write( oss.str() );
}

int main()
{
	std::vector<Point2d> vpts1{
		{ 0, 0 },
		{ 5, 0 },
		{ 7, 2 },
		{ 8, 0 },
		{ 10, 0 },

		{ 10, 5 },
		{ 8, 6 },
		{ 10, 7 },
		{ 10, 10 },

		{ 7, 10 },
		{ 6, 8 },
		{ 5, 10 },

		{ 0, 10 },
		{ 0, 5 },
		{ 2, 4 },
		{ 0, 3 }
	};
	CPolyline pol( vpts1 );


// considered point is exactly in the center of the polygon
	Point2d pt(5,5);

	auto H = Homogr().addTranslation(20,20).addScale(8);
	auto poly = H*pol;
	auto pt0 = H * pt;
	auto bbe = getBB( poly ).getExtended();

	img::Image<img::SvgImage> im( 350,350 );

	drawBase( poly, bbe, pt0, nullptr, im, 0 );

	im.write( "demo_pip_0.svg" );
	auto bbpts = bbe.get4Pts();

// step 1: show that taking segments based on the 4 points
// of the extended BB does not meet the requirements
// (the segments will intersect the polygon on one of its points)
	for( int i=0; i<4; i++ )
	{
		auto seg = Segment( bbpts[i], pt0 );
		drawBase( poly, bbe, pt0, &seg, im, i+1 );
	}

// step 2: show that taking segments based on the middle point of the
// 4 segments of the extended BB does not work either
	auto segs = bbe.getSegs();
	for( int i=0; i<4; i++ )
	{
		auto ptmid = segs[i].getCenter();
		auto seg = Segment( ptmid,  pt0 );
		drawBase( poly, bbe, pt0, &seg, im, i+5 );
	}

// final step: taking the middle point of half the above segments does
// generate a segment that does not intersect the polygon on one of its points
	auto seg2 = segs[0];
//	auto pt = seg2.getCenter();

//	priv::printArray( bbpts );
	draw( im, seg2.getCenter() );
	std::cout << "PT=" << seg2.getCenter() << "\n";
	auto seg = Segment( seg2.getCenter(), bbpts[0] );
	drawBase( poly, bbe, pt0, &seg, im, 9 );
}

