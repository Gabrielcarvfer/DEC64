/* dec64.h

The DEC64 header file. This is the companion to dec64.asm.

dec64.com
2017-06-03
Public Domain

No warranty.
*/

#ifndef DEC64
#define DEC64

#ifdef __cplusplus
extern "C" {
#endif



#define DEC64_NAN           (0x80LL)
#define DEC64_ZERO          (0x00LL)
#define DEC64_ONE           (0x100LL)
#define DEC64_NEGATIVE_ONE  (0xFFFFFFFFFFFFFF00LL)

typedef long long int int64;
typedef unsigned long long uint64;
typedef int64 dec64;

extern int64 dec64_coefficient(dec64 number)                  /* coefficient */;
extern int64 dec64_exponent(dec64 number)                        /* exponent */;

extern dec64 dec64_equal(dec64 comparahend, dec64 comparator)  /* comparison */;
extern dec64 dec64_is_any_nan(dec64 number)                    /* comparison */;
extern dec64 dec64_is_integer(dec64 number)                    /* comparison */;
extern dec64 dec64_is_zero(dec64 number)                       /* comparison */;
extern dec64 dec64_less(dec64 comparahend, dec64 comparator)   /* comparison */;

extern dec64 dec64_abs(dec64 number)                           /* absolution */;
extern dec64 dec64_add(dec64 augend, dec64 addend)                    /* sum */;
extern dec64 dec64_ceiling(dec64 number)                          /* integer */;
extern dec64 dec64_dec(dec64 minuend)                          /* difference */;
extern dec64 dec64_divide(dec64 dividend, dec64 divisor)         /* quotient */;
extern dec64 dec64_floor(dec64 dividend)                          /* integer */;
extern dec64 dec64_half(dec64 dividend)                          /* quotient */;
extern dec64 dec64_inc(dec64 augend)                                  /* sum */;
extern dec64 dec64_int(dec64 number)                              /* integer */;
extern dec64 dec64_integer_divide(dec64 dividend, dec64 divisor) /* quotient */;
extern dec64 dec64_modulo(dec64 dividend, dec64 divisor)       /* modulation */;
extern dec64 dec64_multiply(dec64 multiplicand, dec64 multiplier) /* product */;
extern dec64 dec64_neg(dec64 number)                             /* negation */;
extern dec64 dec64_new(int64 coefficient, int64 exponent)          /* number */;
extern dec64 dec64_normal(dec64 number)                     /* normalization */;
extern dec64 dec64_not(dec64 boolean)                            /* notation */;
extern dec64 dec64_round(dec64 number, dec64 place)          /* quantization */;
extern dec64 dec64_signum(dec64 number)                         /* signature */;
extern dec64 dec64_subtract(dec64 minuend, dec64 subtrahend)   /* difference */;



#ifdef __cplusplus
}
#include <iostream>

class Dec64{
    public:
        Dec64(const int64 coefficient = 0, const int64 exponent=0, const bool copy=false);
        Dec64(std::string);


        ~Dec64();

        Dec64 set_val(const dec64 val);
        Dec64 coefficient() const ;
        Dec64 exponent() const ;
        int64 coefficient_to_int() const ;
        int64 exponent_to_int() const ;
        Dec64 abs() const ;
        Dec64 ceil() const ;
        Dec64 floor() const ;
        Dec64 round(Dec64 places) const ;
        Dec64 half() const ;
        Dec64 neg() const ;
        int64 to_int() const ;
        Dec64 integer_divide(const Dec64 &a) const ;
        bool  is_zero() const ;
        bool  is_nan() const ;
        bool  is_integer() const ;
        Dec64 signum() const ;
        Dec64 normal() const ;



        Dec64 operator!() const ;
        Dec64 operator+(const Dec64& a) const ;
        Dec64 operator++() const ;
        Dec64 operator-(const Dec64& a) const ;
        Dec64 operator--() const ;
        Dec64 operator*(const Dec64& a) const ;
        Dec64 operator/(const Dec64& a) const ;
        Dec64 operator%(const Dec64& a) const ;
        //bool  operator<(const Dec64& a) const ;
        Dec64 operator<(const Dec64 &a) const ;
        bool  operator>(const Dec64& a) const ;
        bool  operator==(const Dec64& a) const ;
        bool  operator!=(const Dec64& a) const ;
        bool  operator<=(const Dec64& a) const ;
        bool  operator>=(const Dec64& a) const ;

        friend std::ostream& operator<<(std::ostream& os, const Dec64& a);



        dec64 value;
};
#endif //__cplusplus

#endif //DEC64
