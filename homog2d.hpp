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
#include <numeric>
#include <set>
#include <list>
#include <vector>
#include <stack>
#include <iomanip>
#include <cassert>
#include <sstream>
#include <type_traits>
#include <functional>

#ifdef HOMOG2D_USE_EIGEN
	#include <Eigen/Dense>
#endif

#define HOMOG2D_VERSION 2.62

#ifdef HOMOG2D_USE_OPENCV
	#include "opencv2/imgproc.hpp"
	#include "opencv2/highgui.hpp"
#endif

#if 0
	#define HOMOG2D_START std::cout << "START: " << __PRETTY_FUNCTION__ << "()\n"
#else
	#define HOMOG2D_START
#endif

#ifdef HOMOG2D_DEBUGMODE
	#define HOMOG2D_LOG(a) \
		std::cout << '-' << __FUNCTION__ << "(), line " << __LINE__ << ": " \
		<< std::scientific << std::setprecision(10) \
		<< a << std::endl;
#else
	#define HOMOG2D_LOG(a) {;}
#endif

#define HOMOG2D_ASSERT( a ) \
	if( !(a) ) \
	{ \
		std::cerr << "ASSERT FAILURE, line=" << __LINE__ << std::endl; \
		std::exit(1); \
	}

/// Assert debug macro, used internally if \c HOMOG2D_DEBUGMODE is defined
#define HOMOG2D_DEBUG_ASSERT(a,b) \
	{ \
		if( (a) == false ) \
		{ \
			std::cerr << "Homog2d assert failure, version:" << HOMOG2D_VERSION \
				<< ", line:" << __LINE__ << "\n -details: " << b << '\n'; \
			std::cout << "homog2d: internal failure, please check stderr and report this on https://github.com/skramm/homog2d/issues\n"; \
			std::exit(1); \
		} \
	}

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

#if !defined(HOMOG2D_BIND_X)
	#define HOMOG2D_BIND_X x
#endif
#if !defined(HOMOG2D_BIND_Y)
	#define HOMOG2D_BIND_Y y
#endif

/// Error throw wrapper macro
#define HOMOG2D_THROW_ERROR_1( msg ) \
	{ \
		std::ostringstream oss; \
		oss << "homog2d: line " <<  __LINE__  << ", " << + __FUNCTION__ << "(): " << msg; \
		throw std::runtime_error( oss.str() ); \
	}

/// Error throw wrapper macro, first arg in the function name
#define HOMOG2D_THROW_ERROR_2( func, msg ) \
	throw std::runtime_error( std::string("homog2d: line ") + std::to_string( __LINE__ ) + ", " + func + "(): " + msg )

///////////////////////////////////////
// Default values for thresholds
#ifndef HOMOG2D_THR_ZERO_DIST
	#define HOMOG2D_THR_ZERO_DIST 1E-10
#endif

#ifndef HOMOG2D_THR_ZERO_ORTHO_DIST
	#define HOMOG2D_THR_ZERO_ORTHO_DIST 1E-14
#endif

// default value: 1 thousand of a radian (tan = 0.001 too)
#ifndef HOMOG2D_THR_ZERO_ANGLE
	#define HOMOG2D_THR_ZERO_ANGLE 0.001
#endif

#ifndef HOMOG2D_THR_ZERO_DENOM
	#define HOMOG2D_THR_ZERO_DENOM 1E-10
#endif

#ifndef HOMOG2D_THR_ZERO_DETER
	#define HOMOG2D_THR_ZERO_DETER 1E-15
#endif

//#ifndef HOMOG2D_ROUNDING_COEFF
//	#define HOMOG2D_ROUNDING_COEFF 1E6
//#endif

///////////////////////////////////////


namespace h2d {

/// Holds the types needed for policy based design
namespace type {

struct IsLine   {};
struct IsPoint  {};
struct IsHomogr {};
struct IsEpipmat {};
struct IsClosed {};
struct IsOpen {};
} // namespace type


namespace detail {

	template<typename FPT> class Matrix_;

	/// Helper class for Root (Point/Line) type, used as a trick to allow partial specialization of member functions
	template<typename> struct RootHelper {};

	/// Helper class for LPBase (Point/Line) type, used only to get the underlying floating-point type, see Dtype and Root::dtype()
	template<typename> struct RootDataType {};

	/// Helper class for PolylineBase, used as a trick to allow partial specialization of member functions
	template<typename> struct PlHelper {};

#ifdef HOMOG2D_FUTURE_STUFF
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

    template<typename> struct HelperPL;

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

/// Holds base classes, not part of API
namespace base {
	template<typename PLT,typename FPT> class PolylineBase;
	template<typename LP, typename FPT> class LPBase;
}

template<typename LP,typename FPT> class Hmatrix_;

template<typename T>
using Homogr_  =  Hmatrix_<type::IsHomogr,T>;
#ifdef HOMOG2D_FUTURE_STUFF
template<typename T>
using Epipmat_ =  Hmatrix_<type::IsEpipmat,T>;
#endif

template<typename FPT> class Segment_;
template<typename FPT> class Circle_;
template<typename FPT> class FRect_;
template<typename FPT> class Ellipse_;


template<typename T>
using Point2d_ = base::LPBase<type::IsPoint,T>;
template<typename T>
using Line2d_  = base::LPBase<type::IsLine,T>;

template<typename T>
using CPolyline_  = base::PolylineBase<type::IsClosed,T>;
template<typename T>
using OPolyline_  = base::PolylineBase<type::IsOpen,T>;

//------------------------------------------------------------------
/// Holds drawing related code, independent of back-end library
namespace img {

/// Color type , see DrawParams
struct Color
{
	uint8_t r = 80;
	uint8_t g = 80;
	uint8_t b = 80;
	Color( uint8_t rr, uint8_t gg, uint8_t bb ): r(rr),g(gg),b(bb) {}
	Color() = default;

	friend std::ostream& operator << ( std::ostream& f, const Color& c )
	{
		f << "Color:" << (int)c.r << '-' << (int)c.g << '-' << (int)c.b;
		return f;
	}
};

/// Opaque data structure, will hold the image type, depending on back-end library.
/// This type is the one used in all the drawing functions.
/**
To expand it, you need to add code for the other library for the
two function cols() and rows(), bounded in a "#define" block, and define the drawing functions
*/
template<typename T>
struct Image
{
	T real_img;
	Image() = default;
	Image( T& m ): real_img(m)
	{}
/// Returns a reference on the underlying image
	T& getReal()
	{
		return real_img;
	}
#ifdef HOMOG2D_USE_OPENCV
	Image( size_t width, size_t height )
	{
		real_img.create( height, width, CV_8UC3 );
		clear();
	}
	int cols() const { return real_img.cols; }
	int rows() const { return real_img.rows; }
	void clear( uint8_t r, uint8_t g=255, uint8_t b=255 ) { real_img = cv::Scalar(b,g,r); }
	void clear( Color c=Color(255,255,255) )                  { clear(c.r,c.g,c.b); }
/// Write image to disk with name \c fname
	void write( std::string fname ) const
	{
		cv::imwrite( fname, real_img );
	}
/// Show image on window \c wname
	void show( std::string wname )
	{
		cv::imshow( wname, real_img );
	}
#endif

//#ifdef HOMOG2D_SOME_OTHER_LIB
//	int cols() const { return real_img.???; }
//	int rows() const { return real_img.???; }
//#endif

};

} // namespace img

/////////////////////////////////////////////////////////////////////////////
// SECTION  - PUBLIC ENUM DECLARATIONS
/////////////////////////////////////////////////////////////////////////////

/// Used in Line2d::getValue() and getOrthogonalLine()
enum class GivenCoord: uint8_t { X, Y };

/// Used in line constructor, to instanciate a H or V line, see \ref LPBase( LineDir, T )
enum class LineDir: uint8_t { H, V };

// Used in Polyline_ constructors
//enum class IsClosed: uint8_t { Yes, No };

/// Type of Root object, see Root::type()
enum class Type: uint8_t { Line2d, Point2d };

/// Type of underlying floating point, see LPBase::dtype()
enum class Dtype: uint8_t { Float, Double, LongDouble };

const char* getString( Type t )
{
	return t==Type::Line2d ? "Line2d" : "Point2d";
}


//------------------------------------------------------------------
/// Holds private stuff
namespace priv {

	Dtype impl_dtype( const detail::RootDataType<float>& )
	{
		return Dtype::Float;
	}
	Dtype impl_dtype( const detail::RootDataType<double>& )
	{
		return Dtype::Double;
	}
	Dtype impl_dtype( const detail::RootDataType<long double>& )
	{
		return Dtype::LongDouble;
	}
} // namespace priv


//------------------------------------------------------------------
/// Holds threshold values and api to access these
namespace thr {

static HOMOG2D_INUMTYPE& nullDistance()
{
	static HOMOG2D_INUMTYPE s_zeroDistance = HOMOG2D_THR_ZERO_DIST;
	return s_zeroDistance;
}
static HOMOG2D_INUMTYPE& nullOrthogDistance()
{
	static HOMOG2D_INUMTYPE s_zeroOrthoDistance = HOMOG2D_THR_ZERO_ORTHO_DIST;
	return s_zeroOrthoDistance;
}
static HOMOG2D_INUMTYPE& nullAngleValue()
{
	static HOMOG2D_INUMTYPE s_zeroAngleValue = HOMOG2D_THR_ZERO_ANGLE;
	return s_zeroAngleValue;
}

static HOMOG2D_INUMTYPE& nullDenom()
{
	static HOMOG2D_INUMTYPE _zeroDenom = HOMOG2D_THR_ZERO_DENOM;
	return _zeroDenom;
}

static HOMOG2D_INUMTYPE& nullDeter()
{
	static HOMOG2D_INUMTYPE _zeroDeter = HOMOG2D_THR_ZERO_DETER;
	return _zeroDeter;
}

/// Helper function, could be needed
void printThresholds( std::ostream& f )
{
	f << "Current threshold values:"
		<< "\n  -nullDistance()="       << nullDistance()
		<< "\n  -nullOrthogDistance()=" << nullOrthogDistance()
		<< "\n  -nullAngleValue()="     << nullAngleValue()
		<< "\n  -nullDenom()="          << nullDenom()
		<< "\n  -nullDeter()="          << nullDeter()
		<< '\n';
}

} // namespace thr

// forward declaration
namespace base {
template<typename LP,typename FPT>
auto
operator << ( std::ostream&, const h2d::base::LPBase<LP,FPT>& )
-> std::ostream&;
}
/*
template<typename LP,typename FPT>
auto
operator << ( std::ostream&, const h2d::Point2d_<FPT>& )
-> std::ostream&;
*/

namespace detail {

// forward declaration
template<typename FPT1,typename FPT2,typename FPT3>
void
product( Matrix_<FPT1>&, const Matrix_<FPT2>&, const Matrix_<FPT3>& );


//------------------------------------------------------------------
/// Private free function, get top-left and bottom-right points from two arbitrary points
template<typename FPT>
std::pair<Point2d_<FPT>,Point2d_<FPT>>
getCorrectPoints( const Point2d_<FPT>& p0, const Point2d_<FPT>& p1 )
{
#ifndef HOMOG2D_NOCHECKS
	if(
		   std::fabs( p0.getX() - p1.getX() ) < thr::nullOrthogDistance()
		|| std::fabs( p0.getY() - p1.getY() ) < thr::nullOrthogDistance()
	)
		HOMOG2D_THROW_ERROR_1(
			"a coordinate of the 2 points is identical, does not define a rectangle:\n p0=" << p0 << " p1=" << p1
		);
#endif
	Point2d_<FPT> p00( std::min(p0.getX(), p1.getX()), std::min(p0.getY(), p1.getY()) );
	Point2d_<FPT> p11( std::max(p0.getX(), p1.getX()), std::max(p0.getY(), p1.getY()) );
	return std::make_pair( p00, p11 );
}

template<typename T>
using matrix_t = std::array<std::array<T,3>,3>;


//------------------------------------------------------------------
/// Common class for all the geometric primitives
template<typename FPT>
class Common
{
public:
	Dtype dtype() const
	{
		return priv::impl_dtype( detail::RootDataType<FPT>() );
	}

};

//------------------------------------------------------------------
/// A simple wrapper over a 3x3 matrix, provides root functionalities
/**
Homogeneous (thus the 'mutable' attribute).
*/
template<typename FPT>
class Matrix_: public Common<FPT>
{
	template<typename T> friend class Matrix_;

	template<typename T1,typename T2,typename FPT1,typename FPT2>
	friend void
	product( base::LPBase<T1,FPT1>&, const detail::Matrix_<FPT2>&, const base::LPBase<T2,FPT1>& );

	template<typename FPT1,typename FPT2,typename FPT3>
	friend void
	product( Matrix_<FPT1>&, const Matrix_<FPT2>&, const Matrix_<FPT3>& );

//private:
//	static HOMOG2D_INUMTYPE _zeroDeterminantValue; /// Used in matrix inversion
//	static HOMOG2D_INUMTYPE _zeroDenomValue;       /// The value under which e wont divide

protected:
	mutable matrix_t<FPT> _mdata;
	mutable bool          _isNormalized = false;

public:
/// Constructor
	Matrix_()
	{
		p_fillZero();
	}

/// Copy-Constructor
	template<typename FPT2>
	Matrix_( const Matrix_<FPT2>& other )
	{
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				_mdata[i][j] = other._mdata[i][j];
		_isNormalized = other._isNormalized;
	}

	matrix_t<FPT>&       getRaw()       { return _mdata; }
	const matrix_t<FPT>& getRaw() const { return _mdata; }

/*	template<typename T>
	void set( size_t r, size_t c, T v )
	{
		#ifndef HOMOG2D_NOCHECKS
			HOMOG2D_CHECK_ROW_COL;
		#endif
		_mdata[r][c] = v;
	}*/

	const FPT& value( size_t r, size_t c ) const
	{
		#ifndef HOMOG2D_NOCHECKS
			HOMOG2D_CHECK_ROW_COL;
		#endif
		return _mdata[r][c];
	}
	FPT& value( size_t r, size_t c )
	{
		#ifndef HOMOG2D_NOCHECKS
			HOMOG2D_CHECK_ROW_COL;
		#endif
		return _mdata[r][c];
	}

/// Return determinant of matrix
/**
See https://en.wikipedia.org/wiki/Determinant
*/
	HOMOG2D_INUMTYPE determ() const
	{
		auto det = _mdata[0][0] * p_det2x2( {1,1, 1,2, 2,1, 2,2} );
		det     -= _mdata[0][1] * p_det2x2( {1,0, 1,2, 2,0, 2,2} );
		det     += _mdata[0][2] * p_det2x2( {1,0, 1,1, 2,0, 2,1} );
		return det;
	}

/// Transpose and return matrix
	Matrix_& transpose()
	{
		matrix_t<FPT> out;
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				out[i][j] = _mdata[j][i];
		_mdata = out;
		_isNormalized = false;
		return *this;
	}

/// Inverse matrix
	Matrix_& inverse()
	{
		auto det = determ();
		if( std::abs(det) < thr::nullDeter() )
			HOMOG2D_THROW_ERROR_1( "matrix is not invertible, det=" << std::scientific << std::abs(det) );

		auto adjugate = p_adjugate();
		p_divideAll(adjugate, det);
		_mdata = adjugate._mdata;
		_isNormalized = false;
		return *this;
	}
	bool isNormalized() const { return _isNormalized; }

protected:
	void p_normalize( int r, int c ) const
	{
#ifndef HOMOG2D_NOCHECKS
		if( std::fabs(_mdata[r][c]) < thr::nullDenom() )
			HOMOG2D_THROW_ERROR_1(
				"Unable to normalize matrix, value at ("
					<< r << ',' << c << ") less than " << thr::nullDenom()
			);
#endif
		p_divideBy( r, c );
		if( std::signbit( _mdata[r][c] ) )
			for( auto& li: _mdata )
				for( auto& e: li )
					e = -e;
		_isNormalized = true;
	}

/// Divide all elements by the value at (r,c), used for normalization.
/** No need to check value, done by caller */
	void p_divideBy( size_t r, size_t c ) const
	{
//		assert( std::fabs( _mdata[r][c] ) > 1000*std::numeric_limits<FPT>::epsilon() );
		for( auto& li: _mdata )
			for( auto& e: li )
				e /= _mdata[r][c];
	}

	void p_fillZero()
	{
		for( auto& li: _mdata )
			for( auto& e: li )
				e = 0.;
	}
	void p_fillEye()
	{
		p_fillZero();
		_mdata[0][0] = 1.;
		_mdata[1][1] = 1.;  // "eye" matrix => unit transformation
		_mdata[2][2] = 1.;
	}

	template<typename T>
	void p_fillWith( const T& in )
	{
		for( auto i=0; i<3; i++ )
			for( auto j=0; j<3; j++ )
				_mdata[i][j] = in[i][j];
		_isNormalized = false;
	}

/// Divide all elements of \c mat by \c value
	template<typename FPT2>
	void p_divideAll( detail::Matrix_<FPT>& mat, FPT2 value ) const
	{
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				mat._mdata[i][j] /= value;
		_isNormalized = false;
	}
	template<typename T1,typename T2>
	friend Matrix_<T1> operator * ( const Matrix_<T1>&, const Matrix_<T2>& );

private:
	HOMOG2D_INUMTYPE p_det2x2( const std::vector<int>& v ) const
	{
		auto det = static_cast<HOMOG2D_INUMTYPE>( _mdata[v[0]][v[1]] ) * _mdata[v[6]][v[7]];
		det -=     static_cast<HOMOG2D_INUMTYPE>( _mdata[v[2]][v[3]] ) * _mdata[v[4]][v[5]];
		return det;
	}
/// Computes adjugate matrix, see https://en.wikipedia.org/wiki/Adjugate_matrix#3_%C3%97_3_generic_matrix
	detail::Matrix_<FPT> p_adjugate() const
	{
		detail::Matrix_<FPT> mat_out;
		matrix_t<FPT>& out = mat_out._mdata;

		out[ 0 ][ 0 ] =  p_det2x2( {1,1, 1,2, 2,1, 2,2} );
		out[ 0 ][ 1 ] = -p_det2x2( {0,1, 0,2, 2,1, 2,2} );
		out[ 0 ][ 2 ] =  p_det2x2( {0,1, 0,2, 1,1, 1,2} );

		out[ 1 ][ 0 ] = -p_det2x2( {1,0, 1,2, 2,0, 2,2} );
		out[ 1 ][ 1 ] =  p_det2x2( {0,0, 0,2, 2,0, 2,2} );
		out[ 1 ][ 2 ] = -p_det2x2( {0,0, 0,2, 1,0, 1,2} );

		out[ 2 ][ 0 ] =  p_det2x2( {1,0, 1,1, 2,0, 2,1} );
		out[ 2 ][ 1 ] = -p_det2x2( {0,0, 0,1, 2,0, 2,1} );
		out[ 2 ][ 2 ] =  p_det2x2( {0,0, 0,1, 1,0, 1,1} );

		return mat_out;
	}

	friend std::ostream&
	operator << ( std::ostream& f, const Matrix_& h )
	{
		for( const auto& li: h._mdata )
		{
			f << "| ";
			for( const auto& e: li )
				f << std::setw(6) << e << ' ';
			f << " |\n";
		}
		return f;
	}

}; // class Matrix_

template<typename T1,typename T2,typename FPT1,typename FPT2>
void
product( base::LPBase<T1,FPT1>&, const detail::Matrix_<FPT2>&, const base::LPBase<T2,FPT1>& );

template<typename T1,typename T2>
Matrix_<T1> operator * ( const Matrix_<T1>& h1, const Matrix_<T2>& h2 )
{
	HOMOG2D_START;
	Matrix_<T1> out;
	product( out, h1, h2 );
	return out;
}

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
class Hmatrix_ : public detail::Matrix_<FPT>
{
	template<typename T1,typename T2> friend class LPBase;

	template<typename T,typename U>
	friend Line2d_<T>
	operator * ( const Homogr_<U>&, const Line2d_<T>& );

	template<typename T,typename U>
	friend Point2d_<T>
	operator * ( const Homogr_<U>&, const Point2d_<T>& );

	template<typename T,typename U,typename V>
	friend base::LPBase<typename detail::HelperPL<T>::OtherType,V>
	operator * ( const Hmatrix_<type::IsEpipmat,U>& h, const base::LPBase<T,V>& in );

public:

/// \name Constructors
///@{

	/// Default constructor, initialize to unit transformation
	Hmatrix_()
	{
		init();
	}

/// Constructor, set homography to a rotation matrix of angle \c val
	template<typename T>
	explicit Hmatrix_( T val )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		init();
		setRotation( val );
	}

/// Constructor, set homography to a translation matrix ( see Hmatrix_( T ) )
	template<typename T1,typename T2>
	explicit Hmatrix_( T1 tx, T2 ty )
	{
		HOMOG2D_CHECK_IS_NUMBER(T1);
		HOMOG2D_CHECK_IS_NUMBER(T2);
		init();
		setTranslation( tx, ty );
	}

/// Constructor, used to fill with a vector of vector matrix
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
			HOMOG2D_THROW_ERROR_1( "Invalid line size for input: " << in.size() );
		for( auto li: in )
			if( li.size() != 3 )
				HOMOG2D_THROW_ERROR_1( "Invalid column size for input: " << li.size() );
#endif
		detail::Matrix_<FPT>::p_fillWith( in );
		normalize();
	}

/// Constructor, used to fill with a std::array
	template<typename T>
	Hmatrix_( const std::array<std::array<T,3>,3>& in )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		detail::Matrix_<FPT>::p_fillWith( in );
		normalize();
	}

/// Copy-constructor
	Hmatrix_( const Hmatrix_<M,FPT>& other )
		: detail::Matrix_<FPT>( other)
		, _hasChanged   ( true )
		, _hmt          (  nullptr )
	{
		detail::Matrix_<FPT>::getRaw() = other.getRaw();
	}

#ifdef HOMOG2D_USE_OPENCV
/// Constructor used to initialise with a cv::Mat, call the assignment operator
	Hmatrix_( const cv::Mat& mat )
	{
		*this = mat;
	}
#endif
///@}

/// Assignment operator
	Hmatrix_& operator = ( const Hmatrix_<M,FPT>& other )
	{
		if( this != &other )
			detail::Matrix_<FPT>::getRaw() = other.getRaw();
		_hasChanged = true;
		return *this;
	}

/// Inverse matrix
	Hmatrix_& inverse()
	{
		detail::Matrix_<FPT>::inverse();
		normalize();
		return *this;
	}

#if 0
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
		_hasChanged = true;
	}

/// Getter
	FPT get( size_t r, size_t c ) const
	{
		#ifndef HOMOG2D_NOCHECKS
			HOMOG2D_CHECK_ROW_COL;
		#endif
		return _data[r][c];
	}

	detail::Matrix_<FPT>&       getMat()       { return static_cast<detail::Matrix_<FPT>>(*this); }
	const detail::Matrix_<FPT>& getMat() const { return static_cast<detail::Matrix_<FPT>>(*this); }
#endif // 0

	void init()
	{
		impl_mat_init0( detail::RootHelper<M>() );
	}

/// \name Adding/assigning a transformation
///@{

/// Adds a translation \c tx,ty to the matrix
	template<typename T>
	Hmatrix_& addTranslation( T tx, T ty )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		Hmatrix_ out;
		out.setTranslation( tx, ty );
		*this = out * *this;
		return *this;
	}
/// Sets the matrix as a translation \c tx,ty
	template<typename T1,typename T2>
	Hmatrix_& setTranslation( T1 tx, T2 ty )
	{
		HOMOG2D_CHECK_IS_NUMBER(T1);
		HOMOG2D_CHECK_IS_NUMBER(T2);
		init();
		auto& mat = detail::Matrix_<FPT>::_mdata;
		mat[0][2] = tx;
		mat[1][2] = ty;
		detail::Matrix_<FPT>::_isNormalized = true;
		_hasChanged = true;
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
		return *this;
	}
/// Sets the matrix as a rotation with an angle \c theta (radians)
	template<typename T>
	Hmatrix_& setRotation( T theta )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		auto& mat = detail::Matrix_<FPT>::_mdata;
		init();
		mat[0][0] = mat[1][1] = std::cos(theta);
		mat[1][0] = std::sin(theta);
		mat[0][1] = -mat[1][0];
		detail::Matrix_<FPT>::_isNormalized = true;
		_hasChanged = true;
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
	template<typename T1,typename T2>
	Hmatrix_& addScale( T1 kx, T2 ky )
	{
		HOMOG2D_CHECK_IS_NUMBER(T1);
		HOMOG2D_CHECK_IS_NUMBER(T2);
		Hmatrix_ out;
		out.setScale( kx, ky );
		*this = out * *this;
		_hasChanged = true;
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
	template<typename T1,typename T2>
	Hmatrix_& setScale( T1 kx, T2 ky )
	{
		HOMOG2D_CHECK_IS_NUMBER(T1);
		HOMOG2D_CHECK_IS_NUMBER(T2);
		init();
		auto& mat = detail::Matrix_<FPT>::_mdata;
		mat[0][0] = kx;
		mat[1][1] = ky;
		detail::Matrix_<FPT>::_isNormalized = true;
		_hasChanged = true;
		return *this;
	}
///@}

	template<typename T>
	void applyTo( T& ) const;

#ifdef HOMOG2D_USE_OPENCV
	void copyTo( cv::Mat&, int type=CV_64F ) const;
	Hmatrix_& operator = ( const cv::Mat& );
#endif

/// Homography normalisation
	void normalize() const
	{
		detail::Matrix_<FPT>::p_normalize(2,2);
		_hasChanged = true;
	}

	void buildFrom4Points( const std::vector<Point2d_<FPT>>&, const std::vector<Point2d_<FPT>>&, int method=1 );

/// Matrix multiplication, call the base class product
	friend Hmatrix_ operator * ( const Hmatrix_& h1, const Hmatrix_& h2 )
	{
		Hmatrix_ out;
		detail::product( out, static_cast<detail::Matrix_<FPT>>(h1), static_cast<detail::Matrix_<FPT>>(h2) ) ;
		out.normalize();
		out._hasChanged = true;
		return out;
	}

/// Comparison operator. Does normalization if required
/**
This does an absolute comparison of all matrix elements, one by one,
and if one differs more than the threshold, it will return false
*/
	bool operator == ( const Hmatrix_& h ) const
	{
		auto& data = detail::Matrix_<FPT>::_mdata;

		if( !detail::Matrix_<FPT>::isNormalized() )
			normalize();
		if( !h.isNormalized() )
			h.normalize();

		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				if( std::fabs(
					static_cast<HOMOG2D_INUMTYPE>( data[i][j] ) - h.value(i,j) )
					>= thr::nullDeter()
				)
					return false;
		return true;
	}

/// Comparison operator. Does normalization if required
	bool operator != ( const Hmatrix_& h ) const
	{
		return !(*this == h);
	}

//////////////////////////
//   PRIVATE FUNCTIONS  //
//////////////////////////

private:
#ifdef HOMOG2D_FUTURE_STUFF
/// Implementation for epipolar matrices: initialize to aligned axis
	void impl_mat_init0( const detail::RootHelper<type::IsEpipmat>& )
	{
		_data.fillZero();
		_data[2][1] = 1.;
		_data[1][2] = 1.;
		_isNormalized = true;
	}
#endif

/// Implementation for homographies: initialize to unit transformation
	void impl_mat_init0( const detail::RootHelper<type::IsHomogr>& )
	{
		detail::Matrix_<FPT>::p_fillEye();
		detail::Matrix_<FPT>::_isNormalized = true;
	}

//////////////////////////
//      DATA SECTION    //
//////////////////////////
private:
	mutable bool _hasChanged   = true;
	mutable std::unique_ptr<detail::Matrix_<FPT>> _hmt; ///< used to store \f$ H^{-1} \f$, but only if required

	friend std::ostream& operator << ( std::ostream& f, const Hmatrix_& h )
	{
		f << "Hmatrix:\n"
			<< static_cast<const detail::Matrix_<FPT>&>(h);
		return f;
	}

}; // class Hmatrix_


namespace img {

//------------------------------------------------------------------
/// Point drawing style, see DrawParams
/**
\warning Check nextPointStyle() in case of added values here!
*/
enum class PtStyle: uint8_t
{
	Plus,   ///< "+" symbol
	Times,  ///< "times" symbol
	Star,   ///< "*" symbol
	Diam,   ///< diamond
	Dot     ///< dot (circle)
};

//------------------------------------------------------------------
/// Draw parameters, independent of back-end library
class DrawParams
{

/// Inner struct, holds the values. Needed so we can assign a default value as static member
	struct Dp_values
	{
		Color       _color;
		int         _lineThickness = 1;
		int         _pointSize     = 4;
		int         _lineType      = 1; /// if OpenCv: 1 for cv::LINE_AA, 2 for cv::LINE_8
		uint8_t     _ptDelta       = 8;           ///< pixels, used for drawing points
		PtStyle     _ptStyle       = PtStyle::Plus;
		bool        _enhancePoint  = false;       ///< to draw selected points
		bool        _showPoints    = false;       ///< show the points (useful only for Segment_ and Polyline_)
		bool        _showIndex     = false;     ///< show the index as number

/// Returns the point style following the current one
		PtStyle nextPointStyle() const
		{
			if( _ptStyle == PtStyle::Diam )
				return PtStyle::Plus;
			auto curr = static_cast<int>(_ptStyle);
			return static_cast<PtStyle>(curr+1);
		}
#ifdef HOMOG2D_USE_OPENCV
		cv::Scalar color() const
		{
			return cv::Scalar( _color.b, _color.g, _color.r );
		}
#endif // HOMOG2D_USE_OPENCV
	};

public:
	Dp_values _dpValues;

private:
	static Dp_values& p_getDefault()
	{
		static Dp_values s_defValue;
		return s_defValue;
	}

public:
	DrawParams()
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
	DrawParams& setPointStyle( PtStyle ps )
	{
		if( (int)ps > (int)PtStyle::Dot )
			throw std::runtime_error( "Error: invalid value for point style");
		_dpValues._ptStyle = ps;
		return *this;
	}
	DrawParams& setPointSize( uint8_t ps )
	{
		_dpValues._ptDelta = ps;
		return *this;
	}
	DrawParams& setThickness( uint8_t t )
	{
		_dpValues._lineThickness = t;
		return *this;
	}
	DrawParams& setColor( uint8_t r, uint8_t g, uint8_t b )
	{
		_dpValues._color = Color{r,g,b};
		return *this;
	}
	DrawParams& setColor( Color col )
	{
		_dpValues._color = col;
		return *this;
	}
	DrawParams& selectPoint()
	{
		_dpValues._enhancePoint = true;
		return *this;
	}
/// Set or unset the drawing of points (useful only for Segment_ and Polyline_)
	DrawParams& showPoints( bool b )
	{
		_dpValues._showPoints = b;
		return *this;
	}

/// Set or unset the drawing of points (useful only for Segment_ and Polyline_)
	DrawParams& showIndex( bool b=true )
	{
		_dpValues._showIndex = b;
		return *this;
	}

}; // class DrawParams

} // namespace img

//------------------------------------------------------------------
namespace detail {

/// Holds 9 parameters of ellipse
template<typename T>
struct EllParams
{
	T x0, y0; ///< center
	T theta = 0.; ///< angle
	T sint, cost; ///< \f$ \sin( \theta), \cos( \theta) \f$
	T a, b;
	T a2, b2; ///< squared values of a and b

	template<typename U>
	friend std::ostream& operator << ( std::ostream& f, const EllParams<U>& par );
};

template<typename U>
std::ostream& operator << ( std::ostream& f, const EllParams<U>& par )
{
	f << "EllParams: origin=" << par.x0 << "," << par.y0
		<< " angle=" << par.theta *180./M_PI
		<< " a=" << par.a << " b=" << par.b
		<< '\n';
	return f;
}

} // namespace detail

//------------------------------------------------------------------
/// Ellipse as a conic in matrix form.
/**
This enables its projection using homography

See:
- https://en.wikipedia.org/wiki/Ellipse#General_ellipse
- https://en.wikipedia.org/wiki/Matrix_representation_of_conic_sections

General equation of an ellipse:
\f[
A x^2 + B x y + C y^2 + D x + E y + F = 0
\f]
It can be written as a 3 x 3 matrix:
\f[
\begin{bmatrix}
  A & B/2 & D/2 \\
  B/2 & C & E/2 \\
  D/2 & E/2 & F
\end{bmatrix}
\f]

Matrix coefficients computed from center x0,y0, major and minor distances (a,b) and angle theta:
\f[
\begin{aligned}
  A &=   a^2 \sin^2\theta + b^2 \cos^2\theta \\
  B &=  2\left(b^2 - a^2\right) \sin\theta \cos\theta \\
  C &=   a^2 \cos^2\theta + b^2 \sin^2\theta \\
  D &= -2A x_\circ   -  B y_\circ \\
  E &= - B x_\circ   - 2C y_\circ \\
  F &=   A x_\circ^2 +  B x_\circ y_\circ + C y_\circ^2 - a^2 b^2
\end{aligned}
\f]

Homography projection: https://math.stackexchange.com/a/2320082/133647

\f[
Q' = H^{-T} \cdot Q \cdot H^{-1}
\f]

*/
template<typename FPT>
class Ellipse_: public detail::Matrix_<FPT>
{
public:
	using FType = FPT;

	template<typename T> friend class Ellipse_;

	template<typename FPT1,typename FPT2>
	friend Ellipse_<FPT1>
	operator * ( const Homogr_<FPT2>&, const Circle_<FPT1>& );

	template<typename FPT1,typename FPT2>
	friend Ellipse_<FPT1>
	operator * ( const Homogr_<FPT2>&, const Ellipse_<FPT1>& );

public:
/// \name Constructors
///@{

/// Default constructor: centered at (0,0), major=2, minor=1
	Ellipse_(): Ellipse_( 0., 0., 2., 1., 0. )
	{}

/// Constructor 1
	template<typename T1,typename T2=double,typename T3=double>
	Ellipse_( const Point2d_<T1>& pt, T2 major=2., T2 minor=1., T3 angle=0. )
		: Ellipse_( pt.getX(), pt.getY(), major, minor, angle )
	{}

/// Constructor 2
	template<typename T1,typename T2=double,typename T3=double>
	explicit Ellipse_( T1 x, T1 y, T2 major=2., T2 minor=1., T3 angle=0. )
	{
		HOMOG2D_CHECK_IS_NUMBER(T1);
		HOMOG2D_CHECK_IS_NUMBER(T2);
		HOMOG2D_CHECK_IS_NUMBER(T3);
		if( major<minor )
			std::swap( major, minor );
		p_init( x, y, major, minor, angle );
	}

/// Constructor 3, import from circle
	explicit Ellipse_( const Circle_<FPT>& cir )
	{
		p_init( cir.center().getX(), cir.center().getY(), cir.radius(), cir.radius(), 0. );
	}

/// Copy-Constructor
	template<typename FPT2>
	Ellipse_( const Ellipse_<FPT2>& other )
		: detail::Matrix_<FPT>( other )
	{}
///@}

	template<typename T1, typename T2>
	void translate( T1 dx, T2 dy )
	{
		HOMOG2D_CHECK_IS_NUMBER( T1 );
		HOMOG2D_CHECK_IS_NUMBER( T2 );
		auto par = p_getParams<HOMOG2D_INUMTYPE>();
		p_init( par.x0+dx, par.y0+dy, par.a, par.b, par.theta );
	}

	template<typename FPT2>
	bool pointIsInside( const Point2d_<FPT2>& ) const;

	template<typename T>
	void draw( img::Image<T>&, img::DrawParams dp=img::DrawParams() ) const;

/// \name attributes
///@{
	bool isCircle( HOMOG2D_INUMTYPE thres=1.E-10 )           const;
	Point2d_<FPT>                                center()    const;
	CPolyline_<FPT>                              getOBB()    const;
	FRect_<FPT>                                  getBB()     const;
	HOMOG2D_INUMTYPE                             angle()     const;
	std::pair<HOMOG2D_INUMTYPE,HOMOG2D_INUMTYPE> getMajMin() const;

///@}

/// Area of ellipse
	HOMOG2D_INUMTYPE area() const
	{
		auto par = p_getParams();
		return M_PI * par.a * par.b;
	}

	std::pair<Line2d_<FPT>,Line2d_<FPT>> getAxisLines() const;

	template<typename T>
	friend std::ostream&
	operator << ( std::ostream& f, const Ellipse_<T>& ell );

//////////////////////////
//   PRIVATE FUNCTIONS  //
//////////////////////////
private:
/// private constructor, needed in friend function only
/**
This is not public, because: 1-useless 2-not guarantee would be given that the input
is indeed a valid ellipse
*/
	explicit Ellipse_( const detail::Matrix_<FPT>& mat ): detail::Matrix_<FPT>( mat )
	{}

	template<typename T>
	detail::EllParams<T> p_getParams() const;
	template<typename T>
	detail::EllParams<T> p_computeParams() const;

/// Called by all the constructors, fills the matrix.
	void p_init( double x0, double y0, double a, double b, double theta=0. )
	{
		auto& data = detail::Matrix_<FPT>::_mdata;
		HOMOG2D_INUMTYPE sin1 = std::sin(theta);
		HOMOG2D_INUMTYPE cos1 = std::cos(theta);
		HOMOG2D_INUMTYPE sin2 = sin1 * sin1;
		HOMOG2D_INUMTYPE cos2 = cos1 * cos1;
		HOMOG2D_INUMTYPE a2   = a*a;
		HOMOG2D_INUMTYPE b2   = b*b;

		HOMOG2D_INUMTYPE A = a2*sin2 + b2*cos2;
		HOMOG2D_INUMTYPE B = 2.*(b2-a2) * std::sin(theta) * std::cos(theta);
		HOMOG2D_INUMTYPE C = a2 * cos2 + b2 * sin2;
		HOMOG2D_INUMTYPE D = -2.*A * x0 -    B * y0;
		HOMOG2D_INUMTYPE E =   - B * x0 - 2.*C * y0;
		HOMOG2D_INUMTYPE F = A*x0*x0 + B*x0*y0 + C*y0*y0 - a2*b2;

		data[0][0] = A;
		data[1][1] = C;
		data[2][2] = F;

		data[0][1] = data[1][0] = B / 2.;
		data[0][2] = data[2][0] = D / 2.;
		data[1][2] = data[2][1] = E / 2.;

#ifdef HOMOG2D_OPTIMIZE_SPEED
		_epHasChanged = false;
		_par.a = a;
		_par.b = b;
		_par.a2 = a2;
		_par.b2 = b2;
		_par.theta = theta;
		_par.sint = sin1;
		_par.cost = cos1;
		_par.x0 = x0;
		_par.y0 = y0;
#endif
	}

//////////////////////////
//      DATA SECTION    //
//////////////////////////
// (matrix is inherited from base class)
#ifdef HOMOG2D_OPTIMIZE_SPEED
	mutable bool _epHasChanged = true;   ///< if true, means we need to recompute parameters
	mutable detail::EllParams<FPT> _par;
#endif
}; // class Ellipse


//------------------------------------------------------------------
namespace detail {

// forward declaration of template instanciation
template<typename T1,typename T2,typename FPT1,typename FPT2>
base::LPBase<T1,FPT1> crossProduct( const base::LPBase<T2,FPT1>&, const base::LPBase<T2,FPT2>& );

struct Inters_1 {};
struct Inters_2 {};

/// Common stuff for intersection code
struct IntersectCommon
{
protected:
	bool _doesIntersect = false;
public:
	bool operator()() const
	{
		return _doesIntersect;
	}
};

/// Base class for intersection, gets specialized
template<typename T,typename FPT>
struct Intersect {};

//------------------------------------------------------------------
/// One point intersection
template<typename FPT>
class Intersect<Inters_1,FPT>: public IntersectCommon
{
	template<typename U>
	friend class ::h2d::Segment_;

	public:
		Point2d_<FPT>
		get() const
		{
			if( !_doesIntersect )
				HOMOG2D_THROW_ERROR_1( "No intersection points" );
			return _ptIntersect;
		}
		void set( const Point2d_<FPT>& pt )
		{
			_ptIntersect = pt;
			_doesIntersect = true;
		}
		size_t size() const { return _doesIntersect?1:0; }

		Intersect() {}
		Intersect( const Point2d_<FPT>& ptInter )
			: _ptIntersect(ptInter)
		{
			_doesIntersect = true;
		}
/// To enable conversions from different floating-point types
		template<typename FPT2>
		Intersect( const Intersect<Inters_1,FPT2>& other )
			: IntersectCommon( other )
		{
			_ptIntersect   = other._ptIntersect;
		}

	private:
		Point2d_<FPT> _ptIntersect;
};

//------------------------------------------------------------------
/// Two points intersection
template<typename FPT>
class Intersect<Inters_2,FPT>: public IntersectCommon
{
	template<typename U,typename V>
	friend class ::h2d::base::LPBase;

	public:
		Intersect() {}
		Intersect( const Point2d_<FPT>& p1, const Point2d_<FPT>& p2 )
			: _ptIntersect_1(p1), _ptIntersect_2(p2)
		{
				_doesIntersect = true;
		}
/// To enable conversions from different floating-point types
		template<typename FPT2>
		Intersect( const Intersect<Inters_2,FPT2>& other )
			: IntersectCommon( other )
		{
			auto ppts = other.get();
			_ptIntersect_1 = ppts.first;
			_ptIntersect_2 = ppts.second;
		}
		size_t size() const { return _doesIntersect?2:0; }

		std::pair<Point2d_<FPT>,Point2d_<FPT>>
		get() const
		{
			if( !_doesIntersect )
				HOMOG2D_THROW_ERROR_1( "No intersection points" );
			return std::make_pair( _ptIntersect_1, _ptIntersect_2 );
		}

	private:
		Point2d_<FPT> _ptIntersect_1, _ptIntersect_2;

	friend std::ostream& operator << ( std::ostream& f, const Intersect<Inters_2,FPT>& inters )
	{
		f << "bool=" << inters._doesIntersect
			<< " p1:" << inters._ptIntersect_1
			<< " p2:" << inters._ptIntersect_2;
		return f;
	}
};

//------------------------------------------------------------------
/// Multiple points intersections
template<typename FPT>
class IntersectM
{
private:
	mutable std::vector<Point2d_<FPT>> _vecInters; ///< mutable, because it can get sorted in const functions
public:
	IntersectM() {}
/// To enable conversions from different floating-point types
	template<typename FPT2>
	IntersectM( const IntersectM<FPT2>& other )
	{
		_vecInters.resize( other.size() );
		auto it = _vecInters.begin();
		for( const auto& elem: other.get() )
			*it++ = elem; // automatic type conversion
	}

	bool operator()() const
	{
		return !_vecInters.empty();
	}
	size_t size() const { return _vecInters.size(); }
	void add( const Point2d_<FPT>& pt )
	{
		_vecInters.push_back(pt);
	}

	void add( const std::vector<Point2d_<FPT>>& vpt )
	{
		for( const auto& pt: vpt )
			_vecInters.push_back(pt);
	}

/// Returns the intersection points, sorted
	std::vector<Point2d_<FPT>> get() const
	{
		std::sort( std::begin(_vecInters), std::end(_vecInters) );
		return _vecInters;
	}
	friend std::ostream&
	operator << ( std::ostream& f, IntersectM i )
	{
		f << "IntersectM: size=" << i.size() << '\n' << i._vecInters;
		return f;
	}
};

//------------------------------------------------------------------
/// Helper class, holds result of intersections of two FRect_
/// \sa FRect_::intersectArea()
template<typename T>
class RectArea
{
private:
	bool      _success = false;
	FRect_<T> _area;

public:
	RectArea() = default;
	RectArea( const FRect_<T>& r ) : _success(true), _area(r)
	{}
	bool operator()() const
	{
		return _success;
	}

	FRect_<T> get() const
	{
		assert( _success );
		return _area;
	}
};


} // namespace detail


//------------------------------------------------------------------
/// A Flat Rectangle, modeled by its two opposite points
template<typename FPT>
class FRect_: public detail::Common<FPT>
{
public:
	using FType = FPT;

	template<typename T> friend class FRect_;

private:
	Point2d_<FPT> _ptR1,_ptR2;

public:
/** \name Constructors */
///@{
/// Default constructor, initialize rectangle to (0,0)-(1,1)
	FRect_()
	{
		_ptR2.set( 1., 1. );
	}
/// Constructor from 2 points
	template<typename FPT2>
	FRect_( const Point2d_<FPT2>& pa, const Point2d_<FPT2>& pb )
	{
		set( pa, pb );
	}

/// Constructor from center point, width and height
	template<typename FPT2,typename T1, typename T2>
	FRect_( const Point2d_<FPT2>& p0, T1 w, T2 h )
	{
		HOMOG2D_CHECK_IS_NUMBER(T1);
		HOMOG2D_CHECK_IS_NUMBER(T2);
		set(
			Point2d_<FPT>( p0.getX()-0.5L*w, p0.getY()-0.5L*h ),
			Point2d_<FPT>( p0.getX()+0.5L*w, p0.getY()+0.5L*h )
		);
	}

/// Constructor from x1, y1, x2, y2 (need to be all the same type)
	template<typename T>
	FRect_( T x1, T y1, T x2, T y2 )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		set( Point2d_<FPT>(x1,y1), Point2d_<FPT>(x2,y2) );
	}

/// Copy-Constructor
	template<typename FPT2>
	FRect_( const FRect_<FPT2>& other )
		: _ptR1(other._ptR1),_ptR2(other._ptR2)
	{}
///@}

private:
/// Private constructor from 4 points, used in intersectArea( const FRect_& )
	template<typename T>
	FRect_(
		const Point2d_<T>& pt1,
		const Point2d_<T>& pt2,
		const Point2d_<T>& pt3,
		const Point2d_<T>& pt4
	)
	{
		HOMOG2D_LOG( "pt1" << pt1 << " pt2="<< pt2 << " pt3=" << pt3 << " pt4=" << pt4 );
		auto x0 = std::min( pt1.getX(), pt2.getX() );
		x0 = std::min( x0, pt3.getX() );
		x0 = std::min( x0, pt4.getX() );
		auto y0 = std::min( pt1.getY(), pt2.getY() );
		y0 = std::min( y0, pt3.getY() );
		y0 = std::min( y0, pt4.getY() );

		auto x1 = std::max( pt1.getX(), pt2.getX() );
		x1 = std::max( x1, pt3.getX() );
		x1 = std::max( x1, pt4.getX() );
		auto y1 = std::max( pt1.getY(), pt2.getY() );
		y1 = std::max( y1, pt3.getY() );
		y1 = std::max( y1, pt4.getY() );

		_ptR1 = Point2d_<FPT>(x0,y0);
		_ptR2 = Point2d_<FPT>(x1,y1);
		HOMOG2D_LOG( "ptR1=" <<_ptR1 << " _ptR2=" << _ptR2 );
	}

public:
	void set( const Point2d_<FPT>& pa, const Point2d_<FPT>& pb )
	{
		auto ppts = detail::getCorrectPoints( pa, pb );
		_ptR1 = ppts.first;
		_ptR2 = ppts.second;
	}

/// \name Attributes access
///@{

	HOMOG2D_INUMTYPE height() const { return  _ptR2.getY() - _ptR1.getY(); }
	HOMOG2D_INUMTYPE width()  const { return  _ptR2.getX() - _ptR1.getX(); }
	HOMOG2D_INUMTYPE area()   const { return height() * width(); }
	HOMOG2D_INUMTYPE length() const { return 2.*height() + 2.*width(); }
/// Return size of rectangle in a std::pair<width, height>
	std::pair<HOMOG2D_INUMTYPE,HOMOG2D_INUMTYPE> size() const
	{
		return std::make_pair( width(), height() );
	}
/// Needed for getBB( pair of objects )
	FRect_<FPT> getBB() const
	{
		return *this;
	}
/// Returns the 2 major points of the rectangle
/// \sa getPts( const FRect_<FPT>& )
	std::pair<Point2d_<FPT>,Point2d_<FPT>>
	getPts() const
	{
		return std::make_pair( _ptR1, _ptR2 );
	}

	Point2d_<FPT> center() const
	{
		return Point2d_<FPT>(
			(static_cast<HOMOG2D_INUMTYPE>(_ptR1.getX() ) + _ptR2.getX() ) * 0.5,
			(static_cast<HOMOG2D_INUMTYPE>(_ptR1.getY() ) + _ptR2.getY() ) * 0.5
		);
	}

	Circle_<FPT> getBoundingCircle() const;

///@}

public:
	template<typename T1, typename T2>
	void translate( T1 dx, T2 dy )
	{
		HOMOG2D_CHECK_IS_NUMBER( T1 );
		HOMOG2D_CHECK_IS_NUMBER( T2 );
		_ptR1.set( _ptR1.getX() + dx, _ptR1.getY() + dy );
		_ptR2.set( _ptR2.getX() + dx, _ptR2.getY() + dy );
	}


/// \name Union/intersection area
///@{
	template<typename FPT2>
	CPolyline_<FPT> unionArea( const FRect_<FPT2>& other ) const;
	template<typename FPT2>
	detail::RectArea<FPT> intersectArea( const FRect_<FPT2>& other ) const;
	template<typename FPT2>
	CPolyline_<FPT> operator | ( const FRect_<FPT2>& other ) const
	{
		return this->unionArea( other );
	}
	template<typename FPT2>
	detail::RectArea<FPT> operator & ( const FRect_<FPT2>& other ) const
	{
		return this->intersectArea( other );
	}
///@}

/// Returns the 4 points of the rectangle, starting from "smallest" one, and
/// in clockwise order
/**
\verbatim

 p1 +------+ p2
    |      |
    |      |
    |      |
 p0 +------+ p3

\endverbatim
\sa get4Pts( const FRect_<FPT>& )
*/
	std::array<Point2d_<FPT>,4>
	get4Pts() const
	{
		std::array<Point2d_<FPT>,4> arr;
		arr[0] = _ptR1;
		arr[1] = Point2d_<FPT>( _ptR1.getX(), _ptR2.getY() );
		arr[2] = _ptR2;
		arr[3] = Point2d_<FPT>( _ptR2.getX(), _ptR1.getY() );
		return arr;
	}

/// Returns the 4 segments of the rectangle, starting with the first vertical one
/**
\verbatim
      s1
   +------+p2
   |      |
s0 |      | s2
   |      |
 p1+------+
      s3
\endverbatim
\sa \ref h2d::getSegs( const FRect_& )
*/
	std::array<Segment_<FPT>,4>
	getSegs() const
	{
		auto pts = get4Pts();
		std::array<Segment_<FPT>,4> out;
		out[0] = Segment_<FPT>( pts[0], pts[1] );
		out[1] = Segment_<FPT>( pts[1], pts[2] );
		out[2] = Segment_<FPT>( pts[2], pts[3] );
		out[3] = Segment_<FPT>( pts[3], pts[0] );
		return out;
	}
/// Returns true if rectangle is inside \c shape (circle or rectangle)
/// \todo maybe add some SFINAE to enable only for Circle_ or FRect_?
	template<typename T>
	bool isInside( const T& shape )
	{
		for( const auto& pt: get4Pts() )
			if( !pt.isInside( shape ) )
				return false;
		return true;
	}

/// \name Intersection functions
///@{

/// FRect/Line intersection
	template<typename FPT2>
	detail::Intersect<detail::Inters_2,FPT> intersects( const Line2d_<FPT2>& line ) const
	{
		HOMOG2D_START;
		return line.intersects( *this );
	}

/// FRect/Segment intersection
	template<typename FPT2>
	detail::IntersectM<FPT> intersects( const Segment_<FPT2>& seg ) const
	{
		HOMOG2D_START;
		detail::IntersectM<FPT> out;
		for( const auto& rseg: getSegs() )
		{
			auto inters = rseg.intersects( seg ); // call of Segment/Segment
			if( inters() )
			{
				auto pt =  inters.get();
				bool addPoint = true;
				if( out.size() == 1 ) // if we have already one
					if( out.get()[0] == pt )
						addPoint = false;
				if( addPoint )
					out.add( pt );
				if( out.size() == 2 )
					break;
			}
		}
		return out;
	}

/// FRect/Circle intersection
	template<typename FPT2>
	detail::IntersectM<FPT> intersects( const Circle_<FPT2>& circle ) const
	{
		HOMOG2D_START;
		return p_intersects_R_C( circle );
	}

/// FRect/Polyline intersection
	template<typename PLT2,typename FPT2>
	detail::IntersectM<FPT> intersects( const base::PolylineBase<PLT2,FPT2>& pl ) const
	{
		HOMOG2D_START;
		return pl.intersects( *this );
	}

/// FRect/FRect intersection
	template<typename FPT2>
	detail::IntersectM<FPT> intersects( const FRect_<FPT2>& rect ) const
	{
		HOMOG2D_START;
		if( *this == rect )
			return detail::IntersectM<FPT>();
		return p_intersects_R_C( rect );
	}
///@}

/// \name Operators
///@{
	template<typename FPT2>
	bool operator == ( const FRect_<FPT2>& other ) const
	{
		if( _ptR1 != other._ptR1 )
			return false;
		if( _ptR2 != other._ptR2 )
			return false;
		return true;
	}
	template<typename FPT2>
	bool operator != ( const FRect_<FPT2>& other ) const
	{
		return !( *this == other );
	}
///@}

	template<typename T>
	friend std::ostream&
	operator << ( std::ostream& f, const FRect_<T>& r );

	template<typename T>
	void draw( img::Image<T>&, img::DrawParams dp=img::DrawParams() ) const;

private:
	template<typename FPT2>
	std::vector<Point2d_<FPT>> p_pointsInside( const FRect_<FPT2>& other ) const
	{
		std::vector<Point2d_<FPT>> out;
		for( const auto& pt: get4Pts() )
			if( pt.isInside( other ) )
				out.push_back( pt );
		return out;
	}

/// Intersection of FRect vs FRect or Circle
/**
- We use a \c std::set to avoid having multiple times the same point.
- second arg is used to fetch the indexes of intersecting segments, when needed
*/
	template<typename T>
	detail::IntersectM<FPT> p_intersects_R_C( const T& other ) const
	{
		std::set<Point2d_<FPT>> pts;
		for( const auto& rseg: getSegs() )
		{
			auto inters = rseg.intersects( other ); // call of Segment/FRect => FRect/Segment, or Segment/Circle
			if( inters() )
			{
				auto vpts = inters.get();
				assert( vpts.size() < 3 );
				if( vpts.size() > 0 )
					pts.insert( vpts[0] );
				if( vpts.size() > 1 )
					pts.insert( vpts[1] );
			}
		}
		detail::IntersectM<FPT> out;
		for( const auto& elem: pts )
			out.add( elem );
		return out;
	}

}; // class FRect_


//------------------------------------------------------------------
/// A circle
template<typename FPT>
class Circle_: public detail::Common<FPT>
{
public:
	using FType = FPT;

	template<typename T> friend class Circle_;

private:
	FPT           _radius;
	Point2d_<FPT> _center;

public:
/// \name Constructors
///@{

/// Default constructor, unit-radius circle at (0,0)
	Circle_() : _radius(1.)
	{}

/// Constructor 2, given radius circle at (0,0)
	template<typename T>
	explicit Circle_( T rad )
		: Circle_( Point2d_<FPT>(), rad )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
	}

/// Constructor 3: point and radius
	template<typename T1, typename T2>
	Circle_( const Point2d_<T1>& center, T2 rad )
		: _radius(rad), _center(center)
	{
#ifndef HOMOG2D_NOCHECKS
		if( std::abs(rad) < thr::nullDistance() )
			HOMOG2D_THROW_ERROR_1( "radius must value too small: " << std::scientific << std::abs(rad) );
		if( rad < 0. )
			HOMOG2D_THROW_ERROR_1( "radius must not be <0" );
#endif
	}

/// Constructor 4: x, y, radius
	template<typename T1, typename T2>
	Circle_( T1 x, T1 y, T2 rad )
		: Circle_( Point2d_<FPT>(x,y), rad )
	{
		HOMOG2D_CHECK_IS_NUMBER(T1);
		HOMOG2D_CHECK_IS_NUMBER(T2);
	}

/// Copy-Constructor
	template<typename FPT2>
	Circle_( const Circle_<FPT2>& other )
		: _radius(other._radius), _center(other._center)
	{}
///@}

/// \name Attributes access
///@{
	FPT&       radius()       { return _radius; }
	const FPT& radius() const { return _radius; }

	Point2d_<FPT>&       center()       { return _center; }
	const Point2d_<FPT>& center() const { return _center; }

/// Returns Bounding Box
	FRect_<FPT> getBB() const
	{
		return FRect_<FPT>(
			_center.getX()-_radius, _center.getY()-_radius,
			_center.getX()+_radius, _center.getY()+_radius
		);
	}
///@}

	template<typename FPT2,typename FPT3>
	void set( const Point2d_<FPT2>& center, FPT3 rad )
	{
		Circle_<FPT> c( center, rad );
		std::swap( c, *this ); /// \todo 20211216: replace with move
	}
	template<typename FPT2,typename FPT3>
	void set( FPT2 x, FPT2 y, FPT3 rad )
	{
		Circle_<FPT> c( x, y, rad );
		std::swap( c, *this ); /// \todo 20211216: replace with move
	}

	template<typename T1, typename T2>
	void translate( T1 dx, T2 dy )
	{
		HOMOG2D_CHECK_IS_NUMBER( T1 );
		HOMOG2D_CHECK_IS_NUMBER( T2 );
		_center.translate( dx, dy );
	}

/// Returns true if circle is inside \c other circle
	template<typename FPT2>
	bool isInside( const Circle_<FPT2>& other ) const
	{
		return( _radius + _center.distTo( other.center() ) < other.radius() );
	}

/// Returns true if circle is inside rectangle defined by \c p1 and \c p2
	template<typename FPT2>
	bool isInside( const Point2d_<FPT2>& p1, const Point2d_<FPT2>& p2 ) const
	{
		return implC_isInside( detail::getCorrectPoints( p1, p2 ) );
	}

/// Returns true if circle is inside flat rectangle \c rect
	template<typename FPT2>
	bool isInside( const FRect_<FPT2>& rect )
	{
		return implC_isInside( rect.getPts() );
	}

/// \name Intersection
///@{

/// Circle/Line intersection
	template<typename FPT2>
	detail::Intersect<detail::Inters_2,FPT>
	intersects( const Line2d_<FPT2>& li ) const
	{
		HOMOG2D_START;
		return li.intersects( *this );
	}

/// Circle/Segment intersection
	template<typename FPT2>
	detail::IntersectM<FPT>
	intersects( const Segment_<FPT2>& seg ) const
	{
		HOMOG2D_START;
		return seg.intersects( *this );
	}

// Circle/Circle intersection
	template<typename FPT2>
	detail::Intersect<detail::Inters_2,FPT>
	intersects( const Circle_<FPT2>& seg ) const;

/// Circle/FRect intersection
	template<typename FPT2>
	detail::IntersectM<FPT> intersects( const FRect_<FPT2>& rect ) const
	{
		HOMOG2D_START;
		return rect.intersects( * this );
	}

/// Circle/Polyline intersection
	template<typename PLT,typename FPT2>
	detail::IntersectM<FPT> intersects( const base::PolylineBase<PLT,FPT2>& pl ) const
	{
		HOMOG2D_START;
		return pl.intersects( * this );
	}
///@}

private:
	template<typename FPT2>
	bool implC_isInside( const std::pair<Point2d_<FPT2>, Point2d_<FPT2>>& ppts ) const
	{
		const auto& p1 = ppts.first;
		const auto& p2 = ppts.second;
		HOMOG2D_INUMTYPE rad = _radius;   // convert to highest precision
		if( _center.getX() + rad < p2.getX() )
			if( _center.getX() - rad > p1.getX() )
				if( _center.getY() + rad < p2.getY() )
					if( _center.getY() - rad > p1.getY() )
						return true;
		return false;
	}

public:
/// \name Operators
///@{
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
///@}

	template<typename T>
	friend std::ostream&
	operator << ( std::ostream& f, const Circle_<T>& r );

	template<typename T>
	void draw( img::Image<T>&, img::DrawParams dp=img::DrawParams() ) const;

}; // class Circle_

//------------------------------------------------------------------
/// Return circle passing through 4 points of flat rectangle
/// \sa h2d::getBoundingCircle()
template<typename FPT>
Circle_<FPT>
FRect_<FPT>::getBoundingCircle() const
{
	auto pts = get4Pts();
	auto seg1 = pts[1] * pts[3];
	auto seg2 = pts[0] * pts[2];

	auto middle_pt = seg1 * seg2;
	return Circle_<FPT>( middle_pt, middle_pt.distTo( pts[0] ) );
}


/*
/// Constructor: build Ellipse from Circle
/// \todo finish this
template<typename FPT>
Ellipse_<FPT>::Ellipse_( const Circle_<FPT>& cir )
{
	p_init( cir.center().getX(), cir.center().getY(), radius(), radius(), 0. );
}
*/

//------------------------------------------------------------------
/// Holds private stuff
namespace priv {

/// Private free function, swap the points so that \c ptA.x <= \c ptB.x, and if equal, sorts on y
template<typename FPT>
void
fix_order( Point2d_<FPT>& ptA, Point2d_<FPT>& ptB )
{
	if( !(ptA < ptB) )
		std::swap( ptA, ptB );
}


/// Helper function, factorized here for the two impl_getPoints_A() implementations
template<typename FPT, typename FPT2>
std::pair<Point2d_<FPT>,Point2d_<FPT>>
getPoints_B2( const Point2d_<FPT>& pt, FPT2 dist, const Line2d_<FPT>& li )
{
	auto arr = li.get();
	const HOMOG2D_INUMTYPE a = static_cast<HOMOG2D_INUMTYPE>(arr[0]);
	const HOMOG2D_INUMTYPE b = static_cast<HOMOG2D_INUMTYPE>(arr[1]);
	auto coeff = static_cast<HOMOG2D_INUMTYPE>(dist) / std::sqrt( a*a + b*b );

	Point2d_<FPT> pt1(
        pt.getX() -  b * coeff,
        pt.getY() +  a * coeff
	);
	Point2d_<FPT> pt2(
        pt.getX() +  b * coeff,
        pt.getY() -  a * coeff
	);
	fix_order( pt1, pt2 );
	return std::make_pair( pt1, pt2 );
}

/// Helper function for impl_getOrthogonalLine_A() and impl_getOrthogonalLine_B(),
/// Compute orthogonal line to \c li at point \c pt (that must lie on the line)
template<typename T1,typename T2>
Line2d_<T1>
getOrthogonalLine_B2( const Point2d_<T2>& pt, const Line2d_<T1>& li )
{
	auto arr = li.get(); // get array of 3 values
	Line2d_<T1> out(
		-arr[1],
		arr[0],
		arr[1] * pt.getX() - arr[0] * pt.getY()
	);
	out.p_normalizeLine();
	return out;
}

#ifdef HOMOG2D_DEBUGMODE
template<typename T>
void printVector( const std::vector<T>& v, std::string msg=std::string() )
{
	std::cout << "vector: ";
	if( !msg.empty() )
		std::cout << msg;
	std::cout << " #=" << v.size() << '\n';
	for( const auto& elem: v )
		std::cout << elem << "-";
	std::cout << '\n';
}
template<typename T,size_t N>
void printArray( const std::array<T,N>& v, std::string msg=std::string() )
{
	std::cout << "array: " << msg << " #=" << N<< '\n';
	for( const auto& elem: v )
		std::cout << elem << "-";
	std::cout << '\n';
}
template<typename T>
void printVectorPairs( const std::vector<std::pair<T,T>>& v )
{
	std::cout << "vector of pairs: #=" << v.size() << '\n';
	for( const auto& elem: v )
		std::cout << " [" << (int)elem.first << "-" << (int)elem.second << "] ";
	std::cout << '\n';
}
#endif
} // namespace priv

// forward declaration
template<typename FPT1,typename FPT2>
Line2d_<FPT1>
operator * ( const Point2d_<FPT1>&, const Point2d_<FPT2>& );

// forward declaration
template<typename FPT1,typename FPT2>
Point2d_<FPT1>
operator * ( const Line2d_<FPT1>&, const Line2d_<FPT2>& );

namespace base {

//------------------------------------------------------------------
/// Base class, will be instanciated as a \ref Point2d_ or a \ref Line2d_<>
/**
Type parameters:
- LP: type::IsPoint or type::IsLine
- FPT: Floating Point Type (float, double or long double)
*/
template<typename LP,typename FPT>
class LPBase: public detail::Common<FPT>
{
public:
	using FType = FPT;

private:
	template<typename U,typename V> friend class Hmatrix_;

// This is needed so we can convert from, say, Point2d_<float> to Point2d_<double>
	template<typename U,typename V> friend class LPBase;

	template<typename FPT1,typename FPT2>
	friend Point2d_<FPT1>
	h2d::operator * ( const h2d::Line2d_<FPT1>&, const h2d::Line2d_<FPT2>& );

	template<typename FPT1,typename FPT2>
	friend //Line2d_<FPT1>
	auto
	h2d::operator * ( const h2d::Point2d_<FPT1>&, const h2d::Point2d_<FPT2>& )
	-> h2d::Line2d_<FPT1>;

	template<typename T,typename U>
	friend auto //Line2d_<T>
	h2d::operator * ( const h2d::Homogr_<U>&, const h2d::Line2d_<T>& )
	-> h2d::Line2d_<T>;

	template<typename T1,typename T2,typename FPT1,typename FPT2>
	friend base::LPBase<T1,FPT1>
	detail::crossProduct( const base::LPBase<T2,FPT1>&, const base::LPBase<T2,FPT2>& );

	template<typename U,typename V>
	friend auto
	operator << ( std::ostream& f, const h2d::base::LPBase<U,V>& r )
	-> std::ostream&;
/*
{
	r.impl_op_stream( f, r );
	return f;
}*/

	template<typename T1,typename T2,typename FPT1,typename FPT2>
	friend void
	detail::product( base::LPBase<T1,FPT1>&, const detail::Matrix_<FPT2>&, const base::LPBase<T2,FPT1>& );

	template<typename T1,typename T2>
	friend Line2d_<T1>
	priv::getOrthogonalLine_B2( const Point2d_<T2>&, const Line2d_<T1>& );

public:

/// Constructor: build a point from two lines
	template<typename FPT2>
	LPBase( const Line2d_<FPT2>& v1, const Line2d_<FPT2>& v2 )
	{
#ifndef HOMOG2D_NOCHECKS
		if( v1.isParallelTo(v2) )
			HOMOG2D_THROW_ERROR_1( "unable to build point from these two lines, are parallel" );
#endif
		*this = detail::crossProduct<type::IsPoint>( v1, v2 );
	}

/// Constructor: build a line from two points
	template<typename FPT2>
	LPBase( const Point2d_<FPT2>& v1, const Point2d_<FPT2>& v2 )
	{
#ifndef HOMOG2D_NOCHECKS
		if( v1 == v2 )
			HOMOG2D_THROW_ERROR_1( "unable to build line from these two points, are the same: " << v1 );
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
	LPBase( const Line2d_<T>& li )
	{
		impl_init_1_Line<T>( li, detail::RootHelper<LP>() );
	}

/// Constructor with single arg of type "Point"
/**
This will call one of the two overloads of \c impl_init_1_Point(), depending on type of object:
- if type is a point, then it can be seen as a copy-constructor
- if type is a line, this will build a line from (0,0] to \c pt
*/
	template<typename T>
	LPBase( const Point2d_<T>& pt )
	{
		impl_init_1_Point<T>( pt, detail::RootHelper<LP>() );
	}

/// Constructor: build from two numerical values, depends on the type
	template<typename T1,typename T2>
	LPBase( const T1& v1, const T2& v2 )
	{
		HOMOG2D_CHECK_IS_NUMBER(T1);
		HOMOG2D_CHECK_IS_NUMBER(T2);
		impl_init_2( v1, v2, detail::RootHelper<LP>() );
	}

/// Constructor of line/point from 3 values
	template<typename T>
	LPBase( T v0, T v1, T v2 )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		_v[0] = v0;
		_v[1] = v1;
		_v[2] = v2;
	}

/// Constructor of line from 4 values x1,y1,x2,y2
	template<typename T>
	LPBase( T x1, T y1, T x2, T y2 )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		impl_init_4( x1, y1, x2, y2, detail::RootHelper<LP>() );
	}

/// Constructor of Point/Line from random type
	template<typename T>
	LPBase( T val )
	{
		impl_init_2( val.HOMOG2D_BIND_X, val.HOMOG2D_BIND_Y, detail::RootHelper<LP>() );
	}

/// Default constructor, depends on the type
	LPBase()
	{
		impl_init( detail::RootHelper<LP>() );
	}

/// Constructor of horizontal/vertical line
	template<typename T>
	LPBase( LineDir orient, T value )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		impl_init_or( orient, value, detail::RootHelper<LP>() );
	}

private:
	template<typename T,typename U>
	void p_copyFrom( const LPBase<T,U>& other )
	{
		_v[0] = static_cast<FPT>(other._v[0]);
		_v[1] = static_cast<FPT>(other._v[1]);
		_v[2] = static_cast<FPT>(other._v[2]);
	}
	/// Arg is a point, object is a point => copy-constructor
	template<typename T>
	void impl_init_1_Point( const Point2d_<T>& pt, const detail::RootHelper<type::IsPoint>& )
	{
		p_copyFrom( pt );
	}
	/// Arg is a point, object is a line: we build the line passing though (0,0) ant the given point
	template<typename T>
	void impl_init_1_Point( const Point2d_<T>& pt, const detail::RootHelper<type::IsLine>& )
	{
		*this = detail::crossProduct<type::IsLine>( pt, Point2d_<FPT>() );
		p_normalizeLine();
	}

	/// Arg is a line, object is a point: ILLEGAL INSTANCIATION
	template<typename T>
	constexpr void impl_init_1_Line( const Line2d_<T>&, const detail::RootHelper<type::IsPoint>& )
	{
		static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot build a point from a line" );
	}
	/// Arg is a line, object is a line => copy-constructor
	template<typename T>
	void impl_init_1_Line( const Line2d_<T>& li, const detail::RootHelper<type::IsLine>& )
	{
		p_copyFrom( li );
	}

	template<typename T>
	constexpr void impl_init_or( LineDir, T, const detail::RootHelper<type::IsPoint>& )
	{
		static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot build a horiz/vertical point" );
	}
	template<typename T>
	void impl_init_or( LineDir dir, T value, const detail::RootHelper<type::IsLine>& )
	{
		_v[2] = -value;
		if( dir == LineDir::V )
		{
			_v[0] = 1.; _v[1] = 0.;
		}
		else  // = LineDir::H
		{
			_v[0] = 0.; _v[1] = 1.;
		}
	}

	template<typename T>
	constexpr void
	impl_init_4( T, T, T, T, const detail::RootHelper<type::IsPoint>& )
	{
		static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot build a point from 4 values" );
	}
	template<typename T>
	void
	impl_init_4( T x1, T y1, T x2, T y2, const detail::RootHelper<type::IsLine>& )
	{
		*this = Point2d_<HOMOG2D_INUMTYPE>(x1, y1) * Point2d_<HOMOG2D_INUMTYPE>(x2, y2);
	}

public:
	Type type() const
	{
		return impl_type( detail::RootHelper<LP>() );
	}

private:
	Type impl_type( const detail::RootHelper<type::IsPoint>& ) const
	{
		return Type::Point2d;
	}
	Type impl_type( const detail::RootHelper<type::IsLine>& ) const
	{
		return Type::Line2d;
	}

public:
	FPT
	getCoord( GivenCoord gc, FPT other ) const
	{
		return impl_getCoord( gc, other, detail::RootHelper<LP>() );
	}

	Point2d_<FPT>
	getPoint( GivenCoord gc, FPT other ) const
	{
		return impl_getPoint( gc, other, detail::RootHelper<LP>() );
	}

	/// Returns a pair of points that are lying on line at distance \c dist from a point defined by one of its coordinates.
	template<typename FPT2>
	std::pair<Point2d_<FPT>,Point2d_<FPT>>
	getPoints( GivenCoord gc, FPT coord, FPT2 dist ) const
	{
		return impl_getPoints_A( gc, coord, dist, detail::RootHelper<LP>() );
	}

	/// Returns a pair of points that are lying on line at distance \c dist from point \c pt, assuming that one is lying on the line.
	template<typename FPT2>
	std::pair<Point2d_<FPT>,Point2d_<FPT>>
	getPoints( const Point2d_<FPT>& pt, FPT2 dist ) const
	{
		return impl_getPoints_B( pt, dist, detail::RootHelper<LP>() );
	}

	/// Returns an orthogonal line to the one it is called on, at a point defined by one of its coordinates.
	Line2d_<FPT>
	getOrthogonalLine( GivenCoord gc, FPT other ) const
	{
		return impl_getOrthogonalLine_A( gc, other, detail::RootHelper<LP>() );
	}

	/// Returns an orthogonal line to the one it is called on, at point \c pt, assuming that one is lying on the line.
	Line2d_<FPT>
	getOrthogonalLine( const Point2d_<FPT>& pt ) const
	{
		return impl_getOrthogonalLine_B( pt, detail::RootHelper<LP>() );
	}

	/// Returns an parallel line to the one it is called on, with \c pt lying on it.
	Line2d_<FPT>
	getParallelLine( const Point2d_<FPT>& pt ) const
	{
		return impl_getParallelLine( pt, detail::RootHelper<LP>() );
	}

	/// Returns the pair of parallel lines at a distance \c dist from line.
	template<typename T>
	std::pair<Line2d_<FPT>,Line2d_<FPT>>
	getParallelLines( T dist ) const
	{
		return impl_getParallelLines( dist, detail::RootHelper<LP>() );
	}

	FPT getX() const { return impl_getX( detail::RootHelper<LP>() ); }
	FPT getY() const { return impl_getY( detail::RootHelper<LP>() ); }

	template<typename T1,typename T2>
	void translate( T1 dx, T2 dy )
	{
		HOMOG2D_CHECK_IS_NUMBER( T1 );
		HOMOG2D_CHECK_IS_NUMBER( T2 );
		impl_move( dx, dy, detail::RootHelper<LP>() );
	}

	std::array<FPT,3> get() const { return impl_get( detail::RootHelper<LP>() ); }

	template<typename T1,typename T2>
	void set( T1 x, T2 y ) { impl_set( x, y, detail::RootHelper<LP>() ); }

	template<typename FPT2>
	HOMOG2D_INUMTYPE distTo( const Point2d_<FPT2>& pt ) const
	{
		return impl_distToPoint( pt, detail::RootHelper<LP>() );
	}
	template<typename FPT2>
	HOMOG2D_INUMTYPE distTo( const Line2d_<FPT2>& li ) const
	{
		return impl_distToLine( li, detail::RootHelper<LP>() );
	}

	template<typename T,typename FPT2>
	bool isParallelTo( const LPBase<T,FPT2>& li ) const
	{
		return impl_isParallelTo( li, detail::RootHelper<T>() );
	}
	template<typename T>
	bool isParallelTo( const Segment_<T>& seg ) const
	{
		return impl_isParallelTo( seg.getLine(), detail::RootHelper<LP>() );
	}
/// Returns angle in rad. between the lines. \sa h2d::getAngle()
/**
Please check out warning described in impl_getAngle()
*/
	template<typename T,typename FPT2>
	HOMOG2D_INUMTYPE getAngle( const LPBase<T,FPT2>& other ) const
	{
		return impl_getAngle( other, detail::RootHelper<T>() );
	}

/// Returns angle in rad. between line and segment \c seg. \sa  h2d::getAngle()
	template<typename FPT2>
	HOMOG2D_INUMTYPE getAngle( const Segment_<FPT2>& seg ) const
	{
		return impl_getAngle( seg.getLine(), detail::RootHelper<LP>() );
	}

private:
	FPT impl_getX( const detail::RootHelper<type::IsPoint>& ) const
	{
		return _v[0]/_v[2];
	}
	FPT impl_getY( const detail::RootHelper<type::IsPoint>& ) const
	{
		return _v[1]/_v[2];
	}

	constexpr std::array<FPT,3>
	impl_get( const detail::RootHelper<type::IsPoint>& ) const
	{
		static_assert( detail::AlwaysFalse<LP>::value, "illegal for points" );
	}
	std::array<FPT,3>
	impl_get( const detail::RootHelper<type::IsLine>& ) const
	{
		return std::array<FPT,3> { _v[0], _v[1], _v[2] };
	}
	template<typename T1,typename T2>
	void impl_set( T1 x, T2 y, const detail::RootHelper<type::IsPoint>& )
	{
		_v[0] = x;
		_v[1] = y;
		_v[2] = 1.;
	}
	template<typename T1,typename T2>
	constexpr void
	impl_set( T1, T2, const detail::RootHelper<type::IsLine>& )
	{
		static_assert( detail::AlwaysFalse<LP>::value, "Invalid call for lines" );
	}

	template<typename T1,typename T2>
	void impl_move( T1 dx, T2 dy, const detail::RootHelper<type::IsPoint>& )
	{
		_v[0] = static_cast<HOMOG2D_INUMTYPE>(_v[0]) / _v[2] + dx;
		_v[1] = static_cast<HOMOG2D_INUMTYPE>(_v[1]) / _v[2] + dy;
		_v[2] = 1.;
	}
	template<typename T1,typename T2>
	constexpr void
	impl_move( T1, T2, const detail::RootHelper<type::IsLine>& )
	{
		static_assert( detail::AlwaysFalse<LP>::value, "Invalid call for lines" );
	}

	template<typename FPT2>
	HOMOG2D_INUMTYPE impl_distToPoint( const Point2d_<FPT2>&, const detail::RootHelper<type::IsPoint>& ) const;
	template<typename FPT2>
	HOMOG2D_INUMTYPE impl_distToPoint( const Point2d_<FPT2>&, const detail::RootHelper<type::IsLine>&  ) const;
	template<typename FPT2>
	HOMOG2D_INUMTYPE           impl_distToLine(  const Line2d_<FPT2>&, const detail::RootHelper<type::IsPoint>& ) const;
	template<typename FPT2>
	constexpr HOMOG2D_INUMTYPE impl_distToLine(  const Line2d_<FPT2>&, const detail::RootHelper<type::IsLine>&  ) const;

	HOMOG2D_INUMTYPE           impl_getAngle( const LPBase<LP,FPT>&, const detail::RootHelper<type::IsLine>&  ) const;
	constexpr HOMOG2D_INUMTYPE impl_getAngle( const LPBase<LP,FPT>&, const detail::RootHelper<type::IsPoint>& ) const;

	template<typename FPT2>
	bool           impl_isParallelTo( const LPBase<LP,FPT2>&, const detail::RootHelper<type::IsLine>&  ) const;
	template<typename FPT2>
	constexpr bool impl_isParallelTo( const LPBase<LP,FPT2>&, const detail::RootHelper<type::IsPoint>& ) const;

	FPT           impl_getCoord( GivenCoord gc, FPT other, const detail::RootHelper<type::IsLine>& ) const;
	constexpr FPT impl_getCoord( GivenCoord gc, FPT other, const detail::RootHelper<type::IsPoint>& ) const;

	Point2d_<FPT>           impl_getPoint( GivenCoord gc, FPT other, const detail::RootHelper<type::IsLine>& ) const;
	constexpr Point2d_<FPT> impl_getPoint( GivenCoord gc, FPT other, const detail::RootHelper<type::IsPoint>& ) const;

	template<typename FPT2>
	std::pair<Point2d_<FPT>,Point2d_<FPT>>           impl_getPoints_A( GivenCoord, FPT, FPT2, const detail::RootHelper<type::IsLine>& ) const;
	template<typename FPT2>
	constexpr std::pair<Point2d_<FPT>,Point2d_<FPT>> impl_getPoints_A( GivenCoord, FPT, FPT2, const detail::RootHelper<type::IsPoint>& ) const;
	template<typename FPT2>
	std::pair<Point2d_<FPT>,Point2d_<FPT>>           impl_getPoints_B( const Point2d_<FPT>&, FPT2, const detail::RootHelper<type::IsLine>& ) const;
	template<typename FPT2>
	constexpr std::pair<Point2d_<FPT>,Point2d_<FPT>> impl_getPoints_B( const Point2d_<FPT>&, FPT2, const detail::RootHelper<type::IsPoint>& ) const;

	void impl_op_stream( std::ostream&, const Point2d_<FPT>& ) const;
	void impl_op_stream( std::ostream&, const Line2d_<FPT>&  ) const;

public:
/// Line/Line intersection
	template<typename FPT2>
	detail::Intersect<detail::Inters_1,FPT> intersects( const Line2d_<FPT2>& other ) const
	{
		detail::Intersect<detail::Inters_1,FPT> out;
		if( this->isParallelTo( other ) )
			return out;
		 out.set( *this * other );
		 return out;
	}
/// Line/FRect intersection
	template<typename FPT2>
	detail::Intersect<detail::Inters_2,FPT> intersects( const Point2d_<FPT2>& pt1, const Point2d_<FPT2>& pt2 ) const
	{
		HOMOG2D_START;
		return intersects( FRect_<FPT2>( pt1, pt2 ) ) ;
	}
/// Line/FRect intersection
	template<typename FPT2>
	detail::Intersect<detail::Inters_2,FPT> intersects( const FRect_<FPT2>& rect ) const
	{
		HOMOG2D_START;
		return impl_intersectsFRect( rect, detail::RootHelper<LP>() );
	}

/// Line/Segment intersection
/** \warning no implementation for points */
	template<typename FPT2>
	detail::Intersect<detail::Inters_1,FPT> intersects( const Segment_<FPT2>& seg ) const
	{
		HOMOG2D_START;
		return seg.intersects( *this );
	}

/// Line/Circle intersection
/** <br>The Sfinae below is needed to avoid ambiguity with the other 2 args function (with 2 points defining a FRect, see above) */
	template<
		typename T,
		typename std::enable_if<
			(std::is_arithmetic<T>::value && !std::is_same<T,bool>::value)
			,T
		>::type* = nullptr
	>
	detail::Intersect<detail::Inters_2,FPT> intersects( const Point2d_<FPT>& pt0, T radius ) const
	{
		HOMOG2D_START;
		return impl_intersectsCircle( pt0, radius, detail::RootHelper<LP>() );
	}
/// Line/Circle intersection
	template<typename T>
	detail::Intersect<detail::Inters_2,FPT> intersects( const Circle_<T>& cir ) const
	{
		HOMOG2D_START;
		return impl_intersectsCircle( cir.center(), cir.radius(), detail::RootHelper<LP>() );
	}
/// Line/Polyline intersection
	template<typename PLT,typename FPT2>
	detail::IntersectM<FPT> intersects( const base::PolylineBase<PLT,FPT2>& pl ) const
	{
		HOMOG2D_START;
		return pl.intersects( *this );
	}

/// Point is inside flat rectangle
	bool isInside( const Point2d_<FPT>& pt1, const Point2d_<FPT>& pt2 ) const
	{
		return impl_isInsideRect( FRect_<FPT>(pt1, pt2), detail::RootHelper<LP>() );
	}

/// Point is inside FRect
	template<typename FPT2>
	bool isInside( const FRect_<FPT2>& rect ) const
	{
		return impl_isInsideRect( rect, detail::RootHelper<LP>() );
	}

/// Point is inside circle
	template<typename T>
	bool isInside( const Point2d_<FPT>& center, T radius ) const
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		return impl_isInsideCircle( center, radius, detail::RootHelper<LP>() );
	}
/// Point is inside Circle
	template<typename T>
	bool isInside( Circle_<T> cir ) const
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		return impl_isInsideCircle( cir.center(), cir.radius(), detail::RootHelper<type::IsPoint>() );
	}

/// Point is inside Ellipse
	template<typename FPT2>
	bool isInside( const Ellipse_<FPT2>& ell ) const
	{
		return impl_isInsideEllipse( ell, detail::RootHelper<LP>() );
	}

//////////////////////////
//       OPERATORS      //
//////////////////////////
	bool operator == ( const base::LPBase<LP,FPT>& other ) const
	{
		return impl_op_equal( other, detail::RootHelper<LP>() );
	}
	bool operator != ( const base::LPBase<LP,FPT>& other ) const
	{
		return !(*this == other);
	}
	bool operator < ( const base::LPBase<LP,FPT>& other ) const
	{
		return impl_op_sort( other, detail::RootHelper<LP>() );
	}

	template<typename T>
	bool draw( img::Image<T>& img, img::DrawParams dp=img::DrawParams() ) const;

#ifdef HOMOG2D_USE_OPENCV
	template<typename RT>
	RT getCvPt() const { return RT( getX(), getY() ); }

	cv::Point2i getCvPti() const { return impl_getCvPt( detail::RootHelper<LP>(), cv::Point2i() ); }
	cv::Point2d getCvPtd() const { return impl_getCvPt( detail::RootHelper<LP>(), cv::Point2d() ); }
	cv::Point2f getCvPtf() const { return impl_getCvPt( detail::RootHelper<LP>(), cv::Point2f() ); }

/// Constructor: build from a single OpenCv point.
	template<typename T>
	LPBase( cv::Point_<T> pt )
	{
		impl_init_opencv( pt, detail::RootHelper<LP>() );
	}
#endif // HOMOG2D_USE_OPENCV

#if 1
//	static HOMOG2D_INUMTYPE& nullDenom()          { return _zeroDenom; }

//////////////////////////
//      DATA SECTION    //
//////////////////////////

private:
	std::array<FPT,3> _v; ///< data, uses the template parameter FPT (for "Floating Point Type")

//	static HOMOG2D_INUMTYPE _zeroAngleValue;       /// Used in isParallel();
//	static HOMOG2D_INUMTYPE _zeroDistance;         /// Used to define points as identical
//	static HOMOG2D_INUMTYPE _zeroOffset;           /// Used to compare lines
//	static HOMOG2D_INUMTYPE _zeroOrthoDistance;    /// Used to check for different points on a flat rectangle, see LPBase::getCorrectPoints()
//	static HOMOG2D_INUMTYPE _zeroDenom;            /// Used to check for null denominator
#endif

//////////////////////////
//   PRIVATE FUNCTIONS  //
//////////////////////////
private:
	void p_normalizeLine() const { impl_normalizeLine( detail::RootHelper<LP>() ); }

	template<typename FPT2>
	detail::Intersect<detail::Inters_2,FPT>
	impl_intersectsFRect( const FRect_<FPT2>&, const detail::RootHelper<type::IsLine>& ) const;

	template<typename FPT2>
	constexpr detail::Intersect<detail::Inters_2,FPT>
	impl_intersectsFRect( const FRect_<FPT2>&, const detail::RootHelper<type::IsPoint>& ) const;

	template<typename T>
	detail::Intersect<detail::Inters_2,FPT>
	impl_intersectsCircle( const Point2d_<FPT>&, T r, const detail::RootHelper<type::IsLine>& ) const;

	template<typename T>
	constexpr detail::Intersect<detail::Inters_2,FPT>
	impl_intersectsCircle( const Point2d_<FPT>&, T r, const detail::RootHelper<type::IsPoint>& ) const;

	template<typename FPT2>
	bool           impl_isInsideRect( const FRect_<FPT2>&, const detail::RootHelper<type::IsPoint>& ) const;
	template<typename FPT2>
	constexpr bool impl_isInsideRect( const FRect_<FPT2>&, const detail::RootHelper<type::IsLine>&  ) const;

	template<typename FPT2>
	bool           impl_isInsideEllipse( const Ellipse_<FPT2>&, const detail::RootHelper<type::IsPoint>& ) const;
	template<typename FPT2>
	constexpr bool impl_isInsideEllipse( const Ellipse_<FPT2>&, const detail::RootHelper<type::IsLine>& ) const;

	template<typename T>
	bool           impl_isInsideCircle( const Point2d_<FPT>&, T r, const detail::RootHelper<type::IsPoint>& ) const;
	template<typename T>
	constexpr bool impl_isInsideCircle( const Point2d_<FPT>&, T r, const detail::RootHelper<type::IsLine>&  ) const;

	void impl_normalizeLine( const detail::RootHelper<type::IsLine>& ) const;

	Line2d_<FPT>           impl_getOrthogonalLine_A( GivenCoord, FPT, const detail::RootHelper<type::IsLine>&  ) const;
	constexpr Line2d_<FPT> impl_getOrthogonalLine_A( GivenCoord, FPT, const detail::RootHelper<type::IsPoint>& ) const;
	Line2d_<FPT>           impl_getOrthogonalLine_B( const Point2d_<FPT>&, const detail::RootHelper<type::IsLine>&  ) const;
	constexpr Line2d_<FPT> impl_getOrthogonalLine_B( const Point2d_<FPT>&, const detail::RootHelper<type::IsPoint>& ) const;
	Line2d_<FPT>           impl_getParallelLine( const Point2d_<FPT>&, const detail::RootHelper<type::IsLine>&  ) const;
	constexpr Line2d_<FPT> impl_getParallelLine( const Point2d_<FPT>&, const detail::RootHelper<type::IsPoint>& ) const;

	template<typename T>
	std::pair<Line2d_<FPT>,Line2d_<FPT>>           impl_getParallelLines( T, const detail::RootHelper<type::IsLine>&  ) const;
	template<typename T>
	constexpr std::pair<Line2d_<FPT>,Line2d_<FPT>> impl_getParallelLines( T, const detail::RootHelper<type::IsPoint>& ) const;

	bool impl_op_equal( const LPBase<LP,FPT>&, const detail::RootHelper<type::IsLine>&  ) const;
	bool impl_op_equal( const LPBase<LP,FPT>&, const detail::RootHelper<type::IsPoint>& ) const;

	bool           impl_op_sort( const LPBase<LP,FPT>&, const detail::RootHelper<type::IsPoint>& ) const;
	constexpr bool impl_op_sort( const LPBase<LP,FPT>&, const detail::RootHelper<type::IsLine>&  ) const;

	Point2d_<FPT> impl_op_product( const Line2d_<FPT>& , const Line2d_<FPT>& , const detail::RootHelper<type::IsPoint>& ) const;
	Line2d_<FPT>  impl_op_product( const Point2d_<FPT>&, const Point2d_<FPT>&, const detail::RootHelper<type::IsLine>&  ) const;

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
		Point2d_<FPT> p(pt);
		impl_init_1_Point<FPT>( p, detail::RootHelper<type::IsLine>() );
	}
#endif // HOMOG2D_USE_OPENCV

	template<typename T>
	bool impl_draw( img::Image<T>& img, img::DrawParams dp, const detail::RootHelper<type::IsPoint>& ) const;
	template<typename T>
	bool impl_draw( img::Image<T>& img, img::DrawParams dp, const detail::RootHelper<type::IsLine>& ) const;

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
	template<typename T1,typename T2>
	void impl_init_2( const T1&, const T2&, const detail::RootHelper<type::IsPoint>& );
	template<typename T1,typename T2>
	void impl_init_2( const T1&, const T2&, const detail::RootHelper<type::IsLine>& );

}; // class

} // namespace base

/////////////////////////////////////////////////////////////////////////////
// SECTION  - INSTANCIATION OF STATIC VARIABLES
/////////////////////////////////////////////////////////////////////////////


//template<typename LP,typename FPT>
//HOMOG2D_INUMTYPE LPBase<LP,FPT>::_zeroDenom = 1E-10;
//template<typename FPT>
//HOMOG2D_INUMTYPE detail::Matrix_<FPT>::_zeroDeterminantValue = 1E-20;
//template<typename FPT>
//HOMOG2D_INUMTYPE detail::Matrix_<FPT>::_zeroDenomValue = 1E-15;


/////////////////////////////////////////////////////////////////////////////
// SECTION  - OPENCV API CODE
/////////////////////////////////////////////////////////////////////////////

#ifdef HOMOG2D_USE_OPENCV
template<typename FPT>
FRect_<FPT> getFRect( cv::Mat& mat )
{
	if(  mat.cols == 0 || mat.rows == 0 )
		HOMOG2D_THROW_ERROR_1(
			"Illegal values: cols=" << mat.cols << ", rows=" << mat.rows
		);

	return FRect_<FPT>(
		Point2d_<FPT>(),                      // (0,0)
		Point2d_<FPT>( mat.cols, mat.rows )   // (w,h)
	);
}

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
getCvPt( const Point2d_<FPT>& pt )
{
	return pt.template getCvPt<RT>();
}

/// Free function to return an OpenCv point (double)
template<typename FPT>
cv::Point2d
getCvPtd( const Point2d_<FPT>& pt )
{
	return pt.getCvPtd();
}
/// Free function to return an OpenCv point (float)
template<typename FPT>
cv::Point2f
getCvPtf( const Point2d_<FPT>& pt )
{
	return pt.getCvPtf();
}
/// Free function to return an OpenCv point (integer)
template<typename FPT>
cv::Point2i
getCvPti( const Point2d_<FPT>& pt )
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
getCvPts( const std::vector<Point2d_<FPT>>& vpt )
{
	std::vector<RT> vout( vpt.size() );
	auto it = vout.begin();
	for( const auto& pt: vpt )
		*it++ = getCvPt<RT>(pt);
	return vout;
}
#endif // HOMOG2D_USE_OPENCV

//------------------------------------------------------------------
/// This namespace holds some private stuff
namespace detail {

//------------------------------------------------------------------
/// Private free function, returns true if point \c pt is inside the rectangle defined by (\c p00 , \c p11)
template<typename FPT1,typename FPT2>
bool
ptIsInside( const Point2d_<FPT1>& pt, const Point2d_<FPT2>& p00, const Point2d_<FPT2>& p11 )
{
	if( pt.getX() > p00.getX() && pt.getX() < p11.getX() )
		if( pt.getY() > p00.getY() && pt.getY() < p11.getY() )
			return true;
	return false;
}

#ifdef HOMOG2D_USE_EIGEN
///  Build Homography from 2 sets of 4 points, using Eigen
/**
See
- https://eigen.tuxfamily.org/dox/group__TutorialLinearAlgebra.html
- https://eigen.tuxfamily.org/dox/group__DenseMatrixManipulation__chapter.html
*/
template<typename FPT>
Homogr_<FPT>
buildFrom4Points_Eigen(
	const std::vector<Point2d_<FPT>>& vpt1, ///< source points
	const std::vector<Point2d_<FPT>>& vpt2  ///< destination points
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

	Homogr_<FPT> H;
	for( int i=0; i<8; i++ )
		H.set( i/3, i%3, X(i) );
	H.set(2, 2, 1.);

	return H;
}
#endif // HOMOG2D_USE_EIGEN

//------------------------------------------------------------------
#ifdef HOMOG2D_USE_OPENCV
///  Build Homography from 2 sets of 4 points, using Opencv
/**
- see https://docs.opencv.org/master/d9/d0c/group__calib3d.html#ga4abc2ece9fab9398f2e560d53c8c9780

\note With current Opencv installed on current machine, it seems that \c cv::getPerspectiveTransform()
requires that the points are "CV_32F" (\c float), and NOT double.
*/
template<typename FPT>
Homogr_<FPT>
buildFrom4Points_Opencv (
	const std::vector<Point2d_<FPT>>& vpt1, ///< source points
	const std::vector<Point2d_<FPT>>& vpt2  ///< destination points
)
{
	const auto& src = getCvPts<cv::Point2f>( vpt1 );
	const auto& dst = getCvPts<cv::Point2f>( vpt2 );
	return cv::getPerspectiveTransform( src, dst ); // automatic type conversion to Hmatrix_
}
#endif

} // namespace detail

//------------------------------------------------------------------
/// Build Homography from 2 sets of 4 points (free function)
/// \sa Homogr_::buildFrom4Points()
template<typename FPT>
Homogr_<FPT>
buildFrom4Points(
	const std::vector<Point2d_<FPT>>& vpt1,     ///< source points
	const std::vector<Point2d_<FPT>>& vpt2,     ///< destination points
	int                               method=1  ///< 0: Eigen, 1: Opencv
)
{
	Homogr_<FPT> H;
	H.buildFrom4Points( vpt1, vpt2, method );
	return H;
}

//------------------------------------------------------------------
/// Build Homography from 2 sets of 4 points
/**
- Requires either Eigen or Opencv
- we build a 8x8 matrix A and a 8x1 vector B, and get the solution from X = A^-1 B
- see this for details:
https://skramm.lautre.net/files/misc/Kramm_compute_H_from_4pts.pdf

\sa free function: h2d::buildFrom4Points()

\todo fix this so that user can provide a std::array of points
*/
template<typename M,typename FPT>
void
Hmatrix_<M,FPT>::buildFrom4Points(
	const std::vector<Point2d_<FPT>>& vpt1,   ///< source points
	const std::vector<Point2d_<FPT>>& vpt2,   ///< destination points
	int                               method  ///< 0: Eigen, 1: Opencv (default)
)
{
	if( vpt1.size() != 4 )
		HOMOG2D_THROW_ERROR_1( "invalid vector size for source points, should be 4, value=" << vpt1.size() );
	if( vpt2.size() != 4 )
		HOMOG2D_THROW_ERROR_1( "invalid vector size for dest points, should be 4, value=" << vpt2.size() );
	assert( method == 0 || method == 1 );

	if( method == 0 )
	{
#ifdef HOMOG2D_USE_EIGEN
		*this = detail::buildFrom4Points_Eigen( vpt1, vpt2 );
#else
		throw std::runtime_error( "Unable, build without Eigen support" );
#endif
	}
	else
	{
#ifdef HOMOG2D_USE_OPENCV
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
class Segment_: public detail::Common<FPT>
{
public:
	using FType = FPT;

	template<typename T> friend class Segment_;

private:
	Point2d_<FPT> _ptS1, _ptS2;

public:
/// \name Constructors
///@{

/// Default constructor: initializes segment to (0,0)--(1,1)
	Segment_(): _ptS2(1.,1.)
	{}
/// Constructor 2: build segment from two points
	Segment_( Point2d_<FPT> p1, Point2d_<FPT> p2 )
		: _ptS1(p1), _ptS2(p2)
	{
#ifndef HOMOG2D_NOCHECKS
		if( p1 == p2 )
			HOMOG2D_THROW_ERROR_1( "cannot build a segment with two identical points: " << p1 << " and " << p2 );
#endif
		priv::fix_order( _ptS1, _ptS2 );
	}

/// Constructor 3: build segment from two points coordinates, call constructor 2
	template<typename T>
	Segment_( T x1, T y1, T x2, T y2 )
		: Segment_( Point2d_<FPT>(x1,y1), Point2d_<FPT>(x2,y2) )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
	}

/// Constructor 4: build segment from pair of points
	Segment_( const std::pair<Point2d_<FPT>,Point2d_<FPT>>& ppts )
		: Segment_(ppts.first, ppts.second)
	{}

/// Copy-Constructor
	template<typename FPT2>
	Segment_( const Segment_<FPT2>& other )
		: _ptS1(other._ptS1), _ptS2(other._ptS2)
	{}
///@}

/// Setter
	template<typename FPT2>
	void set( const Point2d_<FPT>& p1, const Point2d_<FPT2>& p2 )
	{
#ifndef HOMOG2D_NOCHECKS
		if( p1 == p2 )
			HOMOG2D_THROW_ERROR_1( "cannot define a segment with two identical points" << p1 << " and " << p2 );
#endif
		_ptS1 = p1;
		_ptS2 = p2;
		priv::fix_order( _ptS1, _ptS2 );
	}

/// Setter from a std::pair (points need to be of same underlying type)
	template<typename FPT2>
	void set( const std::pair<Point2d_<FPT2>,Point2d_<FPT2>> ppts )
	{
		set( ppts.first, ppts.second );
	}

/// Setter from 4 raw point coordinates
	template<typename FPT2>
	void set( FPT2 x1, FPT2 y1, FPT2 x2, FPT2 y2 )
	{
		set( Point2d_<FPT2>(x1,y1), Point2d_<FPT2>(x2,y2) );
	}

	template<typename T1,typename T2>
	void translate( T1 dx, T2 dy )
	{
		HOMOG2D_CHECK_IS_NUMBER( T1 );
		HOMOG2D_CHECK_IS_NUMBER( T2 );
		_ptS1.translate( dx, dy );
		_ptS2.translate( dx, dy );
	}

/// \name Attributes access
///@{

/// Get segment length
	HOMOG2D_INUMTYPE length() const
	{
		return _ptS1.distTo( _ptS2 );
	}

/// Get angle between segment and other segment/line
/**
This will call the line angle function, thus the returned value will be
in the range \f$ [0,\pi/2] \f$
*/
	template<typename U>
	HOMOG2D_INUMTYPE getAngle( const U& other ) const
	{
		return other.getAngle( this->getLine() );
	}
///@}

/// \name Operators
///@{
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
///@}

/// Returns the points as a std::pair
/** The one with smallest x coordinate will be returned as "first". If x-coordinate are equal, then
the one with smallest y-coordinate will be returned first */
	std::pair<Point2d_<FPT>,Point2d_<FPT>>
	getPts() const
	{
		return std::make_pair( _ptS1, _ptS2 );
	}

/// Segment "isInside", S can be Circle or FRect
	template<typename S>
	bool isInside( const S& shape ) const
	{
		if( !_ptS1.isInside( shape ) )
			return false;
		if( !_ptS2.isInside( shape ) )
			return false;
		return true;
	}

/// Returns supporting line
	Line2d_<FPT> getLine() const
	{
		Point2d_<HOMOG2D_INUMTYPE> pt1( _ptS1 );
		Point2d_<HOMOG2D_INUMTYPE> pt2( _ptS2 );
		auto li = pt1 * pt2;
		return Line2d_<FPT>(li);
	}

/// \name Intersection functions
///@{
	template<typename FPT2>
	detail::Intersect<detail::Inters_1,FPT> intersects( const Segment_<FPT2>& ) const;
	template<typename FPT2>
	detail::Intersect<detail::Inters_1,FPT> intersects( const Line2d_<FPT2>&  ) const;
	template<typename FPT2>
	detail::IntersectM<FPT>                 intersects( const Circle_<FPT2>&  ) const;
/// Segment/FRect intersection
	template<typename FPT2>
	detail::IntersectM<FPT> intersects( const FRect_<FPT2>& r ) const
	{
		HOMOG2D_START;
		return r.intersects( *this );
	}

/// Segment/Polyline intersection
	template<typename PLT,typename FPT2>
	detail::IntersectM<FPT> intersects( const base::PolylineBase<PLT,FPT2>& other ) const
	{
		HOMOG2D_START;
		return other.intersects( *this );
	}
///@}

	template<typename T>
	constexpr bool
	isParallelTo( const T& other ) const
	{
		static_assert(
			std::is_same<T,Segment_<FPT>>::value ||
			std::is_same<T,Line2d_<FPT>>::value,
			"type needs to be a segment or a line" );
		return getLine().isParallelTo( other );
	}

	/// Returns point that at middle distance between \c p1 and \c p2
	Point2d_<FPT>
	getMiddlePoint() const
	{
		return Point2d_<FPT>(
			( static_cast<HOMOG2D_INUMTYPE>(_ptS1.getX()) + _ptS2.getX() ) / 2.,
			( static_cast<HOMOG2D_INUMTYPE>(_ptS1.getY()) + _ptS2.getY() ) / 2.
		);
	}

	template<typename T>
	void draw( img::Image<T>&, img::DrawParams dp=img::DrawParams() ) const;

	template<typename T>
	friend std::ostream&
	operator << ( std::ostream& f, const Segment_<T>& seg );

}; // class Segment_


/// Circle/Circle intersection
/**
Ref:
- https://stackoverflow.com/questions/3349125/

\todo benchmark the two approaches below
*/
template<typename FPT>
template<typename FPT2>
detail::Intersect<detail::Inters_2,FPT>
Circle_<FPT>::intersects( const Circle_<FPT2>& other ) const
{
	HOMOG2D_START;

	if( *this == other )
		return detail::Intersect<detail::Inters_2,FPT>();

	HOMOG2D_INUMTYPE r1 = _radius;
	HOMOG2D_INUMTYPE r2 = other._radius;
	Point2d_<HOMOG2D_INUMTYPE> pt1 = _center;
	Point2d_<HOMOG2D_INUMTYPE> pt2 = other._center;

	HOMOG2D_INUMTYPE x1 = pt1.getX();
	HOMOG2D_INUMTYPE y1 = pt1.getY();
	HOMOG2D_INUMTYPE x2 = pt2.getX();
	HOMOG2D_INUMTYPE y2 = pt2.getY();
	HOMOG2D_INUMTYPE d_squared = (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);

	if( d_squared > r1*r1 + r2*r2 + 2.*r1*r2 )              // no intersection
		return detail::Intersect<detail::Inters_2,FPT>();

	if( d_squared < ( r1*r1 + r2*r2 - 2.*r1*r2 ) )          // no intersection: one circle inside the other
		return detail::Intersect<detail::Inters_2,FPT>();

	auto d = std::sqrt( d_squared );
	auto a = (r1*r1 - r2*r2 + d_squared) / 2. / d;
	auto h = std::sqrt( r1*r1 - a*a );

	Point2d_<FPT> P0(
		( pt2.getX() - pt1.getX() ) * a / d + pt1.getX(),
		( pt2.getY() - pt1.getY() ) * a / d + pt1.getY()
	);

	Point2d_<FPT> pt3(
		P0.getX() + h*( pt1.getY() - pt2.getY() ) / d,
		P0.getY() - h*( pt1.getX() - pt2.getX() ) / d
	);
	Point2d_<FPT> pt4(
		P0.getX() - h*( pt1.getY() - pt2.getY() ) / d,
		P0.getY() + h*( pt1.getX() - pt2.getX() ) / d
	);
	return detail::Intersect<detail::Inters_2,FPT>( pt3, pt4 );
}

//------------------------------------------------------------------
namespace priv {

/// Traits class, used in generic draw() function
template<typename T> struct IsDrawable              : std::false_type {};
template<typename T> struct IsDrawable<Circle_<T>>  : std::true_type  {};
template<typename T> struct IsDrawable<FRect_<T>>   : std::true_type  {};
template<typename T> struct IsDrawable<Segment_<T>> : std::true_type  {};
template<typename T> struct IsDrawable<Line2d_<T>>  : std::true_type  {};
template<typename T> struct IsDrawable<Point2d_<T>>  : std::true_type  {};
template<typename T1,typename T2> struct IsDrawable<base::PolylineBase<T1,T2>>: std::true_type  {};

/// Traits class, used in intersects() for Polyline
template<typename T> struct IsShape              : std::false_type {};
template<typename T> struct IsShape<Circle_<T>>  : std::true_type  {};
template<typename T> struct IsShape<FRect_<T>>   : std::true_type  {};
template<typename T> struct IsShape<Segment_<T>> : std::true_type  {};
template<typename T> struct IsShape<Line2d_<T>>  : std::true_type  {};
template<typename T1,typename T2> struct IsShape<base::PolylineBase<T1,T2>>: std::true_type  {};
//template<typename T> struct IsShape<Ellipse_<T>>:  std::true_type  {};

/// Traits class used in operator * ( const Hmatrix_<type::IsHomogr,FPT>& h, const Cont& vin ),
/// used to detect if container is valid
template <typename T>               struct IsContainer: std::false_type { };
template <typename T,std::size_t N> struct IsContainer<std::array<T,N>>:     std::true_type { };
template <typename... Ts>           struct IsContainer<std::vector<Ts...>> : std::true_type { };
template <typename... Ts>           struct IsContainer<std::list<Ts...  >> : std::true_type { };

/// Traits class used to detect if container \c T is a \c std::array
/** (because allocation is different, see \ref alloc() ) */
template <typename T>
struct Is_std_array: std::false_type {};
template <typename V, size_t n>
struct Is_std_array<std::array<V, n>>: std::true_type {};


//------------------------------------------------------------------
/// A value that needs some computing, associated with its flag
template<typename T>
class ValueFlag
{
private:
	T    _value;
	bool _valIsCorrect = false;
public:
	ValueFlag() = default;
	void set( T v )
	{
		_value = v;
		_valIsCorrect = true;
	}
	T value() const    { return _value; }
	void setBad()      { _valIsCorrect = false; }
	bool isBad() const { return !_valIsCorrect; }
};


//------------------------------------------------------------------
/// Holds attribute of a Polyline, allows storage of last computed value through the use of ValueFlag
struct PolylineAttribs
{
	priv::ValueFlag<HOMOG2D_INUMTYPE> _length;
	priv::ValueFlag<HOMOG2D_INUMTYPE> _area;
	priv::ValueFlag<bool>             _isPolygon;
	priv::ValueFlag<Point2d_<HOMOG2D_INUMTYPE>> _centroid;

	void setBad()
	{
		_length.setBad();
		_area.setBad();
		_isPolygon.setBad();
		_centroid.setBad();
	}
};

//------------------------------------------------------------------
/// Returns the bounding box of points in vector/list/array \c vpts
/**
\todo This loops twice on the points. Maybe some improvement here.
*/
template<
	typename T,
	typename std::enable_if<
		priv::IsContainer<T>::value,
		T
	>::type* = nullptr
>
FRect_<typename T::value_type::FType>
getPointsBB( const T& vpts )
{
	using FPT = typename T::value_type::FType;
#ifndef HOMOG2D_NOCHECKS
	if( vpts.empty() )
		HOMOG2D_THROW_ERROR_1( "cannot get bounding box of empty set" );
#endif
	auto mm_x = std::minmax_element(
		std::begin( vpts ),
		std::end( vpts ),
		[]                  // lambda
		( const Point2d_<FPT>& pt1, const Point2d_<FPT>& pt2 )
		{
			return pt1.getX() < pt2.getX();
		}
	);
	auto mm_y = std::minmax_element(
		std::begin( vpts ),
		std::end( vpts ),
		[]                  // lambda
		( const Point2d_<FPT>& pt1, const Point2d_<FPT>& pt2 )
		{
			return pt1.getY() < pt2.getY();
		}
	);

	return FRect_<typename T::value_type::FType>(
		mm_x.first->getX(),  mm_y.first->getY(),
		mm_x.second->getX(), mm_y.second->getY()
	);
}

} // namespace priv

// Forward declaration
namespace base {
template<typename PLT,typename FPT> class PolylineBase;
}

template<typename FPT1,typename FPT2>
CPolyline_<FPT1>
operator * ( const Homogr_<FPT2>&, const FRect_<FPT1>& );

// forward declaration
template<typename T1,typename T2>
std::ostream&
operator << ( std::ostream&, const base::PolylineBase<T1,T2>& );

// Forward declaration
template<typename FPT1,typename FPT2,typename PLT2>
auto
operator * ( const Homogr_<FPT2>&, const base::PolylineBase<PLT2,FPT1>& ) -> base::PolylineBase<PLT2,FPT1>;

namespace base {

//------------------------------------------------------------------
/// Polyline, will be instanciated either as \ref OPolyline_ (open polyline) or \ref CPolyline_
/**
\warning When closed, in order to be able to compare two objects describing the same structure
but potentially in different order, the comparison operator will proceed a sorting.<br>
The consequence is that when adding points, if you have done a comparison before, you might not
add point after the one you thought!

template args:
 - PLT: PolyLine Type: type::IsClosed or type::IsOpen
 - FPT: Floating Point Type
*/
template<typename PLT,typename FPT>
class PolylineBase: public detail::Common<FPT>
{
public:
	using FType = FPT;

	template<typename T1,typename T2> friend class PolylineBase;
	template<typename T1> friend class h2d::Ellipse_;

	template<typename FPT1,typename FPT2>
	friend CPolyline_<FPT1>
	h2d::operator * ( const h2d::Homogr_<FPT2>&, const h2d::FRect_<FPT1>& );

	template<typename FPT1,typename FPT2,typename PLT2>
	friend auto
	h2d::operator * ( const Homogr_<FPT2>&, const base::PolylineBase<PLT2,FPT1>& ) -> base::PolylineBase<PLT2,FPT1>;

private:
	mutable std::vector<Point2d_<FPT>> _plinevec;
	mutable bool _plIsNormalized = false;
	mutable priv::PolylineAttribs _attribs;    ///< Attributes. Will get stored upon computing.

public:
/// \name Constructors
///@{

/// Default constructor
	PolylineBase()
	{}

/// Constructor from FRect. Enabled for "closed" type, disabled for "open" type
	template<typename FPT2>
	PolylineBase( const FRect_<FPT2>& rect )
	{
		imp_constrFRect( rect, detail::PlHelper<PLT>() );
	}

	template<typename FPT2>
	PolylineBase( const Segment_<FPT2>& seg )
	{
		const auto& ppts = seg.getPts();
		p_addPoint( ppts.first );
		p_addPoint( ppts.second );
	}

/// Constructor from a vector of points.
template<
	typename T,
	typename std::enable_if<
		priv::IsContainer<T>::value,
		T
	>::type* = nullptr
>
	PolylineBase( const T& vec )
	{
		set( vec );
	}

/// Copy-Constructor from Closed Polyline
	template<typename FPT2>
	constexpr PolylineBase( const CPolyline_<FPT2>& other )
	{
		static_assert(
			!(std::is_same<PLT,type::IsOpen>::value),
			"Error, cannot build an Open Polyline from a closed one"
		);
		set( other._plinevec );
	}
/// Copy-Constructor from Open Polyline
	template<typename FPT2>
	PolylineBase( const OPolyline_<FPT2>& other )
	{
		set( other._plinevec );
	}

///@}

private:
	template<typename FPT2>
	void imp_constrFRect( const FRect_<FPT2>& rect, const detail::PlHelper<type::IsClosed>& )
	{
		for( const auto& pt: rect.get4Pts() )
			_plinevec.push_back( pt );
	}
	template<typename FPT2>
	constexpr void
	imp_constrFRect( const FRect_<FPT2>&, const detail::PlHelper<type::IsOpen>& )
	{
		static_assert( detail::AlwaysFalse<PLT>::value, "cannot build an OPolyline object from a FRect");
	}

public:
/// \name Attributes access
///@{

/// Returns the number of points
	size_t size() const { return _plinevec.size(); }
	constexpr bool isClosed() const
	{
		return impl_isClosed( detail::PlHelper<PLT>());
	}
	HOMOG2D_INUMTYPE length()    const;
	HOMOG2D_INUMTYPE area()      const;
	bool             isPolygon() const;

/// Returns Bounding Box
	FRect_<FPT> getBB() const
	{
		return priv::getPointsBB( getPts() );
	}

	LPBase<type::IsPoint,HOMOG2D_INUMTYPE> centroid() const;

/// Returns the number of segments . If "closed",
/** the last segment (going from last to first point) is counted */
	size_t nbSegs() const
	{
		if( size() < 2 )
			return 0;
		return impl_nbSegs( detail::PlHelper<PLT>() );
	}

	bool isConvex() const;

///@}

private:
	HOMOG2D_INUMTYPE p_ComputeSignedArea() const;

	constexpr bool impl_isClosed( const detail::PlHelper<type::IsOpen>& ) const
	{
		return false;
	}
	constexpr bool impl_isClosed( const detail::PlHelper<type::IsClosed>& ) const
	{
		return true;
	}

	size_t impl_nbSegs( const detail::PlHelper<type::IsOpen>& ) const
	{
		return size() - 1;
	}
	size_t impl_nbSegs( const detail::PlHelper<type::IsClosed>& ) const
	{
		return size();
	}

	bool impl_isPolygon( const detail::PlHelper<type::IsOpen>& )   const;
	bool impl_isPolygon( const detail::PlHelper<type::IsClosed>& ) const;

/// Add single point. private, because only to be used from other member functions
/**
\warning This function was discarded from public API in dec. 2021, because
this will add the new point after the previous one \b only if the object has \b not
been normalized. This normalizing operation will happen if you do a comparison (== or !=)
*/
	template<typename FPT2>
	void p_addPoint( const Point2d_<FPT2>& pt )
	{
#ifndef HOMOG2D_NOCHECKS
		if( size() )
			if( pt == _plinevec.back() )
				HOMOG2D_THROW_ERROR_1(
					"cannot add a point identical to previous one: pt=" << pt << " size=" << size()
				);
#endif
		_attribs.setBad();
		_plIsNormalized=false;
		_plinevec.push_back( pt );
	}

public:

/// \name Data access
///@{

/// Returns the points (reference)
	std::vector<Point2d_<FPT>>& getPts()
	{
		_attribs.setBad();  // because we send the non-const reference
		_plIsNormalized=false;
		return _plinevec;
	}
/// Returns the points (const reference)
	const std::vector<Point2d_<FPT>>& getPts() const
	{
		return _plinevec;
	}

/// Returns (as a copy) the segments of the polyline
	std::vector<Segment_<FPT>> getSegs() const
	{
		if( size() < 2 ) // nothing to return
			return std::vector<Segment_<FPT>>();

		std::vector<Segment_<FPT>> out;
		out.reserve( size() );
		for( size_t i=0; i<size()-1; i++ )
		{
			const auto& pt1 = _plinevec[i];
			const auto& pt2 = _plinevec[i+1];
//			out.push_back( Segment_<FPT>(pt1,pt2) );
			out.emplace_back( Segment_<FPT>(pt1,pt2) );
		}
		impl_getSegs( out, detail::PlHelper<PLT>() );
		return out;
	}

/// Returns one point of the polyline.
	Point2d_<FPT> getPoint( size_t idx ) const
	{
#ifndef HOMOG2D_NOCHECKS
		if( idx >= size() )
			HOMOG2D_THROW_ERROR_1( "requesting point " << idx
				<< ", only has "  << size()
			);
#endif
		return _plinevec[idx];
	}

/// Returns one segment of the polyline.
/**
Segment \c n is the one between point \c n and point \c n+1
*/
	Segment_<FPT> getSegment( size_t idx ) const
	{
#ifndef HOMOG2D_NOCHECKS
		if( idx >= nbSegs() )
			HOMOG2D_THROW_ERROR_1( "requesting segment " << idx
				<< ", only has "  << nbSegs()
			);

		if( size() < 2 )
			HOMOG2D_THROW_ERROR_1( "no segment " << idx );
#endif

		return impl_getSegment( idx, detail::PlHelper<PLT>() );
	}

	CPolyline_<FPT> convexHull() const;
///@}

private:
/// empty implementation
	void impl_getSegs( std::vector<Segment_<FPT>>&, const detail::PlHelper<type::IsOpen>& ) const
	{}
/// that one is for closed Polyline, adds the last segment
	void impl_getSegs( std::vector<Segment_<FPT>>& out, const detail::PlHelper<type::IsClosed>& ) const
	{
		out.push_back( Segment_<FPT>(_plinevec.front(),_plinevec.back() ) );
	}

	Segment_<FPT>
	impl_getSegment( size_t idx, const detail::PlHelper<type::IsClosed>& ) const
	{
		return Segment_<FPT>( _plinevec[idx], _plinevec[idx+1==nbSegs()?0:idx+1] );
	}
	Segment_<FPT>
	impl_getSegment( size_t idx, const detail::PlHelper<type::IsOpen>& ) const
	{
		return Segment_<FPT>( _plinevec[idx], _plinevec[idx+1] );
	}

public:

/// \name Modifiers (non-const member functions)
///@{

/// Clear all (does not change the "open/close" status).
	void clear()
	{
		_plinevec.clear();
		_plIsNormalized=false;
		_attribs.setBad();
	}

/// Miminize the PolylineBase: remove all points that lie in the middle of two segments with same angle.
/**
For example, if we have the following points ("Open" polyline):
(0,0)-(1,0)-(2,0)<br>
This will be replaced by: (0,0)--(2,0)

\note We cannot use the Segment_::getAngle() function because it returns a
value in [0,PI/2], so we would'nt be able to detect a segment going
at 180° of the previous one.
*/
	void
	minimize()
	{
		if( size()<3 )
			return;
		impl_minimizePL( detail::PlHelper<PLT>() );
	}

/// Translate Polyline using \c dx, \c dy
	template<typename T1,typename T2>
	void translate( T1 dx, T2 dy )
	{
		HOMOG2D_CHECK_IS_NUMBER( T1 );
		HOMOG2D_CHECK_IS_NUMBER( T2 );
		for( auto& pt: _plinevec )
			pt.translate( dx, dy );
	}

/// Set from FRect
	template<typename FPT2>
	void set( const FRect_<FPT2>& rec )
	{
		impl_setFromFRect( rec, detail::PlHelper<PLT>() );
	}

/// Set from vector/array/list of points (discards previous points)
/**
- nb of elements must be 0 or 2 or more
*/
	template<
		typename CONT,
		typename std::enable_if<
			priv::IsContainer<CONT>::value,
			CONT
		>::type* = nullptr
	>
	void set( const CONT& vec )
	{
		if( vec.size() == 1 )
			HOMOG2D_THROW_ERROR_1( "Invalid: number of points must be 0, 2 or more" );

#ifndef HOMOG2D_NOCHECKS
		if( vec.size() > 1 )
			p_checkInputData( vec );
#endif

		_attribs.setBad();
		_plIsNormalized=false;
		_plinevec.resize( vec.size() );
		auto it = std::begin( _plinevec );
		for( const auto& pt: vec )   // copying one by one will
			*it++ = pt;              // allow type conversions (std::copy implies same type)
	}

///@}

private:
	template<typename FPT2>
	void impl_setFromFRect( const FRect_<FPT2>&, const detail::PlHelper<type::IsOpen>& )
	{
		static_assert( detail::AlwaysFalse<PLT>::value, "Invalid: cannot build a OPolyline from a FRect" );
	}
	template<typename FPT2>
	void impl_setFromFRect( const FRect_<FPT2>& rect, const detail::PlHelper<type::IsClosed>& )
	{
		CPolyline_<FPT> tmp(rect);
		std::swap( *this, tmp );
	}

/// Checks that no contiguous identical	points are stored
	template<typename CONT>
	void p_checkInputData( const CONT& pts )
	{
		for( auto it=pts.begin(); it!=std::prev(pts.end()); it++ )
		{
			const auto& elem = *it;
			const auto& next = *std::next(it);
			if( elem == next )
				HOMOG2D_THROW_ERROR_1(
					"cannot add two consecutive identical points:\npt:"
					<< elem << " and pt:" << next
				);
		}
	}

	void impl_minimizePL( const detail::PlHelper<type::IsOpen>& );
	void impl_minimizePL( const detail::PlHelper<type::IsClosed>& );
	void p_minimizePL( PolylineBase<PLT,FPT>&, size_t istart, size_t iend );

public:
/// \name Operators
///@{

	template<typename PLT2,typename FPT2>
	bool operator == ( const PolylineBase<PLT2,FPT2>& other ) const
	{
		if( size() != other.size() )          // for quick exit
			return false;
		return impl_operatorComp( other, detail::PlHelper<PLT>() );
	}

	template<typename PLT2,typename FPT2>
	bool operator != ( const PolylineBase<PLT2,FPT2>& other ) const
	{
		return !( *this == other );
	}
///@}

private:
/// This one is guaranteed to operate on same 'PLT' types, is called by the four others.
/**  Assumes the size is the same */
	template<typename FPT2>
	bool impl_operatorComp_root( const PolylineBase<PLT,FPT2>& other ) const
	{
		p_normalizePL();
		other.p_normalizePL();

		auto it = other._plinevec.begin();
		for( const auto& elem: _plinevec )
			if( *it++ != elem )
				return false;
		return true;
	}

	template<typename FPT2>
	bool
	impl_operatorComp( const OPolyline_<FPT2>& other, const detail::PlHelper<type::IsOpen>& ) const
	{
		return impl_operatorComp_root( other );
	}
	template<typename FPT2>
	bool
	impl_operatorComp( const CPolyline_<FPT2>& other, const detail::PlHelper<type::IsClosed>& ) const
	{
		return impl_operatorComp_root( other );
	}

/// Comparing open and closed polyline objects returns always \c false
	template<typename FPT2>
	constexpr bool
	impl_operatorComp( const OPolyline_<FPT2>&, const detail::PlHelper<type::IsClosed>& ) const
	{
		return false;
	}
/// Comparing open and closed polyline objects returns always \c false
	template<typename FPT2>
	constexpr bool
	impl_operatorComp( const CPolyline_<FPT2>&, const detail::PlHelper<type::IsOpen>& ) const
	{
		return false;
	}

public:
/// Polyline intersection with Line, Segment, FRect, Circle
	template<
		typename T,
		typename std::enable_if<
			priv::IsShape<T>::value,
			T
		>::type* = nullptr
	>
	detail::IntersectM<FPT> intersects( const T& other ) const
	{
		detail::IntersectM<FPT> out;
		for( const auto& pseg: getSegs() )
		{
			auto inters = pseg.intersects( other );
			if( inters() )
				out.add( inters.get() );
		}
		return out;
	}

	template<typename T>
	bool
	isInside( const T& cont ) const
	{
		for( const auto& pt: getPts() )
			if( !pt.isInside( cont ) )
				return false;
		return true;
	}

	template<typename T1,typename T2>
	friend std::ostream&
	h2d::operator << ( std::ostream&, const h2d::base::PolylineBase<T1,T2>& );

	template<typename T>
	void draw( img::Image<T>&, img::DrawParams dp=img::DrawParams() ) const;

private:
#ifdef HOMOG2D_USE_OPENCV
	template<typename T>
	void impl_draw_pl( img::Image<T>& ) const;
#else
	template<typename T>
	void impl_draw_pl( img::Image<T>& ) const    // this one does nothing
	{}
#endif

public:
#ifdef HOMOG2D_TEST_MODE
/// this is only needed for testing
	bool isNormalized() const { return _plIsNormalized; }
#endif

private:
	void impl_normalizePL( const detail::PlHelper<type::IsClosed>& ) const
	{
		auto minpos = std::min_element( _plinevec.begin(), _plinevec.end() );
		std::rotate( _plinevec.begin(), minpos, _plinevec.end() );
		const auto& p1 = _plinevec[1];
		const auto& p2 = _plinevec.back();
		if( p2 < p1 )
		{
			std::reverse( _plinevec.begin(), _plinevec.end() );
			minpos = std::min_element( _plinevec.begin(), _plinevec.end() );
			std::rotate( _plinevec.begin(), minpos, _plinevec.end() );
		}
	}
	void impl_normalizePL( const detail::PlHelper<type::IsOpen>& ) const
	{
		if( _plinevec.back() < _plinevec.front() )
			std::reverse( _plinevec.begin(), _plinevec.end() );
	}

/// Normalisation of CPolyline_
/**
Two tasks:
- rotating so that the smallest one is first
- reverse if needed, so that the second point is smaller than the last one
*/
	void p_normalizePL() const
	{
		if( size() == 0 )
			return;

		if( !_plIsNormalized )
		{
			impl_normalizePL( detail::PlHelper<PLT>() );
			_plIsNormalized=true;
		}
	}

}; // class Polyline_


//------------------------------------------------------------------
/// Free function, called by PolylineBase::impl_minimizePL()
template<typename PLT,typename FPT>
void
PolylineBase<PLT,FPT>::p_minimizePL( PolylineBase<PLT,FPT>& pl, size_t istart, size_t iend )
{
	auto nbpts = pl.size();
// step 1: check each point to see if it is the middle point of two segments with same angle
	std::vector<size_t> ptset;
	for( size_t i=istart; i<iend; i++ )
	{
		const auto& p0 = pl._plinevec.at(i);
		const auto& pnext = getPoint( i==nbpts-1 ? 0 : i+1 );
		const auto& pprev = getPoint( i==0 ? nbpts-1 : i-1 );

		auto vx1 = pnext.getX() - p0.getX();
		auto vy1 = pnext.getY() - p0.getY();
		auto vx2 = p0.getX() - pprev.getX();
		auto vy2 = p0.getY() - pprev.getY();
		auto a1 = std::atan2( vx1, vy1 );
		auto a2 = std::atan2( vx2, vy2 );

		if( std::abs(a1-a2) < thr::nullAngleValue() )
			ptset.push_back( i );
	}

	if( ptset.size() == 0 ) // if no same angle segment (=no "middle point")
		return;             // then no change

// step 2: build new Polyline without those points
	PolylineBase<PLT,FPT> out;
	size_t vec_idx = 0;
	for( size_t i=0; i<nbpts; i++ )
	{
//		HOMOG2D_LOG("ptset.size()=" << ptset.size() << " vec_idx=" << vec_idx );

		if( vec_idx<ptset.size() ) // if there is more points to remove
		{
			if( ptset.at(vec_idx) != i )               // if regular point, add it
				out.p_addPoint( pl._plinevec.at(i) );  //  to the output set
			else                                       // else, we found a "middle point"
				vec_idx++;                             // and switch to next one
		}
		else                                        // no more points to remove
			out.p_addPoint( pl._plinevec.at(i) );   // so just add the point to the output set
	}
	std::swap( out, pl ); // maybe we can "move" instead?
}

template<typename PLT,typename FPT>
void
PolylineBase<PLT,FPT>::impl_minimizePL( const detail::PlHelper<type::IsOpen>& )
{
	assert( size() > 2 );
	p_minimizePL( *this, 1, size()-1 );
}

template<typename PLT,typename FPT>
void
PolylineBase<PLT,FPT>::impl_minimizePL( const detail::PlHelper<type::IsClosed>& )
{
	p_minimizePL( *this, 0, size() );
}


//------------------------------------------------------------------
/// Returns true if object is a polygon (closed, and no segment crossing)
template<typename PLT,typename FPT>
bool
PolylineBase<PLT,FPT>::isPolygon() const
{
	if( size()<3 )       // needs at least 3 points to be a polygon
		return false;
	return impl_isPolygon( detail::PlHelper<PLT>() );
}

/// If open, then not a polygon
template<typename PLT,typename FPT>
bool
PolylineBase<PLT,FPT>::impl_isPolygon( const detail::PlHelper<type::IsOpen>& ) const
{
	return false;
}

/// If closed, we need to check for crossings
template<typename PLT,typename FPT>
bool
PolylineBase<PLT,FPT>::impl_isPolygon( const detail::PlHelper<type::IsClosed>& ) const
{
	if( _attribs._isPolygon.isBad() )
	{
		auto nbs = nbSegs();
		size_t i=0;
		bool notDone = true;
		bool hasIntersections = false;
		do
		{
			auto seg1 = getSegment(i);
			auto lastone = i==0?nbs-1:nbs;
			for( auto j=i+2; j<lastone; j++ )
				if( getSegment(j).intersects(seg1)() )
				{
					notDone = false;
					hasIntersections = true;
					break;
				}
			i++;
		}
		while( i<nbs && notDone );
		_attribs._isPolygon.set( !hasIntersections );
	}
	return _attribs._isPolygon.value();
}

//------------------------------------------------------------------
/// Returns true if polygon is convex
/**
This implies that:
 - the Polyline is a Polygon
 - cross product of consecutive points have same sign
 (see https://stackoverflow.com/a/1881201/193789 )
*/
template<typename PLT,typename FPT>
bool
PolylineBase<PLT,FPT>::isConvex() const
{
	if( !isPolygon() )
		return false;

	int8_t sign = 0;
	const auto& vpts = getPts();
	if( vpts.size() == 3 )   // 3 pts => always convex
		return true;

	for( size_t i=0; i<vpts.size(); i++ )
	{
		const auto& pt0 = vpts[ i==0?vpts.size()-1:i-1 ];
		const auto& pt1 = vpts[ i ];
		const auto& pt2 = vpts[ i==vpts.size()-1?0:i+1 ];
		auto dx1 = pt1.getX() - pt0.getX();
		auto dy1 = pt1.getY() - pt0.getY();

		auto dx2 = pt2.getX() - pt1.getX();
		auto dy2 = pt2.getY() - pt1.getY();

		auto crossproduct = dx1*dy2 - dy1*dx2;
		if( sign == 0 )                          // initial sign value
			sign = crossproduct>0 ? +1 : -1;
		else
			if (sign != (crossproduct>0 ? +1 : -1) )
				return false;
	}
	return true;
}

//------------------------------------------------------------------
/// Returns length
template<typename PLT,typename FPT>
HOMOG2D_INUMTYPE
PolylineBase<PLT,FPT>::length() const
{
	if( _attribs._length.isBad() )
	{
		HOMOG2D_INUMTYPE sum = 0.;
		for( const auto& seg: getSegs() )
			sum += static_cast<HOMOG2D_INUMTYPE>( seg.length() );
		_attribs._length.set( sum );
	}
	return _attribs._length.value();
}

//------------------------------------------------------------------
/// Returns area of polygon (computed only if necessary)
template<typename PLT,typename FPT>
HOMOG2D_INUMTYPE
PolylineBase<PLT,FPT>::area() const
{
	if( !isPolygon() )  // implies that is both closed and has no intersections
		return 0.;

	if( _attribs._area.isBad() )
		_attribs._area.set( std::abs( p_ComputeSignedArea() ) );
	return _attribs._area.value();
}

//------------------------------------------------------------------
/// Compute and returns signed area (used in area() and in centroid() )
template<typename PLT,typename FPT>
HOMOG2D_INUMTYPE
PolylineBase<PLT,FPT>::p_ComputeSignedArea() const
{
	HOMOG2D_INUMTYPE area = 0.;
	for( size_t i=0; i<size(); i++ )
	{
		auto j = (i == size()-1 ? 0 : i+1);
		auto pt1 = _plinevec[i];
		auto pt2 = _plinevec[j];
		area += static_cast<HOMOG2D_INUMTYPE>(pt1.getX()) * pt2.getY();
		area -= static_cast<HOMOG2D_INUMTYPE>(pt1.getY()) * pt2.getX();
	}
	return area / 2.;
}

//------------------------------------------------------------------
/// Compute centroid of polygon
/**
ref: https://en.wikipedia.org/wiki/Centroid#Of_a_polygon
*/
template<typename PLT,typename FPT>
Point2d_<HOMOG2D_INUMTYPE>
base::PolylineBase<PLT,FPT>::centroid() const
{
	if( !isPolygon() )
		HOMOG2D_THROW_ERROR_2( "PolylineBase::centroid", "unable, is not a polygon" );

	if( _attribs._centroid.isBad() )
	{
		HOMOG2D_INUMTYPE cx = 0.;
		HOMOG2D_INUMTYPE cy = 0.;
		for( size_t i=0; i<size(); i++ )
		{
			auto j = (i == size()-1 ? 0 : i+1);
			const auto& pt1 = _plinevec[i];
			const auto& pt2 = _plinevec[j];
			auto x1 = pt1.getX();
			auto x2 = pt2.getX();
			auto y1 = pt1.getY();
			auto y2 = pt2.getY();

			auto prod = x1*y2 - x2*y1;
			cx += (x1+x2) * prod;
			cy += (y1+y2) * prod;
		}
		auto signedArea = p_ComputeSignedArea();
		cx /= (6. * signedArea);
		cy /= (6. * signedArea);

		auto c = Point2d_<HOMOG2D_INUMTYPE>( cx, cy );
		_attribs._centroid.set( c );
	}
	return _attribs._centroid.value();
}

} // namespace base

//------------------------------------------------------------------
/// Returns Rectangle of the intersection area
/**
\return An object of type detail::RectArea, that can be checked for success using the ()
operator, and if success, will hold the resulting FRect_

Algorithm:

4 situations need to be considered, depending on the number of intersection points:

- A: 2 intersection points on same segment => we have 2 points inside.
\verbatim
  +------+                   +------+
  |      |                   |      |
  |   +--+-----+          +--+----+ |
  |   |  |     |          |  |    | |
  |   |  |     |    or    |  |    | |  or ...
  |   +--+-----+          +--+----+ |
  |      |                   |      |
  +------+                   +------+
\endverbatim

- B: 2 intersection points on different segments => for each rectangle, 1 point is inside the other.
\verbatim
  +------+
  |      |
  |      |
  |   +--+-----+
  |   |  |     |   or ...
  +---+--+     |
      |        |
      +--------+
\endverbatim

- C: 3 points
\verbatim
  +------+-----+
  |      |     |
  |      |     |
  +------+-----+
  |      |
  +------+
\endverbatim

- D: 4 intersection points: the intersection rectangle is made of these 4 points
\verbatim
     +------+
     |      |
  +--+------+-----+
  |  |      |     |
  |  |      |     |
  +--+------+-----+
     |      |
     +------+
\endverbatim
*/
template<typename FPT>
template<typename FPT2>
detail::RectArea<FPT>
FRect_<FPT>::intersectArea( const FRect_<FPT2>& other ) const
{
	if( *this == other )                      // if same rectangles, then
		return detail::RectArea<FPT>(other);  // the intersection area is the rectangle

	auto inter = this->intersects( other );

	if( !inter() )                        // rectangles do not intersect
		return detail::RectArea<FPT>();

	if( inter.size() < 2 )                // only one intersection point
		return detail::RectArea<FPT>();   // => no intersection area!

	if( inter.size() == 4 )  // 4 intersection points => case "D"
		return detail::RectArea<FPT>(FRect_<FPT>( inter.get().at(0), inter.get().at(3) ) );

	if( inter.size() == 3 )  // 3 intersection points => case "C"
	{
		auto v = inter.get();

		auto xmin = std::min( v[0].getX(), std::min( v[1].getX(),v[2].getX() ) );
		auto ymin = std::min( v[0].getY(), std::min( v[1].getY(),v[2].getY() ) );
		auto xmax = std::max( v[0].getX(), std::max( v[1].getX(),v[2].getX() ) );
		auto ymax = std::max( v[0].getY(), std::max( v[1].getY(),v[2].getY() ) );
#ifndef HOMOG2D_DEBUGMODE
		assert( xmax-xmin > thr::nullOrthogDistance() );
		assert( ymax-ymin > thr::nullOrthogDistance() );
#else
		HOMOG2D_DEBUG_ASSERT(
			( ( xmax-xmin > thr::nullOrthogDistance() )
			&&
			( ymax-ymin > thr::nullOrthogDistance() )) ,
			std::scientific
			<< "this=" << *this << " other=" << other
			<< "\nxmax=" << xmax << " xmin=" << xmin
			<< "\nymax=" << ymax << " ymin=" << ymin
			<< "\nnod=" << thr::nullOrthogDistance()
		);
#endif
		return detail::RectArea<FPT>( FRect_<FPT>( xmin, ymin, xmax,ymax ) );
	}
/*------------------------
 If we get to this point, we have 2 intersection points
------------------------*/
#ifndef HOMOG2D_DEBUGMODE
	assert( inter.size() == 2 );
#else
	HOMOG2D_DEBUG_ASSERT(
		inter.size() == 2,
		"inter.size()=" << inter.size() << "\n-this=" << *this << "\n-other=" << other
		<<"\n-inter:" << inter
	);
#endif
	const auto& r1 = *this;
	const auto& r2 = other;
	HOMOG2D_LOG( "r1="<<r1 << " r2="<<r2 );
	auto v1 = r1.p_pointsInside( r2 );
	auto v2 = r2.p_pointsInside( r1 );
	auto c1 = v1.size();
	auto c2 = v2.size();
	HOMOG2D_LOG( "c1=" << c1 << " c2=" << c2 );

	if( c1==0 && c2==0 ) // unable, rectangles share a segment
		return detail::RectArea<FPT>();

	assert(
		( c1==1 && c2==1 )
		||
		( c1==0 && c2==2 )
		||
		( c2==0 && c1==2 )
	);
	if( c1==1 || c2==1 ) // only 1 point inside => the rectangle is defined by the intersection points
		return detail::RectArea<FPT>( FRect_<FPT>( inter.get().at(0), inter.get().at(1) ) );

// here: 2 points inside, then build rectangle using the 4 points:
// the 2 inside, and the two intersection points
	assert( c1 == 2 || c2 == 2 );

	if( c1 == 2 )
		return detail::RectArea<FPT>(
			FRect_<FPT>(
				inter.get().at(0),
				inter.get().at(1),
				v1.at(0),
				v1.at(1)
			)
		);
	return detail::RectArea<FPT>(
		FRect_<FPT>(
			inter.get().at(0),
			inter.get().at(1),
			v2.at(0),
			v2.at(1)
		)
	);
}

namespace priv {
/// Common stuff for FRect_ union, see FRect_::unionArea()
namespace runion {
//------------------------------------------------------------------
/// Holds x or y coordinate value of rectangle during computation of intersection area
/**
- \c T will be the floating-point type
- used in FRect_<FPT>::unionArea()
*/
template<typename T>
struct Index
{
	T       value;
	uint8_t rect_idx=0;   // 0 means none, will be 1 or 2

	Index() = default;
	Index( T v, uint8_t r )
		: value(v), rect_idx(r)
	{}
	friend bool operator < ( const Index& i1, const Index& i2 )
	{

		if( i1.value == i2.value )
			return i1.rect_idx < i2.rect_idx;
		return i1.value < i2.value;
	}
	friend std::ostream& operator << ( std::ostream& f, const Index& idx )
	{
		f << idx.value << " ";
		return f;
	}
};

/// A wrapper around a bool, that gets set if that point defined
/// by indexes \c ix and \c iy is a corner in the table
/**
used in FRect_<FPT>::unionArea()
*/
struct Cell
{
	bool isCorner = false;
	Cell() = default;
#ifdef HOMOG2D_DEBUGMODE
	Cell( bool b ) : isCorner(b)
	{}
#endif
	template<typename T>
	Cell( const Index<T>& ix, const Index<T>& iy )
	{
		if( ix.rect_idx == iy.rect_idx )
			isCorner = true;
	}
};


using Table  = std::array<std::array<Cell,4>,4>;
using PCoord = std::pair<uint8_t,uint8_t>;

enum class Direction: uint8_t { N, E, S, W };

/* NEEDED TO DEBUG
const char* getString( Direction dir )
{
	switch( dir )
	{
		case Direction::W: return "W"; break;
		case Direction::S: return "S"; break;
		case Direction::E: return "E"; break;
		case Direction::N: return "N"; break;
	}
	return 0; // to avoid a warning
}*/

enum class Turn: uint8_t { Left, Right };

Direction turn( Direction dir, Turn turn )
{
	switch( dir )
	{
		case Direction::W:
			return turn == Turn::Left ? Direction::S : Direction::N;
		case Direction::S:
			return turn == Turn::Left ? Direction::E : Direction::W;
		case Direction::E:
			return turn == Turn::Left ? Direction::N : Direction::S;
		case Direction::N:
			return turn == Turn::Left ? Direction::W : Direction::E;
	}
	return Direction::N; // to avoid a warning
}

void
moveToNextCell( uint8_t& row, uint8_t& col, const Direction& dir )
{
	switch( dir )
	{
		case Direction::N: row--; break;
		case Direction::S: row++; break;
		case Direction::E: col++; break;
		case Direction::W: col--; break;
	}
}

/// Helper function for FRect_<FPT>::unionArea()
/**
- Start from 0,0, direction East
*/
std::vector<PCoord>
parseTable( Table& table)
{
	bool firstTime = true;
	bool done = false;
	uint8_t row = 0;
	uint8_t col = 0;
	Direction dir = Direction::E;
	std::vector<PCoord> out;
	do
	{
		if( table[row][col].isCorner )
		{
			auto new_pair = std::make_pair(row,col);
			if( out.size() > 0 )
				if( new_pair == out.front() && out.size() > 2 )
					done = true;
			if( !done )
				out.push_back( new_pair );
			if( firstTime )
				firstTime = false;
			else
				dir = turn( dir, Turn::Right );
		}
		else
		{
			if( ( row==2 || row==1 ) && ( col==2 || col==1 ) )
			{
				out.push_back( std::make_pair(row,col) );
				dir = turn( dir, Turn::Left );
			}
		}
		moveToNextCell( row, col, dir );
	}
	while( !done );
	return out;
}

#ifdef HOMOG2D_DEBUGMODE
void
printTable( const Table& t, std::string msg )
{
	std::cout << "Table: " << msg << "\n  | ";
	for( uint8_t r=0;r<4; r++ )
		std::cout << (int)r << " ";
	std::cout << "\n--|---------|\n";
	for( uint8_t r=0;r<4; r++ )
	{
		std::cout << (int)r << " | ";
		for( uint8_t c=0;c<4; c++ )
			std::cout << (t[r][c].isCorner?'F':'.') << " ";
		std::cout << "|\n";
	}
}
#endif

//------------------------------------------------------------------
/// Helper function, used in FRect_<FPT>::unionArea()
template<typename FPT>
CPolyline_<FPT>
convertToCoord(
	const std::vector<PCoord>&      v_coord, ///< vector of coordinate indexes
	const std::array<Index<FPT>,4>& vx,      ///< holds x-coordinates
	const std::array<Index<FPT>,4>& vy       ///< holds y-coordinates
)
{
	std::vector<Point2d_<FPT>> v_pts;
	for( const auto& elem: v_coord )
	{
		auto id_x = elem.first;
		auto id_y = elem.second;
		assert( id_x<4 && id_y<4 );

		auto pt = Point2d_<FPT>( vx[id_x].value, vy[id_y].value );
		if( v_pts.empty() )
			v_pts.push_back( pt );
		else                             // add to vector only  if not same as previous
		{                                // and not same as first
			if( v_pts.back() != pt && v_pts.front() != pt )
				v_pts.push_back( pt );
		}
	}
//	printVector( v_pts, "polyline" );
	return CPolyline_<FPT>( v_pts );//, IsClosed::Yes );
}

} // namespace priv
} // namespace runion
//------------------------------------------------------------------
/// Computes the CPolyline of the union of two rectangles
/**
Algorithm:
 -# build vectors of x and y coordinates (4 elements)
 -# build table x-y (4x4), with corners tagged
 -# parse the table by turning right at each corner, and left if position is not one the outside row/col
 -# convert back indexes to real coordinates, to build the final CPolyline object

Two examples:
\verbatim
9      +----+              +-------+
       |    |              |       |
8  +---+----+---+      +---+---+   |
   |   |    |   |      |   |   |   |
7  +---+----+---+      +---+---+   |
       |    |              |       |
6      +----+              +-------+
   1   2    3   4      1   2   3   4
\endverbatim
Step 1 will build (for both situations above):
 - vx: 1,2,3,4
 - vy: 6,7,8,9

 Step 2 will build a table showing where the corners are:
\verbatim
  | 0 1 2 3             | 0 1 2 3
--|---------|         --|---------|
0 | . F F . |          0| . F . F
1 | F F F F |          1| F F . .
2 | F F F F |          2| F F . .
3 | . F F . |          3| . F . F
\endverbatim

Step 3: parse that table and turn on each corner:
- left if "inner" corner (row and col = 1 or = 2)
- right if "outer" corner (row and col = 0 or = 3)

Final step: convert indexes to real coordinates

Special note: if the rectangles have an identical coordinate, as in this example:
\verbatim
9      +----+
       |    |
8  +---+----+
   |   |    |
7  +---+----+
       |    |
6      +----+
   1   2    3
\endverbatim
Then the vectors are:
- vx: 1,2,3,3
- vy: 6,7,8,9

(notice the duped coordinate)

This will produce a Polyline_ with 2 extra points:<br>
(1,7)-(1,8)-(2,8)-(2,9)-(3,9)-(3,8)-(3,7)-(3,6)-(2,6)-(2,7)
<br>instead of:<br>
(1,7)-(1,8)-(2,8)-(2,9)-(3,9)-(3,6)-(2,6)-(2,7)
<br>We solve this by proceeding an extra Polyline minimization,
see PolylineBase::minimize()
*/
template<typename FPT>
template<typename FPT2>
CPolyline_<FPT>
FRect_<FPT>::unionArea( const FRect_<FPT2>& other ) const
{
	using namespace priv::runion;

	if( *this == other )                  // if same rectangles, then
		return CPolyline_<FPT>( other );   // returns one of them

	if( !this->intersects(other)() )  // if no intersection,
		return CPolyline_<FPT>();      // return empty polygon

/* step 0: make sure the rect with highest x is first.
 This is needed to avoid this kind of situation in table:

  F F . .
  . . F F
  . . F F
  F F . .

  That would happen if we have an identical x in the two rect
  (thus,they DO intersect), because when sorting, the rect with smallest index
  (1 or 2) is placed first in the vector of coordinates */
	const auto* pr1 = this;
	const auto* pr2 = &other;
	if( pr1->getPts().first.getX() < pr2->getPts().first.getX() )
		std::swap( pr1, pr2 );
	const auto& r1 = *pr1;
	const auto& r2 = *pr2;

// step 1: build vectors of coordinates and sort them
	std::array<Index<FPT>,4> vx, vy;
	int i=0;
	vx[i++] = Index<FPT>( r1.getPts().first.getX(),  1 );
	vx[i++] = Index<FPT>( r1.getPts().second.getX(), 1 );
	vx[i++] = Index<FPT>( r2.getPts().first.getX(),  2 );
	vx[i++] = Index<FPT>( r2.getPts().second.getX(), 2 );

	i=0;
	vy[i++] = Index<FPT>( r1.getPts().first.getY(),  1 );
	vy[i++] = Index<FPT>( r1.getPts().second.getY(), 1 );
	vy[i++] = Index<FPT>( r2.getPts().first.getY(),  2 );
	vy[i++] = Index<FPT>( r2.getPts().second.getY(), 2 );

	std::sort( vx.begin(), vx.end() );
	std::sort( vy.begin(), vy.end() );
//	priv::printArray( vx, "vx" ); priv::printArray( vy, "vy");

// step 2: fill table\n";
	Table table;
	for( uint8_t r=0;r<4; r++ )
		for( uint8_t c=0;c<4; c++ )
			table[r][c] = Cell( vx[r], vy[c] );
//	printTable( table, "after step 2" );

// step 3: parse table
	auto vpts = parseTable( table );
//	priv::printVectorPairs( vpts );

// step 4: convert back vector of coordinates indexes into vector of coordinates
	auto res1 = convertToCoord( vpts, vx, vy );
	res1.minimize(); // remove unecessary points
	return res1;
}

//------------------------------------------------------------------
namespace detail {

/// Used in isBetween() and isInArea()
enum class Rounding: uint8_t { Yes, No };

#if 0
/// Helper function
template<typename T1,typename T2>
bool
isBetween( T1 v, T2 v1, T2 v2 )
{
	HOMOG2D_CHECK_IS_NUMBER(T1);
	HOMOG2D_CHECK_IS_NUMBER(T2);
	if( v >= std::min( v1, v2 ) )
		if( v <= std::max( v1, v2 ) )
			return true;
	return false;
}

/// Does some small rounding (if requested), to avoid some numerical issues
/**
Edit 20220425: no need for llroundl(), check https://en.cppreference.com/w/cpp/numeric/math/round
*/
template<typename FPT>
long double
doRounding( FPT value, Rounding r )
{
	if( r == Rounding::No )
		return value;
	HOMOG2D_INUMTYPE coeff = HOMOG2D_ROUNDING_COEFF;
//	return std::llroundl( value * coeff ) / coeff;
	return std::round( value * coeff ) / coeff;
}

/// Helper function, checks if \c pt is in the area defined by \c pt1 and \c pt2
template<typename T1,typename T2>
bool
isInArea(
	const Point2d_<T1>& pt,    ///< the considered point
	const Point2d_<T2>& pt1,   ///< pt1
	const Point2d_<T2>& pt2,   ///< pt2
	Rounding r = Rounding::No  ///< default behavior is "no rounding"
)
{
	HOMOG2D_START;
	if( isBetween( doRounding(pt.getX(), r), pt1.getX(), pt2.getX() ) )
		if( isBetween( doRounding(pt.getY(), r), pt1.getY(), pt2.getY() ) )
			return true;
	return false;
}
#endif // 0

//------------------------------------------------------------------
/// See getPtLabel( const Point2d_<FPT>& pt, const Circle_<FPT2>& circle )
enum class PtTag: uint8_t
{
	Inside, Outside, OnEdge
};

/// Returns a label characterizing point \c pt, related to \c circle: inside, outside, or on edge of circle
template<typename FPT,typename FPT2>
PtTag
getPtLabel( const Point2d_<FPT>& pt, const Circle_<FPT2>& circle )
{
	if( pt.isInside( circle ) )
		return PtTag::Inside;
	if(
		std::abs( pt.distTo( circle.center() ) - circle.radius() )
			< thr::nullDistance()
	)
		return PtTag::OnEdge;
	return PtTag::Outside;
}
#if 0
/// Debug, can be removed after
void
printTag( std::string txt, PtTag tag )
{
	std::cout << "point " << txt << ": ";
	switch( tag )
	{
		case PtTag::Inside: std::cout << "Inside\n"; break;
		case PtTag::Outside: std::cout << "Outside\n"; break;
		case PtTag::OnEdge: std::cout << "OnEdge\n"; break;
	}
}
#endif
} // namespace detail

//------------------------------------------------------------------
/// Segment/Segment intersection
/**
Algorithm:<br>
- first compute the intersection point
- then check if the intersection point lies in between the range of both segments, both on x and on y
*/
template<typename FPT>
template<typename FPT2>
detail::Intersect<detail::Inters_1,FPT>
Segment_<FPT>::intersects( const Segment_<FPT2>& s2 ) const
{
	HOMOG2D_START;
	if( *this == s2 )              // same segment => no intersection
		return detail::Intersect<detail::Inters_1,FPT>();

	Line2d_<HOMOG2D_INUMTYPE> l1 = getLine();
	Line2d_<HOMOG2D_INUMTYPE> l2 = s2.getLine();
	if( l1.isParallelTo( l2 ) )                                // if parallel,
			return detail::Intersect<detail::Inters_1,FPT>();  // then, no intersection

	const auto& ptA1 = getPts().first;
	const auto& ptA2 = getPts().second;
	const auto& ptB1 = s2.getPts().first;
	const auto& ptB2 = s2.getPts().second;

	auto ptInter = l1 * l2;   // intersection point

	auto dA1 = ptA1.distTo( ptInter );
	auto dA2 = ptA2.distTo( ptInter );
	if( std::abs(dA1+dA2 - length()) < thr::nullDistance() )
	{
		auto dB1 = ptB1.distTo( ptInter );
		auto dB2 = ptB2.distTo( ptInter );
		if( std::abs(dB1+dB2 - s2.length()) < thr::nullDistance() )
			return detail::Intersect<detail::Inters_1,FPT>( ptInter );
	}
	return detail::Intersect<detail::Inters_1,FPT>(); // no intersection
}

//------------------------------------------------------------------
/// Segment/Line intersection
/**
Algorithm:<br>
We check if the intersection point lies in between the range of the segment, both on x and on y
*/
template<typename FPT>
template<typename FPT2>
detail::Intersect<detail::Inters_1,FPT>
Segment_<FPT>::intersects( const Line2d_<FPT2>& li1 ) const
{
	HOMOG2D_START;
//	HOMOG2D_LOG( "seg=" << *this << " line=" << li1 );
	detail::Intersect<detail::Inters_1,FPT> out;
	auto li2 = getLine();

	if( li1.isParallelTo( li2 ) ) // if parallel, no intersection
		return out;

	out._ptIntersect = li1 * li2;   // intersection point

	const auto& pi  = out._ptIntersect;
	const auto& pt1 = getPts().first;
	const auto& pt2 = getPts().second;

	auto d1 = pt1.distTo( pi );
	auto d2 = pt2.distTo( pi );
	if( std::abs(d1+d2-length()) < thr::nullDistance() )
 		out._doesIntersect = true;

	return out;
}

//------------------------------------------------------------------
/// Segment/Circle intersection
/**
For each point of the segment, we need to consider 3 different situations
 - point is inside (PI)
 - point is outside (PO)
 - point is on the edge (PE)

That makes 6 different situations to handle:

 - S1: PI-PI => no intersection
 - S2: PI-PO => 1 intersection
 - S3: PI-PE => 1 intersection
 - S4: PO-PO => depends on the support line:
  - S4A: if line does NOT intersects circle, no intersection pts
  - S4B: if line does intersects circle, and intersections point in segment area => 2 intersection pts
  - S4C: if line does intersects circle, and intersections point NOT in segment area => no intersection pts
 - S5: PO-PE => 1 intersection
 - S6: PE-PE => 2 intersections
*/
template<typename FPT>
template<typename FPT2>
detail::IntersectM<FPT>
Segment_<FPT>::intersects( const Circle_<FPT2>& circle ) const
{
	HOMOG2D_START;
	using detail::PtTag;

	auto tag_ptS1 = detail::getPtLabel( _ptS1, circle ); // get status of segment points related to circle (inside/outside/on-edge)
	auto tag_ptS2 = detail::getPtLabel( _ptS2, circle );

	if( tag_ptS1 == PtTag::Inside )
		if( tag_ptS2 == PtTag::Inside )
			return detail::IntersectM<FPT>();

	auto int_lc = getLine().intersects( circle );
	if( !int_lc() )
		return detail::IntersectM<FPT>();

	auto p_pts = int_lc.get();      // get the line intersection points
	const auto& p1 = p_pts.first;
	const auto& p2 = p_pts.second;

	if(                                                            // one inside, the other outside
		( tag_ptS1 == PtTag::Inside  && tag_ptS2 == PtTag::Outside )
		||
		( tag_ptS1 == PtTag::Outside && tag_ptS2 == PtTag::Inside )
	)
	{
		detail::IntersectM<FPT> out;

		auto d1 = _ptS1.distTo( p1 );
		auto d2 = _ptS2.distTo( p1 );
		if( std::abs( d1+d2-length() ) < thr::nullDistance() )
			out.add( p1 );                          // points is inside
		else
			out.add( p2 );
		return out;
	}

	detail::IntersectM<FPT> out;
	if( tag_ptS1 == PtTag::Outside && tag_ptS2 == PtTag::Outside ) // both outside
	{
		auto d1 = _ptS1.distTo( p1 );
		auto d2 = _ptS2.distTo( p1 );
		if( std::abs( d1+d2-length() ) >= thr::nullDistance() )   // if sum of the two distances is higher than length
			return detail::IntersectM<FPT>();

		out.add( p1 );
		out.add( p2 );
		return out;
	}

	if( tag_ptS1 == PtTag::OnEdge )
		out.add( _ptS1 );
	if( tag_ptS2 == PtTag::OnEdge )
		out.add( _ptS2 );

	return out;
}

/////////////////////////////////////////////////////////////////////////////
// SECTION  - STREAMING OPERATORS
/////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------
/// Overload for points
template<typename LP,typename FPT>
void
base::LPBase<LP,FPT>::impl_op_stream( std::ostream& f, const Point2d_<FPT>& r ) const
{
	f
//	<< std::scientific << std::setprecision(25)
	 << '[' << r.getX() << ',' << r.getY() << "]";
}

/// Overload for lines
template<typename LP,typename FPT>
void
base::LPBase<LP,FPT>::impl_op_stream( std::ostream& f, const Line2d_<FPT>& r ) const
{
	f << '[' << r._v[0] << ',' << r._v[1] << ',' << r._v[2] << "]";
}

namespace base {
/// Stream operator, free function, call member function pseudo operator impl_op_stream()
template<typename LP,typename FPT>
std::ostream&
operator << ( std::ostream& f, const h2d::base::LPBase<LP,FPT>& r )
{
	r.impl_op_stream( f, r );
	return f;
}
}

/// Stream operator for a container of points/lines, free function
template<
	typename T,
	typename std::enable_if<
		priv::IsContainer<T>::value,
		T
	>::type* = nullptr
>
std::ostream&
operator << ( std::ostream& f, const T& vec )
{
	f << "#="<< vec.size() << '\n';
	for( const auto& elem: vec )
		f << elem << '\n';
	return f;
}

/// Stream operator for a pair of points/lines, free function
template<typename LP1,typename LP2,typename FPT>
std::ostream&
operator << ( std::ostream& f, const std::pair<base::LPBase<LP1,FPT>,base::LPBase<LP2,FPT>>& pr )
{
	f << "std::pair (" << getString(pr.first.type()) << "-" << getString(pr.second.type())
		<< "):\n -first="  << pr.first
		<< "\n -second=" << pr.second
		<< ' ';
	return f;
}

template<typename T>
std::ostream&
operator << ( std::ostream& f, const FRect_<T>& r )
{
	f << "pt1: " << r._ptR1 << " pt2: " << r._ptR2;
	return f;
}

template<typename T>
std::ostream&
operator << ( std::ostream& f, const Circle_<T>& r )
{
	f << "center: " << r._center << ", radius=" << r._radius;
	return f;
}

template<typename T>
std::ostream&
operator << ( std::ostream& f, const Segment_<T>& seg )
{
	f << seg._ptS1 << "-" << seg._ptS2;
	return f;
}

template<typename PLT,typename FPT>
std::ostream&
operator << ( std::ostream& f, const h2d::base::PolylineBase<PLT,FPT>& pl )
{
	f << (pl.isClosed() ? 'C' : 'O' ) << "Polyline: ";
	if( !pl.size() )
		f << "empty";
	else
	{
		for( const auto& pt: pl._plinevec )
			f << pt << "-";
	}
	f << '\n';
	return f;
}

template<typename T>
std::ostream&
operator << ( std::ostream& f, const Ellipse_<T>& ell )
{
	auto par = ell.template p_getParams<HOMOG2D_INUMTYPE>();
	f << par;
	return f;
}

//------------------------------------------------------------------
/// Returns the 2 parallel lines at distance \c dist from \c li (free function)
template<typename FPT,typename T>
std::pair<Line2d_<FPT>,Line2d_<FPT>>
getParallelLines( const Line2d_<FPT>& li, T dist )
{
	return li.getParallelLines( dist );
}

/////////////////////////////////////////////////////////////////////////////
// SECTION  - MEMBER FUNCTION IMPLEMENTATION: CLASS Ellipse_
/////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------
/// Returns standard parameters from matrix coeffs
template<typename FPT>
template<typename T>
detail::EllParams<T>
Ellipse_<FPT>::p_getParams() const
{
#ifdef HOMOG2D_OPTIMIZE_SPEED
	if( _epHasChanged )
	{
		_par = p_computeParams<T>();
		_epHasChanged = false;
	}
	return _par;
#else
	return p_computeParams<T>();
#endif // HOMOG2D_OPTIMIZE_SPEED
}
//------------------------------------------------------------------
/// Compute and returns standard parameters from matrix coeffs
/**
\note In the checking below, we **cannot** print the matrix using the
<< operator, because it call this function, thus it would enter
an infinite loop (an eventually SO).
*/
template<typename FPT>
template<typename T>
detail::EllParams<T>
Ellipse_<FPT>::p_computeParams() const
{
	const auto& m = detail::Matrix_<FPT>::_mdata;
	HOMOG2D_INUMTYPE A = m[0][0];
	HOMOG2D_INUMTYPE C = m[1][1];
	HOMOG2D_INUMTYPE F = m[2][2];
	HOMOG2D_INUMTYPE B = 2. * m[0][1];
	HOMOG2D_INUMTYPE D = 2. * m[0][2];
	HOMOG2D_INUMTYPE E = 2. * m[1][2];

	detail::EllParams<T> par;  // theta already set to zero

	auto denom = B*B - 4. * A * C;

#ifndef HOMOG2D_NOCHECKS
	if( std::abs(denom) < thr::nullDenom() )
		HOMOG2D_THROW_ERROR_1(
			"unable to compute parameters, denom=" << std::scientific << std::setprecision(15) << denom
		);
#endif

	par.x0 = ( 2.*C*D - B*E ) / denom;
	par.y0 = ( 2.*A*E - B*D ) / denom;
	auto common_ab = 2. * ( A*E*E + C*D*D - B*D*E + denom*F );
	auto AmC = A-C;
	auto AmC2 = AmC*AmC;
	auto sqr = std::sqrt(AmC2+B*B);
	par.a = -std::sqrt( common_ab * ( A+C+sqr ) )/ denom;
	par.b = -std::sqrt( common_ab * ( A+C-sqr ) )/ denom;

	par.a2 = par.a * par.a;
	par.b2 = par.b * par.b;
	if( std::abs(B) < thr::nullDenom() )
	{
		if( A > C )
			par.theta = 90.;
	}
	else
	{
		auto t = (C - A - sqr) / B;
		par.theta = std::atan( t );
	}
	par.sint = std::sin( par.theta );
	par.cost = std::cos( par.theta );
	return par;
}

//------------------------------------------------------------------
/// Returns true if ellipse is a circle
/**
Using the matrix represention, if A = C and B = 0, then the ellipse is a circle

You can provide the 0 threshold as and argument
*/
template<typename FPT>
bool
Ellipse_<FPT>::isCircle( HOMOG2D_INUMTYPE thres ) const
{
	const auto& m = detail::Matrix_<FPT>::_mdata;
	HOMOG2D_INUMTYPE A  = m[0][0];
	HOMOG2D_INUMTYPE C  = m[1][1];
	HOMOG2D_INUMTYPE B2 = m[0][1];
	if( std::abs(A-C) < thres )
		if( std::abs(B2)*2. < thres )
			return true;
	return false;
}

/// Returns center of ellipse
/// \sa center( const T& )
template<typename FPT>
Point2d_<FPT>
Ellipse_<FPT>::center() const
{
	auto par = p_getParams<HOMOG2D_INUMTYPE>();
	return Point2d_<FPT>( par.x0, par.y0 );
}

template<typename FPT>
std::pair<HOMOG2D_INUMTYPE,HOMOG2D_INUMTYPE>
Ellipse_<FPT>::getMajMin() const
{
	auto par = p_getParams<HOMOG2D_INUMTYPE>();
	return std::make_pair( par.a, par.b );
}

/// Returns angle of ellipse
/// \sa angle( const Ellipse_& )
template<typename FPT>
HOMOG2D_INUMTYPE
Ellipse_<FPT>::angle() const
{
	auto par = p_getParams<HOMOG2D_INUMTYPE>();
	return par.theta;
}

//------------------------------------------------------------------
/// Returns pair of axis lines of ellipse
template<typename FPT>
std::pair<Line2d_<FPT>,Line2d_<FPT>>
Ellipse_<FPT>::getAxisLines() const
{
	auto par = p_getParams<HOMOG2D_INUMTYPE>();
	auto dy = static_cast<HOMOG2D_INUMTYPE>(par.sint) * par.a;
	auto dx = static_cast<HOMOG2D_INUMTYPE>(par.cost) * par.a;
	Point2d_<HOMOG2D_INUMTYPE> ptA(
		par.x0 + dx,
		par.y0 + dy
	);
	auto pt0 = Point2d_<HOMOG2D_INUMTYPE>( par.x0, par.y0 );
	auto li_H = ptA * pt0;
	HOMOG2D_LOG( std::scientific << "ptA=" << ptA << " pt0=" << pt0 << " li_H=" << li_H );

	auto li_V = li_H.getOrthogonalLine( pt0 );
	return std::make_pair( li_H, li_V );
}

//------------------------------------------------------------------
/// Returns axis-aligned bounding box of ellipse
/**
- see https://math.stackexchange.com/questions/91132/how-to-get-the-limits-of-rotated-ellipse
*/
template<typename FPT>
FRect_<FPT>
Ellipse_<FPT>::getBB() const
{
	auto par = p_getParams<HOMOG2D_INUMTYPE>();
	auto vx = par.a2 * par.cost * par.cost	+ par.b2 * par.sint * par.sint;
	auto vy = par.a2 * par.sint * par.sint	+ par.b2 * par.cost * par.cost;
	auto vx_sq = std::sqrt( vx );
	auto vy_sq = std::sqrt( vy );
	return FRect_<FPT>(
		Point2d_<FPT>( par.x0 - vx_sq, par.y0 - vy_sq ),
		Point2d_<FPT>( par.x0 + vx_sq, par.y0 + vy_sq )
	);
}

//------------------------------------------------------------------
/// Returns oriented bounding box of ellipse as a closed Polyline
/**
Algorithm:
 - build line \c liH going through major axis, by using center point and
 point on semi-major axis, intersecting ellipse
 - get opposite point \c ptB, lying on line and at distance \c a
 - get the two parallel lines to \c liH, at a distance \c b
 - get the two orthogonal lines at \c ptA and \c ptB

*/
template<typename FPT>
CPolyline_<FPT>
Ellipse_<FPT>::getOBB() const
{
// step 1: build ptA using angle
	auto par = p_getParams<HOMOG2D_INUMTYPE>();
	auto dy = par.sint * par.a;
	auto dx = par.cost * par.a;
	Point2d_<FPT> ptA(
		par.x0 + dx,
		par.y0 + dy
	);
	auto pt0 = Point2d_<HOMOG2D_INUMTYPE>( par.x0, par.y0 );

// step 2: build main-axis line, going through center and ptA
	auto li_H = ptA * pt0;

// step 3: get ptB, using line and distance
	auto ppts = li_H.getPoints( pt0, par.a );
	auto ptB = ppts.first;
	if( ptB == ptA )
		ptB = ppts.second;

	auto para = getParallelLines( li_H, par.b );
	auto li_V1 = li_H.getOrthogonalLine( ptA );
	auto li_V2 = li_H.getOrthogonalLine( ptB );

	CPolyline_<FPT> out;
#ifndef	HOMOG2D_DEBUGMODE
	out.p_addPoint( para.first  * li_V1 );
	out.p_addPoint( para.second * li_V1 );
	out.p_addPoint( para.second * li_V2 );
	out.p_addPoint( para.first  * li_V2 );
#else
	auto p1 = para.first  * li_V1;
	auto p2 = para.second * li_V1;
	auto p3 = para.second * li_V2;
	auto p4 = para.first  * li_V2;
	HOMOG2D_DEBUG_ASSERT(
		( p2!=p1 && p3!=p2 && p4!=p3 ),
		"p1=" << p1 << " p2=" << p2 << " p3=" << p3 << " p4=" << p4
		<< "\n para.1=" << para.first
		<< "\n li_V1=" << li_V1
		<< "\n ptA=" << ptA
		<< "\n ptB=" << ptB
		<< "\n " << ppts
	);
	out.p_addPoint( p1 );
	out.p_addPoint( p2 );
	out.p_addPoint( p3 );
	out.p_addPoint( p4 );
#endif
	return out;
}

//------------------------------------------------------------------
/// Returns true if point is inside ellipse
/**
taken from https://stackoverflow.com/a/16814494/193789
*/
template<typename FPT>
template<typename FPT2>
bool
Ellipse_<FPT>::pointIsInside( const Point2d_<FPT2>& pt ) const
{
	HOMOG2D_INUMTYPE x = pt.getX();
	HOMOG2D_INUMTYPE y = pt.getY();

	auto par = p_getParams<HOMOG2D_INUMTYPE>();
	const auto& x0 = par.x0;
	const auto& y0 = par.y0;

	auto v1 = par.cost * (x-x0) + par.sint * (y-y0);
	HOMOG2D_INUMTYPE sum = v1*v1 / par.a2;

	auto v2 = par.sint * (x-x0) - par.cost * (y-y0);
	sum += v2*v2 / par.b2;
	if( sum < 1. )
		return true;
	return false;

}

/////////////////////////////////////////////////////////////////////////////
// SECTION  - MEMBER FUNCTION IMPLEMENTATION: CLASS LPBase
/////////////////////////////////////////////////////////////////////////////

namespace base {
//------------------------------------------------------------------
/// Normalize to unit length, and make sure \c a is always >0
/**
\todo Checkout: in what situation will we be unable to normalize?
Is the test below relevant? Clarify.
*/
template<typename LP,typename FPT>
void
LPBase<LP,FPT>::impl_normalizeLine( const detail::RootHelper<type::IsLine>& ) const
{
	auto sq = std::hypot( _v[0], _v[1] );
	if( sq <= std::numeric_limits<double>::epsilon() )
		throw std::runtime_error( "unable to normalize line, sq=" + std::to_string(sq) );

	for( int i=0; i<3; i++ )
		const_cast<LPBase<LP,FPT>*>(this)->_v[i] /= sq; // needed to remove constness

	if( std::signbit(_v[0]) ) // a always >0
		for( int i=0; i<3; i++ )
			const_cast<LPBase<LP,FPT>*>(this)->_v[i] = -_v[i];

	if( _v[0] == 0. ) // then, change sign so that b>0
		if( std::signbit(_v[1]) )
		{
			const_cast<LPBase<LP,FPT>*>(this)->_v[1] = - _v[1];
			const_cast<LPBase<LP,FPT>*>(this)->_v[2] = - _v[2];
		}
}

//------------------------------------------------------------------
template<typename LP,typename FPT>
constexpr FPT
LPBase<LP,FPT>::impl_getCoord( GivenCoord, FPT, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call getCoord() on a point" );
}

template<typename LP,typename FPT>
FPT
LPBase<LP,FPT>::impl_getCoord( GivenCoord gc, FPT other, const detail::RootHelper<type::IsLine>& ) const
{
	const auto a = static_cast<HOMOG2D_INUMTYPE>( _v[0] );
	const auto b = static_cast<HOMOG2D_INUMTYPE>( _v[1] );
	auto denom = ( gc == GivenCoord::X ? b : a );
#ifndef HOMOG2D_NOCHECKS
	if( std::abs(denom) < thr::nullDenom() )
		HOMOG2D_THROW_ERROR_2( "getCoord", "null denominator encountered" );
#endif
	if( gc == GivenCoord::X )
		return ( -a * other - _v[2] ) / b;
	else
		return ( -b * other - _v[2] ) / a;
}


template<typename LP,typename FPT>
constexpr Point2d_<FPT>
LPBase<LP,FPT>::impl_getPoint( GivenCoord, FPT, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call getPoint() on a point" );
}

template<typename LP,typename FPT>
Point2d_<FPT>
LPBase<LP,FPT>::impl_getPoint( GivenCoord gc, FPT other, const detail::RootHelper<type::IsLine>& ) const
{
	auto coord = impl_getCoord( gc, other, detail::RootHelper<type::IsLine>() );
	if( gc == GivenCoord::X )
		return Point2d_<FPT>( other, coord );
	return Point2d_<FPT>( coord, other );
}

//------------------------------------------------------------------
/// ILLEGAL INSTANCIATION
template<typename LP,typename FPT>
template<typename FPT2>
constexpr std::pair<Point2d_<FPT>,Point2d_<FPT>>
LPBase<LP,FPT>::impl_getPoints_A( GivenCoord, FPT, FPT2, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call getPoints() on a point" );
}
template<typename LP,typename FPT>
template<typename FPT2>
constexpr std::pair<Point2d_<FPT>,Point2d_<FPT>>
LPBase<LP,FPT>::impl_getPoints_B( const Point2d_<FPT>&, FPT2, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call getPoints() on a point" );
}

/// Returns pair of points on line at distance \c dist from point on line at coord \c coord. Implementation for lines
template<typename LP,typename FPT>
template<typename FPT2>
std::pair<Point2d_<FPT>,Point2d_<FPT>>
LPBase<LP,FPT>::impl_getPoints_A( GivenCoord gc, FPT coord, FPT2 dist, const detail::RootHelper<type::IsLine>& ) const
{
	const auto pt = impl_getPoint( gc, coord, detail::RootHelper<type::IsLine>() );
	return priv::getPoints_B2( pt, dist, *this );
}

/// Returns pair of points on line at distance \c dist from point on line at coord \c coord. Implementation for lines
template<typename LP,typename FPT>
template<typename FPT2>
std::pair<Point2d_<FPT>,Point2d_<FPT>>
LPBase<LP,FPT>::impl_getPoints_B( const Point2d_<FPT>& pt, FPT2 dist, const detail::RootHelper<type::IsLine>& ) const
{
#ifndef HOMOG2D_NOCHECKS
	if( this->distTo( pt ) > thr::nullDistance() )
	{
		std::cerr << "homog2d: distance=" << std::scientific << this->distTo( pt )
			<< " > null distance (" << thr::nullDistance() << ")\n";
		HOMOG2D_THROW_ERROR_2( "getPoints", "point is not on line" );
	}
#endif

	return priv::getPoints_B2( pt, dist, *this );
}


//------------------------------------------------------------------
/// Illegal instanciation
template<typename LP,typename FPT>
constexpr Line2d_<FPT>
LPBase<LP,FPT>::impl_getOrthogonalLine_A( GivenCoord, FPT, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call getOrthogonalLine() on a point" );
}

/// Illegal instanciation
template<typename LP,typename FPT>
constexpr Line2d_<FPT>
LPBase<LP,FPT>::impl_getOrthogonalLine_B( const Point2d_<FPT>&, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call getOrthogonalLine() on a point" );
}

/// Returns an orthogonal line, implementation of getOrthogonalLine().
template<typename LP,typename FPT>
Line2d_<FPT>
LPBase<LP,FPT>::impl_getOrthogonalLine_A( GivenCoord gc, FPT val, const detail::RootHelper<type::IsLine>& ) const
{
	auto other_val = impl_getCoord( gc, val, detail::RootHelper<type::IsLine>() );

	Point2d_<FPT> pt( other_val, val ) ;
	if( gc == GivenCoord::X )
		pt.set( val, other_val );

	return priv::getOrthogonalLine_B2( pt, *this );
}

/// Returns an orthogonal line, implementation of getOrthogonalLine().
template<typename LP,typename FPT>
Line2d_<FPT>
LPBase<LP,FPT>::impl_getOrthogonalLine_B( const Point2d_<FPT>& pt, const detail::RootHelper<type::IsLine>& ) const
{
#ifndef HOMOG2D_NOCHECKS
	if( this->distTo( pt ) > thr::nullDistance() )
	{
		std::cerr << "homog2d: distance=" << std::scientific << this->distTo( pt )
			<< "> null distance (" << thr::nullDistance() << ")\n";
		HOMOG2D_THROW_ERROR_2( "getOrthogonalLine", "point is not on line" );
	}
#endif

	return priv::getOrthogonalLine_B2( pt, *this );
}

//------------------------------------------------------------------
/// Illegal instanciation
template<typename LP,typename FPT>
constexpr Line2d_<FPT>
LPBase<LP,FPT>::impl_getParallelLine( const Point2d_<FPT>&, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call getParallelLine() on a point" );
}

/// Returns an parallel line, implementation of getParallelLine().
template<typename LP,typename FPT>
Line2d_<FPT>
LPBase<LP,FPT>::impl_getParallelLine( const Point2d_<FPT>& pt, const detail::RootHelper<type::IsLine>& ) const
{
	Line2d_<FPT> out = *this;
	out._v[2] = static_cast<HOMOG2D_INUMTYPE>(-_v[0]) * pt.getX() - _v[1] * pt.getY();
	out.p_normalizeLine();
	return out;
}

//------------------------------------------------------------------
/// Illegal instanciation
template<typename LP,typename FPT>
template<typename T>
constexpr std::pair<Line2d_<FPT>,Line2d_<FPT>>
LPBase<LP,FPT>::impl_getParallelLines( T, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call getParallelLines() on a point" );
}

/// Implementation for lines
template<typename LP,typename FPT>
template<typename T>
std::pair<Line2d_<FPT>,Line2d_<FPT>>
LPBase<LP,FPT>::impl_getParallelLines( T dist, const detail::RootHelper<type::IsLine>& ) const
{
	Line2d_<FPT> l1 = *this;
	Line2d_<FPT> l2 = *this;
	l1._v[2] = static_cast<HOMOG2D_INUMTYPE>(this->_v[2]) + dist;
	l2._v[2] = static_cast<HOMOG2D_INUMTYPE>(this->_v[2]) - dist;
	return std::make_pair( l1, l2 );
}

//------------------------------------------------------------------
/// Comparison operator, used for lines
/**
Definition: two lines will be equal:
- if they are parallel
AND
- if their offset (3 third value) is less than nullDistance()
*/
template<typename LP,typename FPT>
bool
LPBase<LP,FPT>::impl_op_equal( const LPBase<LP,FPT>& other, const detail::RootHelper<type::IsLine>& ) const
{
	if( !this->isParallelTo( other ) )
		return false;

	if( std::fabs( _v[2] - other._v[2] ) > thr::nullDistance() )
		return false;

	return true;
}

/// Comparison operator, used for points
template<typename LP,typename FPT>
bool
LPBase<LP,FPT>::impl_op_equal( const LPBase<LP,FPT>& other, const detail::RootHelper<type::IsPoint>& ) const
{
	return ( this->distTo( other ) < thr::nullDistance() );
}

//------------------------------------------------------------------
/// Sorting operator, for points
template<typename LP,typename FPT>
bool
LPBase<LP,FPT>::impl_op_sort( const LPBase<LP,FPT>& other, const detail::RootHelper<type::IsPoint>& ) const
{
	if( getX() < other.getX() )
		return true;
	if( getX() > other.getX() )
		return false;
	if( getY() < other.getY() )
		return true;
	return false;
}

/// Sorting operator, for lines
template<typename LP,typename FPT>
constexpr bool
LPBase<LP,FPT>::impl_op_sort( const LPBase<LP,FPT>&, const detail::RootHelper<type::IsLine>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid < operator: you cannot sort lines" );
	return false; // to avoid a warning
}

} // namespace base

//------------------------------------------------------------------
/// Inner implementation details
namespace detail {

/// Cross product, see https://en.wikipedia.org/wiki/Cross_product#Coordinate_notation
template<typename Out,typename In,typename FPT1,typename FPT2>
base::LPBase<Out,FPT1> crossProduct( const base::LPBase<In,FPT1>& r1, const base::LPBase<In,FPT2>& r2 )
{
	auto r1_a = static_cast<HOMOG2D_INUMTYPE>(r1._v[0]);
	auto r1_b = static_cast<HOMOG2D_INUMTYPE>(r1._v[1]);
	auto r1_c = static_cast<HOMOG2D_INUMTYPE>(r1._v[2]);
	auto r2_a = static_cast<HOMOG2D_INUMTYPE>(r2._v[0]);
	auto r2_b = static_cast<HOMOG2D_INUMTYPE>(r2._v[1]);
	auto r2_c = static_cast<HOMOG2D_INUMTYPE>(r2._v[2]);

	base::LPBase<Out,FPT1> res;
	res._v[0] = static_cast<FPT1>( r1_b * r2_c  - r1_c * r2_b );
	res._v[1] = static_cast<FPT1>( r1_c * r2_a  - r1_a * r2_c );
	res._v[2] = static_cast<FPT1>( r1_a * r2_b  - r1_b * r2_a );

	return res;
}

} // namespace detail


//------------------------------------------------------------------
///////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////

namespace base {

/// Points overload: generic init from two numeric args
template<typename LP, typename FPT>
template<typename T1,typename T2>
void
LPBase<LP,FPT>::impl_init_2( const T1& v1, const T2& v2, const detail::RootHelper<type::IsPoint>& )
{
	_v[0] = v1;
	_v[1] = v2;
	_v[2] = 1.;
}

/// Lines overload: generic init from two numeric args
template<typename LP, typename FPT>
template<typename T1,typename T2>
void
LPBase<LP,FPT>::impl_init_2( const T1& v1, const T2& v2, const detail::RootHelper<type::IsLine>& )
{
	Point2d_<FPT> pt1;                // 0,0
	Point2d_<FPT> pt2(v1,v2);
	*this = detail::crossProduct<type::IsLine>( pt1, pt2 );
	p_normalizeLine();
}

/// Overload for point to point distance
template<typename LP, typename FPT>
template<typename FPT2>
HOMOG2D_INUMTYPE
LPBase<LP,FPT>::impl_distToPoint( const Point2d_<FPT2>& pt, const detail::RootHelper<type::IsPoint>& ) const
{
	return std::hypot(
		static_cast<HOMOG2D_INUMTYPE>( getX() ) - static_cast<HOMOG2D_INUMTYPE>( pt.getX() ),
		static_cast<HOMOG2D_INUMTYPE>( getY() ) - static_cast<HOMOG2D_INUMTYPE>( pt.getY() )
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
template<typename FPT2>
HOMOG2D_INUMTYPE
LPBase<LP,FPT>::impl_distToPoint( const Point2d_<FPT2>& pt, const detail::RootHelper<type::IsLine>& ) const
{
	return std::fabs(
		static_cast<HOMOG2D_INUMTYPE>( _v[0] ) * pt.getX()
		+ static_cast<HOMOG2D_INUMTYPE>( _v[1] ) * pt.getY()
		+ static_cast<HOMOG2D_INUMTYPE>( _v[2] )
	)
	/ std::hypot( _v[0], _v[1] );
}

/// overload for line to point distance
template<typename LP, typename FPT>
template<typename FPT2>
HOMOG2D_INUMTYPE
LPBase<LP,FPT>::impl_distToLine( const Line2d_<FPT2>& li, const detail::RootHelper<type::IsPoint>& ) const
{
	return li.distTo( *this );
}

/// overload for line to line distance. Aborts build if instanciated (distance between two lines makes no sense).
template<typename LP, typename FPT>
template<typename FPT2>
constexpr HOMOG2D_INUMTYPE
LPBase<LP,FPT>::impl_distToLine( const Line2d_<FPT2>&, const detail::RootHelper<type::IsLine>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot compute distance between two lines" );
	return 0.;    // to avoid warning message on build
}

} // namespace base

//------------------------------------------------------------------
/// Free function, returns the angle (in Rad) between two lines/ or segments
/// \sa Segment_::getAngle()
/// \sa Line2d_::getAngle()
template<typename T1,typename T2>
HOMOG2D_INUMTYPE
getAngle( const T1& li1, const T2& li2 )
{
	return li1.getAngle( li2 );
}

namespace base {

template<typename LP, typename FPT>
template<typename FPT2>
bool
LPBase<LP,FPT>::impl_isParallelTo( const LPBase<LP,FPT2>& li, const detail::RootHelper<type::IsLine>& ) const
{
	if( getAngle(li) < thr::nullAngleValue() )
		return true;
	return false;
}
template<typename LP, typename FPT>
template<typename FPT2>
constexpr bool
LPBase<LP,FPT>::impl_isParallelTo( const LPBase<LP,FPT2>&, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot use IsParallel() with a point" );
	return false;    // to avoid a warning message on build
}

//------------------------------------------------------------------
/// Returns the angle (in rad.) between the line and the other one.
/**
- The returned value will be in the range <code> [0, M_PI/2] </code>

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
LPBase<LP,FPT>::impl_getAngle( const LPBase<LP,FPT>& li, const detail::RootHelper<type::IsLine>& ) const
{
	HOMOG2D_INUMTYPE l1_a = _v[0];
	HOMOG2D_INUMTYPE l1_b = _v[1];
	HOMOG2D_INUMTYPE l2_a = li._v[0];
	HOMOG2D_INUMTYPE l2_b = li._v[1];
	HOMOG2D_INUMTYPE res = l1_a * l2_a + l1_b * l2_b;

	res /= std::sqrt( (l1_a*l1_a + l1_b*l1_b) * (l2_a*l2_a + l2_b*l2_b) );
	HOMOG2D_INUMTYPE fres = std::abs(res);
	if( fres > 1.0 )
	{
		std::cerr << "homog2d: angle computation overflow detected, value "
			<< std::scientific << std::setprecision(20)
			<< fres << ", truncated to 1.0\n";
		fres = 1.0;
	}
	return std::acos( fres );
}

template<typename LP, typename FPT>
constexpr HOMOG2D_INUMTYPE
LPBase<LP,FPT>::impl_getAngle( const LPBase<LP,FPT>&, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "cannot get angle of a point" );
	return 0.; // to avoid a warning
}


//------------------------------------------------------------------
/// Returns true if point is inside (or on the edge) of a flat rectangle defined by (p0,p1)
template<typename LP, typename FPT>
template<typename FPT2>
bool
LPBase<LP,FPT>::impl_isInsideRect( const FRect_<FPT2>& rect, const detail::RootHelper<type::IsPoint>& ) const
{
	auto pair_pts = rect.getPts();
	const auto& p00 = pair_pts.first;
	const auto& p11 = pair_pts.second;
	return detail::ptIsInside( *this, p00, p11 );
}
template<typename LP, typename FPT>
template<typename FPT2>
constexpr bool
LPBase<LP,FPT>::impl_isInsideRect( const FRect_<FPT2>&, const detail::RootHelper<type::IsLine>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "cannot use isInside(FRect) with a line" );
	return false; // to avoid a warning
}

//------------------------------------------------------------------
template<typename LP, typename FPT>
template<typename T>
bool
LPBase<LP,FPT>::impl_isInsideCircle( const Point2d_<FPT>& center, T radius, const detail::RootHelper<type::IsPoint>& ) const
{
	if( distTo( center ) < radius )
		return true;
	return false;
}
template<typename LP, typename FPT>
template<typename T>
constexpr bool
LPBase<LP,FPT>::impl_isInsideCircle( const Point2d_<FPT>&, T, const detail::RootHelper<type::IsLine>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "cannot use isInside(Circle) with a line" );
	return false; // to avoid a warning
}


//------------------------------------------------------------------
template<typename LP, typename FPT>
template<typename FPT2>
bool
LPBase<LP,FPT>::impl_isInsideEllipse( const Ellipse_<FPT2>& ell, const detail::RootHelper<type::IsPoint>& ) const
{
	return ell.pointIsInside( *this );
}

template<typename LP, typename FPT>
template<typename FPT2>
constexpr bool
LPBase<LP,FPT>::impl_isInsideEllipse( const Ellipse_<FPT2>&, const detail::RootHelper<type::IsLine>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "cannot use isInside(Ellipse) with a line" );
	return false; // to avoid a warning
}


//------------------------------------------------------------------
/// Intersection of line and circle: implementation for points
template<typename LP, typename FPT>
template<typename T>
constexpr detail::Intersect<detail::Inters_2,FPT>
LPBase<LP,FPT>::impl_intersectsCircle( const Point2d_<FPT>&, T, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "cannot use intersects(Circle) with a point" );
}

/// Line/Circle intersection: implementation
/// For computation details, checkout http://skramm.lautre.net/files/misc/intersect_circle_line.pdf
template<typename LP, typename FPT>
template<typename T>
detail::Intersect<detail::Inters_2,FPT>
LPBase<LP,FPT>::impl_intersectsCircle(
	const Point2d_<FPT>& pt,       ///< circle origin
	T                    radius,   ///< radius
	const detail::RootHelper<type::IsLine>&  ///< dummy arg, needed so that this overload is only called for lines
) const
{
	detail::Intersect<detail::Inters_2,FPT> out;
	HOMOG2D_CHECK_IS_NUMBER(T);
	const HOMOG2D_INUMTYPE a = static_cast<HOMOG2D_INUMTYPE>(_v[0]); // just to lighten a bit...
	const HOMOG2D_INUMTYPE b = static_cast<HOMOG2D_INUMTYPE>(_v[1]);
	const HOMOG2D_INUMTYPE c = static_cast<HOMOG2D_INUMTYPE>(_v[2]);

// step 1: translate to origin
	auto cp = pt.getX() * a + pt.getY() * b + c;

// step 2: compute distance	between center (origin) and middle point
	auto a2b2 = a * a + b * b;
	auto d0 = std::abs(cp) / std::sqrt( a2b2 );
	if( radius < d0 )                            // if less than radius,
		return out;                         // no intersection

	auto d2 = radius*radius - d0*d0;

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
	out._ptIntersect_1.set( x1 + pt.getX(), y1 + pt.getY() );
	out._ptIntersect_2.set( x2 + pt.getX(), y2 + pt.getY() );
	out._doesIntersect = true;

	priv::fix_order( out._ptIntersect_1, out._ptIntersect_2 );
	return out;
}
//------------------------------------------------------------------
/// Overload used when attempting to use that on a point
template<typename LP, typename FPT>
template<typename FPT2>
constexpr detail::Intersect<detail::Inters_2,FPT>
LPBase<LP,FPT>::impl_intersectsFRect( const FRect_<FPT2>&, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call intersects(FRect) on a point" );
}

/// Line/FRect intersection
template<typename LP, typename FPT>
template<typename FPT2>
detail::Intersect<detail::Inters_2,FPT>
LPBase<LP,FPT>::impl_intersectsFRect( const FRect_<FPT2>& rect, const detail::RootHelper<type::IsLine>& ) const
{
	HOMOG2D_START;
	HOMOG2D_LOG( "line=" << *this << " rect=" << rect );
	std::vector<Point2d_<FPT>> pti;
	for( const auto seg: rect.getSegs() ) // get segment of rectangle
	{
		auto ppts_seg = seg.getPts();
		HOMOG2D_LOG( " -ppts seg=" << ppts_seg.first << "-" << ppts_seg.second );
		auto inters = seg.intersects( *this );
		if( inters() )
		{
			bool storePoint(true);
			auto pt = inters.get();
			if( pt == ppts_seg.first || pt == ppts_seg.second )  // if intersection point is one of the segment pts
				if( pti.size() == 1 )                            // AND if there is already one
					if( pti[0] == pt )                           // AND that one is already stored
						storePoint = false;
			if( storePoint )
				pti.push_back( pt );

			if( pti.size() == 2 )  // already got 2, done
				break;
		}
	}
	if( pti.empty() )
		return detail::Intersect<detail::Inters_2,FPT>();

	if( pti.size() == 1 )         // if single intersections, add one,
		pti.push_back( pti[0] );  // we return two identical points
	else
		priv::fix_order( pti[0], pti[1] );
	return detail::Intersect<detail::Inters_2,FPT>( pti[0], pti[1] );
}

//------------------------------------------------------------------
/// Draw lines or points
template<typename LP,typename FPT>
template<typename T>
bool LPBase<LP,FPT>::draw( img::Image<T>& img, img::DrawParams dp ) const
{
	return impl_draw( img, dp, detail::RootHelper<LP>() );
}

} // namespace base

/////////////////////////////////////////////////////////////////////////////
// SECTION  - PRODUCT OPERATORS DEFINITIONS (HELPER FUNCTIONS)
/////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------
/// Apply homography to a vector/array/list (type T) of points or lines.
template<typename W,typename FPT>
template<typename T>
void
Hmatrix_<W,FPT>::applyTo( T& vin ) const
{
#if 0
	std::transform(
		std::begin( vin ),
		std::end( vin ),
		std::begin( vin ),
		[](){}
	);
#else
	for( auto& elem: vin )
		elem = *this * elem;
#endif
}

namespace detail {

/// Implementation of product 3x3 by 3x3

template<typename FPT1,typename FPT2,typename FPT3>
void
product(
	Matrix_<FPT1>&       out,
	const Matrix_<FPT2>& h1,
	const Matrix_<FPT3>& h2
)
{
	out.p_fillZero();
	for( int i=0; i<3; i++ )
		for( int j=0; j<3; j++ )
			for( int k=0; k<3; k++ )
				out.value(i,j) +=
					static_cast<HOMOG2D_INUMTYPE>( h1.value(i,k) ) * h2.value(k,j);
}

/// Implementation of product 3x3 by 3x1
/**
- T1 and T2: type::IsLine or type::IsPoint (same but also different)
*/
template<typename T1,typename T2,typename FPT1,typename FPT2>
void
product(
	base::LPBase<T1,FPT1>&        out,
	const Matrix_<FPT2>&          h,
	const base::LPBase<T2,FPT1>&  in
)
{
	for( int i=0; i<3; i++ )
	{
		auto sum  = static_cast<HOMOG2D_INUMTYPE>(h._mdata[i][0]) * in._v[0];
		sum      += h._mdata[i][1] * in._v[1];
		sum      += h._mdata[i][2] * in._v[2];
		out._v[i] = sum;
	}
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////
// SECTION  - PRODUCT OPERATORS DEFINITIONS
/////////////////////////////////////////////////////////////////////////////

/// Free function template, product of two lines, returns a point
template<typename FPT,typename FPT2>
Point2d_<FPT>
operator * ( const Line2d_<FPT>& lhs, const Line2d_<FPT2>& rhs )
{
#ifndef HOMOG2D_NOCHECKS
	if( lhs.isParallelTo(rhs) )
		HOMOG2D_THROW_ERROR_1( "lines are parallel, unable to compute product:\nlhs="
			<< lhs << " rhs=" << rhs
		);
#endif

	return detail::crossProduct<type::IsPoint,type::IsLine,FPT>(lhs, rhs);
}

/// Free function template, product of two segments, returns a point
template<typename FPT,typename FPT2>
Point2d_<FPT>
operator * ( const Segment_<FPT>& lhs, const Segment_<FPT2>& rhs )
{
	return lhs.getLine() * rhs.getLine();
}

/// Free function template, product of two points, returns a line
template<typename FPT,typename FPT2>
Line2d_<FPT>
operator * ( const Point2d_<FPT>& lhs, const Point2d_<FPT2>& rhs )
{
#ifndef HOMOG2D_NOCHECKS
	if( lhs == rhs )
		HOMOG2D_THROW_ERROR_1( "points are identical, unable to compute product:" << lhs );
#endif
	Line2d_< FPT> line = detail::crossProduct<type::IsLine,type::IsPoint,FPT>(lhs, rhs);
	line.p_normalizeLine();
	return line;
}

#ifdef HOMOG2D_FUTURE_STUFF
/// Apply Epipolar matrix to a point or line, this will return the opposite type.
/// Free function, templated by point or line
template<typename T,typename U,typename V>
base::LPBase<typename detail::HelperPL<T>::OtherType,V>
operator * ( const Hmatrix_<type::IsEpipmat,U>& h, const base::LPBase<T,V>& in )
{
	LPBase<typename detail::HelperPL<T>::OtherType,V> out;
	detail::product( out, h._data, in );
	return out;
}
#endif

/// Free function, apply homography to a point.
template<typename T,typename U>
Point2d_<T>
operator * ( const Homogr_<U>& h, const Point2d_<T>& in )
{
	Point2d_<T> out;
	detail::product( out, h, in );
	return out;
}

/// Free function, apply homography to a line.
template<typename T,typename U>
Line2d_<T>
operator * ( const Homogr_<U>& h, const Line2d_<T>& in )
{
	if( h._hmt == nullptr )             // if H^-T	not allocated yet, do it
	{
		h._hmt = std::unique_ptr<detail::Matrix_<U>>( new detail::Matrix_<U>() );
		h._hasChanged = true;
	}

	if( h._hasChanged )                  // if homography has changed, recompute inverse and transposed
	{
		auto hi = h;
		auto mat_inv = hi.inverse().transpose();
		*h._hmt = mat_inv;
		h._hasChanged = false;
	}

	Line2d_<T> out;
	detail::product( out, *h._hmt, in );
	out.p_normalizeLine();
	return out;
}

/// Apply homography to a Segment
template<typename FPT1,typename FPT2>
Segment_<FPT1>
operator * ( const Homogr_<FPT2>& h, const Segment_<FPT1>& seg )
{
	const auto& pts = seg.getPts();
	Point2d_<FPT1> pt1 = h * pts.first;
	Point2d_<FPT1> pt2 = h * pts.second;
	return Segment_<FPT1>( pt1, pt2 );
}

/// Apply homography to a Polyline
template<typename FPT1,typename FPT2,typename PLT>
base::PolylineBase<PLT,FPT1>
operator * ( const Homogr_<FPT2>& h, const base::PolylineBase<PLT,FPT1>& pl )
{
	base::PolylineBase<PLT,FPT1> out;
	const auto& pts = pl.getPts();
	for( const auto pt: pts )
		out.p_addPoint( h * pt );
	return out;
}

/// Apply homography to a flat rectangle produces a closed polyline
template<typename FPT1,typename FPT2>
CPolyline_<FPT1>
operator * ( const h2d::Homogr_<FPT2>& h, const h2d::FRect_<FPT1>& rin )
{
	h2d::CPolyline_<FPT1> out;
	for( const auto& pt: rin.get4Pts() )
		out.p_addPoint( h * pt );
	return out;
}

/// Apply homography to a Ellipse, produces an Ellipse
/**
\f[
Q' = H^{-T} \cdot Q \cdot H^{-1}
\f]
*/
template<typename FPT1,typename FPT2>
Ellipse_<FPT1>
operator * ( const Homogr_<FPT2>& h, const Ellipse_<FPT1>& ell_in )
{
	auto hm = static_cast<detail::Matrix_<FPT2>>(h);
	hm.inverse();
	auto hmt = hm;
	hmt.transpose();

	const auto& ell_in2 = static_cast<detail::Matrix_<FPT1>>(ell_in);
	auto prod = hmt * ell_in2 * hm;

	Ellipse_<FPT1> out( prod );
	return out;
}

/// Apply homography to a Circle, produces an Ellipse
/** Converts the circle to an ellipse, then calls the corresponding code */
template<typename FPT1,typename FPT2>
Ellipse_<FPT1>
operator * ( const Homogr_<FPT2>& h, const Circle_<FPT1>& cir )
{
	Ellipse_<FPT1> ell_in( cir );
	return h * ell_in;
}

//------------------------------------------------------------------
namespace priv {

/// Allocation for \c std::array container
/**
\sa operator * ( const Hmatrix_&, const Cont& );
*/
template<
	typename Cont,
	typename std::enable_if<
		Is_std_array<Cont>::value,
		Cont
	>::type* = nullptr
>
Cont
alloc( std::size_t /* unused here */ )
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
typename std::enable_if<priv::IsContainer<Cont>::value,Cont>::type
operator * (
	const Hmatrix_<type::IsHomogr,FPT>& h,    ///< Matrix
	const Cont&                         vin   ///< Input container
)
{
	Cont vout = priv::alloc<Cont>( vin.size() );
	auto it = std::begin( vout );
	for( const auto& elem: vin )
		*it++ = h * elem;
	return vout;
}

/////////////////////////////////////////////////////////////////////////////
// SECTION  - FREE FUNCTIONS
/////////////////////////////////////////////////////////////////////////////

template<typename FPT>
FPT getX( const Point2d_<FPT>& pt) { return pt.getX(); }
template<typename FPT>
FPT getY( const Point2d_<FPT>& pt) { return pt.getY(); }

/// Free function, distance between points
/// \sa Point2d_::distTo()
template<typename FPT1,typename FPT2>
HOMOG2D_INUMTYPE
dist( const Point2d_<FPT1>& pt1, const Point2d_<FPT2>& pt2 )
{
	return pt1.distTo( pt2 );
}

/// Free function, see FRect_::unionArea()
template<typename FPT1,typename FPT2>
CPolyline_<FPT1>
unionArea( const FRect_<FPT1>& r1, const FRect_<FPT2>& r2 )
{
	return r1.unionArea(r2);
}

/// Free function, see FRect_::intersectArea()
template<typename FPT1,typename FPT2>
detail::RectArea<FPT1>
intersectArea(  const FRect_<FPT1>& r1, const FRect_<FPT2>& r2 )
{
	return r1.intersectArea(r2);
}

/// Returns circle passing through 4 points of flat rectangle (free function)
/// \sa FRect_::getBoundingCircle()
template<typename FPT>
Circle_<FPT>
getBoundingCircle( const FRect_<FPT>& rect )
{
	return rect.getBoundingCircle();
}

/// Returns true if is a polygon (free function)
///  \sa PolylineBase::isPolygon()
template<typename PLT,typename FPT>
bool
isPolygon( const base::PolylineBase<PLT,FPT>& pl )
{
	return pl.isPolygon();
}

/// Returns true if polygon is convex (free function)
/// \sa PolylineBase::isConvex()
template<typename PLT,typename FPT>
bool
isConvex( const base::PolylineBase<PLT,FPT>& poly )
{
	return poly.isConvex();
}

/// Returns the number of segments (free function)
/// \sa PolylineBase::nbSegs()
template<typename PLT,typename FPT>
size_t nbSegs( const base::PolylineBase<PLT,FPT>& pl )
{
	return pl.nbSegs();
}

/// Get segment length (free function)
/// \sa Segment_::length()
template<typename FPT>
HOMOG2D_INUMTYPE
length( const Segment_<FPT>& seg )
{
	return seg.length();
}

/// Returns length (free function)
/// \sa PolylineBase::length()
template<typename PLT,typename FPT>
HOMOG2D_INUMTYPE
length( const base::PolylineBase<PLT,FPT>& pl )
{
	return pl.length();
}

/// Returns the segments of the polyline (free function)
/// \sa PolylineBase::getSegs()
template<typename PLT,typename FPT>
std::vector<Segment_<FPT>>
getSegs( const base::PolylineBase<PLT,FPT>& pl )
{
	return pl.getSegs();
}

/// Returns the number of points (free function)
/// \sa PolylineBase::size()
template<typename PLT,typename FPT>
size_t size( const base::PolylineBase<PLT,FPT>& pl )
{
	return pl.size();
}

/// Returns width, height of rectangle (free function)
/// \sa FRect_::size()
template<typename FPT>
std::pair<HOMOG2D_INUMTYPE,HOMOG2D_INUMTYPE>
size( const FRect_<FPT>& rect )
{
	return rect.size();
}

/// Returns Bounding Box of Ellipse_ (free function)
/// \sa Ellipse_::getBB()
template<typename FPT>
CPolyline_<FPT>
getOBB( const Ellipse_<FPT>& ell )
{
	return ell.getOBB();
}

/// Returns Bounding Box of object (free function)
template<typename T>
FRect_<typename T::FType>
getBB( const T& object )
{
	return object.getBB();
}

namespace priv {
/// Returns Bounding Box of two rectangles (private free function)
template<typename FPT1,typename FPT2>
FRect_<FPT1>
getBB( const FRect_<FPT1>& ra, const FRect_<FPT2>& rb )
{
// first, convert them to internal numerical type
// (same type is needed to use std::min/max)
	const FRect_<HOMOG2D_INUMTYPE> r1(ra);
	const FRect_<HOMOG2D_INUMTYPE> r2(rb);
	auto ppts1 = r1.getPts();
	auto ppts2 = r2.getPts();
	auto min_x = std::min( ppts1.first.getX(),  ppts2.first.getX() );
	auto min_y = std::min( ppts1.first.getY(),  ppts2.first.getY() );
	auto max_x = std::max( ppts1.second.getX(), ppts2.second.getX() );
	auto max_y = std::max( ppts1.second.getY(), ppts2.second.getY() );
	return FRect_<FPT1>( min_x, min_y, max_x, max_y );
}
} // namespace priv

/// Returns Bounding Box of two arbitrary objects (free function)
/**
- available only for Circles, Ellipse, FRect, Polyline
*/
template<typename T1,typename T2>
FRect_<typename T1::FType>
getBB( const T1& elem1, const T2& elem2 )
{
	auto r1 = elem1.getBB();
	auto r2 = elem2.getBB();
	return priv::getBB( r1, r2 );
}

/// Returns Bounding Box of arbitrary container holding points (free function)
template<
	typename T,
	typename std::enable_if<
		priv::IsContainer<T>::value,
		T
	>::type* = nullptr
>
//FRect_<typename T::value_type::FType> // if we dont have C++14 but only C++11
auto
getBB( const T& vpts )
{
	return priv::getPointsBB( vpts );
}

//------------------------------------------------------------------
/// Returns the points of Segment as a std::pair (free function)
/// \sa Segment_::getPts()
template<typename FPT>
std::pair<Point2d_<FPT>,Point2d_<FPT>>
getPts( const Segment_<FPT>& seg )
{
	return seg.getPts();
}

/// Returns Segment supporting line (free function)
/// \sa Segment_::getLine()
template<typename FPT>
Line2d_<FPT> getLine( const Segment_<FPT>& seg )
{
	return seg.getLine();
}

//------------------------------------------------------------------
/// Free function, returns segment between two circle centers
template<typename FPT1,typename FPT2>
Segment_<FPT1>
getSegment( const Circle_<FPT1>& c1, const Circle_<FPT2>& c2 )
{
	return Segment_<FPT1>( c1.center(), c2.center() );
}

/// Free function, returns line between two circle centers
template<typename FPT1,typename FPT2,typename FPT3>
Line2d_<FPT1>
getLine( const Circle_<FPT2>& c1, const Circle_<FPT3>& c2 )
{
	return Line2d_<FPT1>( c1.center(), c2.center() );
}

/// Free function, returns middle point of segment
/// \sa Segment_::getMiddlePoint()
template<typename FPT>
Point2d_<FPT>
getMiddlePoint( const Segment_<FPT>& seg )
{
	return seg.getMiddlePoint();
}

/// Free function, returns middle point of set of segments
/**
\sa Segment_::getMiddlePoint()
- input: set of segments
- output: set of points (same container)
*/
template<typename FPT>
std::vector<Point2d_<FPT>>
getMiddlePoints( const std::vector<Segment_<FPT>>& vsegs )
{
	std::vector<Point2d_<FPT>> vout( vsegs.size() );

	auto it = std::begin( vout );
	for( const auto& seg: vsegs )
		*it++ = getMiddlePoint( seg );
	return vout;
}

/// Free function, returns a set of lines from a set of segments
/**
\sa Segment_::getLine()
*/
template<typename FPT>
std::vector<Line2d_<FPT>>
getLines( const std::vector<Segment_<FPT>>& vsegs )
{
	std::vector<Line2d_<FPT>> vout( vsegs.size() );

	auto it = std::begin( vout );
	for( const auto& seg: vsegs )
		*it++ = seg.getLine();
	return vout;
}

/// Free function, returns segments of the rectangle
/// \sa FRect_::getSegs()
template<typename FPT>
std::array<Segment_<FPT>,4>
getSegs( const FRect_<FPT>& seg )
{
	return seg.getSegs();
}

/// Free function, returns the pair of segments tangential to the two circles
template<typename FPT1,typename FPT2>
std::pair<Segment_<FPT1>,Segment_<FPT1>>
getTanSegs( const Circle_<FPT1>& c1, const Circle_<FPT2>& c2 )
{
#ifndef HOMOG2D_NOCHECKS
	if( c1 == c2 )
		HOMOG2D_THROW_ERROR_1( "c1 and c2 identical" );
#endif

	auto li0 = Line2d_<FPT1>( c1.center(), c2.center() );
	auto li1 = li0.getOrthogonalLine( c1.center() );
	auto li2 = li0.getOrthogonalLine( c2.center() );

	const auto ri1 = li1.intersects( c1 );
	const auto ri2 = li2.intersects( c2 );
	assert( ri1() && ri2() );
	const auto& ppts1 = ri1.get();
	const auto& ppts2 = ri2.get();

	return std::make_pair(
		Segment_<FPT1>( ppts1.first,  ppts2.first  ),
		Segment_<FPT1>( ppts1.second, ppts2.second )
	);
}

/// Returns the 4 points of the rectangle (free function)
/// \sa FRect_::get4Pts()
template<typename FPT>
std::array<Point2d_<FPT>,4>
get4Pts( const FRect_<FPT>& rect )
{
	return rect.get4Pts();
}

/// Returns the 2 major points of the rectangle (free function)
/// \sa FRect_::getPts()
template<typename FPT>
std::pair<Point2d_<FPT>,Point2d_<FPT>>
getPts( const FRect_<FPT>& rect )
{
	return rect.getPts();
}

/// Free function
template<typename FPT>
HOMOG2D_INUMTYPE height( const FRect_<FPT>& rect )
{
	return rect.height();
}
/// Free function
template<typename FPT>
HOMOG2D_INUMTYPE width( const FRect_<FPT>& rect )
{
	return rect.width();
}

/// Free function
/// \sa FRect_::area()
template<typename FPT>
HOMOG2D_INUMTYPE area( const FRect_<FPT>& rect )
{
	return rect.area();
}

/// Free function
/// \sa Ellipse_::area()
template<typename FPT>
HOMOG2D_INUMTYPE area( const Ellipse_<FPT>& ell )
{
	return ell.area();
}

/// Free function
/// \sa PolylineBase::area()
template<typename PLT,typename FPT>
HOMOG2D_INUMTYPE area( const base::PolylineBase<PLT,FPT>& pl )
{
	return pl.area();
}

/// Free function
/// \sa PolylineBase::centroid()
template<typename PLT,typename FPT>
base::LPBase<type::IsPoint,FPT> centroid( const base::PolylineBase<PLT,FPT>& pl )
{
	return pl.centroid();
}

/// Free function
template<typename FPT>
HOMOG2D_INUMTYPE length( const FRect_<FPT>& rect )
{
	return rect.length();
}

/// Free function, return floating-point type
template<typename T>
Dtype dtype( const T& t )
{
	return t.dtype();
}

//------------------------------------------------------------------
/// Returns the distance between 2 parallel lines (free function)
/**
- ref: https://en.wikipedia.org/wiki/Distance_between_two_parallel_lines

We first check that the two lines are indeed parallel.
This should ensure that a1=a2 and b1=b2.
But due to numeric issues they could by slightly different. In order to gain accuracy,
we use the geometric mean of these.
*/
template<typename FPT>
HOMOG2D_INUMTYPE
getParallelDistance( const Line2d_<FPT>& li1, const Line2d_<FPT>& li2 )
{
#ifndef HOMOG2D_NOCHECKS
	if( !li1.isParallelTo(li2) )
		HOMOG2D_THROW_ERROR_1( "lines are not parallel" );
#endif
	const auto ar1 = li1.get();
	const auto ar2 = li2.get();
	const HOMOG2D_INUMTYPE a1 = ar1[0];
	const HOMOG2D_INUMTYPE b1 = ar1[1];
	const HOMOG2D_INUMTYPE c1 = ar1[2];
	const HOMOG2D_INUMTYPE a2 = ar2[0];
	const HOMOG2D_INUMTYPE b2 = ar2[1];
	const HOMOG2D_INUMTYPE c2 = ar2[2];

	HOMOG2D_INUMTYPE a = std::sqrt( a1*a2 );
	HOMOG2D_INUMTYPE b = std::sqrt( b1*b2 );
	return std::abs( c1 - c2 ) / std::sqrt( a*a + b*b );
}

/// Return angle of ellipse (free function)
/// \sa Ellipse_::angle()
template<typename FPT>
HOMOG2D_INUMTYPE
angle( const Ellipse_<FPT>& ell )
{
	return ell.angle();
}

/// Return center of Circle_ or Ellipse_ (free function)
/// \sa Ellipse_::center()
/// \sa Circle_::center()
template<typename T>
Point2d_<typename T::FType>
center(const T& other )
{
	return other.center();
}

/// Returns true if ellipse is a circle
/// \sa Ellipse_::isCircle()
template<typename FPT>
bool
isCircle( const Ellipse_<FPT>& ell, HOMOG2D_INUMTYPE thres=1.E-10 )
{
	return ell.isCircle( thres );
}

/// Returns ellipse axis lines
/// \sa Ellipse_::getAxisLines()
template<typename FPT>
std::pair<Line2d_<FPT>,Line2d_<FPT>>
getAxisLines( const Ellipse_<FPT>& ell )
{
	return ell.getAxisLines();
}

/////////////////////////////////////////////////////////////////////////////
// SECTION  - GENERIC DRAWING FREE FUNCTIONS (BACK-END INDEPENDENT)
/////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------
/// Free function, draws any of the primitives
template<
	typename U,
	typename Prim,
	typename std::enable_if<
		priv::IsDrawable<Prim>::value,
		Prim
	>::type* = nullptr
>
void draw( img::Image<U>& img, const Prim& prim, const img::DrawParams& dp=img::DrawParams() )
{
	prim.draw( img, dp );
}

namespace priv {
#ifdef HOMOG2D_USE_OPENCV
template<typename U,typename FPT>
void
impl_drawIndexes( img::Image<U>& img, size_t c, const img::DrawParams& dp, const Point2d_<FPT>& pt )
{
	if( dp._dpValues._showIndex )
		cv::putText( img.getReal(), std::to_string(c), pt.getCvPtd(), 0, 0.8, cv::Scalar( 250,0,0 ), 2 );
}
template<typename U,typename FPT>
void
impl_drawIndexes( img::Image<U>& img, size_t c, const img::DrawParams& dp, const Segment_<FPT>& seg )
{
	if( dp._dpValues._showIndex )
		cv::putText( img.getReal(), std::to_string(c), seg.getMiddlePoint().getCvPtd(), 0, 0.8, cv::Scalar( 250,0,0 ), 2 );
}
#endif

/// Default signature, will be instanciated if no other fits (and does nothing)
template<typename U,typename DUMMY>
void
impl_drawIndexes( img::Image<U>&, size_t, const img::DrawParams&, const DUMMY& )
{}

} // namespace priv

/// Free function, draws a set of primitives
/**
Type \c T can be \c std::array<type> or \c std::vector<type>, with \c type being anything drawable
*/
template<
	typename U,
	typename T,
	typename std::enable_if<
		priv::IsContainer<T>::value,
		T
	>::type* = nullptr
>
void draw( img::Image<U>& img, const T& cont, const img::DrawParams& dp=img::DrawParams() )
{
	size_t c=0;
	for( const auto& elem: cont )
	{
		elem.draw( img, dp );
		priv::impl_drawIndexes( img, c, dp, elem );
		c++;
	}
}
/// This version holds a \c std::function as 3th parameter. It can be used to pass a function
/// that will return a different img::DrawParams for a given index of the container.
template<
	typename U,
	typename T,
	typename std::enable_if<
		priv::IsContainer<T>::value,
		T
	>::type* = nullptr
>
void draw( img::Image<U>& img, const T& cont, std::function<img::DrawParams(int)>& func )
{
	size_t c=0;
	for( const auto& elem: cont )
	{
		auto dp = func(c);
		elem.draw( img, dp );
		priv::impl_drawIndexes( img, c, dp, elem );
		c++;
	}
}

//------------------------------------------------------------------
/// Free function, draws a pair of objects
/**
Type \c T can be anything drawable
*/
template<typename T,typename U>
void draw( img::Image<U>& img, const std::pair<T,T>& ppts, const img::DrawParams& dp=img::DrawParams() )
{
	ppts.first.draw(  img, dp );
	ppts.second.draw( img, dp );
}

namespace detail {

/// Private helper function, used by LPBase<IsPoint>::draw(). Draw point on image.
template<typename T>
void
drawPt(
	img::Image<T>&               img,
	img::PtStyle                 ps,
	std::vector<Point2d_<float>> vpt,
	const img::DrawParams&       dp,
	bool                         drawDiag=false
)
{
	auto delta  = dp._dpValues._ptDelta;
	auto delta2 = std::round( 0.85 * delta);
	switch( ps )
	{
		case img::PtStyle::Times:
			vpt[0].translate( -delta2, +delta2 );
			vpt[1].translate( +delta2, -delta2 );
			vpt[2].translate( +delta2, +delta2 );
			vpt[3].translate( -delta2, -delta2 );
		break;

		case img::PtStyle::Plus:
		case img::PtStyle::Diam:
			vpt[0].translate( -delta2, 0.      );
			vpt[1].translate( +delta2, 0.      );
			vpt[2].translate( 0.,      -delta2 );
			vpt[3].translate( 0.,      +delta2 );
		break;
		default: assert(0);
	}
	auto dp2(dp);          // we need to do this, because this is called by the
	dp2.showPoints(false); //  segment drawing function. If not, infinite recursion
	if( !drawDiag )
	{
		Segment_<float> s1( vpt[0], vpt[1] );
		Segment_<float> s2( vpt[2], vpt[3] );
		s1.draw( img, dp2 );
		s2.draw( img, dp2 );
	}
	else // draw 4 diagonal lines
	{
		Segment_<float>( vpt[0], vpt[2] ).draw( img, dp2 );
		Segment_<float>( vpt[2], vpt[1] ).draw( img, dp2 );
		Segment_<float>( vpt[1], vpt[3] ).draw( img, dp2 );
		Segment_<float>( vpt[0], vpt[3] ).draw( img, dp2 );
	}
}

} // namespace detail

namespace base {

//------------------------------------------------------------------
/// Draw PolylineBase, independent of back-end library (calls the segment drawing function)
template<typename PLT,typename FPT>
template<typename T>
void
PolylineBase<PLT,FPT>::draw( img::Image<T>& im, img::DrawParams dp ) const
{
	if( size() < 2 ) // nothing to draw
		return;
	for( size_t i=0; i<nbSegs(); i++ )
		getSegment(i).draw( im, dp );

	if( dp._dpValues._showPoints )
	{
		auto newPointStyle = dp._dpValues.nextPointStyle();
		getPoint(0).draw( im, dp.setColor(10,10,10).setPointStyle( newPointStyle ) );
	}
	if( dp._dpValues._showIndex )
		impl_draw_pl( im );
}

} // namespace base

//------------------------------------------------------------------
namespace priv {
/// Holds convex hull code
namespace chull {

//------------------------------------------------------------------
template<typename FPT>
size_t
getPivotPoint( const std::vector<Point2d_<FPT>>& in )
{
	auto pmin = std::min_element(
		in.begin(),
		in.end(),
		[]                  // lambda
		( const Point2d_<FPT>& pt1, const Point2d_<FPT>& pt2 )
		{
			if( pt1.getY() < pt2.getY() )
				return true;
			if( pt1.getY() > pt2.getY() )
				return false;
			return( pt1.getX() < pt2.getX() );
		}
	);
	return static_cast<size_t>( pmin - in.begin() );
}

//------------------------------------------------------------------
/// Sorts points by angle between the lines with horizontal axis
template<typename FPT>
std::vector<size_t>
sortPoints( const std::vector<Point2d_<FPT>>& in, size_t piv_idx )
{
	assert( in.size()>3 );

// step 1: create new vector holding the indexes of the points, including the pivot point (will be in first position)
	std::vector<size_t> out( in.size() );
	std::iota( out.begin(), out.end(), 0 );
	std::swap( out[piv_idx], out[0] );
	auto pt0 = in[piv_idx];

// step 2: sort points by angle of lines between the current point and pivot point
	std::sort(
		out.begin()+1,
		out.end(),
		[&]                  // lambda
		( size_t i1, size_t i2 )
		{
			auto pt1 = in[i1];
			auto pt2 = in[i2];
			auto dx1 = pt1.getX() - pt0.getX();
			auto dy1 = pt1.getY() - pt0.getY();
			auto dx2 = pt2.getX() - pt0.getX();
			auto dy2 = pt2.getY() - pt0.getY();
			return ((dx1 * dy2 - dx2 * dy1) > 0);
		}
	);
	return out;
}

//------------------------------------------------------------------
/// To find orientation of ordered triplet of points (p, q, r).
/** The function returns following values
- 0 --> p, q and r are colinear
- 1 --> Clockwise
- 2 --> Counterclockwise
*/
template<typename T>
int orientation( Point2d_<T> p, Point2d_<T> q, Point2d_<T> r )
{
	HOMOG2D_INUMTYPE px = p.getX();
	HOMOG2D_INUMTYPE py = p.getY();
	HOMOG2D_INUMTYPE qx = q.getX();
	HOMOG2D_INUMTYPE qy = q.getY();
	HOMOG2D_INUMTYPE rx = r.getX();
	HOMOG2D_INUMTYPE ry = r.getY();

	auto val = (qy - py) * (rx - qx) - (qx - px) * (ry - qy);
    if( std::abs(val) < HOMOG2D_THR_ZERO_DETER )
		return 0;  // collinear
    return (val > 0 ? 1 : -1 ); // clock or counterclock wise
}
//------------------------------------------------------------------
/// Inherits std::stack<> and adds a member function to fetch the underlying std::vector.
/// Used in h2d::convexHull()
struct Mystack : std::stack<size_t,std::vector<size_t>>
{
	const std::vector<size_t>& getVect() const
	{
		return this->c;
	}
};
//------------------------------------------------------------------
} // namespace chull
} // namespace priv

//------------------------------------------------------------------
/// Compute Convex Hull (free function)
/**
- type \c T: can be either OPolyline, CPolyline, or std::vector<Point2d>
- Graham scan algorithm: https://en.wikipedia.org/wiki/Graham_scan
*/
template<typename CT,typename FPT>
CPolyline_<FPT>
convexHull( const base::PolylineBase<CT,FPT>& input )
{
	return convexHull( input.getPts() );
}

namespace base {
//------------------------------------------------------------------
/// Computes and returns the convex hull of a set of points (free function)
/**
- Graham scan algorithm: https://en.wikipedia.org/wiki/Graham_scan
*/
template<typename FPT>
CPolyline_<FPT>
convexHull( const std::vector<Point2d_<FPT>>& input )
{
	if( input.size() < 4 )  // if 3 pts or less, then the hull is equal to input set
		return CPolyline_<FPT>( input );

// step 1: find pivot (point with smallest Y coord)
	auto pivot_idx = priv::chull::getPivotPoint( input );

// step 2: sort points by angle of lines between the current point and pivot point
	auto v2 = priv::chull::sortPoints( input, pivot_idx );

	std::stack<size_t> firstPoint;
	priv::chull::Mystack hull;
	hull.push( v2[0] );
	hull.push( v2[1] );
	hull.push( v2[2] );

	firstPoint.push( 0 );
	firstPoint.push( 1 );

// step 3: iterate
	size_t idx1 = 1;
	size_t idx2 = 2;
	size_t idx3 = 3;
	do
	{
		HOMOG2D_LOG( "** loop start, idx1=" << idx1 << ", idx2=" << idx2  << ", idx3=" << idx3 << ", hull " << hull.getVect() );
		auto p = input.at( v2[idx1] );
		auto q = input.at( v2[idx2] );
		auto r = input.at( v2[idx3] );
		auto orient = priv::chull::orientation( p, q, r );
//		HOMOG2D_LOG( "considering pts: " << v2[idx1] << "," << v2[idx2] << "," << v2[idx3] << ": or = " << orient );
		if( orient != 1 )
		{
			hull.push( v2[idx3] );
			idx1 = idx2;
			idx2 = idx3;
			idx3++;
			firstPoint.push( idx1 );
		}
		else
		{
			hull.pop();
			idx2=idx1; // idx3 stays the same
			firstPoint.pop();
			idx1 = firstPoint.top();
		}
	}
	while( idx3 < v2.size() );

// final step: copy hull indexes to vector of points
	const auto v = hull.getVect();
	std::vector<Point2d_<FPT>> vout( v.size() );
	std::transform(
		v.begin(),
		v.end(),
		vout.begin(),
		[&input]               // lambda
		(size_t idx)
		{
			return input.at( idx );
		}
	);

	return CPolyline_<FPT>( vout );
}

} // namespace base

/// Return convex hull (member function implementation)
template<typename CT,typename FPT>
CPolyline_<FPT>
base::PolylineBase<CT,FPT>::convexHull() const
{
	return h2d::convexHull( *this );
}

/////////////////////////////////////////////////////////////////////////////
// SECTION  - OPENCV BINDING - GENERAL
/////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------
#ifdef HOMOG2D_USE_OPENCV
/// Return Opencv 2D point
template<typename LP, typename FPT>
template<typename OPENCVT>
OPENCVT
base::LPBase<LP,FPT>::impl_getCvPt( const detail::RootHelper<type::IsPoint>&, const OPENCVT& ) const
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
	auto& data = detail::Matrix_<FPT>::_mdata;
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
				*it = data[i/3][i%3];
			break;
		case CV_32F:
			for( auto it = mat.begin<float>(); it != mat.end<float>(); it++, i++ )
				*it = data[i/3][i%3];
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

	auto& data = detail::Matrix_<FPT>::_mdata;
	switch( type )
	{
		case CV_64F:
			for( auto it = mat.begin<double>(); it != mat.end<double>(); it++, i++ )
				data[i/3][i%3] = *it;
			break;
		case CV_32F:
			for( auto it = mat.begin<float>(); it != mat.end<float>(); it++, i++ )
				data[i/3][i%3] = *it;
			break;
		default: assert(0);
	}
	return *this;
}
#endif // HOMOG2D_USE_OPENCV

/////////////////////////////////////////////////////////////////////////////
// SECTION  - CLASS DRAWING MEMBER FUNCTIONS (OpenCv)
/////////////////////////////////////////////////////////////////////////////

#ifdef HOMOG2D_USE_OPENCV
//------------------------------------------------------------------
/// Draw points on Cv::Mat: implementation
/// Returns false if point not in image
template<typename LP, typename FPT>
template<typename T>
bool
base::LPBase<LP,FPT>::impl_draw( img::Image<T>& im, img::DrawParams dp, const detail::RootHelper<type::IsPoint>& ) const
{
	if( getX()<0 || getX()>=im.cols() )
		return false;
	if( getY()<0 || getY()>=im.rows() )
		return false;

	std::vector<Point2d_<float>> vpt( 4, *this );
	switch( dp._dpValues._ptStyle )
	{
		case img::PtStyle::Dot:
			cv::circle( im.getReal(), getCvPti(), dp._dpValues._pointSize, dp._dpValues.color(), -1 );
		break;

		case img::PtStyle::Plus:   // "+" symbol
			detail::drawPt( im, img::PtStyle::Plus, vpt, dp );
		break;

		case img::PtStyle::Star:
			detail::drawPt( im, img::PtStyle::Plus,  vpt, dp );
			detail::drawPt( im, img::PtStyle::Times, vpt, dp );
		break;

		case img::PtStyle::Diam:
			detail::drawPt( im, img::PtStyle::Plus, vpt, dp, true );
		break;

		case img::PtStyle::Times:      // "times" symbol
			detail::drawPt( im, img::PtStyle::Times, vpt, dp );
		break;

		default: assert(0);
	}
	return true;
}

//------------------------------------------------------------------
/// Draw Lines on Cv::Mat: implementation
/**
Returns false if line is not in image.

Steps:
 -# builds the 4 corner points of the image
 -# build the 4 corresponding lines (borders of the image)
 -# find the intersection points between the line and these 4 lines. Should find 2
 -# draw a line between these 2 points
*/
template<typename LP, typename FPT>
template<typename T>
bool
base::LPBase<LP,FPT>::impl_draw( img::Image<T>& img, img::DrawParams dp, const detail::RootHelper<type::IsLine>& ) const
{
	assert( img.rows() > 2 );
	assert( img.cols() > 2 );

	Point2d_<FPT> pt1; // 0,0
	Point2d_<FPT> pt2( img.cols()-1, img.rows()-1 );
    auto ri = this->intersects( pt1,  pt2 );
    if( ri() )
    {
    	auto ppts = ri.get();
		cv::Point2d ptcv1 = ppts.first.getCvPtd();
		cv::Point2d ptcv2 = ppts.second.getCvPtd();
		cv::line(
			img.getReal(),
			ptcv1,
			ptcv2,
			dp._dpValues.color(),
			dp._dpValues._lineThickness,
			dp._dpValues._lineType==1?cv::LINE_AA:cv::LINE_8
		);
		return true;
	}
	return false;
}

//------------------------------------------------------------------
/// Extension to PolylineBase<PLT,FPT>::draw(), to draw point indexes
/**
(Opencv-dependent)
*/
template<typename PLT,typename FPT>
template<typename T>
void
base::PolylineBase<PLT,FPT>::impl_draw_pl( img::Image<T>& im ) const
{
	for( size_t i=0; i<size(); i++ )
	{
		auto pt = getPoint(i);
		cv::putText(
			im.getReal(),
			std::to_string(i),
			pt.getCvPti(),
			0,
			0.6,
			cv::Scalar( 20,20,20 ),
			2
		);
	}
}


//------------------------------------------------------------------
/// Draw FRect_
template<typename FPT>
template<typename T>
void
FRect_<FPT>::draw( img::Image<T>& img, img::DrawParams dp ) const
{
	cv::rectangle(
		img.getReal(),
		_ptR1.getCvPti(),
		_ptR2.getCvPti(),
		dp._dpValues.color(),
		dp._dpValues._lineThickness,
		dp._dpValues._lineType==1?cv::LINE_AA:cv::LINE_8
	);
}

//------------------------------------------------------------------
/// Draw Segment_
template<typename FPT>
template<typename T>
void
Segment_<FPT>::draw( img::Image<T>& im, img::DrawParams dp ) const
{
	cv::line(
		im.getReal(),
		_ptS1.getCvPtd(),
		_ptS2.getCvPtd(),
		dp._dpValues.color(),
		dp._dpValues._lineThickness,
		dp._dpValues._lineType==1?cv::LINE_AA:cv::LINE_8
	);
	if( dp._dpValues._showPoints )
	{
		_ptS1.draw( im, dp );
		_ptS2.draw( im, dp );
	}
}

//------------------------------------------------------------------
/// Draw Circle_
template<typename FPT>
template<typename T>
void
Circle_<FPT>::draw( img::Image<T>& img, img::DrawParams dp ) const
{
	cv::circle(
		img.getReal(),
		_center.getCvPti(),
		static_cast<int>(_radius),
		dp._dpValues.color(),
		dp._dpValues._lineThickness,
		dp._dpValues._lineType==1?cv::LINE_AA:cv::LINE_8
	);
}

//------------------------------------------------------------------
/// Draw ellipse using Opencv
/**
- see https://docs.opencv.org/3.4/d6/d6e/group__imgproc__draw.html#ga28b2267d35786f5f890ca167236cbc69
*/
template<typename FPT>
template<typename T>
void
Ellipse_<FPT>::draw( img::Image<T>& img, img::DrawParams dp )  const
{
	auto par = p_getParams<HOMOG2D_INUMTYPE>();
	cv::ellipse(
		img.getReal(),
		cv::Point( par.x0,par.y0 ),
		cv::Size( par.a, par.b ),
		par.theta*180./M_PI,
		0., 360.,
		dp._dpValues.color(),
		dp._dpValues._lineThickness,
		dp._dpValues._lineType==1?cv::LINE_AA:cv::LINE_8
	);
}

//------------------------------------------------------------------
#endif // HOMOG2D_USE_OPENCV


/////////////////////////////////////////////////////////////////////////////
// SECTION  - TYPEDEFS
/////////////////////////////////////////////////////////////////////////////

/// Default line type, uses \c double as numerical type
using Line2d = Line2d_<double>;

/// Default point type, uses \c double as numerical type
using Point2d = base::LPBase<type::IsPoint,double>;

/// Default homography (3x3 matrix) type, uses \c double as numerical type
using Homogr = Homogr_<double>;

/// Default homogeneous matrix, uses \c double as numerical type
using Epipmat = Hmatrix_<type::IsEpipmat,double>;

/// Default segment type
using Segment = Segment_<double>;

/// Default circle type
using Circle = Circle_<double>;

/// Default rectangle type
using FRect = FRect_<double>;

/// Default polyline type
using CPolyline = base::PolylineBase<type::IsClosed,double>;
using OPolyline = base::PolylineBase<type::IsOpen,double>;

/// Default ellipse type
using Ellipse = Ellipse_<double>;

// float types
using Line2dF  = Line2d_<float>;
using Point2dF = Point2d_<float>;
using HomogrF  = Homogr_<float>;
using SegmentF = Segment_<float>;
using CircleF  = Circle_<float>;
using FRectF   = FRect_<float>;
using EllipseF = Ellipse_<float>;

// double types
using Line2dD  = Line2d_<double>;
using Point2dD = Point2d_<double>;
using HomogrD  = Homogr_<double>;
using SegmentD = Segment_<double>;
using CircleD  = Circle_<double>;
using FRectD   = FRect_<double>;
using EllipseD = Ellipse_<double>;

// long double types
using Line2dL  = Line2d_<long double>;
using Point2dL = Point2d_<long double>;
using HomogrL  = Homogr_<long double>;
using SegmentL = Segment_<long double>;
using CircleL  = Circle_<long double>;
using FRectL   = FRect_<long double>;
using EllipseL = Ellipse_<long double>;

using CPolylineF = base::PolylineBase<type::IsClosed,float>;
using CPolylineD = base::PolylineBase<type::IsClosed,double>;
using CPolylineL = base::PolylineBase<type::IsClosed,long double>;

using OPolylineF = base::PolylineBase<type::IsOpen,float>;
using OPolylineD = base::PolylineBase<type::IsOpen,double>;
using OPolylineL = base::PolylineBase<type::IsOpen,long double>;

} // namespace h2d end


#endif // HG_HOMOG2D_HPP

