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
template<typename LP,typename FPT>
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

Templated by Floating-Point Type (FPT)
 */
template<typename FPT>
class Homogr_
{
	template<typename T1,typename T2>
	friend class Root;

	template<typename T,typename U,typename V>
	friend Root<T,V> operator * ( const Homogr_<U>& h, const Root<T,V>& in );

	public:
	/// Default constructor, initialize to unit transformation
	Homogr_()
	{
		clear();
	}
	/// Build a rotation transformation of \c val radians
	template<typename T>
	Homogr_( T val )
	{
		setRotation( val );
	}

/// Constructor, used to fill with another "vector of vector" matrix (or std::array)
/** \warning
- Input matrix \b must be 3 x 3, but type can be anything that can be copied to \c double
- no checking is done on validity of matrix as an homography.
Thus some assert can get triggered elsewhere.
*/
	template<typename T>
	Homogr_( const std::vector<std::vector<T>>& in )
	{
		if( in.size() != 3 )
			throw std::runtime_error( "Invalid line size for input: " + std::to_string(in.size()) );
		for( auto li: in )
			if( li.size() != 3 )
				throw std::runtime_error( "Invalid column size for input: " + std::to_string(li.size()) );

		p_fillWith( in );
	}
	template<typename T>
	Homogr_( const std::array<std::array<T,3>,3>& in )
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
	template<typename T>
	void setValue(
		size_t r, ///< row
		size_t c, ///< col
		T      v  ///< value
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
	Homogr_& addTranslation( T tx, T ty )
	{
		Homogr_ out;
		out.setTranslation( tx, ty );
		*this = out * *this;
//		normalize();
		return *this;
	}
/// Sets the matrix as a translation \c tx,ty
	template<typename T>
	Homogr_& setTranslation( T tx, T ty )
	{
		clear();
		_data[0][2] = tx;
		_data[1][2] = ty;
		_isNormalized = true;
		return *this;
	}
/// Adds a rotation with an angle \c theta (radians) to the matrix
	template<typename T>
	Homogr_& addRotation( T theta )
	{
		Homogr_ out;
		out.setRotation( theta );
		*this = out * *this;
//		normalize();
		return *this;
	}
/// Sets the matrix as a rotation with an angle \c theta (radians)
	template<typename T>
	Homogr_& setRotation( T theta )
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
	Homogr_& addScale( T k )
	{
		return this->addScale( k, k );
	}
/// Adds a scale factor to the matrix
	template<typename T>
	Homogr_& addScale( T kx, T ky )
	{
		Homogr_ out;
		out.setScale( kx, ky );
		*this = out * *this;
//		normalize();
		return *this;
	}
/// Sets the matrix as a scaling transformation (same on two axis)
	template<typename T>
	Homogr_& setScale( T k )
	{
		return setScale( k, k );
	}
/// Sets the matrix as a scaling transformation
	template<typename T>
	Homogr_& setScale( T kx, T ky )
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

#ifdef HOMOG2D_USE_OPENCV
	void getFrom( const cv::Mat& ) const;
	void copyTo( cv::Mat&, int type=CV_64F ) const;
#endif

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
	Homogr_& transpose()
	{
		Homogr_ out;
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				out._data[i][j] = _data[j][i];
		*this = out;
		return *this;
	}

/// Inverse matrix
	Homogr_& inverse()
	{
		Homogr_ adjugate = p_adjugate();
		double det = p_det();

		if( std::abs(det) <= std::numeric_limits<double>::epsilon() )
			throw std::runtime_error( "matrix is not invertible" );

		*this = adjugate / det;

		normalize();
		return *this;
	}

/// Divide all elements by scalar
	template<typename T>
	Homogr_& operator / (T v)
	{
		if( std::abs(v) <= std::numeric_limits<double>::epsilon() )
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
/**
Can't be templated by arg type because it would conflict with operator * for Homogr and line/point
*/
	Homogr_& operator * (double v)
	{
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				_data[i][j] *= v;
		return *this;
	}

/// Matrix multiplication
	friend Homogr_ operator * ( const Homogr_& h1, const Homogr_& h2 )
	{
		Homogr_ out;
		out.p_zero();
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				for( int k=0; k<3; k++ )
					out._data[i][j] += h1._data[i][k] * h2._data[k][j];
		return out;
	}

/// Comparison operator. Does normalization if required
	bool operator == ( const Homogr_& h ) const
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
	bool operator != ( const Homogr_& h ) const
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
		double det = _data[0][0] * p_det2x2( {1,1, 1,2, 2,1, 2,2} );
		det       -= _data[0][1] * p_det2x2( {1,0, 1,2, 2,0, 2,2} );
		det       += _data[0][2] * p_det2x2( {1,0, 1,1, 2,0, 2,1} );
		return det;
	}
	double p_det2x2( std::vector<int> v )
	{
		auto det = _data[v[0]][v[1]] * _data[v[6]][v[7]];
		det -= _data[v[2]][v[3]] * _data[v[4]][v[5]];
		return det;
	}
/// Computes adjugate matrix, see https://en.wikipedia.org/wiki/Adjugate_matrix#3_%C3%97_3_generic_matrix
/**
\todo Decrement each index so we can remove the need for std::transform()
*/
	Homogr_ p_adjugate()
	{
		Homogr_ out;

		out.setValue( 0, 0,  p_det2x2( {1,1, 1,2, 2,1, 2,2} ) );
		out.setValue( 0, 1, -p_det2x2( {0,1, 0,2, 2,1, 2,2} ) );
		out.setValue( 0, 2,  p_det2x2( {0,1, 0,2, 1,1, 1,2} ) );

		out.setValue( 1, 0, -p_det2x2( {1,0, 1,2, 2,0, 2,2} ) );
		out.setValue( 1, 1,  p_det2x2( {0,0, 0,2, 2,0, 2,2} ) );
		out.setValue( 1, 2, -p_det2x2( {0,0, 0,2, 1,0, 1,2} ) );

		out.setValue( 2, 0,  p_det2x2( {1,0, 1,1, 2,0, 2,1} ) );
		out.setValue( 2, 1, -p_det2x2( {0,0, 0,1, 2,0, 2,1} ) );
		out.setValue( 2, 2,  p_det2x2( {0,0, 0,1, 1,0, 1,1} ) );

		return out;
	}

//////////////////////////
//      DATA SECTION    //
//////////////////////////
	private:
	mutable std::array<std::array<FPT,3>,3> _data;
	mutable bool _isNormalized = false;

	friend std::ostream& operator << ( std::ostream& f, const Homogr_& h )
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

// forward declaration of template instanciation
namespace detail {

template<typename T1,typename T2,typename T3>
Root<T1,T3> crossProduct( const Root<T2,T3>&, const Root<T2,T3>& );

}

//------------------------------------------------------------------
namespace detail {

/// Helper class, used as a trick to allow partial specialization of member functions
	template<typename>
	struct RootHelper {};

} // namespace detail
//------------------------------------------------------------------
/// Base class, will be instanciated as a Point2d or a Line2d
template<typename LP,typename FPT>
class Root
{
	template<typename U>
	friend class Homogr_;

	template<typename T,typename U,typename V>
	friend Root<T,V>
	operator * ( const Homogr_<U>&, const Root<T,V>& );

	template<typename T>
	friend Root<IsPoint,T>
	operator * ( const Root<IsLine,T>&, const Root<IsLine,T>& );

	template<typename T>
	friend Root<IsLine,T>
	operator * ( const Root<IsPoint,T>&, const Root<IsPoint,T>& );

	template<typename T1,typename T2,typename T3>
	friend Root<T1,T3>
	detail::crossProduct( const Root<T2,T3>&, const Root<T2,T3>& );

	template<typename U,typename V>
	friend std::ostream&
	operator << ( std::ostream& f, const Root<U,V>& r );

	private:
	Root( double a, double b, double c )
	{
		_v[0] = a;
		_v[1] = b;
		_v[2] = c;
	}

	public:

	Root( const Root<IsLine,FPT>& v1, const Root<IsLine,FPT>& v2 )
	{
		*this = detail::crossProduct<IsPoint>( v1, v2 );
	}

	Root( const Root<IsPoint,FPT>& v1, const Root<IsPoint,FPT>& v2 )
	{
		*this = detail::crossProduct<IsLine>( v1, v2 );
		p_normalizeLine();
	}

	template<typename T>
	Root( const T& v1, const T& v2 )
	{
		impl_init_2( v1, v2, detail::RootHelper<LP>() );
	}

	Root()
	{
		impl_init( detail::RootHelper<LP>() );
	}

	double            getCoord( En_GivenCoord gc, double other ) const { return impl_getCoord( gc, other, detail::RootHelper<LP>() ); }
	Root<IsPoint,FPT> getPoint( En_GivenCoord gc, double other ) const { return impl_getPoint( gc, other, detail::RootHelper<LP>() ); }
	Root<IsLine,FPT>  getOrthogonalLine( En_GivenCoord gc, double other ) const
	{
		return impl_getOrthogonalLine( gc, other, detail::RootHelper<LP>() );
	}

	template<typename T>
	void addOffset( En_OffsetDir dir, T v )
	{
		impl_addOffset( dir, v, detail::RootHelper<LP>() );
	}

	double getX() const              { return impl_getX( detail::RootHelper<LP>() ); }
	double getY() const              { return impl_getY( detail::RootHelper<LP>() ); }
	void   set( double x, double y ) { impl_set( x, y,   detail::RootHelper<LP>() ); }

	double distToPoint( const Root<IsPoint,FPT>& pt ) const
	{
		return impl_distToPoint( pt, detail::RootHelper<LP>() );
	}
	double getAngle( const Root<IsLine,FPT>& li ) const
	{
		return impl_getAngle( li, detail::RootHelper<LP>() );
	}

	private:
		double impl_getX( const detail::RootHelper<IsPoint>& /* dummy */ ) const
		{
			return _v[0]/_v[2];
		}
		double impl_getY( const detail::RootHelper<IsPoint>& /* dummy */ ) const
		{
			return _v[1]/_v[2];
		}
		void impl_set( double x, double y, const detail::RootHelper<IsPoint>& /* dummy */ )
		{
			_v[0] = x;
			_v[1] = y;
			_v[2] = 1.;
		}
		template<typename T>
		void   impl_addOffset( En_OffsetDir dir, T v, const detail::RootHelper<IsLine>& );

		double impl_distToPoint( const Root<IsPoint,FPT>&, const detail::RootHelper<IsPoint>& ) const;
		double impl_distToPoint( const Root<IsPoint,FPT>&, const detail::RootHelper<IsLine>&  ) const;

		double impl_getAngle( const Root<LP,FPT>&, const detail::RootHelper<IsLine>& ) const;

		double impl_getCoord( En_GivenCoord gc, double other, const detail::RootHelper<IsLine>& ) const;
		Root<IsPoint,FPT> impl_getPoint( En_GivenCoord gc, double other, const detail::RootHelper<IsLine>& ) const;

		void impl_op_stream( std::ostream&, const Root<IsPoint,FPT>& ) const;
		void impl_op_stream( std::ostream&, const Root<IsLine,FPT>& ) const;

	public:
/// Sub-type, holds result of rectangle intersection, see intersectsRectangle().
/// Only defined for Point2d
	struct RectIntersect
	{
		template<typename U,typename V>
		friend class Root;

		public:
		bool operator()() const
		{
			return _doesIntersect;
		}
		RectIntersect()
		{}
		RectIntersect( const Root<IsPoint,FPT>& p1, const Root<IsPoint,FPT>& p2 ) : ptA(p1), ptB(p2)
		{
			_doesIntersect = true;
		}
		Root<IsPoint,FPT> ptA;
		Root<IsPoint,FPT> ptB;

		private:
		bool _doesIntersect = false;
	};

	RectIntersect intersectsRectangle( const Root<IsPoint,FPT>& pt1, const Root<IsPoint,FPT>& pt2 ) const;

	bool isInsideRectangle( const Root<IsPoint,FPT>& pt1, const Root<IsPoint,FPT>& pt2 ) const
	{
		return impl_isInsideRectangle( pt1, pt2 /*, detail::RootHelper<LP>()*/ );
	}

//////////////////////////
//       OPERATORS      //
//////////////////////////
	bool operator == ( const Root<LP,FPT>& other ) const
	{
		return impl_op_equal( other, detail::RootHelper<LP>() );
	}
	bool operator != ( const Root<LP,FPT>& other ) const
	{
		return !(*this == other);
	}

// optional stuff
#ifdef HOMOG2D_USE_OPENCV
	bool drawCvMat( cv::Mat& mat, CvDrawParams dp=CvDrawParams() )
	{
		return impl_drawCvMat( mat, dp, detail::RootHelper<LP>() );
	}
	cv::Point2d getCvPtd() const { return impl_getCvPtd( detail::RootHelper<LP>() ); }
	cv::Point2f getCvPtf() const { return impl_getCvPtf( detail::RootHelper<LP>() ); }
#endif

	private:
	FPT _v[3]; ///< data

//////////////////////////
//   PRIVATE FUNCTIONS  //
//////////////////////////

	void p_normalizeLine() const { impl_normalizeLine(  detail::RootHelper<LP>() ); }

	Root<LP,FPT>::RectIntersect
	impl_intersectsRectangle( const Root<IsPoint,FPT>& p0, const Root<IsPoint,FPT>& p1 ) const;

	bool impl_isInsideRectangle( const Root<IsPoint,FPT>&, const Root<IsPoint,FPT>& /*, detail::RootHelper<IsPoint>& */ ) const;
	void impl_normalizeLine( const detail::RootHelper<IsLine>& ) const;

	Root<IsLine,FPT> impl_getOrthogonalLine( En_GivenCoord gc, double val, const detail::RootHelper<IsLine>& ) const;

	bool impl_op_equal( const Root<LP,FPT>&, const detail::RootHelper<IsLine>& ) const;
	bool impl_op_equal( const Root<LP,FPT>&, const detail::RootHelper<IsPoint>& ) const;

	Root<IsPoint,FPT> impl_op_product( const Root<IsLine,FPT>& , const Root<IsLine,FPT>& , const detail::RootHelper<IsPoint>& ) const;
	Root<IsLine,FPT>  impl_op_product( const Root<IsPoint,FPT>&, const Root<IsPoint,FPT>&, const detail::RootHelper<IsLine>&  ) const;

#ifdef HOMOG2D_USE_OPENCV
	cv::Point2f impl_getCvPtf( const detail::RootHelper<IsPoint>& ) const;
	cv::Point2d impl_getCvPtd( const detail::RootHelper<IsPoint>& ) const;
	bool impl_drawCvMat( cv::Mat&, const CvDrawParams&, const detail::RootHelper<IsPoint>& );
	bool impl_drawCvMat( cv::Mat&, const CvDrawParams&, const detail::RootHelper<IsLine>& );
#endif

/// Called by default constructor, overload for lines
	void impl_init( const detail::RootHelper<IsLine>& )
	{
		_v[0] = 1.;
		_v[1] = 0.;
		_v[2] = 0.;
	}
/// Called by default constructor, overload for points
	void impl_init( const detail::RootHelper<IsPoint>& )
	{
		_v[0] = 0.;
		_v[1] = 0.;
		_v[2] = 1.;
	}
	template<typename T>
	void impl_init_2( const T& v1, const T& v2, const detail::RootHelper<IsPoint>& );
	template<typename T>
	void impl_init_2( const T& v1, const T& v2, const detail::RootHelper<IsLine>& );
};

//------------------------------------------------------------------
/// overload for points
/// \todo now member function so we can use the object itself, but need to keep the second parameter so the compiler can select the correct overload
template<typename LP,typename FPT>
void
Root<LP,FPT>::impl_op_stream( std::ostream& f, const Root<IsPoint,FPT>& r ) const
{
	f << '[' << r.getX() << ',' << r.getY() << "] ";
}

/// overload for lines
template<typename LP,typename FPT>
void
Root<LP,FPT>::impl_op_stream( std::ostream& f, const Root<IsLine,FPT>& r ) const
{
	f << '[' << r._v[0] << ',' << r._v[1] << ',' << r._v[2] << "] ";
}

/// Stream operator, free function, call member function pseudo operator impl_op_stream()
template<typename LP,typename FPT>
std::ostream&
operator << ( std::ostream& f, const Root<LP,FPT>& r )
{
	r.impl_op_stream( f, r );
	return f;
}

//------------------------------------------------------------------
/// Normalize to unit length, and make sure \c a is always >0
template<typename LP,typename FPT>
void
Root<LP,FPT>::impl_normalizeLine( const detail::RootHelper<IsLine>& ) const
{
	auto sq = std::hypot( _v[0], _v[1] );
	if( sq <= std::numeric_limits<double>::epsilon() )
		throw std::runtime_error( "unable to normalize line" );
	for( int i=0; i<3; i++ )
		//_v[i] /= sq;
		const_cast<Root<LP,FPT>*>(this)->_v[i] /= sq; // needed to remove constness



	if( std::signbit(_v[0]) ) //a allways >0
		for( int i=0; i<3; i++ )
//			_v[i] = -_v[i];
			const_cast<Root<LP,FPT>*>(this)->_v[i] = -_v[i];

	if( _v[0] == 0. ) // then, change sign so that b>0
		if( std::signbit(_v[1]) )
		{
			const_cast<Root<LP,FPT>*>(this)->_v[1] = - _v[1];
			const_cast<Root<LP,FPT>*>(this)->_v[2] = - _v[2];
		}
}

//------------------------------------------------------------------
template<typename LP,typename FPT>
double
Root<LP,FPT>::impl_getCoord( En_GivenCoord gc, double other, const detail::RootHelper<IsLine>& ) const
{
	if( gc == GC_X )
		return ( -_v[0] * other - _v[2] ) / _v[1];
	else
		return ( -_v[1] * other - _v[2] ) / _v[0];
}

template<typename LP,typename FPT>
Root<IsPoint,FPT>
Root<LP,FPT>::impl_getPoint( En_GivenCoord gc, double other, const detail::RootHelper<IsLine>& dummy ) const
{
	auto coord = impl_getCoord( gc, other, dummy );
	if( gc == GC_X )
		return Root<IsPoint,FPT>( other, coord );
	return Root<IsPoint,FPT>( coord, other );
}

//------------------------------------------------------------------
/// Returns an orthogonal line, at gc=other
template<typename LP,typename FPT>
Root<IsLine,FPT>
Root<LP,FPT>::impl_getOrthogonalLine( En_GivenCoord gc, double val, const detail::RootHelper<IsLine>& ) const
{
	auto other_val = impl_getCoord( gc, val, detail::RootHelper<IsLine>() );

	Root<IsPoint,FPT> pt( other_val, val ) ;
	if( gc == GC_X )
		pt.set( val, other_val );

	Root<IsLine,FPT> out;
	out._v[0] = -_v[1];
	out._v[1] =  _v[0];
	out._v[2] = _v[1] * pt.getX() - _v[0] * pt.getY();
	out.p_normalizeLine();
	return out;
}


//------------------------------------------------------------------
/// Comparison operator, used for lines
template<typename LP,typename FPT>
bool
Root<LP,FPT>::impl_op_equal( const Root<LP,FPT>& other, const detail::RootHelper<IsLine>& ) const
{
	auto eps = std::numeric_limits<double>::epsilon();
	for( int i=0; i<3; i++ )
		if( std::fabs( _v[i] - other._v[i] ) > eps )
			return false;
	return true;
}
/// Comparison operator, used for points
template<typename LP,typename FPT>
bool
Root<LP,FPT>::impl_op_equal( const Root<LP,FPT>& other, const detail::RootHelper<IsPoint>& ) const
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
	template<typename Out, typename In,typename FPT>
	Root<Out,FPT> crossProduct( const Root<In,FPT>& r1, const Root<In,FPT>& r2 )
	{
		Root<Out,FPT> res;
		res._v[0] = r1._v[1] * r2._v[2] - r1._v[2] * r2._v[1];
		res._v[1] = r1._v[2] * r2._v[0] - r1._v[0] * r2._v[2];
		res._v[2] = r1._v[0] * r2._v[1] - r1._v[1] * r2._v[0];
//		for ( int i=0; i<3; i++)
//			std::cout << "v[" << i << "]=" << res._v[i] << " ";
//		std::cout  << '\n';
		return res;
	}
}

/// free function template, product of two lines
template<typename FPT>
Root<IsPoint,FPT>
operator * ( const Root<IsLine,FPT>& lhs, const Root<IsLine,FPT>& rhs )
{
	auto pt = detail::crossProduct<IsPoint,IsLine,FPT>(lhs, rhs);
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

/// free function template, product of two points
template<typename FPT>
Root<IsLine,FPT>
operator * ( const Root<IsPoint,FPT>& lhs, const Root<IsPoint,FPT>& rhs )
{
	Root<IsLine, FPT> line = detail::crossProduct<IsLine,IsPoint,FPT>(lhs, rhs);
	line.p_normalizeLine();
	return line;
}

//------------------------------------------------------------------
///////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////

/// Points overload: generic init from two args
template<typename LP, typename FPT>
template<typename T>
void
Root<LP,FPT>::impl_init_2( const T& v1, const T& v2, const detail::RootHelper<IsPoint>& )
{
	_v[0] = v1;
	_v[1] = v2;
	_v[2] = 1.;
}

/// Lines overload: generic init from two args
template<typename LP, typename FPT>
template<typename T>
void
Root<LP,FPT>::impl_init_2( const T& v1, const T& v2, const detail::RootHelper<IsLine>& )
{
	Root<IsPoint,FPT> pt1;                // 0,0
	Root<IsPoint,FPT> pt2(v1,v2);
	*this = detail::crossProduct<IsLine>( pt1, pt2 );
	p_normalizeLine();
}

/// overload for point to point distance
template<typename LP, typename FPT>
double
Root<LP,FPT>::impl_distToPoint( const Root<IsPoint,FPT>& pt, const detail::RootHelper<IsPoint>& ) const
{
	return std::hypot( getX() - pt.getX(), getY() - pt.getY() );
}
//------------------------------------------------------------------
/// Returns distance between the line and point \b pt. overload for line to point distance.
/**
http://mathworld.wolfram.com/Point-LineDistance2-Dimensional.html
<pre>
        | a.x0 + b.y0 + c |
  d = -----------------------
         sqrt( a*a + b*b )
</pre>
\todo Do we really require computation of hypot ? (because the line is supposed to be normalized, i.e. h=1 ?)
*/

template<typename LP, typename FPT>
double
Root<LP,FPT>::impl_distToPoint( const Root<IsPoint,FPT>& pt, const detail::RootHelper<IsLine>& ) const
{
	return std::fabs( _v[0] * pt.getX() + _v[1] * pt.getY() + _v[2] ) / std::hypot( _v[0], _v[1] );
}

/// overload for lines, undefined for points
template<typename LP, typename FPT>
template<typename T>
void
Root<LP,FPT>::impl_addOffset( En_OffsetDir dir, T v, const detail::RootHelper<IsLine>& )
{
	if( dir == OD_Vert )
		_v[2] = _v[2] - v*_v[1];
	else
		_v[2] = _v[2] - v*_v[0];
	p_normalizeLine();
}

//------------------------------------------------------------------
/// Free function, returns the angle (in Rad) between two lines.
template<typename FPT>
double
getAngle( const Root<IsLine,FPT>& li1, const Root<IsLine,FPT>& li2 )
{
	return li1.getAngle( li2 );
}

//------------------------------------------------------------------
/// Returns the angle (in Rad) between the line and another one.
/**
Will return a value in the range [0,M_PI/2]

\todo add implementation of free function with partial specialization trick
*/
template<typename LP, typename FPT>
double
Root<LP,FPT>::impl_getAngle( const Root<LP,FPT>& li, const detail::RootHelper<IsLine>& ) const
{
	double res = _v[0] * li._v[0] + _v[1] * li._v[1];
	res /= std::sqrt( _v[0]*_v[0] + _v[1]*_v[1] ) * std::sqrt( li._v[0]*li._v[0] + li._v[1]*li._v[1] );
	return std::acos( std::abs(res) );
}

//------------------------------------------------------------------
namespace detail {
/// Private free function, get top-left and bottom-right points from two arbitrary points
template<typename FPT>
std::pair<Root<IsPoint,FPT>,Root<IsPoint,FPT>>
getCorrectPoints( const Root<IsPoint,FPT>& p0, const Root<IsPoint,FPT>& p1 )
{
	if( p0.getX() == p1.getX() || p0.getY() == p1.getY() )
		throw std::runtime_error( "error: a coordinate of the 2 points are identical, does not define a rectangle" );

	Root<IsPoint,FPT> p00( std::min(p0.getX(), p1.getX()), std::min(p0.getY(), p1.getY()) );
	Root<IsPoint,FPT> p11( std::max(p0.getX(), p1.getX()), std::max(p0.getY(), p1.getY()) );
	return std::make_pair( p00, p11 );
}

//------------------------------------------------------------------
/// Private free function, returns true if point \c pt is inside the rectangle defined by (p00,p11)
template<typename FPT>
bool
ptIsInside( const Root<IsPoint,FPT>& pt, const Root<IsPoint,FPT>& p00, const Root<IsPoint,FPT>& p11 )
{
	if( pt.getX() >= p00.getX() && pt.getX() <= p11.getX() )
		if( pt.getY() >= p00.getY() && pt.getY() <= p11.getY() )
			return true;
	return false;
}

} // namespace detail end

//------------------------------------------------------------------
/// Returns true if point is inside (or on the edge) of a flat rectangle defined by (p0,p1)
/// \todo check if we can skip/remove the last argument ? Can't the compiler deduce correct overload from the 2 first arguments ?
template<typename LP, typename FPT>
bool
Root<LP,FPT>::impl_isInsideRectangle( const Root<IsPoint,FPT>& p0, const Root<IsPoint,FPT>& p1 /*, detail::RootHelper<IsPoint>& */ ) const
{
	auto pair_pts = detail::getCorrectPoints( p0, p1 );
	const auto& p00 = pair_pts.first;
	const auto& p11 = pair_pts.second;
	return detail::ptIsInside( *this, p00, p11 );
}
//------------------------------------------------------------------
/// \todo is this additional layer really necessary here ?
template<typename LP, typename FPT>
typename Root<LP,FPT>::RectIntersect
Root<LP,FPT>::intersectsRectangle( const Root<IsPoint,FPT>& pt1, const Root<IsPoint,FPT>& pt2 ) const
{
	return impl_intersectsRectangle( pt1, pt2 );
}

/// Checks for intersection with flat rectangle defined by the two points p00 and p11
/**
Pre-conditions: points are different (throws if not)
*/
template<typename LP, typename FPT>
typename Root<LP,FPT>::RectIntersect
Root<LP,FPT>::impl_intersectsRectangle( const Root<IsPoint,FPT>& p0, const Root<IsPoint,FPT>& p1 ) const
{
	auto pair_pts = detail::getCorrectPoints( p0, p1 );
	const auto& p00 = pair_pts.first;
	const auto& p11 = pair_pts.second;

	Root<IsPoint,FPT> p01( p11.getX(), p00.getY() );
	Root<IsPoint,FPT> p10( p00.getX(), p11.getY() );

	Root<IsLine,FPT> l[4];
	l[0] = Root<IsLine,FPT>( p00, p01 );
	l[1] = Root<IsLine,FPT>(      p01, p11 );
	l[2] = Root<IsLine,FPT>(           p11, p10 );
	l[3] = Root<IsLine,FPT>(                p10, p00 );

	std::vector<Root<IsPoint,FPT>> vec;
	for( int i=0; i<4; i++ )         // compare with each of the 4 lines
	{
		if( *this == l[i] )          // if same line, then we are done: return the two points
			return typename Root<IsLine,FPT>::RectIntersect( p00, p11 );
//			return Root<IsLine,FPT>::RectIntersect_<IsPoint>( p00, p11 );
		else
		{
			Root<IsPoint,FPT> pt;
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
				if( detail::ptIsInside( pt, p00, p11 ) )
					vec.push_back( pt );
			}
		}
	}

	typename Root<LP,FPT>::RectIntersect out;
	if( vec.size() > 1 )                                // if more than one point was found, then
	{
		std::vector<Root<IsPoint,FPT>> vec2( 1, vec[0] );   // build a second vector, holding the first found point as first element
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

//------------------------------------------------------------------
/// Apply homography to a point or line. Free function, templated by point or line
template<typename T,typename U,typename V>
Root<T,V>
operator * ( const Homogr_<U>& h, const Root<T,V>& in )
{
	Root<T,V> out;
	for( int i=0; i<3; i++ )
	{
		out._v[i]  = h._data[i][0] * in._v[0];
		out._v[i] += h._data[i][1] * in._v[1];
		out._v[i] += h._data[i][2] * in._v[2];
	}
	return out;
}
//------------------------------------------------------------------
/// Apply homography to a vector/array/list of points or lines. Free function, templated by point or line
template<typename FPT>
template<typename T>
void
Homogr_<FPT>::applyTo( T& vin ) const
{
	for( auto& elem: vin )
		elem = *this * elem;
}
//------------------------------------------------------------------
#ifdef HOMOG2D_USE_OPENCV
/// Return floating-point coordinates Opencv 2D point (with rounding)
template<typename LP, typename FPT>
cv::Point2d
Root<LP,FPT>::impl_getCvPtd( const detail::RootHelper<IsPoint>& ) const
{
	return cv::Point2d(
		static_cast<int>(std::round(getX())),
		static_cast<int>(std::round(getY()))
	);
}

/// Return integer coordinates Opencv 2D point
template<typename LP, typename FPT>
cv::Point2f
Root<LP,FPT>::impl_getCvPtf( const detail::RootHelper<IsPoint>& ) const
{
	return cv::Point2f( getX(),getY() );
}

//------------------------------------------------------------------
/// Copy matrix to Opencv \c cv::Mat
/**
The output matrix is passed by reference to avoid issues with Opencv copy operator, and is allocated here.

User can pass a type as second argument: CV_32F for \c float, CV_64F for \c double (default)
*/
template<typename FPT>
void
Homogr_<FPT>::copyTo( cv::Mat& mat, int type ) const
{
	if( type != CV_64F && type != CV_32F )
		throw std::runtime_error( "invalid matrix type" );
	mat.create( 3, 3, type ); // default:CV_64F
	size_t i=0;
	switch( type )
	{
		case CV_64F:
			for( auto it = mat.begin<double>(); it != mat.end<double>(); it++, i++ )
				*it = _data[i/3][i%3];
			break;
		case CV_32F:
			for( auto it = mat.begin<float>(); it != mat.end<float>(); it++, i++ )
				*it = _data[i/3][i%3];
			break;
		default: assert(0);
	}
}
//------------------------------------------------------------------
/// Get homography from Opencv \c cv::Mat
template<typename FPT>
void
Homogr_<FPT>::getFrom( const cv::Mat& mat ) const
{
	if( mat.rows != 3 || mat.cols != 3 )
		throw std::runtime_error( "invalid matrix size, rows=" + std::to_string(mat.rows) + " cols=" + std::to_string(mat.cols) );
	if( mat.channels() != 1 )
		throw std::runtime_error( "invalid matrix nb channels: " + std::to_string(mat.channels() ) );

	auto type = mat.type();
	if( type != CV_64F && type != CV_32F )
		throw std::runtime_error( "invalid matrix type" );

	size_t i=0;
	switch( type )
	{
		case CV_64F:
			for( auto it = mat.begin<double>(); it != mat.end<double>(); it++, i++ )
				_data[i/3][i%3] = *it;
			break;
		case CV_32F:
			for( auto it = mat.begin<float>(); it != mat.end<float>(); it++, i++ )
				_data[i/3][i%3] = *it;
			break;
		default: assert(0);
	}
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
/// Draw Point on Cv::Mat. Overload for points.
/// Returns false if point not in image
template<typename LP, typename FPT>
bool
Root<LP,FPT>::impl_drawCvMat( cv::Mat& mat, const CvDrawParams& dp, const detail::RootHelper<IsPoint>& /* dummy */ )
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
/// Draw line on Cv::Mat. Overload for lines.
/**
Returns false if line is not in image.

Steps:
 -# builds the 4 corner points of the image
 -# build the 4 corresponding lines (borders of the image)
 -# find the intersection points between the line and these 4 lines. Should find 2
 -# draw a line between these 2 points
*/
template<typename LP, typename FPT>
bool
Root<LP,FPT>::impl_drawCvMat( cv::Mat& mat, const CvDrawParams& dp, const detail::RootHelper<IsLine>& /* dummy */ )
{
	assert( mat.rows > 2 );
	assert( mat.cols > 2 );

	Root<IsPoint,FPT> pt1; // 0,0
	Root<IsPoint,FPT> pt2( mat.cols-1, mat.rows-1 );
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
#endif // HOMOG2D_USE_OPENCV


/// Default line type, uses \c double as numerical type
using Line2d = Root<IsLine,double>;

/// Default point type, uses \c double as numerical type
using Point2d = Root<IsPoint,double>;

/// Default homography (3x3 matrix) type, uses \c double as numerical type
using Homogr = Homogr_<double>;

// float types
using Line2dF  = Root<IsLine,float>;
using Point2dF = Root<IsPoint,float>;
using HomogrF  = Homogr_<float>;

// double types
using Line2dD  = Root<IsLine,double>;
using Point2dD = Root<IsPoint,double>;
using HomogrD  = Homogr_<double>;

// long double types
using Line2dL  = Root<IsLine,long double>;
using Point2dL = Root<IsPoint,long double>;
using HomogrL  = Homogr_<long double>;


template<typename T>
using Point2d_ = Root<IsPoint,T>;
template<typename T>
using Line2d_  = Root<IsLine,T>;

template<typename T>
using RectIntersect_ = typename Root<IsLine,T>::RectIntersect;

using RectIntersect = RectIntersect_<double>;

} // namespace homog2d end


#endif // HG_HOMOG2D_HPP

