// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc-fig


#include "fig_src.header"

// https://stackoverflow.com/a/4609795/
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

template<typename FPT>
struct Vector2
{
	FPT dx,dy;
	Vector2( FPT x1, FPT y1, FPT x2, FPT y2 ): dx(x2-x1), dy(y2-y1)
	{}

};

template<typename FPT>
FPT
crossProd( FPT x, FPT y, const Vector2<FPT>& v )
{
	std::cout << "x=" << x << " v.dy=" << v.dy << " y=" << y << " v.dx=" << v.dx << '\n';
	return x * v.dy - y * v.dx;
}

template<typename FPT>
FPT
crossProd( const Vector2<FPT>& v1, const Vector2<FPT>& v2 )
{
	return v1.dx * v2.dy - v1.dy * v2.dx;
}

int main()
{
//	std::vector<Point2d> vpts1{ {0,0},{5,1},{3,4} };
	std::vector<Point2d> vpts1{ {0,0},{7,0.5},{6,4},{4,2},{1,6} };
	std::vector<Point2d> vpt(4);

	CPolyline pol0(vpts1);

	auto H = Homogr().setScale(50).addTranslation(100,80);

	auto pol = H*pol0;
	std::cout << "main: pol2=" << pol << '\n';
//	auto poff = pol2.buildOffset( 30 );
	auto dist = 30;
	size_t current = 0;
	const auto segs = pol.getSegs();
	const auto vppts = pol.getPts();
	do
	{
		img::Image<img::SvgImage> im( 800,600 );
		draw( im, pol );
		std::cout << "* current=" << current << '\n';

		auto nextS = (current==pol.size()-1 ? 0 : current+1);
		auto nextPt1 = nextS;
		auto nextPt2 = (nextPt1==pol.size()-1 ? 0 : nextPt1+1);

		auto pt1 = pol.getPoint(current);
		auto pt2 = pol.getPoint(nextPt1);
		auto pt3 = pol.getPoint(nextPt2);

		Vector2<double> v1( pt1.getX(), pt1.getY(), pt2.getX(), pt2.getY() );
		Vector2<double> v2( pt2.getX(), pt2.getY(), pt3.getX(), pt3.getY() );

		std::cout << "s(cp)=" << sgn(crossProd(v1,v2)) << '\n';
//		auto v1 = std::make_pair( pol.get)

//		std::cout << "current=" << current << " next=" << nextS << " nextPt2=" << nextPt2 << '\n';
/*		auto s1 = segs.at(current);
		auto s2 = segs.at(next);
		std::cout << "s1=" << s1 << " s2=" << s2 << '\n';*/

		draw( im, segs.at(current), DrawParams().setColor(250, 0,  0) );
		draw( im, segs.at(nextS),   DrawParams().setColor(  0, 0,250) );



		auto li1 = segs.at(current).getLine();
		auto li2 = segs.at(nextS).getLine();
//		std::cout << "li1=" << li1 << " li2=" << li2 << '\n';
		std::cout << "s(li1)=" << sgn(li1.get()[1]) << " s(li2)=" <<  sgn(li2.get()[1]) << '\n';

		auto pli1 = li1.getParallelLines( dist );
		auto pli2 = li2.getParallelLines( dist );

		draw( im, pli1.first,  DrawParams().setColor(250,100,  0) );
		draw( im, pli1.second, DrawParams().setColor(250,  0,100) );
		draw( im, pli2.first,  DrawParams().setColor(  0,100,250) );
		draw( im, pli2.second, DrawParams().setColor(100,  0,250) );

// compute the 4 intersection points
		vpt[0] = pli1.first  * pli2.first;
		vpt[1] = pli1.first  * pli2.second;
		vpt[2] = pli1.second * pli2.first;
		vpt[3] = pli1.second * pli2.second;

		for( int i=0; i<4; i++ )
		{
			auto pt = vpt[i]; // each of the four points
			std::cout << "i=" << i << " pt=" << pt << " s1=" << std::to_string(side( pt, li1)) << " s2=" << side( pt, li2) << '\n';
//			auto str = std::to_string(side( pt, li1)) + ":" + std::to_string(side( pt, li2 ));

			Vector2 vA( pt1.getX(), pt1.getY(), pt.getX(), pt.getY() );
			auto cpA = crossProd( vA, v1 );
//			std::cout << "  cp=" << cp << "\n";
			if( cpA>0 )
			{
				Vector2 vB( pt2.getX(), pt2.getY(), pt.getX(), pt.getY() );
				auto cpB = crossProd( vB, v2 );
//				auto str = std::to_string(sgn(cp)) + "=>" + std::to_string(side( pt, li1)) + ":" + std::to_string(side( pt, li2 ));;
				auto str = std::to_string(sgn(cpB));;
//				auto str = std::to_string(side( pt, li1)) + ":" + std::to_string(side( pt, li2 ));;
				drawText( im, str, pt, DrawParams().setColor(0,0,0) );
			}


			im.write( "poly_offset_"+std::to_string(current) + ".svg" );
		}
		current++;
	}
	while( current<pol.size() );




}
