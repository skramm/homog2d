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
#include <set>
#include <vector>
#include <iomanip>
#include <cassert>
#include <sstream>

#ifdef HOMOG2D_USE_OPENCV
	#include "opencv2/imgproc.hpp"
#endif

namespace homog2d {

// forward declaration
template<typename LP>
class Root;

//------------------------------------------------------------------
/// A 2D homography, defining a planar transformation
/**
To define an affine or rigid transformation, you can use:
- setRotation()
- setTranslation()
- setScale()

To add an affine or rigid transformation to the current one, you can use:
- addRotation()
- addTranslation()
- addScale()

To return to unit transformation, use clear()

Implemented as a 3x3 matrix
 */
class Homogr
{
	template<typename LP>
	friend Root<LP> operator * ( const Homogr& h, const Root<LP>& in );

	template<typename LP>
	friend class Root;

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
		if( in.size() != 3 )
			throw std::runtime_error( "Invalid line size for input: " + std::to_string(in.size()) );
		for( auto li: in )
			if( li.size() != 3 )
				throw std::runtime_error( "Invalid column size for input: " + std::to_string(li.size()) );

		p_fillWith( in );
	}
	template<typename T>
	Homogr( const std::array<std::array<T,3>,3>& in )
	{
		p_fillWith( in );
	}

/// Initialize to unit transformation
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
	void setValue(
		size_t r, ///< row
		size_t c, ///< col
		double v  ///< value
	)
	{
		_data[r][c] = v;
		_isNormalized = false;
	}
/// Getter
	double getValue( size_t r, size_t c ) const
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
//		normalize();
		return *this;
	}
/// Sets the matrix as a translation \c tx,ty
	template<typename T>
	Homogr& setTranslation( T tx, T ty )
	{
		clear();
		_data[0][2] = tx;
		_data[1][2] = ty;
		_isNormalized = true;
		return *this;
	}
/// Adds a rotation with an angle \c theta (radians) to the matrix
	template<typename T>
	Homogr& addRotation( T theta )
	{
		Homogr out;
		out.setRotation( theta );
		*this = out * *this;
//		normalize();
		return *this;
	}
/// Sets the matrix as a rotation with an angle \c theta (radians)
	template<typename T>
	Homogr& setRotation( T theta )
	{
		clear();
		_data[0][0] = _data[1][1] = std::cos(theta);
		_data[1][0] = std::sin(theta);
		_data[0][1] = -_data[1][0];
		_isNormalized = true;
		return *this;
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
//		normalize();
		return *this;
	}
/// Sets the matrix as a scaling transformation (same on two axis)
	template<typename T>
	Homogr& setScale( T k )
	{
		return setScale( k, k );
	}
/// Sets the matrix as a scaling transformation
	template<typename T>
	Homogr& setScale( T kx, T ky )
	{
		clear();
		_data[0][0] = kx;
		_data[1][1] = ky;
		_isNormalized = true;
		return *this;
	}

	template<typename T>
	void
	applyTo( T& ) const;

/// Normalisation
	void normalize() const
	{
		auto eps = std::numeric_limits<double>::epsilon();

		if( std::fabs(_data[2][2]) > eps ) // if [2][2] is null, then we use [2][1]
			p_divideBy( 2, 2 );
		else
		{
			if( std::fabs(_data[2][1]) > eps )
				p_divideBy( 2, 1 );
			else
				p_divideBy( 2, 0 );
		}
		if( std::signbit(_data[2][2]) )
			for( auto& li: _data )
				for( auto& e: li )
					e = -e;
		_isNormalized = true;
	}
/// Transpose matrix
	Homogr& transpose()
	{
		Homogr out;
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				out._data[i][j] = _data[j][i];
		*this = out;
		return *this;
	}
/// Inverse matrix
	Homogr& inverse()
	{
		Homogr adjugate = p_adjugate();
		double det = p_det();

		if( std::abs(det) <= std::numeric_limits<double>::epsilon() )
			throw std::runtime_error( "matrix is not invertible" );

		*this = adjugate / det;

		normalize();
		return *this;
	}

/// Divide all elements by scalar
	Homogr& operator / (double v)
	{
		if( std::fabs(v) <= std::numeric_limits<double>::epsilon() )
			throw std::runtime_error( "unable to divide by " + std::to_string(v) );
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
//------------------------------------------------------------------
/// Point drawing style
enum PointStyle
{
	PS_PLUS,   ///< "+" symbol
	PS_TIMES,  ///< "times" symbol
	PS_STAR,   ///< "*" symbol
	PS_DIAM    ///< diamond
};

//------------------------------------------------------------------
/// Draw parameters for Opencv binding, see Root::drawCvMat()
struct CvDrawParams
{
/// Inner struct, holds the values. Needed so we can assign a default value as static member
	struct Dp_values
	{
		cv::Scalar _color         = cv::Scalar(128,128,128);
		int        _lineThickness = 1;
		int        _lineType      = cv::LINE_AA; // or cv::LINE_8
		int        _ptDelta       = 8; // pixels, used for drawing points
		PointStyle _ptStyle       = PS_PLUS;
	};
	Dp_values _dpValues;

	private:
	static Dp_values& p_getDefault()
	{
		static Dp_values s_defValue;
		return s_defValue;
	}

	public:
	CvDrawParams()
	{
		_dpValues = p_getDefault();
	}
	void setDefault()
	{
		p_getDefault() = this->_dpValues;
	}
	static void resetDefault()
	{
		p_getDefault() = Dp_values();
	}
	CvDrawParams& setPointStyle( PointStyle ps )
	{
		_dpValues._ptStyle = ps;
		return *this;
	}
	CvDrawParams& setPointSize( int ps )
	{
		assert( ps>1 );
		_dpValues._ptDelta = ps;
		return *this;
	}
	CvDrawParams& setThickness( int t )
	{
		assert( t>0 );
		_dpValues._lineThickness = t;
		return *this;
	}
	CvDrawParams& setColor( uint8_t r, uint8_t g, uint8_t b )
	{
		_dpValues._color = cv::Scalar(b,g,r);
		return *this;
	}
};
#endif
//------------------------------------------------------------------

/// Used in Line2d::getValue() and getOrthogonalLine()
enum En_GivenCoord { GC_X, GC_Y };

/// Used in Line2d::addOffset
enum En_OffsetDir{ OD_Vert, OD_Horiz };

struct IsLine
{
};
struct IsPoint
{
};

// forward declaration of class
//template<typename T> class Root;

// forward declaration of two template instanciation
namespace detail {

template<typename T1,typename T2>
Root<T1> crossProduct( const Root<T2>&, const Root<T2>& );

}

//------------------------------------------------------------------
/// Base class, will be instanciated as a Point or a Line
/**
\todo template root type
*/

template<typename LP>
class Root
{
	friend class Homogr;

	friend Root<IsLine>  operator * (  const Root<IsPoint>&, const Root<IsPoint>& );
	friend Root<IsPoint> operator * (  const Root<IsLine>&,  const Root<IsLine>& );

	template<typename T>
	friend Root<T> operator * ( const Homogr& h, const Root<T>& in );

template<typename T1,typename T2>
friend Root<T1> detail::crossProduct( const Root<T2>&, const Root<T2>& );

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
	Root<IsPoint> getPoint( En_GivenCoord gc, double other ) const;
	Root<LP> getOrthogonalLine( En_GivenCoord gc, double other ) const;

	template<typename T>
	void   addOffset( En_OffsetDir dir, T v );
	double getX() const;
	double getY() const;
	void   set( double x, double y );
	double distToPoint( const Root<IsPoint>& pt ) const;
	double getAngle( const Root<IsLine>& ) const;

/// Sub-type, holds result of rectangle intersection, see intersectsRectangle()
	template<typename T>
	struct RectIntersect_
	{
		template<typename U>
		friend class Root;

		public:
		bool operator()() const
		{
			return _doesIntersect;
		}
		RectIntersect_()
		{}
		RectIntersect_( const Root<T>& p1, const Root<T>& p2 ) : ptA(p1), ptB(p2)
		{
			_doesIntersect = true;
		}
		Root<T> ptA;
		Root<T> ptB;

		private:
		bool _doesIntersect = false;
	};

	RectIntersect_<IsPoint> intersectsRectangle( const Root<IsPoint>& pt1, const Root<IsPoint>& pt2 ) const;

// operators
	bool operator == ( const Root<LP>& other ) const;
	bool operator != ( const Root<LP>& other ) const
	{
		return !(*this == other);
	}

// friend functions and operators
	template<typename T>
	friend std::ostream& operator << ( std::ostream& f, const Root<T>& r );

// optional stuff
#ifdef HOMOG2D_USE_OPENCV
	bool drawCvMat( cv::Mat& mat, CvDrawParams dp=CvDrawParams() );
	cv::Point2d getCvPtd() const;
	cv::Point2f getCvPtf() const;
#endif

	private:
	double _v[3];

//////////////////////////
//   PRIVATE FUNCTIONS  //
//////////////////////////

	void p_normalizeLine();
};

//------------------------------------------------------------------
/// specialization for points (undefined for lines)
template<>
double
Root<IsPoint>::getX() const
{
	return _v[0]/_v[2];
}

/// specialization for points (undefined for lines)
template<>
double
Root<IsPoint>::getY() const
{
	return _v[1]/_v[2];
}

/// specialization for points (undefined for lines)
template<>
void
Root<IsPoint>::set( double x, double y )
{
	_v[0] = x;
	_v[1] = y;
	_v[2] = 1.;
}

//------------------------------------------------------------------
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

//------------------------------------------------------------------
/// Normalize to unit length, and make sure \c a is always >0
template<>
void Root<IsLine>::p_normalizeLine()
{
	auto sq = std::hypot( _v[0], _v[1] );
	if( sq <= std::numeric_limits<double>::epsilon() )
		throw std::runtime_error( "unable to normalize line" );
	for( int i=0; i<3; i++ )
		_v[i] /= sq;
	if( std::signbit(_v[0]) ) //a allways >0
		for( int i=0; i<3; i++ )
			_v[i] = -_v[i];

	if( _v[0] == 0. ) // then, change sign so that b>0
		if( std::signbit(_v[1]) )
		{
			_v[1] = - _v[1];
			_v[2] = - _v[2];
		}
}

//------------------------------------------------------------------
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

//------------------------------------------------------------------
template<>
Root<IsPoint>
Root<IsLine>::getPoint( En_GivenCoord gc, double other ) const
{
	auto coord = getValue( gc, other );
	if( gc == GC_X )
		return Root<IsPoint>( other, coord );
	return Root<IsPoint>( coord, other );
}
//------------------------------------------------------------------
///////////////////////////////////////////
// DEFAULT CONSTRUCTORS
///////////////////////////////////////////

/// Default constructor, generic function, used for points
template<typename LP>
Root<LP>::Root()
{
	_v[0] = 0.;
	_v[1] = 0.;
	_v[2] = 1.;
}

/// Default constructor, specialization for lines, build vertical line at x=0
template<>
Root<IsLine>::Root()
{
	_v[0] = 1.;
	_v[1] = 0.;
	_v[2] = 0.;
}

//------------------------------------------------------------------
/// Returns an orthogonal line, at gc=other
template<>
Root<IsLine>
Root<IsLine>::getOrthogonalLine( En_GivenCoord gc, double val ) const
{
	Root<IsLine> out;
	auto other_val = getValue( gc, val );

	Root<IsPoint> pt( other_val, val ) ;
	if( gc == GC_X )
		pt.set( val, other_val );

	out._v[0] = -_v[1];
	out._v[1] =  _v[0];
	out._v[2] = _v[1] * pt.getX() - _v[0] * pt.getY();
	out.p_normalizeLine();
	return out;
}

//------------------------------------------------------------------
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

//------------------------------------------------------------------
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
//		for ( int i=0; i<3; i++)
//			std::cout << "v[" << i << "]=" << res._v[i] << " ";
//		std::cout  << '\n';
		return res;
	}
}

//------------------------------------------------------------------
/// Product of two points is a line (free function)
Root<IsLine>
operator * ( const Root<IsPoint>& lhs, const Root<IsPoint>& rhs )
{
	Root<IsLine> line = detail::crossProduct<IsLine>(lhs, rhs);
	line.p_normalizeLine();
	return line;
}

//------------------------------------------------------------------
/// Product of two lines is a point (free function)
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


//------------------------------------------------------------------
///////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////

/// generic 2 arg constructor implementation
template<typename LP>
template<typename T>
Root<LP>::Root( const T& v0, const T& v1 )
{
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
}

/// constructor of a line from two values (vector dx/dy). (specialization)
template<>
template<typename T>
Root<IsLine>::Root( const T& dx, const T& dy )
{
	*this = detail::crossProduct<IsLine>( Root<IsPoint>(), Root<IsPoint>( dx, dy ) );
	p_normalizeLine();
}

/// constructor of a point from two lines (specialization)
template<>
template<>
Root<IsPoint>::Root( const Root<IsLine>& v1, const Root<IsLine>& v2 )
{
	*this = detail::crossProduct<IsPoint>( v1, v2 );
}

/// Constructor of a line from two points (specialization)
template<>
template<>
Root<IsLine>::Root( const Root<IsPoint>& v1, const Root<IsPoint>& v2 )
{
	*this = detail::crossProduct<IsLine>( v1, v2 );
	p_normalizeLine();
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
/*	std::cout << std::scientific << "distToPoint: h=" << std::hypot( _v[0], _v[1] )
		<<" x=" << pt.getX() << " y=" << pt.getY()
		<<  "\n p1=" << _v[0] * pt.getX()  << " p2=" << _v[1] * pt.getY() <<  " p3=" << _v[2] << '\n';
*/	return std::fabs( _v[0] * pt.getX() + _v[1] * pt.getY() + _v[2] ) / std::hypot( _v[0], _v[1] );
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
	p_normalizeLine();
}

//------------------------------------------------------------------
/// Returns the angle (in Rad) between the line and another one.
/**
Will return a value in the range [0,M_PI/2]
*/
template<>
double
Root<IsLine>::getAngle( const Root<IsLine>& li ) const
{
	double res = _v[0] * li._v[0] + _v[1] * li._v[1];
	res /= std::sqrt( _v[0]*_v[0] + _v[1]*_v[1] ) * std::sqrt( li._v[0]*li._v[0] + li._v[1]*li._v[1] );
	return std::acos( std::abs(res) );
}
//------------------------------------------------------------------
/// Checks for intersection with flat rectangle defined by the two points p00 and p11
/**
Pre-conditions: points are different (throws if not)
*/
template<>
Root<IsLine>::RectIntersect_<IsPoint>
Root<IsLine>::intersectsRectangle( const Root<IsPoint>& p0, const Root<IsPoint>& p1 ) const
{
	if( p0.getX() == p1.getX() || p0.getY() == p1.getY() )
		throw std::runtime_error( "error: a coordinate of the 2 points are identical, does not define a rectangle" );

	Root<IsPoint> p00( std::min(p0.getX(), p1.getX()), std::min(p0.getY(), p1.getY()) );
	Root<IsPoint> p11( std::max(p0.getX(), p1.getX()), std::max(p0.getY(), p1.getY()) );

	Root<IsPoint> p01( p11.getX(), p00.getY() );
	Root<IsPoint> p10( p00.getX(), p11.getY() );

	Root<IsLine> l[4];
	l[0] = Root<IsLine>( p00, p01 );
	l[1] = Root<IsLine>(      p01, p11 );
	l[2] = Root<IsLine>(           p11, p10 );
	l[3] = Root<IsLine>(                p10, p00 );

	std::vector<Root<IsPoint>> vec;
	for( int i=0; i<4; i++ )         // compare with each of the 4 lines
	{
		if( *this == l[i] )          // if same line, then we are done: return the two points
			return Root<IsLine>::RectIntersect_<IsPoint>( p00, p11 );
		else
		{
			Root<IsPoint> pt;
			bool okFlag(true);
			try{
				pt = *this * l[i];
			}
			catch( const std::exception& )
			{
				okFlag = false; // lines are parallel
			}
			if( okFlag )
			{
				if( pt.getX() >= p00.getX() && pt.getX() <= p11.getX() )
					if( pt.getY() >= p00.getY() && pt.getY() <= p11.getY() )
						vec.push_back( pt );
			}
		}
	}

	Root<IsLine>::RectIntersect_<IsPoint> out;
	if( vec.size() > 1 )                                // if more than one point was found, then
	{
		std::vector<Root<IsPoint>> vec2( 1, vec[0] );   // build a second vector, holding the first found point as first element
		for( size_t i=1; i<vec.size(); i++ )            // and add the other points, only once
		{
			if(
				std::find( std::begin( vec2 ), std::end( vec2 ), vec[i] ) // if not already stored,
				== std::end( vec2 )
			)
				vec2.push_back( vec[i] );                                 // then, add the point
		}

		if( vec2.size() > 1 )
		{
			out._doesIntersect = true;
			out.ptA = vec2[0];
			out.ptB = vec2[1];
		}
	}
	return out;
}

typedef Root<IsLine>::RectIntersect_<IsPoint> RectIntersect;

//------------------------------------------------------------------
/// Apply homography to a point or line. Free function, templated by point or line
template<typename LP>
Root<LP>
operator * ( const Homogr& h, const Root<LP>& in )
{
	Root<LP> out;
	for( int i=0; i<3; i++ )
	{
		out._v[i]  = h._data[i][0] * in._v[0];
		out._v[i] += h._data[i][1] * in._v[1];
		out._v[i] += h._data[i][2] * in._v[2];
	}
	return out;
}
//------------------------------------------------------------------
/// Apply homography to a vector of points or lines. Free function, templated by point or line
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
Returns false if line is not in image.

Steps:
 -# builds the 4 corner points of the image
 -# build the 4 corresponding lines (borders of the image)
 -# find the intersection points between the line and these 4 lines. Should find 2
 -# draw a line between these 2 points
*/
template<>
bool
Root<IsLine>::drawCvMat( cv::Mat& mat, CvDrawParams dp )
{
	assert( mat.rows > 2 );
	assert( mat.cols > 2 );

	Root<IsPoint> pt1; // 0,0
	Root<IsPoint> pt2( mat.cols-1, mat.rows-1 );
    RectIntersect ri = this->intersectsRectangle( pt1,  pt2 );
    if( ri() )
    {
		cv::Point2d ptcv1 = ri.ptA.getCvPtd();
		cv::Point2d ptcv2 = ri.ptB.getCvPtd();
		cv::line( mat, ptcv1, ptcv2, dp._dpValues._color, dp._dpValues._lineThickness, dp._dpValues._lineType );
		return true;
	}
	return false;
}

//------------------------------------------------------------------
namespace detail {
/// Private helper function, used by Root<IsPoint>::drawCvMat()
void
drawPt( cv::Mat& mat, PointStyle ps, std::vector<cv::Point2d> vpt, const CvDrawParams& dp, bool drawDiag=false )
{
	auto delta  = dp._dpValues._ptDelta;
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
		cv::line( mat, vpt[0], vpt[1], dp._dpValues._color );
		cv::line( mat, vpt[2], vpt[3], dp._dpValues._color );
	}
	else // draw 4 diagonal lines
	{
		cv::line( mat, vpt[0], vpt[2], dp._dpValues._color );
		cv::line( mat, vpt[2], vpt[1], dp._dpValues._color );
		cv::line( mat, vpt[1], vpt[3], dp._dpValues._color );
		cv::line( mat, vpt[0], vpt[3], dp._dpValues._color );
	}
}
} // namespace detail
//------------------------------------------------------------------
/// Draw line on Cv::Mat. Specialization for points.
/// Returns false if point not in image
template<>
bool
Root<IsPoint>::drawCvMat( cv::Mat& mat, CvDrawParams dp )
{
	if( getX()<0 || getX()>=mat.cols )
		return false;
	if( getY()<0 || getY()>=mat.rows )
		return false;

	std::vector<cv::Point2d> vpt( 4, getCvPtd() );
	switch( dp._dpValues._ptStyle )
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
	return true;
}

//------------------------------------------------------------------
#endif // HOMOG2D_USE_OPENCV


typedef Root<IsLine> Line2d;
typedef Root<IsPoint> Point2d;

} // namespace homog2d end


#endif // HG_HOMOG2D_HPP

