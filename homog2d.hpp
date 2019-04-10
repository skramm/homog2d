/**************************************************************************

    This file is the C++ library "homog2d", dedicated to
    handle 2D lines and points

    Author & Copyright 2019 Sebastien Kramm

    Contact: firstname.lastname@univ-rouen.fr, or http://www.litislab.eu/

    Licence: LGPL v3

    This library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

	See included lgpl.txt and gpl.txt files.

**************************************************************************/

/**
\file homog2d.hpp
\brief single header file, implements some 2D homogeneous stuff.
See https://github.com/skramm/homog2d
*/

#ifndef HG_HOMOG2D_HPP
#define HG_HOMOG2D_HPP

#include <cmath>
#include <iostream>

namespace homog2d {

//------------------------------------------------------------------
/// A 2D homography (3x3 matrix)
class Homogr
{
	public:
	Homogr()
	{
		_data.resize(3);
		for( auto& li: _data )
			li.resize(3,0.);
		_data[0][0] = _data[1][1] = _data[2][2] = 1.;
		_isNormalized = true;
	}

	Homogr( const std::vector<std::vector<double>>& in )
	{
		_data = in;
		normalize();
	}
	void clear()
	{
#if 1
		_data = { { 1., 0., 0.}, { 0., 1., 0.}, {0., 0., 1.} };
#else
		for( auto& li: _data )
			for( auto& e: li )
				e = 0.;
		_data[0][0] = _data[1][1] = _data[2][2] = 1.;
#endif
		_isNormalized = true;
	}
	/// Setter \warning No normalization is done, as this can be done
	/// several times to store values, we therefore must not normalize in between
	void setValue( size_t r, size_t c, double v )
	{
		_data[r][c] = v;
		_isNormalized = false;
	}
	double getValue( size_t r, size_t c )
	{
		return _data[r][c];
	}
	/// Adds a translation tx,ty to the matrix
	template<typename T>
	void addTranslation( T tx, T ty )
	{
		Homogr trans;
		_data[0][2] = tx;
		_data[1][2] = ty;
		*this = trans * *this;
		normalize();
	}
	/// Sets the matrix as a translation tx,ty
	template<typename T>
	void setTranslation( T tx, T ty )
	{
		clear();
		_data[0][2] = tx;
		_data[1][2] = ty;
		_isNormalized = true;
	}

	void normalize() const
	{
		auto eps = std::numeric_limits<double>::epsilon();

		if( std::fabs(_data[2][2]) < eps ) // if [2][2] is null, then we use [2][1]
			p_divideBy( 2, 1 );
		else
			p_divideBy( 2, 2 );
		if( std::signbit(_data[2][2]) )
			for( auto& li: _data )
				for( auto& e: li )
					e = -e;
		_isNormalized = true;
	}
	friend Homogr operator * ( const Homogr& h1, const Homogr& h2 )
	{
		Homogr out;
		out.p_zero();
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				for( int k=0; k<3; k++ )
					out._data[i][j] += h1._data[i][k] * h2._data[k][j];
		return out;
	}
/// Comparison operator. Does normalisation if required
	bool operator == ( const Homogr& li ) const
	{
		if( !_isNormalized )
			normalize();
		if( !li._isNormalized )
			li.normalize();

		auto eps = std::numeric_limits<double>::epsilon();
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				if( std::fabs( _data[i][j] - li._data[i][j] ) > eps )
					return false;
		return true;
	}

	private:
	void p_zero()
	{
		for( auto& li: _data )
			for( auto& e: li )
				e = 0.;
	}
	void p_divideBy( size_t r, size_t c ) const
	{
		assert( std::fabs( _data[r][c] ) > std::numeric_limits<double>::epsilon() );
			for( auto& li: _data )
				for( auto& e: li )
					e /= _data[r][c];
	}

	mutable std::vector<std::vector<double>> _data;
	mutable bool _isNormalized = false;

	friend std::ostream& operator << ( std::ostream& f, const Homogr& h )
	{
		for( const auto& li: h._data )
		{
			f << "| ";
			for( const auto& e: li )
				f << std::setw(6) << e << ' ';
			f << " |\n";
		}
		return f;
	}
};
//------------------------------------------------------------------
/// "Private" class
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

/// Used in Line2d::getValue()
enum En_GivenCoord { GC_X, GC_Y };

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
		Line2d( double dx, double dy );
		/// Create a line from two points
		Line2d( const Point2d&, const Point2d& );
		/// creates a vertical line
		Line2d(): Root(1.,0.,0.)
		{}
		double distToPoint( const Point2d& pt ) const;
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
		/// Adds vertical offset to line
		void addVertOffset( double v )
		{
			_c = _c - v*_b;
			normalize();
		}
		/// Returns one of the coordinates of a point on the line, given the other one
		double getValue( En_GivenCoord gc, double other )
		{
			if( gc == GC_X )
				return ( -_a * other - _c ) / _b;
			else
				return ( -_b * other - _c ) / _a;
		}
	private:
		Line2d( const Root& r ): Root(r)
		{
			normalize();
		}
		/// Normalise to unit length, and make sure \c a is always >0
		void normalize()
		{
			auto sq = std::hypot( _a, _b );
			_a /= sq;
			_b /= sq;
			_c /= sq;
			if( std::signbit(_a) ) //a allways >0
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
		Point2d( const Line2d&, const Line2d& );
		Line2d operator * ( const Point2d& );
		double getX() const
		{ /// \todo \c c should always be 1 if stored as normalized values, so what's the point here ? Clear out this.
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
	return Point2d( *this, l2 );
}

/// A line is defined by 2 points
inline
Line2d
Point2d::operator * ( const Point2d& p2 )
{
	return Line2d( *this, p2 );
}

//------------------------------------------------------------------
/// Creates a line starting from (0,0) and with the given slope (dx,dy)
inline
Line2d::Line2d( double dx, double dy )
{
	Point2d( dx, dy );
	*this = Point2d() * Point2d( dx, dy );
	normalize();
}
//------------------------------------------------------------------
inline
Line2d::Line2d( const Point2d& p1, const Point2d& p2 )
{
	*this = static_cast<Line2d>(crossProduct( p1, p2 ) );
	normalize();
}
//------------------------------------------------------------------
inline
Point2d::Point2d( const Line2d& l1, const Line2d& l2 )
{
	*this = static_cast<Point2d>(crossProduct( l1, l2 ) );
	normalize();
}
//------------------------------------------------------------------
/// returns slope of line as a unit-length vector (dx, dy), with dx>0
inline
std::pair<double,double>
Line2d::getVector() const
{
	std::pair<double,double> res(_a,_b);
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

\todo Do we really require computation of hypot (because the line is supposed to be normalized, i.e. h=1 ?)
*/
inline
double
Line2d::distToPoint( const Point2d& pt ) const
{
	return std::fabs( (_a * pt.getX() + _b * pt.getY() + _c) / std::hypot( _a, _b ) );
}
//------------------------------------------------------------------




} // namespace homog2d end

#endif
