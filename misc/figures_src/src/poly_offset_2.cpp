// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc-fig


#include "fig_src.header"

int main()
{
//	std::vector<Point2d> vpts1{ {0,0},{5,1},{3,4} };
	std::vector<Point2d> vpts1{ {0,0},{7,0.5},{6,4},{4,2},{1,6} };
	std::vector<Point2d> v_out_1;
	std::vector<Point2d> v_out_2;
	std::vector<Point2d> vpt(4);

	CPolyline pol0(vpts1);

	auto H = Homogr().setScale(50).addTranslation(100,80);

	auto pol = H*pol0;
	std::cout << "main: pol2=" << pol << '\n';
//	auto poff = pol2.buildOffset( 30 );
	auto dist = 20;
	size_t current = 0;
	const auto segs = pol.getSegs();
	const auto vppts = pol.getPts();

	img::Image<img::SvgImage> im( 800,600 );
	draw( im, pol, DrawParams().setColor(250,0,0).showPoints() );

	do
	{
		std::cout << "* current=" << current << '\n';

		auto nextS = (current==pol.size()-1 ? 0 : current+1);
		auto nextPt1 = nextS;
		auto nextPt2 = (nextPt1==pol.size()-1 ? 0 : nextPt1+1);

		auto pt1 = pol.getPoint(current);
		auto pt2 = pol.getPoint(nextPt1);
		auto pt3 = pol.getPoint(nextPt2);

		Vector v1( pt1.getX(), pt1.getY(), pt2.getX(), pt2.getY() );
		Vector v2( pt2.getX(), pt2.getY(), pt3.getX(), pt3.getY() );

		auto li1 = segs.at(current).getLine();
		auto li2 = segs.at(nextS).getLine();

		auto pli1 = li1.getParallelLines( dist );
		auto pli2 = li2.getParallelLines( dist );

// compute the 4 intersection points
		vpt[0] = pli1.first  * pli2.first;
		vpt[1] = pli1.first  * pli2.second;
		vpt[2] = pli1.second * pli2.first;
		vpt[3] = pli1.second * pli2.second;

		int addPoint1 = -1;
		int addPoint2 = -1;
		for( int i=0; i<4; i++ )
		{
//			auto pt = vpt[i]; // each of the four points
//			std::cout << "i=" << i << " pt=" << pt << " s1=" << std::to_string(side( pt, li1)) << " s2=" << side( pt, li2) << '\n';
//			auto str = std::to_string(side( pt, li1)) + ":" + std::to_string(side( pt, li2 ));

			auto s1 = v1.getPointSide( vpt[i] );
			auto s2 = v2.getPointSide( vpt[i] );
			if( s1 == PointSide::Left && s2 == PointSide::Left )
			{
				addPoint1 = i;
//				break;
			}
			if( s1 == PointSide::Right && s2 == PointSide::Right )
			{
				addPoint2 = i;
//				break;
			}


//			im.write( "poly_offset2_"+std::to_string(current) + ".svg" );
		}
		assert( addPoint1 != -1 );
		assert( addPoint2 != -1 );
		v_out_1.push_back( vpt[addPoint1] );
		v_out_2.push_back( vpt[addPoint2] );
		current++;
	}
	while( current<pol.size() );

	CPolyline polOut1( v_out_1 );
	CPolyline polOut2( v_out_2 );
	im.draw( polOut1,  DrawParams().setColor(0,250,0) );
	im.draw( polOut2,  DrawParams().setColor(0,0,250) );
	im.write( "poly_offset2.svg" );

}
