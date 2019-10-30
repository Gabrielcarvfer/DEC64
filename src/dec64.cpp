//
// Created by Gabriel Ferreira (@gabrielcarvfer) on 31-Jul-18.
//

#include "dec64.h"
#include <cstring>
#include <string>
#include <sstream>
#include <cmath>

Dec64::Dec64(const int64 coefficient, const int64 exponent, const bool copy) {
    //Copy coefficient into value if copying a value already in Dec64 format
    // or use the coefficient and exponent to create a new number
    // (by default, create a Dec64 integer if neither exponent or copy are modified)
    this->value = !copy ? dec64_new(coefficient, exponent) : coefficient ;
}

inline void extract_coefficient (int64 * coefficient, int64 * exponent, std::string coeff_str)
{
    std::size_t found2 = coeff_str.find('.');
    if (found2 != std::string::npos)
    {
        //coefficient is decimal

        //len(tok) - found2+1 has number of decimal places
        *exponent -= coeff_str.length()-(found2+1);

        //concatenate the entire number to get an integer
        std::stringstream ss;
        ss << coeff_str.substr(0,found2) << coeff_str.substr(found2+1,coeff_str.length());
        *coefficient = std::stoll(ss.str());
    }
    else
    {
        //coefficient is not decimal
        *coefficient = std::stoll(coeff_str);
    }
}

inline void extract_exponent(int64 * coefficient, int64 * exponent, std::string exp_str)
{
    std::size_t found2 = exp_str.find('.');
    if (found2 != std::string::npos)
    {
        //exponent is decimal

        //add integer part of the exponent
        int64 exp = std::stoll(exp_str.substr(0,found2));
        *exponent += exp;

        //extract decimal part of the exponent
        double exponentd = std::stod(exp_str.substr(found2+1,exp_str.length()))/(10*(exp_str.length()-(found2+1)));

        exponentd = (exp_str.at(0)!='-') ? exponentd : -exponentd;

        //multiply the coefficient with the opposite of the remainer (to get rid of decimal part of the exponent)
        double coefficientd =  (double) *coefficient * std::pow(10, exponentd);

        //try to salvage numbers increasing the exponent exactly like we did before
        std::string coeff_str = std::to_string(coefficientd);

        extract_coefficient(coefficient, exponent, coeff_str);
    }
    else
    {
        //exponent is not decimal
        *exponent += std::stoll(exp_str);
    }
}

Dec64::Dec64(std::string a)
{
    std::size_t found = a.find("nan");
    if (found != std::string::npos)
    {
        //nan
        this->value = DEC64_NAN;
        return;
    }

    found = a.find("e");
    if (found != std::string::npos)
    {
        //has an exponent
        int64 coefficient = 0, exponent = 0;

        //assume double
        auto str = (char *) a.c_str();
        char *tok = strtok(str, "e");

        std::string coeff_str(tok);
        //check if coefficient is decimal and extract value
        extract_coefficient(&coefficient, &exponent, coeff_str);

        //now to the exponent
        tok = strtok(NULL, "");

        std::string exp_str(tok);
        //check if exponent is decimal and extract value
        extract_exponent(&coefficient, &exponent, exp_str);

        //create dec64
        this->value = dec64_new(coefficient, exponent);

        return;
    }

    //assume integer or double without exponent
    found = a.find(".");
    if (found != std::string::npos)
    {
        //assume double

        int64 coefficient = 0, exponent = 0;
        extract_coefficient(&coefficient, &exponent, a);
        this->value = dec64_new(coefficient,exponent);

    }
    else
    {
        //no 'e' or '.'
        //assume long long
        int64 coefficient = std::stoll(a);

        this->value = dec64_new(coefficient,0);

    }
}

Dec64::~Dec64() = default;

Dec64 Dec64::operator+(const Dec64 &a) const {
    Dec64 res( dec64_add(this->value, a.value), 0, true);
    return res;
}

Dec64 Dec64::operator++() const {
    Dec64 res( dec64_inc(this->value), 0, true);
    return res;
}

Dec64 Dec64::operator-(const Dec64 &a) const {
    Dec64 res( dec64_subtract(this->value, a.value), 0, true);
    return res;
}

Dec64 Dec64::operator--() const {
    Dec64 res( dec64_dec(this->value), 0, true);
    return res;
}

Dec64 Dec64::operator*(const Dec64 &a) const {
    Dec64 res( dec64_multiply(this->value, a.value), 0, true);
    return res;
}

Dec64 Dec64::operator/(const Dec64 &a) const {
    Dec64 res( dec64_divide(this->value, a.value), 0, true);
    return res;
}

Dec64 Dec64::operator%(const Dec64 &a) const {
    Dec64 res( dec64_modulo(this->value, a.value), 0, true);
    return res;
}


Dec64 Dec64::operator!() const {
    Dec64 res( dec64_not(this->value), 0, true);
    return res;
}

/*bool Dec64::operator<(const Dec64 &a) const {
    return dec64_is_less(this->value, a.value) == DEC64_ONE;
}*/

Dec64 Dec64::operator<(const Dec64 &a) const {
    return Dec64( dec64_is_less(this->value, a.value),0,true);
}

bool Dec64::operator>(const Dec64 &a) const {
    return dec64_is_less(a.value, this->value) == DEC64_ONE;
}

bool Dec64::operator==(const Dec64 &a) const {
    return dec64_is_equal(this->value, a.value) == DEC64_ONE;
}

bool Dec64::operator!=(const Dec64 &a) const {
    return dec64_is_equal(this->value, a.value) == DEC64_ZERO;
}

bool Dec64::operator>=(const Dec64 &a) const {
    return *this>a || *this==a;
}

bool Dec64::operator<=(const Dec64 &a) const {
    return  *this==a;// *this<a ||
}

Dec64 Dec64::coefficient() const {
    Dec64 res( dec64_coefficient(this->value), 0, true);
    return res;
}

Dec64 Dec64::exponent() const {
    Dec64 res( dec64_exponent(this->value), 0, true);
    return res;
}

int64 Dec64::coefficient_to_int() const {
    return dec64_coefficient(this->value);
}

int64 Dec64::exponent_to_int() const {
    return dec64_exponent(this->value);
}

Dec64 Dec64::abs() const {
    Dec64 res( dec64_abs(this->value), 0, true);
    return res;
}

Dec64 Dec64::ceil() const {
    Dec64 res( dec64_ceiling(this->value), 0, true);
    return res;
}

Dec64 Dec64::floor() const {
    Dec64 res( dec64_floor(this->value), 0, true);
    return res;
}

Dec64 Dec64::round(const Dec64 places) const {
    Dec64 res( dec64_round(this->value, places.value), 0, true);
    return res;
}

Dec64 Dec64::half() const {
    Dec64 res( dec64_half(this->value), 0, true);
    return res;
}

Dec64 Dec64::set_val(const dec64 val) {
    this->value = val;
    return *this;
}

Dec64 Dec64::neg() const {
    Dec64 res( dec64_neg(this->value), 0, true);
    return res;
}

int64 Dec64::to_int() const {
    return dec64_int(this->value);

}

Dec64 Dec64::integer_divide(const Dec64 &a) const {
    Dec64 res( dec64_integer_divide(this->value, a.value), 0, true);
    return res;
}

bool Dec64::is_zero() const {
    return dec64_is_zero(this->value) == DEC64_ONE;
}

bool Dec64::is_nan() const {
    return dec64_is_nan(this->value) == DEC64_ONE;
}

bool Dec64::is_integer() const {
    return dec64_is_integer(this->value) == DEC64_ONE;
}

Dec64 Dec64::signum() const {
    Dec64 res( dec64_signum(this->value), 0, true);
    return res;
}

Dec64 Dec64::normal() const {
    Dec64 res( dec64_normal(this->value), 0, true);
    return res;
}

std::ostream& operator<<(std::ostream& os, const Dec64& a){
    int64 coeff = a.coefficient_to_int();
    int64 exp   = a.exponent_to_int();

    if (coeff == 0 && exp != -128)
    {
        os << "0";
    }
    else if (coeff == 0 && exp == -128)
    {
        os << "nan";
    }
    else if (coeff != 0 && exp == 0)
    {
        os << coeff;
    }
    else
    {
        os << coeff << 'e' << exp;

    }
    return os;
}
