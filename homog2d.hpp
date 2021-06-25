/**************************************************************************

    This file is part of the C++ library "homog2d", dedicated to
    handle 2D lines and points, see https://github.com/skramm/homog2d

    Author & Copyright 2019-2021 Sebastien Kramm

    Contact: firstname.lastname@univ-rouen.fr

    Licence: MPL v2

	This Source Code Form is subject to the terms of the Mozilla Public
	License, v. 2.0. If a copy of the MPL was not distributed with this
	file, You can obtain one at https://mozilla.org/MPL/2.0/.

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
#include <list>
#include <vector>
#include <iomanip>
#include <cassert>
#include <sstream>
#include <type_traits>

#ifdef HOMOG2D_USE_EIGEN
	#include <Eigen/Dense>
#endif

#define HOMOG2D_VERSION 2.4

#ifdef HOMOG2D_USE_OPENCV
	#include "opencv2/imgproc.hpp"
#endif


#define HOMOG2D_CHECK_ROW_COL \
	if( r > 2 ) \
		throw std::runtime_error( "Error: invalid row value: r=" + std::to_string(r) ); \
	if( c > 2 ) \
		throw std::runtime_error( "Error: invalid col value: r=" + std::to_string(r) )


#define HOMOG2D_CHECK_IS_NUMBER(T) \
	static_assert( std::is_arithmetic<T>::value && !std::is_same<T, bool>::value, "Type of value must be numerical" )

/// Internal type used for numerical computations, possible	values: \c double, <code>long double</code>
#if !defined(HOMOG2D_INUMTYPE)
	#define HOMOG2D_INUMTYPE double
#endif

namespace homog2d {

/// Holds the types needed for policy based design
namespace type {

struct IsLine   {};
struct IsPoint  {};
struct IsHomogr {};
struct IsEpipmat {};

} // namespace type


namespace detail {

	/// Helper class for Root (Point/Line) type, used as a trick to allow partial specialization of member functions
	template<typename>
	struct RootHelper {};

#if 0
	/// Helper class for Matrix type
	template<typename T1>
	struct HelperMat {};

	template<>
	struct HelperMat<type::IsHomogr>
	{
		using M_OtherType = IsEpipmat;
	};

	template<>
	struct HelperMat<type::IsEpipmat>
	{
		using M_OtherType = IsHomogr;
	};
#endif

    template<typename>
    struct HelperPL;

    template<>
    struct HelperPL<type::IsPoint>
    {
        using OtherType = type::IsLine;
    };

    template<>
    struct HelperPL<type::IsLine>
    {
        using OtherType = type::IsPoint;
    };

	/// A trick used in static_assert, so it aborts only if function is instanciated
	template<typename T>
	struct AlwaysFalse {
		enum { value = false };
	};


} // namespace detail


// forward declaration
template<typename LP,typename FPT>
class Root;

// forward declaration
template<typename LP,typename FPT>
class Hmatrix_;

// forward declaration
template<typename FPT>
class Segment_;


namespace detail {

// forward declaration of function
template<typename T1,typename T2,typename U,typename FPT1,typename FPT2>
void
product(
	Root<T1,FPT1>&          out,
	const Hmatrix_<U,FPT2>& h,
	const Root<T2,FPT1>&    in
);

} // namespace detail

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

To return to unit transformation, use init()

Implemented as a 3x3 matrix

Templated by Floating-Point Type (FPT) and by type M (type::IsEpipmat or type::IsHomogr)
 */
template<typename M,typename FPT>
class Hmatrix_
{
	template<typename T1,typename T2>
	friend class Root;

	template<typename T1,typename T2,typename U,typename FPT1,typename FPT2>
	friend void detail::product( Root<T1,FPT1>&, const Hmatrix_<U,FPT2>&, const Root<T2,FPT1>& );

	public:
	/// Default constructor, initialize to unit transformation
	Hmatrix_()
	{
		init();
	}

	void init()
	{
		impl_mat_init0( detail::RootHelper<M>() );
	}

/// Build homography and set it to a rotation matrix of angle \c val
	template<typename T>
	Hmatrix_( T val )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		init();
		setRotation( val );
	}

/// Build homography and set it to a translation matrix ( see Hmatrix_( T ) )
	template<typename T>
	Hmatrix_( T tx, T ty )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		init();
		setTranslation( tx, ty );
	}

	private:
/// Implementation for epipolar matrices: initialize to aligned axis
	void impl_mat_init0( const detail::RootHelper<type::IsEpipmat>& )
	{
		p_zero();
		_data[2][1] = 1.;
		_data[1][2] = 1.;
	}

/// Implementation for homographies: initialize to unit transformation
	void impl_mat_init0( const detail::RootHelper<type::IsHomogr>& )
	{
		p_zero();
		_data[0][0] = 1.;
		_data[1][1] = 1.;  // "eye" matrix => unit transformation
		_data[2][2] = 1.;
		_isNormalized = true;
	}

	public:

/// Constructor, used to fill with a vector of vectors matrix
/** \warning
- Input matrix \b must be 3 x 3, but type can be anything that can be copied to \c double
- no checking is done on validity of matrix as an homography.
Thus some assert can get triggered elsewhere.
*/
	template<typename T>
	Hmatrix_( const std::vector<std::vector<T>>& in )
	{
#ifndef HOMOG2D_NOCHECKS
		HOMOG2D_CHECK_IS_NUMBER(T);
		if( in.size() != 3 )
			throw std::runtime_error( "Invalid line size for input: " + std::to_string(in.size()) );
		for( auto li: in )
			if( li.size() != 3 )
				throw std::runtime_error( "Invalid column size for input: " + std::to_string(li.size()) );
#endif
		p_fillWith( in );
	}

/// Constructor, used to fill with a std::array
	template<typename T>
	Hmatrix_( const std::array<std::array<T,3>,3>& in )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		p_fillWith( in );
	}

#ifdef HOMOG2D_USE_OPENCV
/// Constructor used to initialise with a cv::Mat, call the assignment operator
	Hmatrix_( const cv::Mat& mat )
	{
		*this = mat;
	}

#endif

/// Setter \warning No normalization is done, as this can be done
/// several times to store values, we therefore must not normalize in between
	template<typename T>
	void set(
		size_t r, ///< row
		size_t c, ///< col
		T      v  ///< value
	)
	{
		#ifndef HOMOG2D_NOCHECKS
			HOMOG2D_CHECK_ROW_COL;
		#endif
		_data[r][c] = v;
		_isNormalized = false;
	}
/// Getter
	FPT get( size_t r, size_t c ) const
	{
		#ifndef HOMOG2D_NOCHECKS
			HOMOG2D_CHECK_ROW_COL;
		#endif
		return _data[r][c];
	}
/// Adds a translation \c tx,ty to the matrix
	template<typename T>
	Hmatrix_& addTranslation( T tx, T ty )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		Hmatrix_ out;
		out.setTranslation( tx, ty );
		*this = out * *this;
//		normalize();
		return *this;
	}
/// Sets the matrix as a translation \c tx,ty
	template<typename T>
	Hmatrix_& setTranslation( T tx, T ty )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		init();
		_data[0][2] = tx;
		_data[1][2] = ty;
		_isNormalized = true;
		return *this;
	}
/// Adds a rotation with an angle \c theta (radians) to the matrix
	template<typename T>
	Hmatrix_& addRotation( T theta )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		Hmatrix_ out;
		out.setRotation( theta );
		*this = out * *this;
//		normalize();
		return *this;
	}
/// Sets the matrix as a rotation with an angle \c theta (radians)
	template<typename T>
	Hmatrix_& setRotation( T theta )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		init();
		_data[0][0] = _data[1][1] = std::cos(theta);
		_data[1][0] = std::sin(theta);
		_data[0][1] = -_data[1][0];
		_isNormalized = true;
		return *this;
	}
/// Adds the same scale factor to the matrix
	template<typename T>
	Hmatrix_& addScale( T k )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		return this->addScale( k, k );
	}
/// Adds a scale factor to the matrix
	template<typename T>
	Hmatrix_& addScale( T kx, T ky )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		Hmatrix_ out;
		out.setScale( kx, ky );
		*this = out * *this;
//		normalize();
		return *this;
	}
/// Sets the matrix as a scaling transformation (same on two axis)
	template<typename T>
	Hmatrix_& setScale( T k )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		return setScale( k, k );
	}
/// Sets the matrix as a scaling transformation
	template<typename T>
	Hmatrix_& setScale( T kx, T ky )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		init();
		_data[0][0] = kx;
		_data[1][1] = ky;
		_isNormalized = true;
		return *this;
	}

	template<typename T>
	void
	applyTo( T& ) const;

#ifdef HOMOG2D_USE_OPENCV
	void copyTo( cv::Mat&, int type=CV_64F ) const;
	Hmatrix_& operator = ( const cv::Mat& );
#endif

/// Matrix normalisation
	void normalize() const
	{
		auto eps = std::numeric_limits<FPT>::epsilon()*10;

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
	Hmatrix_& transpose()
	{
		Hmatrix_ out;
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				out._data[i][j] = _data[j][i];
		*this = out;
		return *this;
	}

/// Inverse matrix
	Hmatrix_& inverse()
	{
		Hmatrix_ adjugate = p_adjugate();
		auto det = p_det();

		if( std::abs(det) <= Hmatrix_<M,FPT>::nullDeterValue() )
			throw std::runtime_error( "matrix is not invertible" );

		*this = adjugate / det;

		normalize();
		return *this;
	}

	void buildFrom4Points( const std::vector<Root<type::IsPoint,FPT>>&, const std::vector<Root<type::IsPoint,FPT>>&, int method=0 );

/// Divide all elements by scalar
	template<typename T>
	Hmatrix_& operator / (T v)
	{
		HOMOG2D_CHECK_IS_NUMBER( T );
		if( std::abs(v) <= std::numeric_limits<FPT>::epsilon() )
			throw std::runtime_error( "unable to divide by " + std::to_string(v) );
#if 0
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				_data[i][j] /= v;
		return *this;
#else
		return *this * (1.0L/v);
#endif
	}
/// Multiply all elements by scalar
/**
Can't be templated by arg type because it would conflict with operator * for Homogr and line/point
*/
	Hmatrix_& operator * (FPT v)
	{
		HOMOG2D_CHECK_IS_NUMBER( FPT );
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				_data[i][j] *= v;
		return *this;
	}

/// Matrix multiplication
	friend Hmatrix_ operator * ( const Hmatrix_& h1, const Hmatrix_& h2 )
	{
		Hmatrix_ out;
		out.p_zero();
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				for( int k=0; k<3; k++ )
					out._data[i][j] +=
						static_cast<HOMOG2D_INUMTYPE>(   h1._data[i][k] )
						* static_cast<HOMOG2D_INUMTYPE>( h2._data[k][j] );
		return out;
	}

/// Comparison operator. Does normalization if required
	bool operator == ( const Hmatrix_& h ) const
	{
		if( !_isNormalized )
			normalize();
		if( !h._isNormalized )
			h.normalize();

		auto eps = std::numeric_limits<FPT>::epsilon();
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				if( std::fabs(
					static_cast<HOMOG2D_INUMTYPE>( _data[i][j] ) - static_cast<HOMOG2D_INUMTYPE>( h._data[i][j] ) )
					>= eps
				)
					return false;
		return true;
	}
/// Comparison operator. Does normalization if required
	bool operator != ( const Hmatrix_& h ) const
	{
		return !(*this == h);
	}
	static HOMOG2D_INUMTYPE& nullDeterValue() { return _zeroDeterminantValue; }

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
		assert( std::fabs( _data[r][c] ) > std::numeric_limits<FPT>::epsilon() );
		for( auto& li: _data )
			for( auto& e: li )
				e /= _data[r][c];
	}
/// Return determinant of matrix
/**
See https://en.wikipedia.org/wiki/Determinant
*/
	HOMOG2D_INUMTYPE p_det()
	{
		auto det = _data[0][0] * p_det2x2( {1,1, 1,2, 2,1, 2,2} );
		det     -= _data[0][1] * p_det2x2( {1,0, 1,2, 2,0, 2,2} );
		det     += _data[0][2] * p_det2x2( {1,0, 1,1, 2,0, 2,1} );
		return det;
	}
	HOMOG2D_INUMTYPE p_det2x2( const std::vector<int>& v )
	{
		auto det = static_cast<HOMOG2D_INUMTYPE>( _data[v[0]][v[1]] ) * static_cast<HOMOG2D_INUMTYPE>( _data[v[6]][v[7]] );
		det -=     static_cast<HOMOG2D_INUMTYPE>( _data[v[2]][v[3]] ) * static_cast<HOMOG2D_INUMTYPE>( _data[v[4]][v[5]] );
		return det;
	}
/// Computes adjugate matrix, see https://en.wikipedia.org/wiki/Adjugate_matrix#3_%C3%97_3_generic_matrix
	Hmatrix_ p_adjugate()
	{
		Hmatrix_ out;

		out.set( 0, 0,  p_det2x2( {1,1, 1,2, 2,1, 2,2} ) );
		out.set( 0, 1, -p_det2x2( {0,1, 0,2, 2,1, 2,2} ) );
		out.set( 0, 2,  p_det2x2( {0,1, 0,2, 1,1, 1,2} ) );

		out.set( 1, 0, -p_det2x2( {1,0, 1,2, 2,0, 2,2} ) );
		out.set( 1, 1,  p_det2x2( {0,0, 0,2, 2,0, 2,2} ) );
		out.set( 1, 2, -p_det2x2( {0,0, 0,2, 1,0, 1,2} ) );

		out.set( 2, 0,  p_det2x2( {1,0, 1,1, 2,0, 2,1} ) );
		out.set( 2, 1, -p_det2x2( {0,0, 0,1, 2,0, 2,1} ) );
		out.set( 2, 2,  p_det2x2( {0,0, 0,1, 1,0, 1,1} ) );

		return out;
	}

//////////////////////////
//      DATA SECTION    //
//////////////////////////
	private:
	mutable std::array<std::array<FPT,3>,3> _data;
	mutable bool _isNormalized = false;

	friend std::ostream& operator << ( std::ostream& f, const Hmatrix_& h )
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
	static HOMOG2D_INUMTYPE _zeroDeterminantValue; /// Used in matrix inversion
};

//------------------------------------------------------------------

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
/// Draw parameters for Opencv binding, only available if HOMOG2D_USE_OPENCV defined,
/// see Root::draw()
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
		bool       _enhancePoint  = false;   // to draw selected points
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
		if( (int)ps > (int)PS_DIAM )
			throw std::runtime_error( "Error: invalid value for point style");
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
	CvDrawParams& selectPoint()
	{
		_dpValues._enhancePoint = true;
		return *this;
	}
};
#endif // HOMOG2D_USE_OPENCV
//------------------------------------------------------------------

/// Used in Line2d::getValue() and getOrthogonalLine()
//enum En_GivenCoord { GC_X, GC_Y };
enum class GivenCoord { X, Y };

/// Used in Line2d::addOffset()
//enum En_OffsetDir{ OD_Vert, OD_Horiz };
enum class LineOffset { vert, horiz };

// forward declaration of template instanciation
namespace detail {

template<typename T1,typename T2,typename T3>
Root<T1,T3> crossProduct( const Root<T2,T3>&, const Root<T2,T3>& );

}

//------------------------------------------------------------------
/// A circle
template<typename FPT>
class Circle_
{
	private:
	FPT _radius;
	Root<type::IsPoint,FPT> _center;

	public:
/// Default constructor, unit-radius circle at (0,0)
	Circle_() : _radius(1.)
	{}
/// Constructor, given radius circle at (0,0)
	Circle_( FPT rad ) : _radius(rad)
	{}

/// Constructor, given radius circle at (0,0)
	template<typename FPT2>
	Circle_( Root<type::IsPoint,FPT2> center, FPT rad )
		: _radius(rad), _center(center)
	{}

	FPT&       radius()       { return _radius; }
	const FPT& radius() const { return _radius; }

	Root<type::IsPoint,FPT>       center()       { return _center; }
	const Root<type::IsPoint,FPT> center() const { return _center; }

/// Returns true if circle is inside \c other circle
	template<typename FPT2>
	bool isInside( Circle_<FPT2> other ) const
	{
		return( _radius + _center.distTo( other.center() ) < other.radius() );
	}

/// Returns true if circle is inside rectangle defined by \c p1 and \c p2
	template<typename FPT2>
	bool isInside( const Root<type::IsPoint,FPT2>& p1, const Root<type::IsPoint,FPT2>& p2 ) const
	{
		const auto pair_pts = detail::getCorrectPoints( p1, p2 );
		const auto& pt1 = pair_pts.first;
		const auto& pt2 = pair_pts.second;
		if( _center.getX() + _radius < pt2.getX() )
			if( _center.getX() - _radius > pt2.getX() )
				if( _center.getY() + _radius > pt1.getY() )
					if( _center.getY() - _radius < pt1.getY() )
						return true;
		return false;
	}


	template<typename FPT2>
	bool operator == ( const Circle_<FPT2>& other ) const
	{
		if( _radius != other._radius )
			return false;
		if( _center != other._center )
			return false;
		return true;
	}
	template<typename FPT2>
	bool operator != ( const Circle_<FPT2>& other ) const
	{
		return !( *this == other );
	}
#ifdef HOMOG2D_USE_OPENCV
	void draw( cv::Mat& mat, CvDrawParams dp=CvDrawParams() )  const
	{
		cv::circle(
			mat,
			_center.getCvPti(),
			static_cast<int>(_radius),
			dp._dpValues._color,
			dp._dpValues._lineThickness,
			dp._dpValues._lineType
		);
	}
#endif // HOMOG2D_USE_OPENCV
};

//------------------------------------------------------------------
/// Base class, will be instanciated as a \ref Point2d or a \ref Line2d
/**
Parameters:
- LP: Line or Point
- FPT: Floating Point Type
*/
template<typename LP,typename FPT>
class Root
{
	template<typename U,typename V>
	friend class Hmatrix_;

// This is needed so we can convert from, say, Point2d_<float> to Point2d_<double>
	template<typename U,typename V>
	friend class Root;

	template<typename T>
	friend Root<type::IsPoint,T>
	operator * ( const Root<type::IsLine,T>&, const Root<type::IsLine,T>& );

	template<typename T>
	friend Root<type::IsLine,T>
	operator * ( const Root<type::IsPoint,T>&, const Root<type::IsPoint,T>& );

	template<typename T1,typename T2,typename T3>
	friend Root<T1,T3>
	detail::crossProduct( const Root<T2,T3>&, const Root<T2,T3>& );

	template<typename U,typename V>
	friend std::ostream&
	operator << ( std::ostream& f, const Root<U,V>& r );

	template<typename T1,typename T2,typename U,typename FPT1,typename FPT2>
	friend void detail::product( Root<T1,FPT1>&, const Hmatrix_<U,FPT2>&, const Root<T2,FPT1>& );

/*	private:
		Root( double a, double b, double c )
		{
			_v[0] = a;
			_v[1] = b;
			_v[2] = c;
		}*/

	public:

/// Constructor: build a point from two lines
		Root( const Root<type::IsLine,FPT>& v1, const Root<type::IsLine,FPT>& v2 )
		{
#ifndef HOMOG2D_NOCHECKS
			if( v1.isParallelTo(v2) )
				std::runtime_error( "unable to build point from these two lines, are parallel" );
#endif
			*this = detail::crossProduct<type::IsPoint>( v1, v2 );
		}

/// Constructor: build a line from two points
		Root( const Root<type::IsPoint,FPT>& v1, const Root<type::IsPoint,FPT>& v2 )
		{
#ifndef HOMOG2D_NOCHECKS
			if( v1 == v2 )
				std::runtime_error( "unable to build line from these two points, are the same" );
#endif
			*this = detail::crossProduct<type::IsLine>( v1, v2 );
			p_normalizeLine();
		}

/// Constructor: copy-constructor for lines
/**
\todo We should be able to declare this "explicit". This fails at present when attempting
to convert a line (or point) from double to float, but I don't get why...
*/
		template<typename T>
//		explicit
		Root( const Root<type::IsLine,T>& li )
		{
			impl_init_1_Line<T>( li, detail::RootHelper<LP>() );
		}

/// Constructor with single arg of type "Point"
/**
This will call one of the two overloads of \c impl_init_1_Point(), depending on type of object:
- if type is a point, then it can be seen as a copy constructor
- if type is a line, this will build a line from (0,0] to \c pt
*/
		template<typename T>
		Root( const Root<type::IsPoint,T>& pt )
		{
			impl_init_1_Point<T>( pt, detail::RootHelper<LP>() );
		}

/// Constructor: build from two numerical values, depends on the type
		template<typename T>
		Root( const T& v1, const T& v2 )
		{
			HOMOG2D_CHECK_IS_NUMBER(T);
			impl_init_2( v1, v2, detail::RootHelper<LP>() );
		}

/// Default constructor, depends on the type
		Root()
		{
			impl_init( detail::RootHelper<LP>() );
		}

	private:
		template<typename T,typename U>
		void p_copyFrom( const Root<T,U>& other )
		{
			_v[0] = static_cast<FPT>(other._v[0]);
			_v[1] = static_cast<FPT>(other._v[1]);
			_v[2] = static_cast<FPT>(other._v[2]);
		}
		/// Arg is a point, object is a point => copy constructor
		template<typename T>
		void impl_init_1_Point( const Root<type::IsPoint,T>& pt, const detail::RootHelper<type::IsPoint>& )
		{
//			*this = pt;   // we can't use this because it implies calling the constructor to build a converted point from T to FPT, thus infinite loop
			p_copyFrom( pt );
		}
		/// Arg is a point, object is a line: we build the line passing though (0,0) ant the given point
		template<typename T>
		void impl_init_1_Point( const Root<type::IsPoint,T>& pt, const detail::RootHelper<type::IsLine>& )
		{
			*this = detail::crossProduct<type::IsLine>( pt, Root<type::IsPoint,FPT>() );
			p_normalizeLine();
		}

		/// Arg is a line, object is a point: ILLEGAL INSTANCIATION
		template<typename T>
		void impl_init_1_Line( const Root<type::IsLine,T>&, const detail::RootHelper<type::IsPoint>& )
		{
			static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot build a point from a line" );
		}
		/// Arg is a line, object is a line => copy constructor
		template<typename T>
		void impl_init_1_Line( const Root<type::IsLine,T>& li, const detail::RootHelper<type::IsLine>& )
		{
			p_copyFrom( li );
		}

	public:
		FPT
		getCoord( GivenCoord gc, FPT other ) const
		{
			return impl_getCoord( gc, other, detail::RootHelper<LP>() );
		}

		Root<type::IsPoint,FPT>
		getPoint( GivenCoord gc, FPT other ) const
		{
			return impl_getPoint( gc, other, detail::RootHelper<LP>() );
		}

		/// Returns a pair of points that are lying on line ad distance \c dist from a point defined by one of its coordinates.
		template<typename FPT2>
		std::pair<Root<type::IsPoint,FPT>,Root<type::IsPoint,FPT>>
		getPoints( GivenCoord gc, FPT coord, FPT2 dist ) const
		{
			return impl_getPoints( gc, coord, dist, detail::RootHelper<LP>() );
		}

		/// Returns an orthogonal line to the one it is called on, at a point defined by one of its coordinates.
		Root<type::IsLine,FPT>
		getOrthogonalLine( GivenCoord gc, FPT other ) const
		{
			return impl_getOrthogonalLine( gc, other, detail::RootHelper<LP>() );
		}

		/// Returns an parallel line to the one it is called on, with \c pt lying on it.
		Root<type::IsLine,FPT>
		getParallelLine( const Root<type::IsPoint,FPT>& pt ) const
		{
			return impl_getParallelLine( pt, detail::RootHelper<LP>() );
		}

		template<typename T>
		void
		addOffset( LineOffset dir, T v )
		{
			HOMOG2D_CHECK_IS_NUMBER(T);
			impl_addOffset( dir, v, detail::RootHelper<LP>() );
		}

		FPT getX() const         { return impl_getX( detail::RootHelper<LP>() ); }
		FPT getY() const         { return impl_getY( detail::RootHelper<LP>() ); }
		std::array<FPT,3> get() const { return impl_get( detail::RootHelper<LP>() ); }

		void set( FPT x, FPT y ) { impl_set( x, y,   detail::RootHelper<LP>() ); }

		HOMOG2D_INUMTYPE distTo( const Root<type::IsPoint,FPT>& pt ) const
		{
			return impl_distToPoint( pt, detail::RootHelper<LP>() );
		}
		HOMOG2D_INUMTYPE distTo( const Root<type::IsLine,FPT>& li ) const
		{
			return impl_distToLine( li, detail::RootHelper<LP>() );
		}

		template<typename T>
		bool isParallelTo( const Root<T,FPT>& li ) const
		{
			return impl_isParallelTo( li, detail::RootHelper<T>() );
		}
		template<typename T>
		bool isParallelTo( const Segment_<T>& seg ) const;

/// Returns angle in rad. between the lines
/** Please check out warning described in impl_getAngle() */
		template<typename T>
		HOMOG2D_INUMTYPE getAngle( const Root<T,FPT>& other ) const
		{
			return impl_getAngle( other, detail::RootHelper<T>() );
		}
		template<typename T>
		HOMOG2D_INUMTYPE getAngle( const Segment_<T>& seg ) const;

	private:
		FPT impl_getX( const detail::RootHelper<type::IsPoint>& /* dummy */ ) const
		{
			return _v[0]/_v[2];
		}
		FPT impl_getY( const detail::RootHelper<type::IsPoint>& /* dummy */ ) const
		{
			return _v[1]/_v[2];
		}

		std::array<FPT,3> impl_get( const detail::RootHelper<type::IsPoint>& /* dummy */ ) const
		{
			static_assert( detail::AlwaysFalse<LP>::value, "illegal for points" );
		}
		std::array<FPT,3> impl_get( const detail::RootHelper<type::IsLine>& /* dummy */ ) const
		{
			return std::array<FPT,3> { _v[0], _v[1], _v[2] };
		}

		void impl_set( FPT x, FPT y, const detail::RootHelper<type::IsPoint>& /* dummy */ )
		{
			_v[0] = x;
			_v[1] = y;
			_v[2] = 1.;
		}
		void impl_set( FPT, FPT, const detail::RootHelper<type::IsLine>& /* dummy */ )
		{
			static_assert( detail::AlwaysFalse<LP>::value, "Invalid call for lines" );
		}

		template<typename T>
		void
		impl_addOffset( LineOffset dir, T v, const detail::RootHelper<type::IsLine>& );

		HOMOG2D_INUMTYPE impl_distToPoint( const Root<type::IsPoint,FPT>&, const detail::RootHelper<type::IsPoint>& ) const;
		HOMOG2D_INUMTYPE impl_distToPoint( const Root<type::IsPoint,FPT>&, const detail::RootHelper<type::IsLine>&  ) const;
		HOMOG2D_INUMTYPE impl_distToLine(  const Root<type::IsLine,FPT>&,  const detail::RootHelper<type::IsPoint>& ) const;
		HOMOG2D_INUMTYPE impl_distToLine(  const Root<type::IsLine,FPT>&,  const detail::RootHelper<type::IsLine>&  ) const;

		HOMOG2D_INUMTYPE impl_getAngle(     const Root<LP,FPT>&, const detail::RootHelper<type::IsLine>&  ) const;
		HOMOG2D_INUMTYPE impl_getAngle(     const Root<LP,FPT>&, const detail::RootHelper<type::IsPoint>& ) const;
		bool impl_isParallelTo( const Root<LP,FPT>&, const detail::RootHelper<type::IsLine>&  ) const;
		bool impl_isParallelTo( const Root<LP,FPT>&, const detail::RootHelper<type::IsPoint>& ) const;

		FPT impl_getCoord( GivenCoord gc, FPT other, const detail::RootHelper<type::IsLine>& ) const;

		Root<type::IsPoint,FPT> impl_getPoint( GivenCoord gc, FPT other, const detail::RootHelper<type::IsLine>& ) const;
		template<typename FPT2>
		std::pair<Root<type::IsPoint,FPT>,Root<type::IsPoint,FPT>> impl_getPoints( GivenCoord gc, FPT coord, FPT2 dist, const detail::RootHelper<type::IsLine>& ) const;
		template<typename FPT2>
		std::pair<Root<type::IsPoint,FPT>,Root<type::IsPoint,FPT>> impl_getPoints( GivenCoord gc, FPT coord, FPT2 dist, const detail::RootHelper<type::IsPoint>& ) const;

		void impl_op_stream( std::ostream&, const Root<type::IsPoint,FPT>& ) const;
		void impl_op_stream( std::ostream&, const Root<type::IsLine,FPT>&  ) const;

	public:
/// Sub-type, holds result of rectangle intersection, see intersectsRectangle().
/// Only defined for Point2d
/// \todo change name (as this will be used also for line-circle intersection)
	struct Intersect
	{
		template<typename U,typename V>
		friend class Root;

		public:
			bool operator()() const
			{
				return _doesIntersect;
			}
			Intersect()
			{}
			Intersect( const Root<type::IsPoint,FPT>& p1, const Root<type::IsPoint,FPT>& p2 ) : ptA(p1), ptB(p2)
			{
				_doesIntersect = true;
			}
			std::pair<Root<type::IsPoint,FPT>,Root<type::IsPoint,FPT>>
			get() const
			{
				return std::make_pair( ptA, ptB );
			}
		private:
			Root<type::IsPoint,FPT> ptA;
			Root<type::IsPoint,FPT> ptB;
			bool _doesIntersect = false;
	};

	public:
		Intersect intersectsRectangle( const Root<type::IsPoint,FPT>& pt1, const Root<type::IsPoint,FPT>& pt2 ) const;
		template<typename T>
		Intersect intersectsCircle( const Root<type::IsPoint,FPT>& pt0, T radius ) const;

		bool isInside( const Root<type::IsPoint,FPT>& pt1, const Root<type::IsPoint,FPT>& pt2 ) const
		{
			return impl_isInsideRect( pt1, pt2, detail::RootHelper<LP>() );
		}

		template<typename T>
		bool isInside( const Root<type::IsPoint,FPT>& center, T radius ) const
		{
			HOMOG2D_CHECK_IS_NUMBER(T);
			return impl_isInsideCircle( center, radius, detail::RootHelper<LP>() );
		}
		template<typename T>
		bool isInside( Circle_<T> cir ) const
		{
			HOMOG2D_CHECK_IS_NUMBER(T);
			return impl_isInsideCircle( cir.center(), cir.radius(), detail::RootHelper<type::IsPoint>() );
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

// Constructor: build line from two OpenCv points
/*		template<typename T>
		Root( const cv::Point2d& p1, const cv::Point2d& p2 )
		{
			impl_init_2( v1, v2, detail::RootHelper<LP>() );
		}*/

		bool draw( cv::Mat& mat, CvDrawParams dp=CvDrawParams() )  const
		{
			return impl_draw( mat, dp, detail::RootHelper<LP>() );
		}

		template<typename RT>
		RT getCvPt() const { return RT( getX(), getY() ); }

		cv::Point2i getCvPti() const { return impl_getCvPt( detail::RootHelper<LP>(), cv::Point2i() ); }
		cv::Point2d getCvPtd() const { return impl_getCvPt( detail::RootHelper<LP>(), cv::Point2d() ); }
		cv::Point2f getCvPtf() const { return impl_getCvPt( detail::RootHelper<LP>(), cv::Point2f() ); }

/// Constructor: build from a single OpenCv point.
		template<typename T>
		Root( cv::Point_<T> pt )
		{
			impl_init_opencv( pt, detail::RootHelper<LP>() );
		}
#endif

	static HOMOG2D_INUMTYPE& nullAngleValue()     { return _zeroAngleValue; }
	static HOMOG2D_INUMTYPE& nullDistance()       { return _zeroDistance; }
	static HOMOG2D_INUMTYPE& nullOffsetValue()    { return _zeroOffset; }
	static HOMOG2D_INUMTYPE& nullOrthogDistance() { return _zeroOrthoDistance; }

//////////////////////////
//      DATA SECTION    //
//////////////////////////

	private:
		std::array<FPT,3> _v; ///< data, uses the template parameter FPT (for "Floating Point Type")

		static HOMOG2D_INUMTYPE _zeroAngleValue;       /// Used in isParallel();
		static HOMOG2D_INUMTYPE _zeroDistance;         /// Used to define points as identical
		static HOMOG2D_INUMTYPE _zeroOrthoDistance;    /// Used to check for different points on a flat rectangle, see Root::getCorrectPoints()
		static HOMOG2D_INUMTYPE _zeroOffset;           /// Used to compare lines

//////////////////////////
//   PRIVATE FUNCTIONS  //
//////////////////////////
	private:
		void p_normalizeLine() const { impl_normalizeLine( detail::RootHelper<LP>() ); }

		Root<LP,FPT>::Intersect
		impl_intersectsRectangle( const Root<type::IsPoint,FPT>& p0, const Root<type::IsPoint,FPT>& p1, const detail::RootHelper<type::IsLine>& ) const;
		Root<LP,FPT>::Intersect
		impl_intersectsRectangle( const Root<type::IsPoint,FPT>& p0, const Root<type::IsPoint,FPT>& p1, const detail::RootHelper<type::IsPoint>& ) const;

		template<typename T>
		Root<LP,FPT>::Intersect
		impl_intersectsCircle( const Root<type::IsPoint,FPT>& pt, T radius, const detail::RootHelper<type::IsLine>& ) const;
		template<typename T>
		Root<LP,FPT>::Intersect
		impl_intersectsCircle( const Root<type::IsPoint,FPT>& pt, T radius, const detail::RootHelper<type::IsPoint>& ) const;


		bool impl_isInsideRect( const Root<type::IsPoint,FPT>&, const Root<type::IsPoint,FPT>&, const detail::RootHelper<type::IsPoint>& ) const;
		bool impl_isInsideRect( const Root<type::IsPoint,FPT>&, const Root<type::IsPoint,FPT>&, const detail::RootHelper<type::IsLine>&  ) const;

		template<typename T>
		bool impl_isInsideCircle( const Root<type::IsPoint,FPT>&, T radius, const detail::RootHelper<type::IsLine>&  ) const;
		template<typename T>
		bool impl_isInsideCircle( const Root<type::IsPoint,FPT>&, T radius, const detail::RootHelper<type::IsPoint>& ) const;

		void impl_normalizeLine( const detail::RootHelper<type::IsLine>& ) const;

		Root<type::IsLine,FPT> impl_getOrthogonalLine( GivenCoord gc, FPT val, const detail::RootHelper<type::IsLine>& ) const;
		Root<type::IsLine,FPT> impl_getOrthogonalLine( GivenCoord gc, FPT val, const detail::RootHelper<type::IsPoint>& ) const;
		Root<type::IsLine,FPT> impl_getParallelLine( const Root<type::IsPoint,FPT>&,    const detail::RootHelper<type::IsLine>& ) const;
		Root<type::IsLine,FPT> impl_getParallelLine( const Root<type::IsPoint,FPT>&,    const detail::RootHelper<type::IsPoint>& ) const;

		bool impl_op_equal( const Root<LP,FPT>&, const detail::RootHelper<type::IsLine>& ) const;
		bool impl_op_equal( const Root<LP,FPT>&, const detail::RootHelper<type::IsPoint>& ) const;

		Root<type::IsPoint,FPT> impl_op_product( const Root<type::IsLine,FPT>& , const Root<type::IsLine,FPT>& , const detail::RootHelper<type::IsPoint>& ) const;
		Root<type::IsLine,FPT>  impl_op_product( const Root<type::IsPoint,FPT>&, const Root<type::IsPoint,FPT>&, const detail::RootHelper<type::IsLine>&  ) const;

#ifdef HOMOG2D_USE_OPENCV
		template<typename OPENCVT>
		OPENCVT impl_getCvPt( const detail::RootHelper<type::IsPoint>&, const OPENCVT& ) const;

/// Build point from Opencv point
		template<typename T>
		void impl_init_opencv( cv::Point_<T> pt, const detail::RootHelper<type::IsPoint>& )
		{
			impl_init_2( pt.x, pt.y, detail::RootHelper<type::IsPoint>() );
		}
/// Build line from Opencv point
		template<typename T>
		void impl_init_opencv( cv::Point_<T> pt, const detail::RootHelper<type::IsLine>& )
		{
			Root<type::IsPoint,FPT> p(pt);
			impl_init_1_Point<FPT>( p, detail::RootHelper<type::IsLine>() );
		}

		bool impl_draw( cv::Mat&, const CvDrawParams&, const detail::RootHelper<type::IsPoint>& ) const;
		bool impl_draw( cv::Mat&, const CvDrawParams&, const detail::RootHelper<type::IsLine>& )  const;
#endif

		/// Called by default constructor, overload for lines
		void impl_init( const detail::RootHelper<type::IsLine>& )
		{
			_v[0] = 1.;
			_v[1] = 0.;
			_v[2] = 0.;
		}
		/// Called by default constructor, overload for points. Initialize to (0,0)
		void impl_init( const detail::RootHelper<type::IsPoint>& )
		{
			_v[0] = 0.;
			_v[1] = 0.;
			_v[2] = 1.;
		}
		template<typename T>
		void impl_init_2( const T& v1, const T& v2, const detail::RootHelper<type::IsPoint>& );
		template<typename T>
		void impl_init_2( const T& v1, const T& v2, const detail::RootHelper<type::IsLine>& );
};


/// Instanciation of static variable
template<typename LP,typename FPT>
HOMOG2D_INUMTYPE Root<LP,FPT>::_zeroAngleValue = 0.001; // 1 thousand of a radian (tan = 0.001 too)

/// Instanciation of static variable
template<typename LP,typename FPT>
HOMOG2D_INUMTYPE Hmatrix_<LP,FPT>::_zeroDeterminantValue = 1E-20;

/// Instanciation of static variable
template<typename LP,typename FPT>
HOMOG2D_INUMTYPE Root<LP,FPT>::_zeroDistance = 1E-15;

/// Instanciation of static variable
template<typename LP,typename FPT>
HOMOG2D_INUMTYPE Root<LP,FPT>::_zeroOrthoDistance = 1E-18;

/// Instanciation of static variable
template<typename LP,typename FPT>
HOMOG2D_INUMTYPE Root<LP,FPT>::_zeroOffset = 1E-15;


#ifdef HOMOG2D_USE_OPENCV
/// Free function to return an OpenCv point
/**
- RT: return type
- FPT: Floating Point Type

User code needs to provide the requested type as template argument:
\code
auto p1 = getCvPt<cv::Point2di>( pt );
auto p2 = getCvPt<cv::Point2df>( pt );
auto p3 = getCvPt<cv::Point2dd>( pt );
\endcode
*/
template<typename RT,typename FPT>
RT
getCvPt( const Root<type::IsPoint,FPT>& pt )
{
	return pt.template getCvPt<RT>();
}

/// Free function to return an OpenCv point (double)
template<typename FPT>
cv::Point2d
getCvPtd( const Root<type::IsPoint,FPT>& pt )
{
	return pt.getCvPtd();
}
/// Free function to return an OpenCv point (float)
template<typename FPT>
cv::Point2f
getCvPtf( const Root<type::IsPoint,FPT>& pt )
{
	return pt.getCvPtf();
}
/// Free function to return an OpenCv point (integer)
template<typename FPT>
cv::Point2i
getCvPti( const Root<type::IsPoint,FPT>& pt )
{
	return pt.getCvPti();
}

/// Free function, returns a vector of OpenCv points from a vector of points
/**
- RT: return type
- FPT: Floating Point Type

User code needs to provide the requested type as template argument:
\code
auto v1 = getCvPts<cv::Point2di>( myvec );
auto v2 = getCvPts<cv::Point2df>( myvec );
auto v3 = getCvPts<cv::Point2dd>( myvec );
\endcode
*/
template<typename RT,typename FPT>
std::vector<RT>
getCvPts( const std::vector<Root<type::IsPoint,FPT>>& vpt )
{
	std::vector<RT> vout( vpt.size() );
	auto it = vout.begin();
	for( const auto& pt: vpt )
	{
		*it = getCvPt<RT>(pt);
		it++;
	}
	return vout;
}
#endif // HOMOG2D_USE_OPENCV


//------------------------------------------------------------------
/// This namespace holds some private stuff
namespace detail {

/// Private free function, get top-left and bottom-right points from two arbitrary points
template<typename FPT>
std::pair<Root<type::IsPoint,FPT>,Root<type::IsPoint,FPT>>
getCorrectPoints( const Root<type::IsPoint,FPT>& p0, const Root<type::IsPoint,FPT>& p1 )
{
#ifndef HOMOG2D_NOCHECKS
	if(
		   std::fabs( p0.getX() - p1.getX() ) < Root<type::IsPoint,FPT>::nullOrthogDistance()
		|| std::fabs( p0.getY() - p1.getY() ) < Root<type::IsPoint,FPT>::nullOrthogDistance()
	)
		throw std::runtime_error( "error: a coordinate of the 2 points are identical, does not define a rectangle" );
#endif
	Root<type::IsPoint,FPT> p00( std::min(p0.getX(), p1.getX()), std::min(p0.getY(), p1.getY()) );
	Root<type::IsPoint,FPT> p11( std::max(p0.getX(), p1.getX()), std::max(p0.getY(), p1.getY()) );
	return std::make_pair( p00, p11 );
}

//------------------------------------------------------------------
/// Private free function, returns true if point \c pt is inside the rectangle defined by (\c p00 , \c p11)
template<typename FPT>
bool
ptIsInside( const Root<type::IsPoint,FPT>& pt, const Root<type::IsPoint,FPT>& p00, const Root<type::IsPoint,FPT>& p11 )
{
	if( pt.getX() >= p00.getX() && pt.getX() <= p11.getX() )
		if( pt.getY() >= p00.getY() && pt.getY() <= p11.getY() )
			return true;
	return false;
}

/// Private free function, swap the points so that \c ptA.x <= \c ptB.x, and if equal, sorts on y
template<typename FPT>
void
fix_order( Root<type::IsPoint,FPT>& ptA, Root<type::IsPoint,FPT>& ptB )
{
	if( ptA.getX() > ptB.getX() )
		std::swap( ptA, ptB );
	else
		if( ptA.getX() == ptB.getX() )
			if( ptA.getY() > ptB.getY() )
				std::swap( ptA, ptB );
}


#ifdef HOMOG2D_USE_EIGEN
///  Build Homography from 2 sets of 4 points, using Eigen
/**
see
- https://eigen.tuxfamily.org/dox/group__TutorialLinearAlgebra.html
- https://eigen.tuxfamily.org/dox/group__DenseMatrixManipulation__chapter.html
*/
template<typename FPT>
Hmatrix_<type::IsHomogr,FPT>
buildFrom4Points_Eigen(
	const std::vector<Root<type::IsPoint,FPT>>& vpt1, ///< source points
	const std::vector<Root<type::IsPoint,FPT>>& vpt2  ///< destination points
)
{
	Eigen::MatrixXd A = Eigen::MatrixXd::Zero(8,8);
	Eigen::VectorXd b(8);

	for( int i=0; i<4; i++ )
	{
		auto u1 = vpt1[i].getX();
		auto v1 = vpt1[i].getY();
		auto u2 = vpt2[i].getX();
		auto v2 = vpt2[i].getY();

		b(2*i)   = u2;
		b(2*i+1) = v2;

		A(i*2,0) = A(i*2+1,3) = u1;
		A(i*2,1) = A(i*2+1,4) = v1;
		A(i*2,2) = A(i*2+1,5) = 1.;

		A(i*2,6)   = - u1 * u2;
		A(i*2,7)   = - v1 * u2;
		A(i*2+1,6) = - u1 * v2;
		A(i*2+1,7) = - v1 * v2;
	}


#if 0
	Eigen::VectorXd X = A.ldlt().solve(b); // for some reason this does not work...
#else
	Eigen::MatrixXd Ai = A.inverse();
	Eigen::VectorXd X = Ai * b;
#endif

	Hmatrix_<type::IsHomogr,FPT> H;
//	std::cout << H << '\n';

	for( int i=0; i<8; i++ )
		H.set( i/3, i%3, X(i) );
	H.set(2, 2, 1.);

	return H;
}
#endif

//------------------------------------------------------------------
#ifdef HOMOG2D_USE_OPENCV
///  Build Homography from 2 sets of 4 points, using Opencv
/**
- see https://docs.opencv.org/master/d9/d0c/group__calib3d.html#ga4abc2ece9fab9398f2e560d53c8c9780

\note With current Opencv installed on current machine, it seems that \c cv::getPerspectiveTransform()
requires that the points are "CV_32F" (\c float), and NOT double.
*/
template<typename FPT>
Hmatrix_<type::IsHomogr,FPT>
buildFrom4Points_Opencv (
	const std::vector<Root<type::IsPoint,FPT>>& vpt1, ///< source points
	const std::vector<Root<type::IsPoint,FPT>>& vpt2  ///< destination points
)
{
	const auto& src = getCvPts<cv::Point2f>( vpt1 );
	const auto& dst = getCvPts<cv::Point2f>( vpt2 );
	return cv::getPerspectiveTransform( src, dst );
}
#endif

} // namespace detail


//------------------------------------------------------------------
/// Build Homography from 2 sets of 4 points
/**
- Requires either Eigen or Opencv
- we build a 8x8 matrix A and a 8x1 vector B, and get the solution from X = A^-1 B
- see this for details:
https://skramm.lautre.net/files/misc/Kramm_compute_H_from_4pts.pdf
*/
template<typename MT,typename FPT>
void
Hmatrix_<MT,FPT>::buildFrom4Points(
	const std::vector<Root<type::IsPoint,FPT>>& vpt1,  ///< source points
	const std::vector<Root<type::IsPoint,FPT>>& vpt2,  ///< destination points
	int method                                         ///< 0: Eigen, 1: Opencv
)
{
	assert( vpt1.size() == 4 );
	assert( vpt2.size() == 4 );
	assert( method == 0 || method == 1 );

	if( method == 1 )
	{
#ifdef HOMOG2D_USE_EIGEN
//		std::cerr << "H compute: using Eigen\n";
		*this = detail::buildFrom4Points_Eigen( vpt1, vpt2 );
#else
		throw std::runtime_error( "Unable, build without Eigen support" );
#endif
	}
	else
	{
#ifdef HOMOG2D_USE_OPENCV
//		std::cerr << "H compute: using Opencv\n";
		*this = detail::buildFrom4Points_Opencv( vpt1, vpt2 );
#else
		throw std::runtime_error( "Unable, build without Opencv support" );
#endif
	}
}

//------------------------------------------------------------------
/// A line segment, defined by two points
/**
- Storage: "smallest" point is always stored as first element (see constructor)
*/
template<typename FPT>
class Segment_
{
	private:
		Root<type::IsPoint,FPT> _ptS1, _ptS2;

	public:
/// sub type, holds intersection between two segments
	struct SIntersect
	{
		template<typename U>
		friend class Segment_;

		public:
			bool operator()() const
			{
				return _doesIntersect;
			}
			SIntersect()
			{}
/*			explicit SIntersect( const Root<type::IsPoint,FPT>& pti ) : _ptIntersect(pti)
			{
				_doesIntersect = true;
			}*/
			Root<type::IsPoint,FPT>
			get() const
			{
				return _ptIntersect;
			}
		private:
			Root<type::IsPoint,FPT> _ptIntersect;
			bool _doesIntersect = false;
	};

	public:
/// Default constructor: initializes segment to (0,0)--(1,1)
		Segment_(): _ptS2(1.,1.)
		{}
/// Contructor 2: build segment from two points
		Segment_( Root<type::IsPoint,FPT> p1, Root<type::IsPoint,FPT> p2 ):
			_ptS1(p1), _ptS2(p2)
		{
#ifndef HOMOG2D_NOCHECKS
			if( p1 == p2 )
				throw std::runtime_error( "failure, cannot build a segment with two identical points" );
#endif
			detail::fix_order( _ptS1, _ptS2 );
		}
/// Setter
		void set( const Root<type::IsPoint,FPT>& p1, const Root<type::IsPoint,FPT>& p2 )
		{
			_ptS1 = p1;
			_ptS2 = p2;
			detail::fix_order( _ptS1, _ptS2 );
		}

/// Get length
		FPT length() const
		{
			return _ptS1.distTo( _ptS2 );
		}
		template<typename U>
		HOMOG2D_INUMTYPE getAngle( const U& other ) const
		{
			return other.getAngle( this->getLine() );
		}
/// Comparison operator
		bool operator == ( const Segment_& s2 ) const
		{
			if( _ptS1 != s2._ptS1 )
				return false;
			if( _ptS2 != s2._ptS2 )
				return false;
			return true;
		}
		bool operator != ( const Segment_& s2 ) const
		{
			return !(*this == s2);
		}
		template<typename U>
		friend std::ostream& operator << ( std::ostream& f, const Segment_<U>& seg );

/// Returns the points as a std::pair
/** The one with smallest x coordinate will be returned as "first". If x-coordinate are equal, then
the one with smallest y-coordinate will be returned first */
		std::pair<Root<type::IsPoint,FPT>,Root<type::IsPoint,FPT>>
		get() const
		{
			return std::make_pair( _ptS1, _ptS2 );
		}

/// Returns supporting line
		Root<type::IsLine,FPT> getLine() const
		{
			return _ptS1 * _ptS2;
		}
		SIntersect intersects( const Segment_<FPT>& ) const;

		template<typename T>
		bool isParallelTo( const T& other ) const
		{
			static_assert(
				std::is_same<T,Segment_<FPT>>::value ||
				std::is_same<T,Root<type::IsLine,FPT>>::value,
				"type needs to be a segment or a line" );
			return getLine().isParallelTo( other );
		}

		/// Returns point that at middle distance between \c p1 and \c p2
		Root<type::IsPoint,FPT>
		getMiddlePoint() const
		{
			return Root<type::IsPoint,FPT>(
				( _ptS1.getX() + _ptS2.getX() ) / 2.,
				( _ptS1.getY() + _ptS2.getY() ) / 2.
			);
		}

#ifdef HOMOG2D_USE_OPENCV
	void draw( cv::Mat& mat, CvDrawParams dp=CvDrawParams() )
	{
		cv::line( mat, _ptS1.getCvPtd(), _ptS2.getCvPtd(), dp._dpValues._color, dp._dpValues._lineThickness, dp._dpValues._lineType );
	}
#endif
};

template<typename U>
inline
std::ostream& operator << ( std::ostream& f, const Segment_<U>& seg )
{
	f << seg._ptS1 << "-" << seg._ptS2;
	return f;
}

//------------------------------------------------------------------
template<typename FPT1,typename FPT2,typename FPT3>
Segment_<FPT1>
getSegment( const Circle_<FPT2>& c1, const Circle_<FPT3>& c2 )
{
	return Segment_<FPT1>( c1.center(), c2.center() );
}

//------------------------------------------------------------------
/// Polyline
template<typename FPT>
class Polyline_
{
	private:
		std::vector<Root<type::IsPoint,FPT>> _plinevec;
		bool _isClosed = false;

	public:
		Polyline_()
		{}
		Polyline_( Root<type::IsPoint,FPT> pt )
		{
			_plinevec.push_back( pt );
		}
		/// Returns the number of points (not segments!)
		size_t size() const { return _plinevec.size(); }

		void add( Root<type::IsPoint,FPT> pt )
		{
#ifndef HOMOG2D_NOCHECKS
			if( size() )
				if( pt == _plinevec.back() )
					throw std::runtime_error( "Error, cannot add a point identical to previous one" );
#endif
			_plinevec.push_back( pt );
		}
#ifdef HOMOG2D_USE_OPENCV

		template<typename FPT2>
		void setPoints( const std::vector<Root<type::IsPoint,FPT2>>& vec )
		{
			_plinevec.resize( vec.size() );
			auto it = std::begin( _plinevec );
			for( const auto& pt: vec )
			{
				*it = pt;
				it++;
			}
		}
		bool& isClosed() { return _isClosed; }

		void draw( cv::Mat& mat, CvDrawParams dp=CvDrawParams() )
		{
			if( size() < 2 ) // nothing to draw
				return;

			for( size_t i=0; i<size()-1; i++ )
			{
				const auto& pt1 = _plinevec[i];
				const auto& pt2 = _plinevec[i+1];
				Segment_<FPT>(pt1,pt2).draw( mat, dp );
			}
			if( size() < 3 ) // nothing to draw
				return;
			if( _isClosed )
				Segment_<FPT>(_plinevec.front(),_plinevec.back() ).draw( mat, dp );
		}
#endif
};

//------------------------------------------------------------------
/// Implementation of line::isParallelTo( Segment )
template<typename LP,typename FPT>
template<typename T>
bool
Root<LP,FPT>::isParallelTo( const Segment_<T>& seg ) const
{
	return impl_isParallelTo( seg.getLine(), detail::RootHelper<LP>() );
}

/// Returns angle in rad. between the line and the segment \c seg
template<typename LP,typename FPT>
template<typename T>
HOMOG2D_INUMTYPE
Root<LP,FPT>::getAngle( const Segment_<T>& seg ) const
{
	return impl_getAngle( seg.getLine(), detail::RootHelper<LP>() );
}


//------------------------------------------------------------------
namespace detail {

/// Helper function
template<typename T>
bool
isBetween( T v, T v1, T v2 )
{
	if( v >= std::min( v1, v2 ) )
		if( v <= std::max( v1, v2 ) )
			return true;
	return false;
}
} // namespace detail

//------------------------------------------------------------------
/// Computes intersection between 2 segments
/**
Algorithm:<br>
We check if the intersection point lies in between the range of both segments, both on x and on y
*/
template<typename FPT>
typename Segment_<FPT>::SIntersect
Segment_<FPT>::intersects( const Segment_<FPT>& s2 ) const
{
	typename Segment_<FPT>::SIntersect out;
	auto l1 = getLine();
	auto l2 = s2.getLine();
	if( l1.isParallelTo( l2 ) )
		return out;
	out._ptIntersect = l1 * l2;   // intersection point

	const auto& pi   = out._ptIntersect;
	const auto& ptA1 = this->get().first;
	const auto& ptA2 = this->get().second;
	const auto& ptB1 = s2.get().first;
	const auto& ptB2 = s2.get().second;
//	std::cout << "s1=" << *this << " s2=" << s2 << " pi=" << pi << '\n';

	if( detail::isBetween( pi.getX(), ptA1.getX(), ptA2.getX() ) )
		if( detail::isBetween( pi.getY(), ptA1.getY(), ptA2.getY() ) )
			if( detail::isBetween( pi.getX(), ptB1.getX(), ptB2.getX() ) )
				if( detail::isBetween( pi.getY(), ptB1.getY(), ptB2.getY() ) )
					out._doesIntersect = true;
	return out;
}

//------------------------------------------------------------------
/// Overload for points
template<typename LP,typename FPT>
void
Root<LP,FPT>::impl_op_stream( std::ostream& f, const Root<type::IsPoint,FPT>& r ) const
{
	f << '[' << r.getX() << ',' << r.getY() << "] ";
}

/// Overload for lines
template<typename LP,typename FPT>
void
Root<LP,FPT>::impl_op_stream( std::ostream& f, const Root<type::IsLine,FPT>& r ) const
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
/**
\todo Checkout: in what situation will we be unable to normalize?
Is the test below relevant? Clarify.
*/
template<typename LP,typename FPT>
void
Root<LP,FPT>::impl_normalizeLine( const detail::RootHelper<type::IsLine>& ) const
{
	auto sq = std::hypot( _v[0], _v[1] );
	if( sq <= std::numeric_limits<double>::epsilon() )
		throw std::runtime_error( "unable to normalize line, sq=" + std::to_string(sq) );

	for( int i=0; i<3; i++ )
		const_cast<Root<LP,FPT>*>(this)->_v[i] /= sq; // needed to remove constness

	if( std::signbit(_v[0]) ) // a always >0
		for( int i=0; i<3; i++ )
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
FPT
Root<LP,FPT>::impl_getCoord( GivenCoord gc, FPT other, const detail::RootHelper<type::IsLine>& ) const
{
	const auto a = static_cast<HOMOG2D_INUMTYPE>( _v[0] );
	const auto b = static_cast<HOMOG2D_INUMTYPE>( _v[1] );
//	auto c = static_cast<HOMOG2D_INUMTYPE>( _v[2] ); // useless, automatic conversion will take place
	if( gc == GivenCoord::X )
		return ( -a * other - _v[2] ) / b;
	else
		return ( -b * other - _v[2] ) / a;
}

template<typename LP,typename FPT>
Root<type::IsPoint,FPT>
Root<LP,FPT>::impl_getPoint( GivenCoord gc, FPT other, const detail::RootHelper<type::IsLine>& dummy ) const
{
	auto coord = impl_getCoord( gc, other, dummy );
	if( gc == GivenCoord::X )
		return Root<type::IsPoint,FPT>( other, coord );
	return Root<type::IsPoint,FPT>( coord, other );
}

//------------------------------------------------------------------
/// ILLEGAL INSTANCIATION
template<typename LP,typename FPT>
template<typename FPT2>
std::pair<Root<type::IsPoint,FPT>,Root<type::IsPoint,FPT>>
Root<LP,FPT>::impl_getPoints( GivenCoord, FPT, FPT2, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call getPoints() on a point" );
}

//------------------------------------------------------------------
/// Returns pair of points on line at distance \c dist from point on line at coord \c coord. Implementation for lines
template<typename LP,typename FPT>
template<typename FPT2>
std::pair<Root<type::IsPoint,FPT>,Root<type::IsPoint,FPT>>
Root<LP,FPT>::impl_getPoints( GivenCoord gc, FPT coord, FPT2 dist, const detail::RootHelper<type::IsLine>& ) const
{
	auto pt = impl_getPoint( gc, coord, detail::RootHelper<type::IsLine>() );

	const HOMOG2D_INUMTYPE a = static_cast<HOMOG2D_INUMTYPE>(_v[0]);
	const HOMOG2D_INUMTYPE b = static_cast<HOMOG2D_INUMTYPE>(_v[1]);
	auto coeff = static_cast<HOMOG2D_INUMTYPE>(dist) / std::sqrt( a*a + b*b );

	Root<type::IsPoint,FPT> pt1(
        pt.getX() -  b * coeff,
        pt.getY() +  a * coeff
	);
	Root<type::IsPoint,FPT> pt2(
        pt.getX() +  b * coeff,
        pt.getY() -  a * coeff
	);
	detail::fix_order( pt1, pt2 );
	return std::make_pair( pt1, pt2 );
}

//------------------------------------------------------------------
/// Illegal instanciation
template<typename LP,typename FPT>
Root<type::IsLine,FPT>
Root<LP,FPT>::impl_getOrthogonalLine( GivenCoord, FPT, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call getOrthogonalLine() on a point" );
}

/// Returns an orthogonal line, implementation of getOrthogonalLine().
template<typename LP,typename FPT>
Root<type::IsLine,FPT>
Root<LP,FPT>::impl_getOrthogonalLine( GivenCoord gc, FPT val, const detail::RootHelper<type::IsLine>& ) const
{
	auto other_val = impl_getCoord( gc, val, detail::RootHelper<type::IsLine>() );

	Root<type::IsPoint,FPT> pt( other_val, val ) ;
	if( gc == GivenCoord::X )
		pt.set( val, other_val );

	Root<type::IsLine,FPT> out;
	out._v[0] = -_v[1];
	out._v[1] =  _v[0];
	out._v[2] = _v[1] * pt.getX() - _v[0] * pt.getY();
	out.p_normalizeLine();
	return out;
}

//------------------------------------------------------------------
/// Illegal instanciation
template<typename LP,typename FPT>
Root<type::IsLine,FPT>
Root<LP,FPT>::impl_getParallelLine( const Root<type::IsPoint,FPT>&, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call getParallelLine() on a point" );
}

/// Returns an parallel line, implementation of getParallelLine().
template<typename LP,typename FPT>
Root<type::IsLine,FPT>
Root<LP,FPT>::impl_getParallelLine( const Root<type::IsPoint,FPT>& pt, const detail::RootHelper<type::IsLine>& ) const
{
	Root<type::IsLine,FPT> out = *this;
	out._v[2] = -_v[0] * pt.getX() - _v[1] * pt.getY();
	out.p_normalizeLine();
	return out;
}

//------------------------------------------------------------------
/// Comparison operator, used for lines
/**
Definition used: two lines will be equal:
- if they are not parallel
AND
- if their offset (3 third value) is less than nullOffsetValue()
*/
template<typename LP,typename FPT>
bool
Root<LP,FPT>::impl_op_equal( const Root<LP,FPT>& other, const detail::RootHelper<type::IsLine>& ) const
{
//	std::cout << __FUNCTION__ << "(IsLine):\n -this=" << *this << "\n -other=" << other <<"\n";
	if( !this->isParallelTo( other ) )
		return false;

	if( std::fabs( _v[2] - other._v[2] ) > nullOffsetValue() )
		return false;

//	std::cout << "=>TRUE\n";
	return true;
}

/// Comparison operator, used for points
template<typename LP,typename FPT>
bool
Root<LP,FPT>::impl_op_equal( const Root<LP,FPT>& other, const detail::RootHelper<type::IsPoint>& ) const
{
	auto dist = this->distTo( other );
	if( dist < nullDistance() )
		return true;
	return false;
}

//------------------------------------------------------------------
/// Inner implementation details
namespace detail {

	/// Cross product, see https://en.wikipedia.org/wiki/Cross_product#Coordinate_notation
	template<typename Out,typename In,typename FPT>
	Root<Out,FPT> crossProduct( const Root<In,FPT>& r1, const Root<In,FPT>& r2 )
	{
		auto r1_a = static_cast<HOMOG2D_INUMTYPE>(r1._v[0]);
		auto r1_b = static_cast<HOMOG2D_INUMTYPE>(r1._v[1]);
		auto r1_c = static_cast<HOMOG2D_INUMTYPE>(r1._v[2]);
		auto r2_a = static_cast<HOMOG2D_INUMTYPE>(r2._v[0]);
		auto r2_b = static_cast<HOMOG2D_INUMTYPE>(r2._v[1]);
		auto r2_c = static_cast<HOMOG2D_INUMTYPE>(r2._v[2]);

/*		res._v[0] = r1._v[1] * r2._v[2] - r1._v[2] * r2._v[1];
		res._v[1] = r1._v[2] * r2._v[0] - r1._v[0] * r2._v[2];
		res._v[2] = r1._v[0] * r2._v[1] - r1._v[1] * r2._v[0];
*/
		Root<Out,FPT> res;
		res._v[0] = static_cast<FPT>( r1_b * r2_c  - r1_c * r2_b );
		res._v[1] = static_cast<FPT>( r1_c * r2_a  - r1_a * r2_c );
		res._v[2] = static_cast<FPT>( r1_a * r2_b  - r1_b * r2_a );

		return res;
	}
} // namespace detail

/// Free function template, product of two lines, returns a point
template<typename FPT>
Root<type::IsPoint,FPT>
operator * ( const Root<type::IsLine,FPT>& lhs, const Root<type::IsLine,FPT>& rhs )
{
#ifndef HOMOG2D_NOCHECKS
	if( lhs.isParallelTo(rhs) )
		throw std::runtime_error( "lines are parallel, unable to compute product" );
#endif

	return detail::crossProduct<type::IsPoint,type::IsLine,FPT>(lhs, rhs);
}

/// Free function template, product of two points, returns a line
template<typename FPT>
Root<type::IsLine,FPT>
operator * ( const Root<type::IsPoint,FPT>& lhs, const Root<type::IsPoint,FPT>& rhs )
{
#ifndef HOMOG2D_NOCHECKS
	if( lhs == rhs )
		throw std::runtime_error( "points are identical, unable to compute product" );
#endif
	Root<type::IsLine, FPT> line = detail::crossProduct<type::IsLine,type::IsPoint,FPT>(lhs, rhs);
	line.p_normalizeLine();
	return line;
}

//------------------------------------------------------------------
///////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////

/// Points overload: generic init from two numeric args
template<typename LP, typename FPT>
template<typename T>
void
Root<LP,FPT>::impl_init_2( const T& v1, const T& v2, const detail::RootHelper<type::IsPoint>& )
{
	_v[0] = v1;
	_v[1] = v2;
	_v[2] = 1.;
}

/// Lines overload: generic init from two numeric args
template<typename LP, typename FPT>
template<typename T>
void
Root<LP,FPT>::impl_init_2( const T& v1, const T& v2, const detail::RootHelper<type::IsLine>& )
{
	Root<type::IsPoint,FPT> pt1;                // 0,0
	Root<type::IsPoint,FPT> pt2(v1,v2);
	*this = detail::crossProduct<type::IsLine>( pt1, pt2 );
	p_normalizeLine();
}

/// Overload for point to point distance
template<typename LP, typename FPT>
HOMOG2D_INUMTYPE
Root<LP,FPT>::impl_distToPoint( const Root<type::IsPoint,FPT>& pt, const detail::RootHelper<type::IsPoint>& ) const
{
	return static_cast<double>(
		std::hypot(
			static_cast<HOMOG2D_INUMTYPE>( getX() ) - static_cast<HOMOG2D_INUMTYPE>( pt.getX() ),
			static_cast<HOMOG2D_INUMTYPE>( getY() ) - static_cast<HOMOG2D_INUMTYPE>( pt.getY() )
		)
	);
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
HOMOG2D_INUMTYPE
Root<LP,FPT>::impl_distToPoint( const Root<type::IsPoint,FPT>& pt, const detail::RootHelper<type::IsLine>& ) const
{
	return std::fabs( _v[0] * pt.getX() + _v[1] * pt.getY() + _v[2] ) / std::hypot( _v[0], _v[1] );
}

/// overload for line to point distance
template<typename LP, typename FPT>
HOMOG2D_INUMTYPE
Root<LP,FPT>::impl_distToLine( const Root<type::IsLine,FPT>& li, const detail::RootHelper<type::IsPoint>& ) const
{
	return li.distTo( *this );
}

/// overload for line to line distance. Aborts build if instanciated (distance between two lines makes no sense).
template<typename LP, typename FPT>
HOMOG2D_INUMTYPE
Root<LP,FPT>::impl_distToLine( const Root<type::IsLine,FPT>&, const detail::RootHelper<type::IsLine>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot compute distance between two lines" );
	return 0.;    // to avoid warning message on build
}


/// overload for lines, undefined for points
template<typename LP, typename FPT>
template<typename T>
void
Root<LP,FPT>::impl_addOffset( LineOffset dir, T v, const detail::RootHelper<type::IsLine>& )
{
	if( dir == LineOffset::vert )
		_v[2] = _v[2] - v*_v[1];
	else
		_v[2] = _v[2] - v*_v[0];
	p_normalizeLine();
}

//------------------------------------------------------------------
/// Free function, returns the angle (in Rad) between two lines.
template<typename FPT>
HOMOG2D_INUMTYPE
getAngle( const Root<type::IsLine,FPT>& li1, const Root<type::IsLine,FPT>& li2 )
{
	return li1.getAngle( li2 );
}

template<typename LP, typename FPT>
bool
Root<LP,FPT>::impl_isParallelTo( const Root<LP,FPT>& li, const detail::RootHelper<type::IsLine>& ) const
{
	if( getAngle(li) < Root::nullAngleValue() )
		return true;
	return false;
}
template<typename LP, typename FPT>
bool
Root<LP,FPT>::impl_isParallelTo( const Root<LP,FPT>&, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot use IsParallel() with a point" );
	return false;    // to avoid a warning message on build
}

//------------------------------------------------------------------
/// Returns the angle (in rad.) between the line and the other one.
/**
The returned value will be in the range <code> [0, M_PI/2] </code>

If the lines \f$ (a_0,a_1,a_2) \f$ and \f$ (b_0,b_1,b_2) \f$ are correctly normalized (should be, but...)
then the angle between them is \f$ \alpha = acos( a_0*b_0 + a_1*b_1) \f$. <br>
However, in "some situations", even if the lines have been previously normalized (which is the case here)
we can encounter numerical issues, so here we "reinforce the normalization and compute:
\f[
\alpha = acos \left(
	\frac{a_0*b_0 + a_1*b_1} { \sqrt{ a_0*a_0 + a_1*a_1 } * \sqrt{ b_0*b_0 + b_1*b_1 } }
\right)
\f]
In some situations, the value inside the parenthesis "may" be equal to \f$ 1+\epsilon \f$
(typically, something like "1.0000000000123").
This is out of bounds for the \f$ acos() \f$ function, that will then return "nan"
(thus induce some failure further on). <br>
To avoid this, a checking is done, and any value higher than 1 will be truncated.
This is logged on \c std::cerr so that the user may take that into consideration.

\todo more investigation needed ! : what are the exact situation that will lead to this event?
*/
template<typename LP, typename FPT>
HOMOG2D_INUMTYPE
Root<LP,FPT>::impl_getAngle( const Root<LP,FPT>& li, const detail::RootHelper<type::IsLine>& ) const
{
//	double res = _v[0] * li._v[0] + _v[1] * li._v[1];
	HOMOG2D_INUMTYPE l1_a = _v[0];
	HOMOG2D_INUMTYPE l1_b = _v[1];
	HOMOG2D_INUMTYPE l2_a = li._v[0];
	HOMOG2D_INUMTYPE l2_b = li._v[1];
	HOMOG2D_INUMTYPE res = l1_a * l2_a + l1_b * l2_b;

//	res /= std::sqrt( (_v[0]*_v[0] + _v[1]*_v[1] ) * ( li._v[0]*li._v[0] + li._v[1]*li._v[1] ) );
	res /= std::sqrt( (l1_a*l1_a + l1_b*l1_b) * (l2_a*l2_a + l2_b*l2_b) );
//	std::cerr << __FUNCTION__ << "(): res=" << std::setprecision(25) << res << " angle=" << std::acos( std::abs(res) ) << "\n";
//	std::cerr << __FUNCTION__ << "(): a1*b1=" << _v[0]*_v[0] + _v[1]*_v[1] << " a2*b2=" << li._v[0]*li._v[0] + li._v[1]*li._v[1] << "\n";
//	std::cerr << __FUNCTION__ << "(): res=" << std::setprecision(std::numeric_limits<double>::digits10 + 1) << res << " angle=" << std::acos( std::abs(res) ) << "\n";
	HOMOG2D_INUMTYPE fres = std::abs(res);
	if( fres > 1.0 )
	{
		std::cerr << "homog2d: angle computation overflow detected, value " << std::setprecision(20) << fres << ", truncated to 1.0\n";
		fres = 1.0;
	}
	return std::acos( fres );
}

template<typename LP, typename FPT>
HOMOG2D_INUMTYPE
Root<LP,FPT>::impl_getAngle( const Root<LP,FPT>&, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "cannot get angle of a point" );
	return 0.; // to avoid a warning
}


//------------------------------------------------------------------
/// Returns true if point is inside (or on the edge) of a flat rectangle defined by (p0,p1)
template<typename LP, typename FPT>
bool
Root<LP,FPT>::impl_isInsideRect( const Root<type::IsPoint,FPT>& p0, const Root<type::IsPoint,FPT>& p1, const detail::RootHelper<type::IsPoint>& ) const
{
	auto pair_pts = detail::getCorrectPoints( p0, p1 );
	const auto& p00 = pair_pts.first;
	const auto& p11 = pair_pts.second;
	return detail::ptIsInside( *this, p00, p11 );
}
template<typename LP, typename FPT>
bool
Root<LP,FPT>::impl_isInsideRect( const Root<type::IsPoint,FPT>&, const Root<type::IsPoint,FPT>&, const detail::RootHelper<type::IsLine>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "cannot use isInside(Rectangle) with a line" );
	return false; // to avoid a warning
}

template<typename LP, typename FPT>
template<typename T>
bool
Root<LP,FPT>::impl_isInsideCircle( const Root<type::IsPoint,FPT>& center, T radius, const detail::RootHelper<type::IsPoint>& ) const
{
	if( distTo( center ) <= radius )
		return true;
	return false;
}

template<typename LP, typename FPT>
template<typename T>
bool
Root<LP,FPT>::impl_isInsideCircle( const Root<type::IsPoint,FPT>&, T, const detail::RootHelper<type::IsLine>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "cannot use isInside(Circle) with a line" );
	return false; // to avoid a warning
}

//------------------------------------------------------------------
/// Intersection of line and circle
template<typename LP, typename FPT>
template<typename T>
typename Root<LP,FPT>::Intersect
Root<LP,FPT>::intersectsCircle( const Root<type::IsPoint,FPT>& pt, T radius ) const
{
	return impl_intersectsCircle( pt, radius, detail::RootHelper<type::IsLine>() );
}

//------------------------------------------------------------------
/// Intersection of line and circle: implementation for points
template<typename LP, typename FPT>
template<typename T>
typename Root<LP,FPT>::Intersect
Root<LP,FPT>::impl_intersectsCircle( const Root<type::IsPoint,FPT>&, T, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "cannot use intersectsCircle() with a point" );
}
//------------------------------------------------------------------
/// Intersection of line and circle: implementation
/// For computation details, checkout http://skramm.lautre.net/files/misc/intersect_circle_line.pdf
template<typename LP, typename FPT>
template<typename T>
typename Root<LP,FPT>::Intersect
Root<LP,FPT>::impl_intersectsCircle(
	const Root<type::IsPoint,FPT>& pt,       ///< circle origin
	T                              r,        ///< radius
	const detail::RootHelper<type::IsLine>&  ///< dummy arg, needed so that this overload is only called for lines
) const
{
	Intersect out;
	HOMOG2D_CHECK_IS_NUMBER(T);
	const HOMOG2D_INUMTYPE a = static_cast<HOMOG2D_INUMTYPE>(_v[0]); // just to lighten a bit...
	const HOMOG2D_INUMTYPE b = static_cast<HOMOG2D_INUMTYPE>(_v[1]);
	const HOMOG2D_INUMTYPE c = static_cast<HOMOG2D_INUMTYPE>(_v[2]);

// step 1: translate to origin
	auto cp = pt.getX() * a + pt.getY() * b + c;

// step 2: compute distance	between center (origin) and middle point
	auto a2b2 = a * a + b * b;
	auto d0 = std::abs(cp) / std::sqrt( a2b2 );
	if( r < d0 )                            // if less than radius,
		return out;                         // no intersection

	auto d2 = r*r - d0*d0;

// step 3: compute coordinates of middle point B
	auto xb = - a * cp / a2b2;
	auto yb = - b * cp / a2b2;

// step 4: compute coordinates of intersection points, with center at (0,0)
	auto m  = std::sqrt( d2 / a2b2 );
	auto x1 = xb + m*b;
	auto y1 = yb - m*a;

	auto x2 = xb - m*b;
	auto y2 = yb + m*a;

// last step: translate back
	out.ptA.set( x1 + pt.getX(), y1 + pt.getY() );
	out.ptB.set( x2 + pt.getX(), y2 + pt.getY() );
	out._doesIntersect = true;

	detail::fix_order( out.ptA, out.ptB );
	return out;
}
//------------------------------------------------------------------
/// Checks for intersection with flat rectangle defined by the two points \c p0 and \c p1
/**
Pre-conditions: points are different (throws if not)
*/
template<typename LP, typename FPT>
typename Root<LP,FPT>::Intersect
Root<LP,FPT>::intersectsRectangle( const Root<type::IsPoint,FPT>& p0, const Root<type::IsPoint,FPT>& p1 ) const
{
	return impl_intersectsRectangle( p0, p1, detail::RootHelper<LP>() );
}

/// Overload used when attempting to use that on a point
template<typename LP, typename FPT>
typename Root<LP,FPT>::Intersect
Root<LP,FPT>::impl_intersectsRectangle( const Root<type::IsPoint,FPT>& p0, const Root<type::IsPoint,FPT>& p1, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call intersectsRectangle() on a point" );
}

/// Checks for intersection with flat rectangle defined by the two points p00 and p11: implementation
template<typename LP, typename FPT>
typename Root<LP,FPT>::Intersect
Root<LP,FPT>::impl_intersectsRectangle( const Root<type::IsPoint,FPT>& p0, const Root<type::IsPoint,FPT>& p1, const detail::RootHelper<type::IsLine>& ) const
{
	auto pair_pts = detail::getCorrectPoints( p0, p1 );
	const auto& p00 = pair_pts.first;
	const auto& p11 = pair_pts.second;

	Root<type::IsPoint,FPT> p01( p11.getX(), p00.getY() );
	Root<type::IsPoint,FPT> p10( p00.getX(), p11.getY() );

	Root<type::IsLine,FPT> l[4];
	l[0] = Root<type::IsLine,FPT>( p00, p01 );
	l[1] = Root<type::IsLine,FPT>(      p01, p11 );
	l[2] = Root<type::IsLine,FPT>(           p11, p10 );
	l[3] = Root<type::IsLine,FPT>(                p10, p00 );

	std::vector<Root<type::IsPoint,FPT>> vec;
	for( int i=0; i<4; i++ )         // compare with each of the 4 lines
	{
		if( *this == l[i] )          // if same line, then we are done: return the two points
			return typename Root<type::IsLine,FPT>::Intersect( p00, p11 );
		else
		{
			Root<type::IsPoint,FPT> pt;
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

	typename Root<LP,FPT>::Intersect out;
	if( vec.size() > 1 )                                // if more than one point was found, then
	{
		std::vector<Root<type::IsPoint,FPT>> vec2( 1, vec[0] );   // build a second vector, holding the first found point as first element
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
		detail::fix_order( out.ptA, out.ptB );
	}
	return out;
}

//------------------------------------------------------------------
namespace detail {

/// Implementation of product 3x3 by 3x1
/**
- T1 and T2: IsLine or IsPoint

\todo Add checking that T1 and T2 are opposite class
*/
template<typename T1,typename T2,typename U,typename FPT1,typename FPT2>
void
product(
	Root<T1,FPT1>&          out,
	const Hmatrix_<U,FPT2>& h,
	const Root<T2,FPT1>&    in
)
{
	for( int i=0; i<3; i++ )
	{
		auto sum  = static_cast<HOMOG2D_INUMTYPE>(h._data[i][0]) * in._v[0];
		sum      += h._data[i][1] * in._v[1];
		sum      += h._data[i][2] * in._v[2];
		out._v[i] = sum;
	}
}

} // namespace detail

/// Apply Epipolar matrix to a point or line, this will return the opposite type.
/// Free function, templated by point or line
template<typename T,typename U,typename V>
Root<typename detail::HelperPL<T>::OtherType,V>
operator * ( const Hmatrix_<type::IsEpipmat,U>& h, const Root<T,V>& in )
{
	Root<typename detail::HelperPL<T>::OtherType,V> out;
	detail::product( out, h, in );
	return out;
}

/// Apply homography to a point or line. Free function, templated by point or line
template<typename T,typename U,typename V>
Root<T,V>
operator * ( const Hmatrix_<type::IsHomogr,U>& h, const Root<T,V>& in )
{
	Root<T,V> out;
	detail::product( out, h, in );
	return out;
}

//------------------------------------------------------------------
namespace priv {

/// Traits class used in operator * ( const Hmatrix_<type::IsHomogr,FPT>& h, const Cont& vin ),
/// used to detect if container is valid
template <typename T>               struct Is_Container: std::false_type { };
template <typename T,std::size_t N> struct Is_Container<std::array<T,N>>:     std::true_type { };
template <typename... Ts>           struct Is_Container<std::vector<Ts...>> : std::true_type { };
template <typename... Ts>           struct Is_Container<std::list<Ts...  >> : std::true_type { };

/// Traits class used to detect if container \c T is a \c std::array
/** (because allocation is different, see \ref alloc() ) */
template <typename T>
struct Is_std_array: std::false_type {};
template <typename V, size_t n>
struct Is_std_array<std::array<V, n>>: std::true_type {};

/// Allocation for \c std::array container
template<
	typename Cont,
	typename std::enable_if<
		Is_std_array<Cont>::value,
		Cont
	>::type* = nullptr
>
Cont
alloc( std::size_t )
{
	return Cont();
}

/// Allocation for \c std::vector and \c std::list container
template<
	typename Cont,
	typename std::enable_if<
		!Is_std_array<Cont>::value,
		Cont
	>::type* = nullptr
>
Cont
alloc( std::size_t nb )
{
	return Cont(nb);
}

} // namespace priv
//------------------------------------------------------------------
/// Used to proceed multiple products, whatever the container (\c std::list, \c std::vector, or \c std::array).
/// Returned container is of same type as given input
template<typename FPT,typename Cont>
typename std::enable_if<priv::Is_Container<Cont>::value,Cont>::type
operator * (
	const Hmatrix_<type::IsHomogr,FPT>& h,    ///< Matrix
	const Cont&                         vin   ///< Input container
)
{
	Cont vout = priv::alloc<Cont>( vin.size() );
	auto it = std::begin( vout );
	for( const auto& elem: vin )
	{
		detail::product( *it, h, elem );
		it++;
	}
	return vout;
}

/// Apply homography to a segment
template<typename FPT1,typename FPT2>
Segment_<FPT1>
operator * ( const Hmatrix_<type::IsHomogr,FPT2>& h, const Segment_<FPT1>& seg )
{
	const auto& pts = seg.get();
	Root<type::IsPoint,FPT1> pt1 = h * pts.first;
	Root<type::IsPoint,FPT1> pt2 = h * pts.second;
	return Segment_<FPT1>( pt1, pt2 );

}
//------------------------------------------------------------------
/// Apply homography to a vector/array/list (type T) of points or lines.
template<typename W,typename FPT>
template<typename T>
void
Hmatrix_<W,FPT>::applyTo( T& vin ) const
{
	for( auto& elem: vin )
		elem = *this * elem;
}

//------------------------------------------------------------------
/// Free function, returns  the set of four points defining a flat rectangle
template<typename FPT>
std::vector<Root<type::IsPoint,FPT>>
getRectPts( Root<type::IsPoint,FPT> ptA, Root<type::IsPoint,FPT>& ptB )
{
	auto pair_pts = detail::getCorrectPoints( ptA, ptB );
	const auto& pt1 = pair_pts.first;
	const auto& pt2 = pair_pts.second;
	std::vector<Root<type::IsPoint,FPT>> v(4);
	v[0] = pt1;
	v[1] = Root<type::IsPoint,FPT>( pt2.getX(), pt1.getY() );
	v[2] = pt2;
	v[3] = Root<type::IsPoint,FPT>( pt1.getX(), pt2.getY() );
	return v;
}
//------------------------------------------------------------------
#ifdef HOMOG2D_USE_OPENCV
/// Return Opencv 2D point
template<typename LP, typename FPT>
template<typename OPENCVT>
OPENCVT
Root<LP,FPT>::impl_getCvPt( const detail::RootHelper<type::IsPoint>&, const OPENCVT& ) const
{
	return OPENCVT( getX(),getY() );
}


//------------------------------------------------------------------
/// Copy matrix to Opencv \c cv::Mat
/**
The output matrix is passed by reference to avoid issues with Opencv copy operator, and is allocated here.

User can pass a type as second argument: CV_32F for \c float, CV_64F for \c double (default)
*/
template<typename W,typename FPT>
void
Hmatrix_<W,FPT>::copyTo( cv::Mat& mat, int type ) const
{
#ifndef HOMOG2D_NOCHECKS
	if( type != CV_64F && type != CV_32F )
		throw std::runtime_error( "invalid OpenCv matrix type" );
#endif
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
template<typename W,typename FPT>
Hmatrix_<W,FPT>&
Hmatrix_<W,FPT>::operator = ( const cv::Mat& mat )
{
#ifndef HOMOG2D_NOCHECKS
	if( mat.rows != 3 || mat.cols != 3 )
		throw std::runtime_error( "invalid matrix size, rows=" + std::to_string(mat.rows) + " cols=" + std::to_string(mat.cols) );
	if( mat.channels() != 1 )
		throw std::runtime_error( "invalid matrix nb channels: " + std::to_string(mat.channels() ) );
#endif
	auto type = mat.type();
#ifndef HOMOG2D_NOCHECKS
	if( type != CV_64F && type != CV_32F )
		throw std::runtime_error( "invalid matrix type" );
#endif
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
	return *this;
}

//------------------------------------------------------------------
namespace detail {
/// Private helper function, used by Root<IsPoint>::draw()
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
		cv::line( mat, vpt[0], vpt[1], dp._dpValues._color, dp._dpValues._enhancePoint?2:1 );
		cv::line( mat, vpt[2], vpt[3], dp._dpValues._color, dp._dpValues._enhancePoint?2:1 );
	}
	else // draw 4 diagonal lines
	{
		cv::line( mat, vpt[0], vpt[2], dp._dpValues._color, dp._dpValues._enhancePoint?2:1 );
		cv::line( mat, vpt[2], vpt[1], dp._dpValues._color, dp._dpValues._enhancePoint?2:1 );
		cv::line( mat, vpt[1], vpt[3], dp._dpValues._color, dp._dpValues._enhancePoint?2:1 );
		cv::line( mat, vpt[0], vpt[3], dp._dpValues._color, dp._dpValues._enhancePoint?2:1 );
	}
}
} // namespace detail
//------------------------------------------------------------------
/// Draw Point on Cv::Mat. Overload for points.
/// Returns false if point not in image
template<typename LP, typename FPT>
bool
Root<LP,FPT>::impl_draw( cv::Mat& mat, const CvDrawParams& dp, const detail::RootHelper<type::IsPoint>& /* dummy */ )  const
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

		case PS_TIMES:      // "times" symbol
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
Root<LP,FPT>::impl_draw( cv::Mat& mat, const CvDrawParams& dp, const detail::RootHelper<type::IsLine>& /* dummy */ ) const
{
	assert( mat.rows > 2 );
	assert( mat.cols > 2 );

	Root<type::IsPoint,FPT> pt1; // 0,0
	Root<type::IsPoint,FPT> pt2( mat.cols-1, mat.rows-1 );
    Intersect ri = this->intersectsRectangle( pt1,  pt2 );
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
/// Free function, draws a set of points or lines
/**
Template type can be std::array<type> or std::vector<type>, with \c type being Point2d or \c Line2d
*/
template<typename T>
void draw( cv::Mat& mat, const T& cont, const CvDrawParams& dp=CvDrawParams() )
{
	for( const auto& elem: cont )
		elem.draw( mat, dp );
}

//------------------------------------------------------------------
#endif // HOMOG2D_USE_OPENCV

/// Default line type, uses \c double as numerical type
using Line2d = Root<type::IsLine,double>;

/// Default point type, uses \c double as numerical type
using Point2d = Root<type::IsPoint,double>;

/// Default homography (3x3 matrix) type, uses \c double as numerical type
using Homogr = Hmatrix_<type::IsHomogr,double>;

/// Default homogeneous matrix, uses \c double as numerical type
using Epipmat = Hmatrix_<type::IsEpipmat,double>;

/// Default segment type
using Segment = Segment_<double>;

/// Default circle type
using Circle = Circle_<double>;

// float types
using Line2dF  = Root<type::IsLine,float>;
using Point2dF = Root<type::IsPoint,float>;
using HomogrF  = Hmatrix_<type::IsHomogr,float>;
using SegmentF = Segment_<float>;
using HomogrF  = Hmatrix_<type::IsHomogr,float>;

// double types
using Line2dD  = Root<type::IsLine,double>;
using Point2dD = Root<type::IsPoint,double>;
using HomogrD  = Hmatrix_<type::IsHomogr,double>;
using SegmentD = Segment_<double>;
using HomogrD  = Hmatrix_<type::IsHomogr,double>;

// long double types
using Line2dL  = Root<type::IsLine,long double>;
using Point2dL = Root<type::IsPoint,long double>;
using HomogrL  = Hmatrix_<type::IsHomogr,long double>;
using SegmentL = Segment_<long double>;
using HomogrL  = Hmatrix_<type::IsHomogr,long double>;

template<typename T>
using Point2d_ = Root<type::IsPoint,T>;
template<typename T>
using Line2d_  = Root<type::IsLine,T>;
template<typename T>
using Homogr_  =  Hmatrix_<type::IsHomogr,T>;
template<typename T>
using Epipmat_ =  Hmatrix_<type::IsEpipmat,T>;

template<typename T>
using Intersect_ = typename Root<type::IsLine,T>::Intersect;

using Intersect = Intersect_<double>;

} // namespace homog2d end


#endif // HG_HOMOG2D_HPP

