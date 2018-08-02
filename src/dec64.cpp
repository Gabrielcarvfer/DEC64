#include <dec64.h>


Dec64::Dec64(int64 coefficient, int64 exponent,  bool copy) {
    //Copy coefficient into value if copying a value already in Dec64 format
    // or use the coefficient and exponent to create a new number
    // (by default, create a Dec64 integer if neither exponent or copy are modified)
    this->value = !copy ? dec64_new(coefficient, exponent) : coefficient ;
}



Dec64::~Dec64() = default;

Dec64 Dec64::operator+(const Dec64 &a) {
    Dec64 res;
    res.value = dec64_add(this->value, a.value);
    return res;
}

Dec64 Dec64::operator++() {
    Dec64 res;
    res.value = dec64_inc(this->value);
    return res;
}

Dec64 Dec64::operator-(const Dec64 &a) {
    Dec64 res;
    res.value = dec64_subtract(this->value, a.value);
    return res;
}

Dec64 Dec64::operator--() {
    Dec64 res;
    res.value = dec64_dec(this->value);
    return res;
}

Dec64 Dec64::operator*(const Dec64 &a) {
    Dec64 res;
    res.value = dec64_multiply(this->value, a.value);
    return res;
}

Dec64 Dec64::operator/(const Dec64 &a) {
    Dec64 res;
    res.value = dec64_divide(this->value, a.value);
    return res;
}

Dec64 Dec64::operator%(const Dec64 &a) {
    Dec64 res;
    res.value = dec64_modulo(this->value, a.value);
    return res;
}


Dec64 Dec64::operator!() {
    Dec64 res;
    res.value = dec64_not(this->value);
    return res;
}

/*bool Dec64::operator<(const Dec64 &a) {
    return dec64_less(this->value, a.value) == DEC64_ONE;
}*/

Dec64 Dec64::operator<(const Dec64 &a) {
    return Dec64(dec64_less(this->value, a.value),0,true);
}

bool Dec64::operator>(const Dec64 &a) {
    return dec64_less(a.value, this->value) == DEC64_ONE;
}

bool Dec64::operator==(const Dec64 &a) {
    return dec64_equal(this->value, a.value) == DEC64_ONE;
}

bool Dec64::operator!=(const Dec64 &a) {
    return dec64_equal(this->value, a.value) == DEC64_ZERO;
}

bool Dec64::operator>=(const Dec64 &a) {
    return *this>a || *this==a;
}

bool Dec64::operator<=(const Dec64 &a) {
    return  *this==a;// *this<a ||
}

Dec64 Dec64::coefficient() {
    Dec64 res;
    res.value = dec64_coefficient(this->value);
    return res;
}

Dec64 Dec64::exponent() {
    Dec64 res;
    res.value = dec64_exponent(this->value);
    return res;
}

int64 Dec64::coefficient_to_int() {
    return dec64_coefficient(this->value);
}

int64 Dec64::exponent_to_int() {
    return dec64_exponent(this->value);
}

Dec64 Dec64::abs() {
    Dec64 res;
    res.value = dec64_abs(this->value);
    return res;
}

Dec64 Dec64::ceil() {
    Dec64 res;
    res.value = dec64_ceiling(this->value);
    return res;
}

Dec64 Dec64::floor() {
    Dec64 res;
    res.value = dec64_floor(this->value);
    return res;
}

Dec64 Dec64::round(Dec64 places) {
    Dec64 res;
    res.value = dec64_round(this->value, places.value);
    return res;
}

Dec64 Dec64::half() {
    Dec64 res;
    res.value = dec64_half(this->value);
    return res;
}

Dec64 Dec64::set_val(dec64 val) {
    this->value = val;
    return *this;
}

Dec64 Dec64::neg() {
    Dec64 res;
    res.value = dec64_neg(this->value);
    return res;
}

int64 Dec64::to_int(){
    return dec64_int(this->value);

}

Dec64 Dec64::integer_divide(const Dec64 &a){
    Dec64 res;
    res.value = dec64_integer_divide(this->value, a.value);
    return res;
}

bool Dec64::is_zero()
{
    return dec64_is_zero(this->value) == DEC64_ONE;
}

bool Dec64::is_nan()
{
    return dec64_is_any_nan(this->value) == DEC64_ONE;
}

bool Dec64::is_integer()
{
    return dec64_is_integer(this->value) == DEC64_ONE;
}

Dec64 Dec64::signum()
{
    Dec64 res;
    res.value = dec64_signum(this->value);
    return res;
}

Dec64 Dec64::normal()
{
    Dec64 res;
    res.value = dec64_normal(this->value);
    return res;
}