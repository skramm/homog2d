/**
\file homog2d.hpp
\brief single header file, implements some 2D homogeneous stuff
*/


#ifndef HG_HOMOG2D_HPP
#define HG_HOMOG2D_HPP

#include <cmath>

namespace homog2d {

//------------------------------------------------------------------
class Root
{
	protected:
		Root( double a=0., double b=0., double c=1. )
			: _a(a), _b(b), _c(c)
		{}
	double _a, _b, _c;
};

class Point2d;

//------------------------------------------------------------------
class Line2d : public Root
{

	public:
		Line2d( double dx, double dy )
		{}
		double DistToPoint( const Point2d& pt ) const;
		Point2d operator * ( const Line2d& );
	private:
		void normalize()
		{
			auto sq = std::hypot( _a, _b );
			_a /= sq;
			_b /= sq;
			_c /= sq;
		}
};

//------------------------------------------------------------------
class Point2d : public Root
{
	public:
	Point2d( double x=0., double y=0. ) : Root(x, y, 1.)
	{}
	Line2d operator * ( const Point2d& );
	double getX() const
	{
		return _a/_c;
	}
	double getY() const
	{
		return _b/_c;
	}
};

//------------------------------------------------------------------
/// A point is the intersection of 2 lines
inline
Point2d
Line2d::operator * ( const Line2d& l2 )
{

}

/// A line is defined by 2 points
inline
Line2d
Point2d::operator * ( const Point2d& p2 )
{

}

//------------------------------------------------------------------
/// Returns distance between the line and point \b pt
/**
http://mathworld.wolfram.com/Point-LineDistance2-Dimensional.html
<pre>
        | a.x0 + b.y0 + c |
  d = -----------------------
         sqrt( a*a + b*b )
</pre>
*/
inline
double
Line2d::DistToPoint( const Point2d& pt ) const
{
	return std::fabs( (_a * pt.getX() + _b * pt.getY() + _c) / std::hypot( _a, _b ) );
}
//------------------------------------------------------------------




} // namespace homog2d end

#endif
