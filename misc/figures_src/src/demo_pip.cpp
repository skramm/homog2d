/**
\file
\brief Demo of point inside polygon (PIP) algorithm
*/
//#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "fig_src.header"

using namespace h2d;

template<typename P,typename BB, typename IMG>
void drawFig( const P& poly, const BB& bb, const Point2d& pt, const Point2d* pt2, const Segment* source_seg, IMG& im, int n )
{
	im.clear();
	poly.draw( im, DrawParams().setColor(250,0,20)  );
	bb.draw( im, DrawParams().setColor(0,0,250)  );
	pt.draw( im, DrawParams().setColor(250,0,0)  );
	auto polypts = poly.getPts();
	if( pt2 )
	{
		auto seg = Segment( pt, *pt2 );
		std::cout << seg << "\n";
		seg.draw( im, DrawParams().setColor(0,0,250)  );
		auto inter = seg.intersects( poly );
		auto color = DrawParams().setColor(0,250,0).setPointStyle(PtStyle::Dot).setThickness(3).setPointSize(15);
		pt2->draw( im, color );
		if( inter() )
		{
			for( const auto& ptpoly: polypts )
				if( ptpoly == inter.get()[0] )
				{
					color.setColor(250,0,0);
					break;
				}
			inter.get()[0].draw( im, color );
		}
	}
	if( source_seg )
		source_seg->draw( im, DrawParams().setThickness(2).setColor( 0,255,0) );
	im.drawText( std::to_string(n), Point2d(10,20), DrawParams() );

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

	auto H = Homogr().addTranslation(15,15).addScale(8);
	auto poly = H*pol;
	auto pt0 = H * pt;
	auto bbe = getBB( poly ).getExtended();

	img::Image<img::SvgImage> im( 350,350 );

	drawFig( poly, bbe, pt0, nullptr, nullptr, im, 0 );

	im.write( "demo_pip_0.svg" );
	auto bbpts = bbe.get4Pts();

// step 1: show that taking segments based on the 4 points
// of the extended BB does not meet the requirements
// (the segments will intersect the polygon on one of its points)
	for( int i=0; i<4; i++ )
		drawFig( poly, bbe, pt0, &bbpts[i], nullptr, im, i+1 );

// step 2: show that taking segments based on the middle point of the
// 4 segments of the extended BB does not work either
	auto segs = bbe.getSegs();
	for( int i=0; i<4; i++ )
	{
		auto ptmid = segs[i].getCenter();
		drawFig( poly, bbe, pt0, &ptmid, &segs[i], im, i+5 );
	}

// final step: taking the middle point of half the above segments does
// generate a segment that does not intersect the polygon on one of its points
	auto seg = segs[0];
	auto seg2 = Segment( seg.getCenter(), bbpts[0] );
	auto mid = seg2.getCenter();
	drawFig( poly, bbe, pt0, &mid, &seg2, im, 9 );
}


