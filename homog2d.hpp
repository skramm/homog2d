/**
\file homog2d.hpp
\brief single header file, implements some 2D homogeneous stuff
*/


#ifndef HG_HOMOG2D_HPP
#define HG_HOMOG2D_HPP

#include <cmath>
#include <iostream>

namespace homog2d {

//------------------------------------------------------------------
class Root
{
	protected:
		Root( double a=0., double b=0., double c=1. )
			: _a(a), _b(b), _c(c)
		{}
	double _a, _b, _c;

	/// Cross product, see https://en.wikipedia.org/wiki/Cross_product#Coordinate_notation
	friend Root crossProduct( const Root& r1, const Root& r2 )
	{
		Root res;
//		std::cout << "Root crossProduct, r1=" << r1 << " r2=" << r2 << "\n";
		res._a = r1._b * r2._c - r1._c * r2._b;
		res._b = r1._c * r2._a - r1._a * r2._c;
		res._c = r1._a * r2._b - r1._b * r2._a;
		return res;
	}
	friend std::ostream& operator << ( std::ostream& f, const Root& r )
	{
		f << '[' << r._a << ',' << r._b << ',' << r._c << "] ";
		return f;
	}
};

class Point2d;

//------------------------------------------------------------------
/// Homogeneous 2D line
class Line2d : public Root
{
	friend class Point2d;

	friend std::ostream& operator << ( std::ostream& f, const Line2d& li )
	{
		f << static_cast<Root>(li);
		return f;
	}

	public:
		Line2d( double dx, double dy )
		{
			normalize();
		}
		Line2d(): Root(1.,0.,0.)
		{}
		double DistToPoint( const Point2d& ) const;
		Point2d operator * ( const Line2d& );
		std::pair<double,double> getVector() const;
		bool operator == ( const Line2d& li ) const
		{
			auto eps = std::numeric_limits<double>::epsilon();
			if( std::fabs( _a - li._a ) > eps )
				return false;
			if( std::fabs( _b - li._b ) > eps )
				return false;
			if( std::fabs( _c - li._c ) > eps )
				return false;
			return true;

		}
	private:
		/// Normalise to unit length, and make sure \c a is always >0

		Line2d( const Root& r ): Root(r)
		{
			normalize();
		}
		void normalize()
		{
			auto sq = std::hypot( _a, _b );
			_a /= sq;
			_b /= sq;
			_c /= sq;
			if( std::signbit(_a) )
			{
				_a = - _a;
				_b = - _b;
				_c = - _c;
			}
		}

};

//------------------------------------------------------------------
/// Homogeneous 2D point
class Point2d : public Root
{
	friend class Line2d;
	friend std::ostream& operator << ( std::ostream& f, const Point2d& pt )
	{
		f << static_cast<Root>(pt);
		return f;
	}

	public:
		Point2d( double x=0., double y=0. ) : Root(x, y, 1.)
		{
			normalize();
		}
		Line2d operator * ( const Point2d& );
		double getX() const
		{
			return _a/_c;
		}
		double getY() const
		{
			return _b/_c;
		}
		bool operator == ( const Point2d& pt ) const
		{
			auto eps = std::numeric_limits<double>::epsilon();
			if( std::fabs( _a - pt._a ) > eps )
				return false;
			if( std::fabs( _b - pt._b ) > eps )
				return false;
			return true;
		}
	private:
		Point2d( const Root& r ): Root(r)
		{}
		void normalize()
		{
			_a /= _c;
			_b /= _c;
			_c /= 1.;
		}
};

//------------------------------------------------------------------
/// A point is the intersection of 2 lines
inline
Point2d
Line2d::operator * ( const Line2d& l2 )
{
	auto res = static_cast<Point2d>(crossProduct( *this, l2 ));
	res.normalize();
	return res;
}

/// A line is defined by 2 points
inline
Line2d
Point2d::operator * ( const Point2d& p2 )
{
//	std::cout << "Point2d::operator *\n p1=" << *this << " p2=" << p2 << "\n";
	auto res = static_cast<Line2d>(crossProduct( *this, p2 ) );
	res.normalize();
	return res;
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
