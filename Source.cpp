/*
   Code addendum for ISO C++ proposal P0631R3
*/
/*
A proposed set of values of π, together with the suggested reference implementation of the templated version.
Hexadeciment π literals were taken from http://www.exploringbinary.com/pi-and-e-in-binary/
*/

namespace std {
#define MATH_CONSTANT_INITIALIZER(LDV,DV,FV)                 \
[] () {                                                      \
   struct dummy                                              \
   {                                                         \
      constexpr operator long double() const {               \
         if constexpr (sizeof(long double) > sizeof(double)) \
            return LDV;                                      \
         else                                                \
            return DV;                                       \
      }                                                      \
      constexpr operator double() const { return DV; }       \
      constexpr operator float () const { return FV; }       \
   };                                                        \
   return dummy();                                           \
} ()

   template<typename T > inline constexpr T piv = MATH_CONSTANT_INITIALIZER(0x1.921fb54442d18469898cc51701b8p+1l,0x1.921fb54442d18p+1,0x1.921fb6p+1f) ;
   inline constexpr float pif = piv<float>;
   inline constexpr double pi = piv<double>;
   inline constexpr long double pil = piv<long double>;

#undef MATH_CONSTANT_INITIALIZER 
}

/*
   High precision floating point type 
   As of March 2018, only data members, 2 constructors and a validator are implemented
*/

#include <limits>
#include <climits>

template <typename N, typename D, typename E, typename F> class floating_t
{
   N m_numerator;
   D m_denomenator;
   E m_exponent;
   static_assert(std::numeric_limits<N>::is_integer);
   static_assert(std::numeric_limits<D>::is_integer);
   static_assert(std::numeric_limits<E>::is_integer);
   static_assert(!std::numeric_limits<F>::is_integer);
   static_assert(std::numeric_limits<N>::is_signed);
   static_assert(!std::numeric_limits<D>::is_signed);
   static_assert(std::numeric_limits<E>::is_signed);
   static_assert(std::numeric_limits<N>::digits == std::numeric_limits<D>::digits - 1);
   static constexpr unsigned exponent_length = CHAR_BIT * sizeof(F) - std::numeric_limits<F>::digits;
   static constexpr unsigned mantissa_length = std::numeric_limits<F>::digits - 1;
   static_assert(CHAR_BIT * sizeof(D) > mantissa_length);
   static_assert(std::numeric_limits<E>::digits >= exponent_length);
   static_assert(std::numeric_limits<N>::digits > std::numeric_limits<F>::digits);
public:
   
   constexpr floating_t(const N& num, const D& den, const E& e) : m_numerator(num), m_denomenator(den), m_exponent(e) {}
   
   constexpr floating_t(F value) :m_numerator(0), m_denomenator(1), m_exponent(std::numeric_limits<F>::max_exponent - 1)
   {
      m_denomenator <<= mantissa_length;
      bool isNegative = false;
      if (value < 0)
      {
         isNegative = true;
         value = -value;
      }
      if (value < 1)
         do
            m_exponent--;
      while ((value = 2 * value) < 1);
      else if (value > 2)
         do
            m_exponent++;
      while ((value = value / 2) > 2);
      m_numerator = static_cast<N>(value*static_cast<F>(m_denomenator));
      if (isNegative)
         m_numerator = -m_numerator;
   }
   
   constexpr bool validate(F value) const
   {//validate that the supposed value of the class is indeed equal to value
      F val = static_cast<F>(m_numerator) / static_cast<F>(m_denomenator);
      for (E i = std::numeric_limits<F>::max_exponent; i <= m_exponent; i++)
         val *= 2;
      return value == val;
   }
   /*
   Many other lines of code
   */
};

using myfptype = floating_t<signed long long, unsigned long long, short, double>;

/*
   Let's instantiate this type through std::pi and through variable template specialization
*/

constexpr myfptype dp_pi (std::pi);

static_assert(dp_pi.validate(std::pi));

/*
These static asserts demostrate that std::pi is accurate only up to the 16th decimal digit.
*/
static_assert(std::pi == 3.141'592'653'589'793'3);
static_assert(std::pi == 3.141'592'653'589'793'0);

/*
This value of pi will have an internal precision of 19 decimal digits, higher than dp_pi
*/
template<> inline constexpr myfptype std::piv<myfptype>{  3'141'592'653'589'793'238, 1'000'000'000'000'000'000, 1 };
[[maybe_unused]] constexpr myfptype hp_pi = std::piv<myfptype> ;

int main() 
{
}

