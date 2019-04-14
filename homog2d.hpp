/**************************************************************************

    This file is part of the C++ library "homog2d", dedicated to
    handle 2D lines and points, see https://github.com/skramm/homog2d

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
#include <algorithm>
#include <vector>
#include <iomanip>
#include <cassert>
#include <sstream>

#ifdef HOMOG2D_USE_OPENCV
	#include "opencv2/imgproc.hpp"
#endif

namespace homog2d {

template<typename LP>
class Root;

//------------------------------------------------------------------
/// A 2D homography (3x3 matrix)
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
	Homogr& addTranslation( T tx, T ty )
	{
		Homogr out;
		out.setTranslation( tx, ty );
		*this = out * *this;
		normalize();
		return *this;
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
	Homogr& addRotation( T theta )
	{
		Homogr out;
		out.setRotation( theta );
		*this = out * *this;
		normalize();
		return *this;
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
	/// Adds the same scale factor to the matrix
	template<typename T>
	Homogr& addScale( T k )
	{
		return this->addScale( k, k );
	}
	/// Adds a scale factor to the matrix
	template<typename T>
	Homogr& addScale( T kx, T ky )
	{
		Homogr out;
		out.setScale( kx, ky );
		*this = out * *this;
		normalize();
		return *this;
	}
	/// Sets the matrix as a scaling transformation
	template<typename T>
	void setScale( T k )
	{
		setScale( k, k );
	}
	/// Sets the matrix as a scaling transformation
	template<typename T>
	void setScale( T kx, T ky )
	{
		clear();
		_data[0][0] = kx;
		_data[1][1] = ky;
		_isNormalized = true;
	}

/*	template<typename LP>
	void
	applyTo( std::vector<Root<LP>>& ) const;
	*/
	template<typename T>
	void
	applyTo( T& ) const;

/// Normalisation
	void normalize() const
	{
		auto eps = std::numeric_limits<double>::epsilon();

		if( std::fabs(_data[2][2]) <= eps ) // if [2][2] is null, then we use [2][1]
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
		Homogr adjugate = p_adjugate();
//		std::cout << "adj=\n" << adjugate << "\n";
//		std::cout << "AVANT DET:\n"  << *this << "\n";

		double det = p_det();

		if( std::fabs(det) <= std::numeric_limits<double>::epsilon() )
			std::runtime_error( "matrix is not invertible" );
//		std::cout << "det=" << det << "\n";
		*this = adjugate / det;
		normalize();
	}
/// Divide all elements by scalar
	Homogr& operator / (double v)
	{
		if( std::fabs(v) <= std::numeric_limits<double>::epsilon() )
			std::runtime_error( "unable to divide by " + std::to_string(v) );
//	std::cout <<"DIV by " << v << "\n";
#if 0
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				_data[i][j] /= v;
		return *this;
#else
		return *this * (1.0/v);
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
	bool operator == ( const Homogr& h ) const
	{
		if( !_isNormalized )
			normalize();
		if( !h._isNormalized )
			h.normalize();

		auto eps = std::numeric_limits<double>::epsilon();
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				if( std::fabs( _data[i][j] - h._data[i][j] ) >= eps )
					return false;
		return true;
	}
/// Comparison operator. Does normalization if required
	bool operator != ( const Homogr& h ) const
	{
		return !(*this == h);
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
//		std::cout << "compute det:\n";
		double det = _data[0][0] * p_det2x2( {2,2, 2,3, 3,2, 3,3} );
//		std::cout << "det1="<<     p_det2x2( {2,2, 2,3, 3,2, 3,3} ) << "\n";

		det       -= _data[0][1] * p_det2x2( {2,1, 2,3, 3,1, 3,3} );
//		std::cout << "det2="<<     p_det2x2( {2,1, 2,3, 3,1, 3,3} ) << "\n";

		det       += _data[0][2] * p_det2x2( {2,1, 2,2, 3,1, 3,2} );
//		std::cout << "det3="<<     p_det2x2( {2,1, 2,2, 3,1, 3,2} ) << "\n";

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
//		std::cout << "p_det2x2: a=" << _data[v[0]][v[1]] << " b=" << _data[v[6]][v[7]] << " c= " << _data[v[2]][v[3]] << " d=" << _data[v[4]][v[5]] << '\n';
		return det;
	}
/// Computes adjugate matrix, see https://en.wikipedia.org/wiki/Adjugate_matrix#3_%C3%97_3_generic_matrix
/**
\todo Decrement each index so we can remove the need for std::transform()
*/
	Homogr p_adjugate()
	{
		Homogr out;

		out.setValue( 0, 0,  p_det2x2( {2,2, 2,3, 3,2, 3,3} ) );
		out.setValue( 0, 1, -p_det2x2( {1,2, 1,3, 3,2, 3,3} ) );
		out.setValue( 0, 2,  p_det2x2( {1,2, 1,3, 2,2, 2,3} ) );

		out.setValue( 1, 0, -p_det2x2( {2,1, 2,3, 3,1, 3,3} ) );
		out.setValue( 1, 1,  p_det2x2( {1,1, 1,3, 3,1, 3,3} ) );
		out.setValue( 1, 2, -p_det2x2( {1,1, 1,3, 2,1, 2,3} ) );

		out.setValue( 2, 0,  p_det2x2( {2,1, 2,2, 3,1, 3,2} ) );
		out.setValue( 2, 1, -p_det2x2( {1,1, 1,2, 3,1, 3,2} ) );
		out.setValue( 2, 2,  p_det2x2( {1,1, 1,2, 2,1, 2,2} ) );

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

#ifdef HOMOG2D_USE_OPENCV
/// Point drawing style
enum PointStyle
{
	PS_PLUS,   ///< "+" symbol
	PS_TIMES,  ///< "times" symbol
	PS_STAR,   ///< "*" symbol
	PS_DIAM   ///< diamond
};
/// Draw parameters for Opencv binding, see Root::drawCvMat()
struct CvDrawParams
{
	cv::Scalar _color         = cv::Scalar(128,128,128);
	int        _lineThickness = 1;
	int        _lineType      = cv::LINE_AA; // or cv::LINE_8
	int        _ptDelta       = 8; // pixels, used for drawing points
	PointStyle _ptStyle       = PS_PLUS;

	CvDrawParams& setPointStyle( PointStyle ps )
	{
		_ptStyle = ps;
		return *this;
	}
	CvDrawParams& setPointSize( int ps )
	{
		assert( ps>1 );
		_ptDelta = ps;
		return *this;
	}
	CvDrawParams& setThickness( int t )
	{
		assert( t>0 );
		_lineThickness = t;
		return *this;
	}
	CvDrawParams& setColor( uint8_t r, uint8_t g, uint8_t b )
	{
		_color = cv::Scalar(b,g,r);
		return *this;
	}
};
#endif

/// Used in Line2d::getValue()
enum En_GivenCoord { GC_X, GC_Y };

/// Used in Line2d::addOffset
enum En_OffsetDir{ OD_Vert, OD_Horiz };

struct IsLine
{
};
struct IsPoint
{
};
//------------------------------------------------------------------
/// Base class, will be instanciated as a Point or a Line
/**
\todo template root type
\todo put back the data as private (needs some tweaks with friend functions)

*/
template<typename LP>
class Root
{
	friend class Homogr;
//	friend Root operator * ( const Homogr&, const Root& );

	template<typename T>
	friend Root<IsPoint> crossProduct( const Root<IsLine>&, const Root<IsLine>& );
	template<typename T>
	friend Root<IsLine> crossProduct( const Root<IsPoint>&, const Root<IsPoint>& );

	private:
	Root( double a, double b, double c )
	{
		_v[0] = a;
		_v[1] = b;
		_v[2] = c;
	}

	public:

	template<typename T> Root( const T&, const T&);
	Root();

	double getValue( En_GivenCoord gc, double other ) const;
	template<typename T>
	void   addOffset( En_OffsetDir dir, T v );
	double getX() const;
	double getY() const;
	void   set( double x, double y );
	double distToPoint( const Root<IsPoint>& pt ) const;

// operators
	bool operator == ( const Root<LP>& other ) const;
	bool operator != ( const Root<LP>& other ) const
	{
		return !(*this == other);
	}
	template<typename T>
	Root<T> operator * ( const Root<LP>& );

// friend functions and operators
	template<class T>
	friend std::ostream& operator << ( std::ostream& f, const Root<T>& r );

// optional stuff
#ifdef HOMOG2D_USE_OPENCV
	void drawCvMat( cv::Mat& mat, CvDrawParams dp=CvDrawParams() );
	cv::Point2d getCvPtd() const;
	cv::Point2f getCvPtf() const;
#endif

//	private:
	double _v[3];

// PRIVATE FUNCTIONS
	void P_normalizeLine();

};

//------------------------------------------------------------------
/// specialization for points (undefined for lines
template<>
double
Root<IsPoint>::getX() const
{
	return _v[0]/_v[2];
}

/// specialization for points (undefined for lines
template<>
double
Root<IsPoint>::getY() const
{
	return _v[1]/_v[2];
}

/// specialization for points (undefined for lines
template<>
void
Root<IsPoint>::set( double x, double y )
{
	_v[0] = x;
	_v[1] = y;
	_v[2] = 1.;
}

/// Normalize to unit length, and make sure \c a is always >0
template<>
void Root<IsLine>::P_normalizeLine()
{
	auto sq = std::hypot( _v[0], _v[1] );
	auto eps = std::numeric_limits<double>::epsilon();
	if( sq <= eps )
		throw std::runtime_error( "unable to normalize line" );
	for( int i=0; i<3; i++ )
		_v[i] /= sq;
	if( std::signbit(_v[0]) ) //a allways >0
		for( int i=0; i<3; i++ )
			_v[i] = -_v[i];
}

/// Specialization for lines (no implementation for points)
template<>
double
Root<IsLine>::getValue( En_GivenCoord gc, double other ) const
{
	if( gc == GC_X )
		return ( -_v[0] * other - _v[2] ) / _v[1];
	else
		return ( -_v[1] * other - _v[2] ) / _v[0];
}

/// Default comparison operator, used for Lines
template<typename LP>
bool
Root<LP>::operator == ( const Root<LP>& other ) const
{
	auto eps = std::numeric_limits<double>::epsilon();
	for( int i=0; i<3; i++ )
		if( std::fabs( _v[i] - other._v[i] ) > eps )
		{
//			std::cout << "v1=" << _v[i] << " v2=" << other._v[i] << "\n";
			return false;
		}
	return true;
}

/// Specialization of comparison operator for points
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

/// Default operator << for lines
template<typename LP>
std::ostream&
operator << ( std::ostream& f, const Root<LP>& r )
{
	f << '[' << r._v[0] << ',' << r._v[1] << ',' << r._v[2] << "] ";
	return f;
}

/// Specialization of operator << for points
template<>
std::ostream&
operator << ( std::ostream& f, const Root<IsPoint>& r )
{
	f << '[' << r.getX() << ',' << r.getY() << "] ";
	return f;
}

/// Inner implementation details
namespace detail
{
	/// Cross product, see https://en.wikipedia.org/wiki/Cross_product#Coordinate_notation
	template<typename Out, typename In>
	Root<Out> crossProduct( const Root<In>& r1, const Root<In>& r2 )
	{
		Root<Out> res;
		res._v[0] = r1._v[1] * r2._v[2] - r1._v[2] * r2._v[1];
		res._v[1] = r1._v[2] * r2._v[0] - r1._v[0] * r2._v[2];
		res._v[2] = r1._v[0] * r2._v[1] - r1._v[1] * r2._v[0];
		for ( int i=0; i<3; i++)
//			std::cout << "v[" << i << "]=" << res._v[i] << " ";
//		std::cout  << '\n';
		return res;
	}
}

Root<IsLine>
operator * ( const Root<IsPoint>& lhs, const Root<IsPoint>& rhs )
{
	auto line = detail::crossProduct<IsLine>(lhs, rhs);
//	std::cout << "LINE BEFORE NORMALIZE:" << line << '\n';
	line.P_normalizeLine();
//	std::cout << "LINE AFTER NORMALIZE:" << line << '\n';
	return line;
}

Root<IsPoint>
operator * ( const Root<IsLine>& lhs, const Root<IsLine>& rhs )
{
	auto pt = detail::crossProduct<IsPoint>(lhs, rhs);
	auto eps = std::numeric_limits<double>::epsilon();
	if( std::fabs(pt._v[2]) <= eps )
	{
		std::ostringstream lh,lr;
		lh << lhs;
		lr << rhs;
		throw std::runtime_error( "unable to compute point from two lines: lhs=" + lh.str() + "rhs=" + lr.str() );
	}
	return pt;
}


///////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////

/// Default constructor, generic function, used for points
template<typename LP>
Root<LP>::Root()
{
	_v[0] = 0.;
	_v[1] = 0.;
	_v[2] = 1.;
//	std::cout << "C1:" << *this << "\n";
}

/// Default constructor, specialization for lines, build vertical line at x=0
template<>
Root<IsLine>::Root()
{
	_v[0] = 1.;
	_v[1] = 0.;
	_v[2] = 0.;
//	std::cout << "C2:" << *this << "\n";;
}

/// generic 2 arg constructor implementation
template<typename LP>
template<typename T>
Root<LP>::Root( const T& v0, const T& v1 )
{
//	std::cout << "C3:" << *this << "\n";
	assert(0);
}

/// constructor of a point from two values (specialization)
template<>
template<typename T>
Root<IsPoint>::Root( const T& v0, const T& v1 )
{
	_v[0] = v0;
	_v[1] = v1;
	_v[2] = 1.;
//	std::cout << "C4:" << *this << "\n";
}


/// constructor of a line from two values (vector dx/dy). (specialization)
template<>
template<typename T>
Root<IsLine>::Root( const T& dx, const T& dy )
{
	*this = detail::crossProduct<IsLine>( Root<IsPoint>(), Root<IsPoint>( dx, dy ) );
	P_normalizeLine();
//	std::cout << "C5:" << *this << "\n";
}

/// constructor of a point from two lines (specialization)
template<>
template<>
Root<IsPoint>::Root( const Root<IsLine>& v1, const Root<IsLine>& v2 )
{
	*this = detail::crossProduct<IsPoint>( v1, v2 );
//	std::cout << "C6:" << *this << "\n";
}

/// Constructor of a line from two points (specialization)
template<>
template<>
Root<IsLine>::Root( const Root<IsPoint>& v1, const Root<IsPoint>& v2 )
{
	*this = detail::crossProduct<IsLine>( v1, v2 );
	P_normalizeLine();
//	std::cout << "C7:" << *this << "\n";
}


//------------------------------------------------------------------
/// Returns distance between the line and point \b pt. Specialization
/**
http://mathworld.wolfram.com/Point-LineDistance2-Dimensional.html
<pre>
        | a.x0 + b.y0 + c |
  d = -----------------------
         sqrt( a*a + b*b )
</pre>
\todo Do we really require computation of hypot ? (because the line is supposed to be normalized, i.e. h=1 ?)
*/
template<>
double
Root<IsLine>::distToPoint( const Root<IsPoint>& pt ) const
{
	return std::fabs( (_v[0] * pt.getX() + _v[1] * pt.getY() + _v[2]) / std::hypot( _v[0], _v[1] ) );
}

//------------------------------------------------------------------
/// Returns distance between the point and another point \b pt. Specialization
template<>
double
Root<IsPoint>::distToPoint( const Root<IsPoint>& pt ) const
{
	return std::hypot( getX() - pt.getX(), getY() - pt.getY() );
}

/// Add offset (vertical or horizontal) to line (implementation for lines)
template<>
template<typename T>
void Root<IsLine>::addOffset( En_OffsetDir dir, T v )
{
	if( dir == OD_Vert )
		_v[2] = _v[2] - v*_v[1];
	else
		_v[2] = _v[2] - v*_v[0];
	P_normalizeLine();
}

//------------------------------------------------------------------
/// Apply homography to a point or line. Free function, templated by point or line
template<typename LP>
Root<LP>
operator * ( const Homogr& h, const Root<LP>& in )
{
	Root<LP> out;
	out._v[2] = 0.;
	for( int i=0; i<3; i++ )
		for( int j=0; j<3; j++ )
			out._v[i] += h._data[i][j] * in._v[j];
	return out;
}
//------------------------------------------------------------------
/// Apply homography to a vector of points or lines. Free function, templated by point or line
/*template<typename LP>
void
Homogr::applyTo( std::vector<Root<LP>>& vin ) const
{
	for( auto& elem: vin )
		elem = *this * elem;
}*/
template<typename T>
void
Homogr::applyTo( T& vin ) const
{
	for( auto& elem: vin )
		elem = *this * elem;
}
//------------------------------------------------------------------
#ifdef HOMOG2D_USE_OPENCV
template<>
cv::Point2d
Root<IsPoint>::getCvPtd() const
{
	return cv::Point2d(
		static_cast<int>(std::round(getX())),
		static_cast<int>(std::round(getY()))
	);
}

template<>
cv::Point2f
Root<IsPoint>::getCvPtf() const
{
	return cv::Point2f( getX(),getY() );
}

//------------------------------------------------------------------
/// Draw line on Cv::Mat. Specialization for lines.
/**
Steps:
 -# builds the 4 corner points of the image
 -# build the 4 corresponding lines (borders of the image)
 -# find the intersection points between the line and these 4 lines. Should find 2
 -# draw a line between these 2 points
*/
template<>
void
Root<IsLine>::drawCvMat( cv::Mat& mat, CvDrawParams dp )
{
	assert( mat.rows > 2 );
	assert( mat.cols > 2 );
	Root<IsPoint> p00(0,        0);
	Root<IsPoint> p01(0,        mat.rows);
	Root<IsPoint> p10(mat.cols, 0);
	Root<IsPoint> p11(mat.cols, mat.rows  );
	Root<IsLine> l[4];
	l[0] = Root<IsLine>( p00, p01 );
	l[1] = Root<IsLine>(      p01, p11 );
	l[2] = Root<IsLine>(           p11, p10 );
	l[3] = Root<IsLine>(                p10, p00 );
	std::vector<Root<IsPoint>> v;
	for( int i=0; i<4; i++ )
	{
		Root<IsPoint> pt;
		bool okFlag(true);
		try{
			pt = *this * l[i];
		}
		catch( const std::exception& err )
		{
			okFlag = false;
		}
		if( okFlag )
			if( pt.getX()>=0 && pt.getX()<=mat.cols )
				if( pt.getY()>=0 && pt.getY()<=mat.rows )
					v.push_back( pt );
	}

/*	if( v.size()>2 )
	{
		for( int i=0; i<v.size(); i++ )
			std::cout << "i=" << i << " pt=" << v[i] << '\n';
	}*/
	if( v.size()>1 )
		cv::line( mat, v[0].getCvPtd(),  v[1].getCvPtd(), dp._color, dp._lineThickness, dp._lineType );
}

//------------------------------------------------------------------
namespace detail {
/// Private helper function, used by Root<IsPoint>::drawCvMat()
void
drawPt( cv::Mat& mat, PointStyle ps, std::vector<cv::Point2d> vpt, const CvDrawParams& dp, bool drawDiag=false )
{
	auto delta  = dp._ptDelta;
	auto delta2 = std::round( 0.85 * delta);
	switch( ps )
	{
		case PS_TIMES:
			vpt[0].x -= delta2;
			vpt[0].y += delta2;
			vpt[1].x += delta2;
			vpt[1].y -= delta2;

			vpt[2].x += delta2;
			vpt[2].y += delta2;
			vpt[3].x -= delta2;
			vpt[3].y -= delta2;
		break;

		case PS_PLUS:
		case PS_DIAM:
			vpt[0].x -= delta;
			vpt[1].x += delta;
			vpt[2].y -= delta;
			vpt[3].y += delta;
		break;
		default: assert(0);
	}
	if( !drawDiag )
	{
		cv::line( mat, vpt[0], vpt[1], dp._color );
		cv::line( mat, vpt[2], vpt[3], dp._color );
	}
	else
	{
		cv::line( mat, vpt[0], vpt[2], dp._color );
		cv::line( mat, vpt[2], vpt[1], dp._color );
		cv::line( mat, vpt[1], vpt[3], dp._color );
		cv::line( mat, vpt[0], vpt[3], dp._color );
	}
}
} // namespace detail
//------------------------------------------------------------------
/// Draw line on Cv::Mat. Specialization for points
template<>
void
Root<IsPoint>::drawCvMat( cv::Mat& mat, CvDrawParams dp )
{
	std::vector<cv::Point2d> vpt( 4, getCvPtd() );
	switch( dp._ptStyle )
	{
		case PS_PLUS:   // "+" symbol
			detail::drawPt( mat, PS_PLUS,  vpt, dp );
		break;

		case PS_STAR:
			detail::drawPt( mat, PS_PLUS,  vpt, dp );
			detail::drawPt( mat, PS_TIMES, vpt, dp );
		break;

		case PS_DIAM:
			detail::drawPt( mat, PS_PLUS,  vpt, dp, true );
		break;

		case PS_TIMES:      ///< "times" symbol
			detail::drawPt( mat, PS_TIMES, vpt, dp );
		break;

		default: assert(0);
	}
}

//------------------------------------------------------------------
#endif // HOMOG2D_USE_OPENCV


typedef Root<IsLine> Line2d;
typedef Root<IsPoint> Point2d;

} // namespace homog2d end


#endif // HG_HOMOG2D_HPP

