#pragma once

namespace sge { namespace Math {

	template<class T> constexpr T max(const T& a, const T& b) { return a > b ? a : b; }
	template<class T> constexpr T min(const T& a, const T& b) { return a < b ? a : b; }

	template<class T> constexpr T clamp		(const T& v, const T& a, const T& b) { return max(a, min(v, b)) ; }
	template<class T> constexpr T clamp01	(const T& v) { return clamp(v, T(0), T(1)); }
	template<class T> constexpr T saturate	(const T& v) { return clamp01(v); }

	template<class T> constexpr T byteToK(const T& v) { return v / 1024; }
	template<class T> constexpr T byteToM(const T& v) { return v / (1024 * 1024); }
	template<class T> constexpr T byteToG(const T& v) { return v / (1024 * 1024 * 1024); }

	struct _Helper {
		template<class T>
		static constexpr T alignTo_uint(T n, T a) {
			SGE_STATIC_ASSERT(TypeTraits::isUnsigned<T>::value);
			if (a == 0) return 0;
			T r = n % a;
			return r ? (n + a - r) : n;
		//	return (n + a - 1) / a * a;
		}

		template<class T>
		static constexpr T alignTo_int(T n, T a) {
			SGE_STATIC_ASSERT(TypeTraits::isSigned<T>::value);
			if (a == 0) return 0;
			a = Math::abs(a);

			if (n >= 0) {
				T r = n % a;
				if (r == 0) return n;
				return n + (a - r);
			}
			else {
				T r = (-n) % a;
				if (r == 0) return n;
				return n - (a - r);
			}
		}
	}; // _Helper
	SGE_STATIC_ASSERT_NO_MEMBER_CLASS(_Helper);

	constexpr i8  alignTo(i8  n, i8  a) { return _Helper::alignTo_int(n, a); }
	constexpr i16 alignTo(i16 n, i16 a) { return _Helper::alignTo_int(n, a); }
	constexpr i32 alignTo(i32 n, i32 a) { return _Helper::alignTo_int(n, a); }
	constexpr i64 alignTo(i64 n, i64 a) { return _Helper::alignTo_int(n, a); }

	constexpr u8  alignTo(u8  n, u8  a) { return _Helper::alignTo_uint(n, a); }
	constexpr u16 alignTo(u16 n, u16 a) { return _Helper::alignTo_uint(n, a); }
	constexpr u32 alignTo(u32 n, u32 a) { return _Helper::alignTo_uint(n, a); }
	constexpr u64 alignTo(u64 n, u64 a) { return _Helper::alignTo_uint(n, a); }

//--------
	template< class T > constexpr T		NaN		()				{ return std::numeric_limits<T>::quiet_NaN(); }
	template< class T > constexpr bool	isNaN	( const T& v )	{ return std::isnan(v); }
	
	template < class T > constexpr T	inf		()				{ return std::numeric_limits<T>::infinity(); }
	template < class T > constexpr bool	isInf	( const T& v )	{ return std::numeric_limits<T>::has_infinity && v == inf<T>(); }

//--------
	constexpr u64 nextPow2(u64 x) {
		x -= 1;
		x |= (x >> 1);
		x |= (x >> 2);
		x |= (x >> 4);
		x |= (x >> 8);
		x |= (x >> 16);
		x |= (x >> 32);
		return x + 1;
	}

	constexpr u64 nextPow16(u64 x) {
		x = nextPow2(x);
		while (x % 16 != 0) {
			x *= 2;
		}
		return x;
	}

	constexpr bool isPow2(i64 v) {
		return !(v & (v - 1)) && v;
	}

	template<class T, class ENABLE = std::enable_if_t< std::is_integral_v<T> > >
	constexpr T lowbit(T x) { return x & (-static_cast<i64>(x)); }

	//Splits a floating-point value into fractional and integer parts
	SGE_INLINE float	modf	( float  v, float  *i ) { return std::modf( v, i ); }
	SGE_INLINE double	modf	( double v, double *i ) { return std::modf( v, i ); }

	SGE_INLINE float	frac	( float  v )	{ float  intPart; return modf(v, &intPart); }
	SGE_INLINE double	frac	( double v )	{ double intPart; return modf(v, &intPart); }

	SGE_INLINE float	ceil	( float  a )	{ return std::ceil(a); }
	SGE_INLINE double	ceil	( double a )	{ return std::ceil(a); }

	SGE_INLINE float	floor	( float  a )	{ return std::floor(a); }
	SGE_INLINE double	floor	( double a )	{ return std::floor(a); }

	//get remainder
	SGE_INLINE float	fmod	( float  a, float  b ) { return std::fmod(a,b); }
	SGE_INLINE double	fmod	( double a, double b ) { return std::fmod(a,b); }

	#if SGE_COMPILER_VC | SGE_OS_CYGWIN
		SGE_INLINE float  trunc	( float  n )	{ float  i; return std::modf( n, &i ); }
		SGE_INLINE double trunc	( double n )	{ double i; return std::modf( n, &i ); }

		SGE_INLINE float  round	( float  a )	{ return a > 0 ? floor(a+0.5f) : ceil(a-0.5f); }
		SGE_INLINE double round	( double a )	{ return a > 0 ? floor(a+0.5 ) : ceil(a-0.5 ); }
	#else
		SGE_INLINE float  trunc	( float  n )	{ return std::trunc(n); }
		SGE_INLINE double trunc	( double n )	{ return std::trunc(n); }

		SGE_INLINE float  round	( float  a )	{ return std::round(a); }
		SGE_INLINE double round	( double a )	{ return std::round(a); }
	#endif

	SGE_INLINE int	truncToInt	( float  a )	{ return static_cast<int>( trunc(a) ); }
	SGE_INLINE int	truncToInt	( double a )	{ return static_cast<int>( trunc(a) ); }

	SGE_INLINE int	roundToInt	( float  a )	{ return static_cast<int>( round(a) ); }
	SGE_INLINE int	roundToInt	( double a )	{ return static_cast<int>( round(a) ); }

	SGE_INLINE int	floorTo_Int	( float  a )	{ return static_cast<int>( floor(a) ); }
	SGE_INLINE int	floorTo_Int	( double a )	{ return static_cast<int>( floor(a) ); }

	SGE_INLINE int	ceilToInt	( float  a )	{ return static_cast<int>( ceil (a) ); }
	SGE_INLINE int	ceilToInt	( double a )	{ return static_cast<int>( ceil (a) ); }

//----------

#if 0
#pragma mark ------ Trigonometry ------
#endif
	template<class T> constexpr T	PI		()	{ return static_cast<T>(3.14159265358979323846); }
	template<class T> constexpr T	twoPI	()	{ return PI<T>() * 2; }
	template<class T> constexpr T	halfPI	()	{ return PI<T>() * 0.5; }

	template<class T> SGE_INLINE T	radians	(T deg) { return deg * (PI<T>() / static_cast<T>(180)); }
	template<class T> SGE_INLINE T	degrees	(T rad) { return rad * (static_cast<T>(180) / PI<T>()); }

	SGE_INLINE float  sin(float  rad) { return ::sinf(rad); }
	SGE_INLINE double sin(double rad) { return ::sin (rad); }

	SGE_INLINE float  cos(float  rad) { return ::cosf(rad); }
	SGE_INLINE double cos(double rad) { return ::cos (rad); }

	SGE_INLINE float  tan(float  rad) { return ::tanf(rad); }
	SGE_INLINE double tan(double rad) { return ::tan (rad); }

	SGE_INLINE float  atan2(float  a, float  b) { return ::atan2f(a, b); }
	SGE_INLINE double atan2(double a, double b) { return ::atan2 (a, b); }

	SGE_INLINE float  asin(float  rad) { return ::asinf(rad); }
	SGE_INLINE double asin(double rad) { return ::asin (rad); }

	SGE_INLINE float  acos(float  rad) { return ::acosf(rad); }
	SGE_INLINE double acos(double rad) { return ::acos(rad); }

#if SGE_OS_MACOSX
	SGE_INLINE void sincos( float  rad, float  & out_sin, float  & out_cos ) { ::__sincosf(rad, &out_sin, &out_cos); }
	SGE_INLINE void sincos( double rad, double & out_sin, double & out_cos ) { ::__sincos (rad, &out_sin, &out_cos); }
#elif SGE_OS_LINUX
	SGE_INLINE void sincos( float  rad, float  & out_sin, float  & out_cos ) { ::sincosf(rad, &out_sin, &out_cos); }
	SGE_INLINE void sincos( double rad, double & out_sin, double & out_cos ) { ::sincos (rad, &out_sin, &out_cos); }
#else
	SGE_INLINE void sincos( float  rad, float  & out_sin, float  & out_cos ) { out_sin = sin(rad); out_cos = cos(rad); }
	SGE_INLINE void sincos( double rad, double & out_sin, double & out_cos ) { out_sin = sin(rad); out_cos = cos(rad); }
#endif


#if 0
#pragma mark ----------------
#endif

	template<class T> T abs(const T& v) { return v < 0 ? -v : v; }

	template<class T> constexpr T	 epsilon();
	template<>		  constexpr int	 epsilon<int >() { return 0; }
	template<>		  constexpr f32	 epsilon<f32 >() { return FLT_EPSILON; }
	template<>		  constexpr f64	 epsilon<f64 >() { return DBL_EPSILON; }
	template<>		  constexpr f128 epsilon<f128>() { return LDBL_EPSILON; }

	template<class T, class EP = T> SGE_INLINE constexpr bool equals (const T& a, const T& b, const EP& ep = epsilon<T>()) { return abs(a-b) <= ep; }
	template<class T, class EP = T> SGE_INLINE constexpr bool equals0(const T& a,             const EP& ep = epsilon<T>()) { return abs( a ) <= ep; }

//-------------- Lerp ----------------
//! linear interpolation out = a+w*(b-a)

	template<class T, class W>
	SGE_INLINE T	lerp(const T& a, const T& b, const W& w );

	template<class T, class ENABLE = std::enable_if_t< std::is_floating_point_v<T> > >
	SGE_INLINE T	lerp(T a, T b, T w ) { return (1-w)*a + w*b; }

	template<class T, class ENABLE = std::enable_if_t< std::is_integral_v<T> > >
	SGE_INLINE T	lerp(T a, T b, double w ) {
		double a_ = static_cast<double>(a);
		double b_ = static_cast<double>(b);
		return static_cast<T>(lerp<double>(a_, b_, w));
	}

	template<>
	SGE_INLINE float lerp(const float& a, const float& b, const double& w) {
		return static_cast<float>((1 - w) * a + w * b);
	}

	//---------
	SGE_INLINE float	sqrt(float  n) { return std::sqrt(n); }
	SGE_INLINE double	sqrt(double n) { return std::sqrt(n); }
	SGE_INLINE int		sqrt(int    n) { return static_cast<int>(std::sqrt(static_cast<double>(n))); }

	SGE_INLINE float	cbrt(float  n) { return std::cbrt(n); }
	SGE_INLINE double	cbrt(double n) { return std::cbrt(n); }
	SGE_INLINE int		cbrt(int    n) { return static_cast<int>(std::cbrt(static_cast<double>(n))); }

	//------- reciprocal square root ---------------
	SGE_INLINE float rsqrt_fast(float n) {
	#if SGE_CPU_FEATURE_SSE2
		return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(n)));
	#else // https://en.wikipedia.org/wiki/Fast_inverse_square_root
		uint32_t i;
		float x2, y;
		const float threehalfs = 1.5f;

		x2 = n * 0.5f;
		y = n;
		i = *reinterpret_cast<uint32_t *>(&y);
		i = 0x5F375A86UL - (i >> 1);
		y = *reinterpret_cast<float *>(&i);
		y = y * (threehalfs - (x2 * y * y));
		//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
		return y;
	#endif // SGE_CPU_FEATURE_SSE2
	}

	SGE_INLINE double rsqrt_fast(double n) {
		uint64_t i;
		double x2, y;
		const float threehalfs = 1.5;

		x2 = n * 0.5f;
		y = n;
		i = *reinterpret_cast<uint64_t *>(&y);
		i = 0x5FE6EB50C7B537A9ULL - (i >> 1);
		y = *reinterpret_cast<double *>(&i);
		y = y * (threehalfs - (x2 * y * y));
		//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
		return y;
	}

	SGE_INLINE int rsqrt_fast(int n) {
		double d = static_cast<double>(n);
		return roundToInt(rsqrt_fast(d));
	}

	SGE_INLINE float rsqrt(float v) {
	#if 0 //SGE_CPU_FEATURE_SSE2
		float  xrsqrt_est = rsqrt_fast(v);
		return xrsqrt_est * (1.5f - v * 0.5f * xrsqrt_est * xrsqrt_est); // NR iteration
	#else
		return 1.0f / sqrt(v); 
	#endif
	}

	SGE_INLINE double rsqrt(double v) { return 1.0 / sqrt(v); }
	SGE_INLINE int    rsqrt(int    v) { return roundToInt(rsqrt(static_cast<double>(v))); }

}} // namespace sge/Math