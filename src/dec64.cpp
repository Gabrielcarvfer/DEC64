#include <dec64.h>
#include <cstring>
#include<string>

Dec64::Dec64(const int64 coefficient, const int64 exponent, const bool copy) {
    //Copy coefficient into value if copying a value already in Dec64 format
    // or use the coefficient and exponent to create a new number
    // (by default, create a Dec64 integer if neither exponent or copy are modified)
    this->value = !copy ? dec64_new(coefficient, exponent) : coefficient ;
}

Dec64::Dec64(std::string a)
{
    std::size_t found = a.find("nan");
    if (found != std::string::npos)
    {
        //nan
        return;
    }


    found = a.find("e");
    if (found != std::string::npos)
    {
        //has an exponent
        found = a.find(".");
        if (found != std::string::npos)
        {
            //assume double

            normalize to long long 
        }
        else
        {
            //assume long long
            char *str = (char *) a.c_str();
            char *tok = strtok(str, "e");
            int64 coefficient, exponent;
            coefficient = std::stoll(tok);
            tok = strtok(NULL, "e");
            exponent = std::stoll(tok);

            this->value = dec64_new(coefficient, exponent);

        }
        return;
    }

    //assume integer or double without exponent
    found = a.find(".");
    if (found != std::string::npos)
    {
        //assume double

        //Normalize to long long
    }
    else
    {
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
    return dec64_less(this->value, a.value) == DEC64_ONE;
}*/

Dec64 Dec64::operator<(const Dec64 &a) const {
    return Dec64( dec64_less(this->value, a.value),0,true);
}

bool Dec64::operator>(const Dec64 &a) const {
    return dec64_less(a.value, this->value) == DEC64_ONE;
}

bool Dec64::operator==(const Dec64 &a) const {
    return dec64_equal(this->value, a.value) == DEC64_ONE;
}

bool Dec64::operator!=(const Dec64 &a) const {
    return dec64_equal(this->value, a.value) == DEC64_ZERO;
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
    return dec64_is_any_nan(this->value) == DEC64_ONE;
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
