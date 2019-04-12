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
\file homog2d_T.hpp
\brief single header file, implements some 2D homogeneous stuff.
See https://github.com/skramm/homog2d
*/

#ifndef HG_HOMOG2DT_HPP
#define HG_HOMOG2DT_HPP

#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <cassert>

#ifdef HOMOG2D_USE_OPENCV
	#include "opencv2/imgproc.hpp"
#endif

namespace homog2d {

// forward declaration
//class Point2d;

template<typename LP>
class Root;


//------------------------------------------------------------------
/// A 2D homography (3x3 matrix)
//template<typename LP>
class Homogr
{
	template<typename LP>
	friend Root<LP> operator * ( const Homogr& h, const Root<LP>& in );
	public:
	/// Default constructor, initialize to unit transformation
	Homogr()
	{
		clear();
	}
	/// Build a rotation transformation of \c val radians
	template<typename T>
	Homogr( T val )
	{
		setRotation( val );
	}

/// Constructor, used to fill with another "vector of vector" matrix (or std::array)
/** \warning
- Input matrix \b must be 3 x 3, but type can be anything that can be copied to \c double
- no checking is done on validity of matrix as an homography.
Thus some assert can be triggered elsewhere
*/

	template<typename T>
	Homogr( const std::vector<std::vector<T>>& in )
	{
		assert( in.size() == 3 );
		for( auto li: in )
			assert( li.size() == 3 );
		p_fillWith( in );
	}
	template<typename T>
	Homogr( const std::array<std::array<T,3>,3>& in )
	{
		p_fillWith( in );
	}

	void clear()
	{
		for( auto& li: _data )
			for( auto& elem: li )
				elem = 0.;
		_data[0][0] = 1.;
		_data[1][1] = 1.;
		_data[2][2] = 1.;
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
	/// Adds a translation \c tx,ty to the matrix
	template<typename T>
	void addTranslation( T tx, T ty )
	{
		Homogr out;
		out.setTranslation( tx, ty );
		*this = out * *this;
		normalize();
	}
	/// Sets the matrix as a translation \c tx,ty
	template<typename T>
	void setTranslation( T tx, T ty )
	{
		clear();
		_data[0][2] = tx;
		_data[1][2] = ty;
		_isNormalized = true;
	}
	/// Adds a rotation with an angle \c theta (radians) to the matrix
	template<typename T>
	void addRotation( T theta )
	{
		Homogr out;
		out.setRotation( theta );
		*this = out * *this;
		normalize();
	}
	/// Sets the matrix as a rotation with an angle \c theta (radians)
	template<typename T>
	void setRotation( T theta )
	{
		clear();
		_data[0][0] = _data[1][1] = std::cos(theta);
		_data[1][0] = std::sin(theta);
		_data[0][1] = -_data[1][0];
		_isNormalized = true;
	}
	/// Adds a scale factor to the matrix
	template<typename T>
	void addScale( T kx, T ky )
	{
		Homogr out;
		out.setScale( kx, ky );
		*this = out * *this;
		normalize();
	}
	/// Sets the matrix as a scaling
	template<typename T>
	void setScale( T kx, T ky )
	{
		clear();
		_data[0][0] = kx;
		_data[1][1] = ky;
		_isNormalized = true;
	}

/// Normalisation
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
/// Transpose matrix
	void transpose()
	{
		Homogr out;
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				out._data[i][j] = _data[j][i];
		*this = out;
	}
/// Inverse matrix \todo UNTESTED YET !
	void inverse()
	{
		Homogr adjugate = p_adjugate() / p_det();
		*this = adjugate;
	}
/// Divide all elements by scalar
	Homogr& operator / (double v)
	{
#if 0
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				_data[i][j] /= v;
		return *this;
#else
		return *this * 1.0/v;
#endif
	}
/// Multiply all elements by scalar
	Homogr& operator * (double v)
	{
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				_data[i][j] *= v;
		return *this;
	}

/// Matrix multiplication
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

/// Comparison operator. Does normalization if required
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

//////////////////////////
//   PRIVATE FUNCTIONS  //
//////////////////////////
	private:
	void p_zero()
	{
		for( auto& li: _data )
			for( auto& e: li )
				e = 0.;
	}
	template<typename T>
	void p_fillWith( const T& in )
	{
		for( auto i=0; i<3; i++ )
			for( auto j=0; j<3; j++ )
				_data[i][j] = in[i][j];
		normalize();
	}
	void p_divideBy( size_t r, size_t c ) const
	{
		assert( std::fabs( _data[r][c] ) > std::numeric_limits<double>::epsilon() );
			for( auto& li: _data )
				for( auto& e: li )
					e /= _data[r][c];
	}
/// Return determinant of matrix
/**
See https://en.wikipedia.org/wiki/Determinant
*/
	double p_det()
	{
		double det = _data[0][0] * p_det2x2( {2,2, 2,3, 3,2, 3,3} );
		det       -= _data[0][1] * p_det2x2( {2,1, 2,3, 3,1, 3,3} );
		det       += _data[0][2] * p_det2x2( {2,1, 2,2, 3,1, 3,2} );
		return det;
	}
	double p_det2x2( std::vector<int> v )
	{
		std::transform(
			std::begin(v),
			std::end(v),
			std::begin(v),
			[](int& a){ return a-1;} // lambda
		);
		auto det = _data[v[0]][v[1]] * _data[v[6]][v[7]];
		det -= _data[v[2]][v[3]] * _data[v[4]][v[5]];
		return det;
	}
/// Computes adjugate matrix, see https://en.wikipedia.org/wiki/Adjugate_matrix#3_%C3%97_3_generic_matrix
/**
\todo Decrement each index so we can remove the need for std::transform()
*/
	Homogr p_adjugate()
	{
		Homogr out;
		setValue( 0, 0,  p_det2x2( {2,2, 2,3, 3,2, 2,3} ) );
		setValue( 0, 1, -p_det2x2( {1,2, 1,3, 3,2, 3,3} ) );
		setValue( 0, 2,  p_det2x2( {1,2, 1,3, 2,2, 2,3} ) );

		setValue( 1, 0, -p_det2x2( {2,1, 2,3, 3,1, 3,3} ) );
		setValue( 1, 1,  p_det2x2( {1,1, 1,3, 3,1, 3,3} ) );
		setValue( 1, 2, -p_det2x2( {1,1, 1,3, 2,1, 2,3} ) );

		setValue( 2, 0,  p_det2x2( {2,1, 2,2, 3,1, 3,2} ) );
		setValue( 2, 1, -p_det2x2( {1,1, 1,2, 3,1, 3,2} ) );
		setValue( 2, 2,  p_det2x2( {1,1, 1,2, 2,1, 2,2} ) );

		return out;
	}

//////////////////////////
//      DATA SECTION    //
//////////////////////////
	private:
	mutable std::array<std::array<double,3>,3> _data;
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


struct IsLine
{
};
struct IsPoint
{
};
//------------------------------------------------------------------
/// "Private" class
/// \todo template root type
template<typename LP>
class Root
{
	friend class Homogr;
//	friend Root operator * ( const Homogr&, const Root& );
	public:
//	protected:
	Root( double a=0., double b=0., double c=1. )
	{
		_v[0] = a;
		_v[1] = b;
		_v[2] = c;
	}

	private:
	double _v[3];

	/// Cross product, see https://en.wikipedia.org/wiki/Cross_product#Coordinate_notation
	friend Root<LP> crossProduct( const Root<LP>& r1, const Root<LP>& r2 )
	{
		Root<LP> res;
		std::cout << "CP r1=" << r1 << " r2=" << r2 << "\n";
		res._v[0] = r1._v[1] * r2._v[2] - r1._v[2] * r2._v[1];
		res._v[1] = r1._v[2] * r2._v[0] - r1._v[0] * r2._v[2];
		res._v[2] = r1._v[0] * r2._v[1] - r1._v[1] * r2._v[0];
		std::cout << "CP res=" << res << "\n";
		return res;
	}

	public:

	template<class T>
	friend std::ostream& operator << ( std::ostream& f, const Root<T>& r );

	bool operator == ( const Root<LP>& other ) const;

	double getX() const;
	double getY() const;
	void set( double x, double y );

	template<typename T>
	Root<T> operator * ( const Root<LP>& );
};

/// default comparison operator, used for Lines
template<typename LP>
bool
Root<LP>::operator == ( const Root<LP>& other ) const
{
	auto eps = std::numeric_limits<double>::epsilon();
	for( int i=0; i<3; i++ )
		if( std::fabs( _v[i] - other._v[i] ) > eps )
		{
			std::cout << "v1=" << _v[i] << " v2=" << other._v[i] << "\n";
			return false;
		}
	return true;
}

/// specialization of comparison operator for points
template<>
bool
Root<IsPoint>::operator == ( const Root<IsPoint>& other ) const
{
	auto eps = std::numeric_limits<double>::epsilon();
	if( std::fabs( getX() - other.getX() ) > eps )
		return false;
	if( std::fabs( getY() - other.getY() ) > eps )
		return false;
	return true;
}

/// default operator << for lines
template<typename LP>
std::ostream&
operator << ( std::ostream& f, const Root<LP>& r )
{
	f << '[' << r._v[0] << ',' << r._v[1] << ',' << r._v[2] << "] ";
	return f;
}

/// specialization of operator << for points
template<>
std::ostream&
operator << ( std::ostream& f, const Root<IsPoint>& r )
{
	f << '[' << r.getX() << ',' << r.getY() << "] ";
	return f;
}


template<typename LP,typename T>
Root<T> Root<LP>::operator * ( const Root<LP>& other )
{
	static_assert( std::is_same<LP,T>::value, "cannot return same type" );

}


// specialization for points
template<>
double
Root<IsPoint>::getX() const
{
	return _v[0]/_v[2];
}
template<>
double
Root<IsPoint>::getY() const
{
	return _v[1]/_v[2];
}

template<>
void
Root<IsPoint>::set( double x, double y )
{
	_v[0] = x;
	_v[1] = y;
	_v[2] = 1.;
}








/// Used in Line2d::getValue()
enum En_GivenCoord { GC_X, GC_Y };
/// Used in Line2d::addOffset
enum En_OffsetDir{ OD_Vert, OD_Horiz };

#if 0
//------------------------------------------------------------------
/// Homogeneous 2D line
class Line2d : public Root
{
	friend class Point2d;
	friend Line2d operator * ( const Homogr& h, const Line2d& in );

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
			return Root::operator == (li);
		}

/// Add offset (vertical or horizontal) to line
	template<typename T>
	void addOffset( En_OffsetDir dir, T v )
	{
		if( dir == OD_Vert )
			_v[2] = _v[2] - v*_v[1];
		else
			_v[2] = _v[2] - v*_v[0];
		normalize();
	}

/// Returns one of the coordinates of a point on the line, given the other one
		double getValue( En_GivenCoord gc, double other )
		{
			if( gc == GC_X )
				return ( -_v[0] * other - _v[2] ) / _v[1];
			else
				return ( -_v[1] * other - _v[2] ) / _v[0];
		}

#ifdef HOMOG2D_USE_OPENCV
		void drawCvMat( cv::Mat& mat, const cv::Scalar& color, int thickness = 1, int lineType = cv::LINE_8 );
#endif

	private:
		explicit Line2d( const Root& r ): Root(r)
		{
			normalize();
		}
/// Normalise to unit length, and make sure \c a is always >0
		void normalize()
		{
			auto sq = std::hypot( _v[0], _v[1] );
			for( int i=0; i<3; i++ )
				_v[i] /= sq;
			if( std::signbit(_v[0]) ) //a allways >0
				for( int i=0; i<3; i++ )
					_v[i] = -_v[i];
		}
};

//------------------------------------------------------------------
/// Homogeneous 2D point
/**
Values are stored "as is" (not normalized), so we can handle far away values
*/
class Point2d : public Root
{
	friend class Line2d;
	friend Point2d operator * ( const Homogr& h, const Point2d& in );

	friend std::ostream& operator << ( std::ostream& f, const Point2d& pt )
	{
#if 0
		f << static_cast<Root>(pt);
#else
		f << '[' << pt.getX() << ',' << pt.getY() << ']';
#endif
		return f;
	}

	public:
		Point2d( double x=0., double y=0. ) : Root(x, y, 1.)
		{
		}
		template<typename T>
		Point2d( T pt ) : Root( pt.x, pt.y, 1.)
		{}

		Point2d( const Line2d&, const Line2d& );
		Line2d operator * ( const Point2d& );
		double getX() const
		{
			return _v[0]/_v[2];
		}
		double getY() const
		{
			return _v[1]/_v[2];
		}
		void set( double x, double y )
		{
			_v[0] = x;
			_v[1] = y;
			_v[2] = 1.;
		}

		double distToPoint( const Point2d& pt ) const;

		bool operator == ( const Point2d& pt ) const
		{
			auto eps = std::numeric_limits<double>::epsilon();
			if( std::fabs( getX() - pt.getX() ) > eps )
				return false;
			if( std::fabs( getY() - pt.getY() ) > eps )
				return false;
			return true;
		}
#ifdef HOMOG2D_USE_OPENCV
		cv::Point2d getCvPtd() const
		{
			return cv::Point2d(
				static_cast<int>(std::round(getX())),
				static_cast<int>(std::round(getY()))
			);
		}
		cv::Point2f getCvPtf() const
		{
			return cv::Point2f( getX(),getY() );
		}
#endif
	private:
//		explicit Point2d( const Root& r ): Root(r)
//		{}
/*		void normalize()
		{
			_v[0] /= _v[2];
			_v[1] /= _v[2];
			_v[2] /= 1.;
		}*/
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
	*this = Point2d() * Point2d( dx, dy );
	normalize();
}
//------------------------------------------------------------------
inline
Line2d::Line2d( const Point2d& p1, const Point2d& p2 )
{
//	*this = static_cast<Line2d>(crossProduct( p1, p2 ) );
	*this = crossProduct( p1, p2 );
	normalize();
}
//------------------------------------------------------------------
inline
Point2d::Point2d( const Line2d& l1, const Line2d& l2 )
{
//	*this = static_cast<Point2d>(crossProduct( l1, l2 ) );
	*this = crossProduct( l1, l2 );
	std::cout << "Point2d::Point2d(): l1=" << l1 << " l2=" << l2 << " res=" << *this << "\n";
//	normalize();
}
//------------------------------------------------------------------
/// returns slope of line as a unit-length vector (dx, dy), with dx>0
inline
std::pair<double,double>
Line2d::getVector() const
{
	std::pair<double,double> res(_v[0],_v[1]);
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
	return std::fabs( (_v[0] * pt.getX() + _v[1] * pt.getY() + _v[2]) / std::hypot( _v[0], _v[1] ) );
}

//------------------------------------------------------------------
/// Returns distance between the point and another point \b pt
inline
double
Point2d::distToPoint( const Point2d& pt ) const
{
	return std::hypot( getX() - pt.getX(), getY() - pt.getY() );
}

//------------------------------------------------------------------
/// Apply homography to a point or line
Root
operator * ( const Homogr& h, const Root& in )
{
	Root out;
	out._v[2] = 0.;
	for( int i=0; i<3; i++ )
		for( int j=0; j<3; j++ )
			out._v[i] += h._data[i][j] * in._v[j];
	return out;
}
//------------------------------------------------------------------
/// Apply homography to a point
Point2d
operator * ( const Homogr& h, const Point2d& in )
{
	return static_cast<Point2d>( h * static_cast<Root>(in) );
}
//------------------------------------------------------------------
/// Apply homography to a line
Line2d
operator * ( const Homogr& h, const Line2d& in )
{
	return h * static_cast<Root>(in);
//	return h * in;
}
//------------------------------------------------------------------
#ifdef HOMOG2D_USE_OPENCV
/// Draw the line on image \c mat
inline
void
Line2d::drawCvMat( cv::Mat& mat, const cv::Scalar& color, int thickness, int lineType )
{
//	std::cout << "drawCvMat line= " << *this << "\n";
	Point2d p00;
	Point2d p01(0,mat.cols);
	Point2d p10(mat.rows,0);
	Point2d p11(mat.rows,mat.cols);
	Line2d l[4];
	l[0] = Line2d( p00, p01 );
	l[1] = Line2d(      p01, p11 );
	l[2] = Line2d(           p11, p10 );
	l[3] = Line2d(                p10, p00 );
	std::vector<Point2d> v;
	for( int i=0; i<4; i++ )
	{
		Point2d pt = *this * l[i];
		std::cout << "line: " << l[i] << " pt=" << pt << '\n';
		if( pt.getX()>0 && pt.getX()<mat.cols )
			if( pt.getY()>0 && pt.getY()<mat.rows )
				v.push_back( pt );
	}
/*	std::cout << "v size=" << v.size() << '\n';

	if( v.size()>2 )
	{
		for( int i=0; i<v.size(); i++ )
			std::cout << "i=" << i << " pt=" << v[i] << '\n';
	}*/
	if( v.size()>1 )
		cv::line( mat, v[0].getCvPtd(),  v[1].getCvPtd(), color, thickness, lineType );
}
#endif
//------------------------------------------------------------------

#endif //0

typedef Root<IsLine> Line2d;
typedef Root<IsPoint> Point2d;

} // namespace homog2d end


#endif

