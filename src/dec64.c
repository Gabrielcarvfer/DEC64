//
// Created by Gabriel Ferreira (@gabrielcarvfer) on 04-Aug-18.
//
#include <dec64.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#define MAXNUM 0x007FFFFFFFFFFFFF
#define MAXEXP 0x007F

static int64 powers10[] = {(int64) 1,                       // 10^00
                           (int64) 10,                      // 10^01
                           (int64) 100,                     // 10^02
                           (int64) 1000,                    // 10^03
                           (int64) 10000,                   // 10^03
                           (int64) 100000,                  // 10^04
                           (int64) 1000000,                 // 10^05
                           (int64) 10000000,                // 10^06
                           (int64) 100000000,               // 10^07
                           (int64) 1000000000,              // 10^08
                           (int64) 10000000000,             // 10^09
                           (int64) 100000000000,            // 10^10
                           (int64) 1000000000000,           // 10^11
                           (int64) 10000000000000,          // 10^12
                           (int64) 100000000000000,         // 10^13
                           (int64) 1000000000000000,        // 10^13
                           (int64) 10000000000000000,       // 10^14
                           (int64) 100000000000000000,      // 10^15
                           (int64) 1000000000000000000,     // 10^16
                           (int64) 10000000000000000000,     // 10^17
                           (int64) 100000000000000000000,     // 10^18
                           (int64) 1000000000000000000000,     // 10^19


                           };

int64 dec64_coefficient(int64 number)
{
    //shift guarantees signal bit
    return number >> 8;
}

int64 dec64_exponent(int64 number)
{
    //two casts to extend signal instead of messing with masks
    int8_t val = (uint8_t) number;
    return (int64) val;
}

int64 dec64_build(int64 coeff, int64 exp)
{
    return (coeff << 8) | (0x0FF & exp);
}




/*
dec64_round: function_with_two_parameters
;(number: dec64, place: dec64) returns quantization: dec64

; The place argument indicates at what decimal place to round.
;    -2        nearest cent
;     0        nearest integer
;     3        nearest thousand
;     6        nearest million
;     9        nearest billion

; The place should be between -16 and 16.

mov     r11,r1          ; preserve the number
        mov     r1,r2           ; pass the place
        call_with_one_parameter dec64_int
        mov     r9,r0           ; r9 is the normalized place
        cmp     r11_b,128       ; is the first operand not nan?
setne   r1_b
sar     r9,8            ; r9 is the place as int64
cmp     r0_b,128        ; is the second operand nan?
setne   r1_h
movsx   r8,r11_b        ; r8 is the current exponent
        mov     r0,r11          ; r0 is the number
test    r1_b,r1_h       ; is either nan?
jz      return_nan      ; %if so, the result is nan

sar     r0,8            ; r0 is the coefficient
jz      return          ; %if the coefficient is zero, the result is zero
        cmp     r8,r9           ; compare the exponents
        jge     pack            ; no rounding required
        mov     r2,r0           ; r2 is the coefficient
neg     r2              ; rs is the coefficient negated
        cmovns  r0,r2           ; r0 is absolute value of the coefficient
        mov     r10,eight_over_ten ; magic
        pad

round_loop:

; Increment the exponent and divide the coefficient by 10 until the target
; exponent is reached.

mul     r10             ; r2 is the coefficient * 8 / 10
mov     r0,r2           ; r0 is the coefficient * 8 / 10
shr     r0,3            ; r0 is the coefficient / 10

add     r8,1            ; increment the exponent
        cmp     r8,r9           ; compare the exponents
        jne     round_loop      ; loop %if the exponent has not reached the target

; Round %if necessary and return the result.

shr     r2,2            ; Isolate the carry bit
and     r2,1            ; r2 is 1 %if rounding is needed
        add     r0,r2           ; r0 is rounded
        mov     r2,r0           ; r2 is the result
neg     r2              ; r2 is the result negated
        test    r11,r11         ; was the original number negative
        cmovs   r0,r2           ; %if so, use the negated result
jmp     pack            ; pack it up

        pad; -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
*/

int64 dec64_pack(int64 coeff, int64 exp)
{
    //Zero is zero
    if (coeff == 0 && exp != -128)
        return 0;

    //Nan is nan
    if (coeff == 0 && exp == -128)
        return DEC64_NAN;

    int negc = coeff < 0;
    int64 maxval = negc ? MAXNUM+1 : MAXNUM;
    coeff = negc ? -coeff : coeff;

    int64 original_exp = exp;
    int64 original_coeff = coeff;

    int round=0;

    //Pack procedures
    while (1)
    {
        //equivalent to pack_decrease
        if (exp > 127)
        {
            int64 tcoeff = coeff * 10;

            if (tcoeff < 0)             //If overflows, nan
                return DEC64_NAN;
            coeff = tcoeff;             //Otherwise, continue
            exp--;
            continue;
        }

        //equivalent to pack_increase and
        //equivalent to pack_large
        if(coeff > maxval || exp < -127)
        {
            //Couldn't save the value
            if (exp == 127)
                return DEC64_NAN;
            //No more divisions to make
            if (coeff == 0)
                break;
            coeff /= 10;
            exp++;
            continue;
        }

        //If doesn't need any procedure, break loop and procede
        break;
    }

    //round properly - may overflow maxval
    int muls = exp - original_exp;
    if (muls > 0 && muls <= 19)
    {
        int64 tcoeff = coeff;
        int64 temp2 = original_coeff / powers10[muls-1];
        coeff = coeff + ((temp2 % 10 >= 5) ? 1 : 0);

        if (coeff > maxval)
        {
            round = coeff % 10;
            coeff += (round >= 5 && coeff > 10) ? 10 - round : 0;
            coeff /= 10;
            exp++;
        }

        //Verify the distance between rounded and non-rounded numbers
        //if ( (original_coeff - coeff) > (original_coeff - tcoeff) )
        //    coeff = tcoeff;
    }

    if (coeff == 0)
        return 0;


    coeff = negc ? -coeff : coeff;
    return dec64_build(coeff, exp);
}


int64 dec64_new(int64 coeff, int64 exp)
{
    return dec64_pack(coeff, exp);
}

int checkPowerOf10(int64 coeff)
{
    for (int i = 0; i < 20; i++)
    {
        if (coeff < powers10[i])
            return i;
    }
    return 0;
}

int64 dec64_add_proc(int64 augend, int64 addend, int subtraction)
{
    int8_t exp1, exp2;
    exp1 = (int8_t) augend;
    exp2 = (int8_t) addend;

    //If exponent is nan
    if (exp1 == -128 || exp2 == -128)
        return DEC64_NAN;

    //If not, we continue
    int64 coeff1, coeff2;
    coeff1 = augend>>8;
    coeff2 = addend>>8;

    //If one of the coefficients is zero, return the other argument
    if (coeff1 == 0)
        return coeff2 == 0 ? 0 : subtraction ? dec64_pack(-coeff2, exp2): addend; // << 8 | 0x0FF & exp2 : addend;
    if (coeff2 == 0)
        return coeff1 == 0 ? 0 : augend;


    //If the difference between exponents is bigger than 17, the bigger number isn't affected,
    // unless we're working on a subtraction and the second parameter is being returned
    if(exp1-exp2 > 17)
    {
        //Before returning, check if the second value can be truncated (n divisions by 10 makes it fit)
        int k = checkPowerOf10(coeff2);
        if (exp1-exp2-k > 17)
            return augend;
    }
    if(exp2-exp1 > 17)
        return (subtraction ? -coeff2 << 8 | 0x0FF & exp2 : addend);

    //Invert arguments and keep the higher exponent on coeff1|exp1
    int inverted = 0;
    if(exp1 < exp2)
    {
        int64 temp = exp1;
        exp1 = exp2;
        exp2 = temp;
        temp = coeff1;
        coeff1 = coeff2;
        coeff2 = temp;
        inverted = 1;
    }

    //Convert coefficient to positive and save flags
    int neg1 = coeff1 < 0;
    int neg2 = coeff2 < 0;
    coeff1 = neg1 ? -coeff1 : coeff1;
    coeff2 = neg2 ? -coeff2 : coeff2;

    //Adjust numbers
    int pow10coeff1 = checkPowerOf10(coeff1);
    int pow10coeff2 = checkPowerOf10(coeff2);

    //Align mantissas through the exponents until
    //equivalent to add slower_decrease and add_slower
    int64   tcoeff1 = coeff1 * 10;
    int8_t  texp1 = exp1-1;
    while( (tcoeff1 < MAXNUM || ( subtraction & (!neg1&!neg2) | (neg1&!neg2) ) ) && exp1 > exp2 && pow10coeff1 < 18)
    {
        coeff1 = tcoeff1;
        exp1 = texp1;
        tcoeff1 *= 10;
        texp1--;
        pow10coeff1++;
    }

     //divide mantissa and increase exponent of addend
    int expdiff = exp1-exp2;

    //while (expdiff > 17)
    if (expdiff > 17)
        return dec64_pack(coeff1, exp1);

    if (expdiff > 0)
    {
        int64 tcoeff2, original_coeff2;
        tcoeff2 = original_coeff2 = coeff2;

        coeff2 /= powers10[expdiff-1];
        int round = coeff2 % 10;
        coeff2 += (round >= 5) ? 10 - round : 0;
        coeff2 /= 10;
        exp2 = exp1;

        /* Proper rounding to nearest number?
        tcoeff2 /= powers10[expdiff];

        int64 diffNotRounded = tcoeff2*powers10[expdiff] - original_coeff2;
        int64 diffRounded = coeff2*powers10[expdiff] - original_coeff2;
        diffNotRounded = diffNotRounded < 0 ? -diffNotRounded : diffNotRounded;
        diffRounded = diffRounded < 0 ? -diffRounded : diffRounded;

        if (diffNotRounded < diffRounded)
            coeff2 = tcoeff2;
        */

    }

    if(coeff2 == 0)
        return inverted? addend : augend;

    coeff1 = neg1? -coeff1 : coeff1;
    coeff2 = neg2? -coeff2 : coeff2;
    
    //Convert mantissas back to the original sign and sum
    int64 result = 0;
    if (subtraction)
        result = inverted ? coeff2 - coeff1 : coeff1 - coeff2;
    else
        result = coeff1 + coeff2;

    //Try to fit and return value
    return dec64_pack(result, exp1);

}

int64 dec64_inc(int64 augend)
{
    int64 exp = (int8_t) augend;

    //A nan is a nan
    if (exp == -128)
        return DEC64_NAN;

    //If exponents are not equal, resort to ADD
    if (exp != 0)
        return dec64_add(augend, DEC64_ONE);

    //If exponents are equal, then add 1 and adjust to fit if necessary
    int64 coeff = augend>>8;

    //Increment
    coeff++;

    //In case the result is bigger than the limit, try to reduce
    return dec64_pack(coeff, exp);
}

int64 dec64_dec(int64 minuend)
{
    int64 exp = (int8_t) minuend;

    //A nan is a nan
    if (exp == -128)
        return DEC64_NAN;

    //If exponents are not equal, resort to ADD
    if (exp != 0)
        return dec64_subtract(minuend, DEC64_ONE);

    //If exponents are equal, then add 1 and adjust to fit if necessary
    int64 coeff = minuend>>8;

    //Decrement
    coeff--;

    //In case the result is bigger than the limit, try to reduce
    return dec64_pack(coeff, exp);
}

int64 dec64_add(int64 augend, int64 addend)
{
    return dec64_add_proc(augend, addend, 0);
}

int64 dec64_subtract(int64 minuend, int64 subtrahend)
{
    return dec64_add_proc( minuend, subtrahend, 1);
}
/*

dec64_ceiling: function_with_one_parameter
;(number: dec64) returns integer: dec64

; Produce the smallest integer that is greater than or equal to the number. In
; the result, the exponent will be greater than or equal to zero unless it is
; nan. Numbers with positive exponents will not be mod%ified, even %if the
; numbers are outside of the safe integer range.

; Preserved: r11.

mov     r9,1            ; r9 is the round up flag
jmp     floor_begin

pad; -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

dec64_floor: function_with_one_parameter
;(number: dec64) returns integer: dec64

; Produce the largest integer that is less than or equal to the number. This
; is sometimes called the entier function. In the result, the exponent will be
; greater than or equal to zero unless it is nan. Numbers with positive
; exponents will not be mod%ified, even %if the numbers are outside of the safe
; integer range.

; Preserved: r11.

mov     r9,-1           ; r9 is the round down flag
pad

        floor_begin:

cmp     r1_b,128        ; compare the exponent to nan
        je      return_nan       ; %if the exponent is nan, the result is nan
        mov     r0,r1           ; r0 is the number
movsx   r8,r1_b         ; r8 is the exponent
sar     r0,8            ; r0 is the coefficient
cmovz   r1,r0           ; %if the coefficient is zero, the number is zero
        neg     r8              ; r8 is the negated exponent
        test    r1_b,r1_b       ; examine the exponent
        jns     return_r1       ; nothing to do unless the exponent was negative
        cmp     r8,17           ; is the exponent is too extreme?
jae     floor_micro     ; deal with a micro number
        mov     r10,power
        mov     r10,[r10+r8*8] ; r10 is the power of ten
cqo                     ; sign extend r0 into r2
        idiv    r10             ; divide r2:r0 by 10
test    r2,r2           ; examine the remainder
        jnz     floor_remains   ; deal with the remainder
shl     r0,8            ; pack the coefficient
        ret
pad

        floor_micro:

mov     r2,r0           ; r2 is the coefficient
xor     r0,r0           ; r0 is zero
        pad

floor_remains:

; %if the remainder is negative and the rounding flag is negative, then we need
; to decrement r0. But %if the remainder and the rounding flag are both
; positive, then we need to increment r0.

xor     r10,r10         ; r10 is zero
        xor     r2,r9           ; xor the remainder and the rounding
cmovs   r9,r10          ; %if they had d%ifferent signs, clear the rounding
add     r0,r9           ; add the rounding to the coefficient
shl     r0,8            ; pack the coefficient
        ret

pad; -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --


dec64_multiply: function_with_two_parameters
;(multiplicand: dec64, multiplier: dec64) returns product: dec64

; Multiply two dec64 numbers together.

; Unpack the exponents into r8 and r9.

movsx   r8,r1_b         ; r8 is the first exponent
        movsx   r9,r2_b         ; r9 is the second exponent

; Set flags in r0 %if either operand is nan.

cmp     r1_b,128        ; is the first operand nan?
sete    r0_b            ; r0_b is 1 %if the first operand is nan
        cmp     r2_b,128        ; is the second operand nan?
sete    r0_h            ; r0_h is 1 %if the second operand is nan

; Unpack the coefficients. Set flags in r1 %if either is not zero.

sar     r1,8            ; r1 is the first coefficient
        mov     r10,r1          ; r10 is the first coefficient
        setnz   r1_b            ; r1_b is 1 %if the first coefficient is not zero
sar     r2,8            ; r2 is the second coefficient
        setnz   r1_h            ; r1_h is 1 %if the second coefficient is not zero

; The result is nan %if one or both of the operands is nan and neither of the
; operands is zero.

or      r1_w,r0_w       ; is either coefficient zero and not nan?
xchg    r1_b,r1_h
test    r0_w,r1_w
jnz     return_nan

mov     r0,r10          ; r0 is the first coefficient
        add     r8,r9           ; r8 is the product exponent
        imul    r2              ; r2:r0 is r1 * r2
jno     pack            ; %if the product fits in 64 bits, start packing

; There was overflow.

; Make the 110 bit coefficient in r2:r0Er8 all fit. Estimate the number of
; digits of excess, and increase the exponent by that many digits.
; We use 77/256 to convert log2 to log10.

mov     r9,r2           ; r9 is the excess sign%ificance
        xor     r1,r1           ; r1 is zero anticipating bsr
        neg     r9
        cmovs   r9,r2           ; r9 is absolute value of the excess sign%ificance

        bsr     r1,r9           ; find the position of the most sign%ificant bit
imul    r1,77           ; multiply the bit number by 77/256 to
        shr     r1,8            ;     convert a bit number to a digit number
add     r1,2            ; add two extra digits to the scale
        add     r8,r1           ; increase the exponent
        mov     r9,power
        idiv    qword [r9+r1*8] ; divide by the power of ten
jmp     pack

pad; -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

dec64_divide: function_with_two_parameters
;(dividend: dec64, divisor: dec64) returns quotient: dec64

; Divide a dec64 number by another.

; Begin unpacking the components.

movsx   r8,r1_b         ; r8 is the first exponent
        movsx   r9,r2_b         ; r9 is the second exponent
        mov     r10,r1          ; r10 is the first number
        mov     r11,r2          ; r11 is the second number

; Set nan flags in r0.

cmp     r1_b,128        ; is the first operand nan?
sete    r0_b            ; r0_b is 1 %if the first operand is nan
        cmp     r2_b,128        ; is the second operand nan?
sete    r0_h            ; r0_h is 1 %if the second operand is nan

        sar     r10,8           ; r10 is the dividend coefficient
        setnz   r1_b            ; r1_b is 1 %if the dividend coefficient is zero
        sar     r11,8           ; r11 is the divisor coefficient
        setz    r1_h            ; r1_h is 1 %if dividing by zero
        or      r0_h,r0_b       ; r0_h is 1 %if either is nan
        or      r1_b,r0_b       ; r1_b is zero %if the dividend is zero and not nan
        jz      return_zero     ; %if the dividend is zero, the quotient is zero
        sub     r8,r9           ; r8 is the quotient exponent
        or      r0_b,r1_h       ; r0_b is 1 %if either is nan or the divisor is zero
jnz     return_nan
pad

        divide_measure:

; We want to get as many bits into the quotient as possible in order to capture
; enough sign%ificance. But %if the quotient has more than 64 bits, then there
; will be a hardware fault. To avoid that, we compare the magnitudes of the
; dividend coefficient and divisor coefficient, and use that to scale the
; dividend to give us a good quotient.

mov     r0,r10          ; r0 is the first coefficient
        mov     r1,r11          ; r1 is the second coefficient
        neg     r0              ; r0 is negated
        cmovs   r0,r10          ; r0 is abs of dividend coefficient
neg     r1              ; r1 is negated
        cmovs   r1,r11          ; r1 is abs of divisor coefficient
bsr     r0,r0           ; r0 is the dividend most sign%ificant bit
bsr     r1,r1           ; r1 is the divisor most sign%ificant bit

; Scale up the dividend to be approximately 58 bits longer than the divisor.
; Scaling uses factors of 10, so we must convert from a bit count to a digit
; count by multiplication by 77/256 (approximately LN2/LN10).

add     r1,58           ; we want approximately 58 bits in the raw quotient
        sub     r1,r0           ; r1 is the number of bits to add to the dividend
        imul    r1,77           ; multiply by 77/256 to convert bits to digits
        shr     r1,8            ; r1 is the number of digits to scale the dividend

; The largest power of 10 that can be held in an int64 is 1e18.

cmp     r1,18           ; prescale the dividend %if 10**r1 won't fit
jg      divide_prescale

; Multiply the dividend by the scale factor, and divide that 128 bit result by
; the divisor.  Because of the scaling, the quotient is guaranteed to use most
; of the 64 bits in r0, and never more. Reduce the final exponent by the number
; of digits scaled.

mov     r0,r10          ; r0 is the dividend coefficient
        mov     r9,power
        imul    qword [r9+r1*8] ; r2:r0 is the dividend coefficient * 10**r1
        idiv    r11             ; r0 is the quotient
sub     r8,r1           ; r8 is the exponent
jmp     pack            ; pack it up
        pad

divide_prescale:

; %if the number of scaling digits is larger than 18, then we will have to
; scale in two steps: first prescaling the dividend to fill a register, and
; then repeating to fill a second register. This happens when the divisor
; coefficient is much larger than the dividend coefficient.

mov     r1,58           ; we want 58 bits or so in the dividend
sub     r1,r0           ; r1 is the number of additional bits needed
imul    r1,77           ; convert bits to digits
shr     r1,8            ; sh%ift 8 is cheaper than div 256
mov     r9,power
imul    r10,qword [r9+r1*8] ; multiply the dividend by power of ten
        sub     r8,r1           ; reduce the exponent
        jmp     divide_measure  ; try again

pad; -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

dec64_integer_divide: function_with_two_parameters
;(dividend: dec64, divisor: dec64) returns quotient: dec64

; Divide, with a floored integer result. It produces the same result as
;    dec64_floor(dec64_divide(dividend, divisor))
; but can sometimes produce that result more quickly.

cmp     r1_b,r2_b       ; are the exponents equal?
jne     integer_divide_slow

mov     r0,r1           ; r0 is the dividend
mov     r11,r2          ; r11 is the divisor
sar     r1,8            ; r1 is the dividend coefficient
        setnz   r2_h            ; r2_h is 1 %if the dividend coefficient is not zero
cmp     r0_b,128        ; are the operands nan?
sete    r2_b            ; r2_b is 1 %if the operands are nan
and     r0,-256         ; zero the dividend's exponent
or      r2_h,r2_b       ; r2_h is zero %if the dividend is zero and not nan
        jz      return_zero     ; the quotient is zero %if the dividend is zero
sar     r11,8           ; r11 is the divisor coefficient
        setz    r2_h            ; r2_h is 1 %if the divisor coefficient is zero
        or      r2_b,r2_h       ; r2_b is 1 %if the result is nan
jnz     return_nan
cqo                     ; sign extend r0 into r2
        idiv    r11             ; r0 is the quotient
and     r0,-256         ; zero the exponent again
ret                     ; no need to pack
pad

        integer_divide_slow:

; The exponents are not the same, so do it the hard way.

call_with_two_parameters dec64_divide
mov     r1,r0
tail_with_one_parameter dec64_floor

pad; -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

dec64_modulo: function_with_two_parameters
;(dividend: dec64, divisor: dec64) returns modulus: dec64

; Modulo. It produces the same result as
;    dec64_subtract(
;        dividend,
;        dec64_multiply(
;            dec64_integer_divide(dividend, divisor),
;            divisor
;        )
;    )

cmp     r1_b,r2_b       ; are the two exponents the same?
jnz     modulo_slow     ; %if not take the slow path
        mov     r0,r1           ; r0 is the dividend
mov     r11,r2          ; r11 is the divisor
cmp     r1_b,128        ; is the first operand nan?
setne   r2_h            ; r2_h is 1 %if the operands are not nan
        sar     r0,8            ; r0 is the dividend coefficient
        setz    r2_b            ; r2_b is 1 %if the dividend coefficient is zero
        test    r2_b,r2_h       ; is the dividend is zero and not nan?
jnz     return_zero     ; the quotient is zero %if the dividend is zero
sar     r11,8           ; r11 is the divisor coefficient
        setnz   r2_b            ; r2_b is 1 %if the divisor coefficient is not zero
test    r2_h,r2_b       ; is either operand nan or is the divisor zero?
jz      return_nan
cqo                     ; sign extend r0 into r2
        idiv    r11             ; divide r2:r0 by the divisor

; %if the signs of the divisor and remainder are d%ifferent and the remainder is
; not zero, add the divisor to the remainder.

xor     r0,r0           ; r0 is zero
        mov     r10,r2          ; r10 is the remainder
test    r2,r2           ; examine the remainder
        cmovz   r11,r0          ; r11 is zero %if the remainder is zero
xor     r10,r11         ; r10 is remainder xor divisor
        cmovs   r0,r11          ; r0 is the divisor %if the signs were d%ifferent
        add     r0,r2           ; r0 is the corrected result
        cmovz   r1,r0           ; %if r0 is zero, so is the exponent
shl     r0,8            ; position the coefficient
        mov     r0_b,r1_b       ; mix in the exponent
ret
        pad

modulo_slow:

; The exponents are not the same, so do it the hard way.

push    r1              ; save the dividend
        push    r2              ; save the divisor

        call_with_two_parameters dec64_integer_divide
        pop     r2
        mov     r1,r0
        call_with_two_parameters dec64_multiply
        pop     r1
        mov     r2,r0
        tail_with_two_parameters dec64_subtract

        pad; -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

dec64_half: function_with_one_parameter
;(dividend: dec64) returns quotient: dec64

; Divide a dec64 number by two. This will always be faster than dec64_divide.

cmp     r1_b,128
je      return_nan
test    r1_h,1
jz      half_fast

; Unpack the components into r8 and r1, multiply by 5 and divide by 10.

movsx   r8,r1_b         ; r8 is the exponent
sar     r1,8            ; r1 is the coefficient
sub     r8,1            ; bump down the exponent
lea     r0,[r1+r1*4]    ; r0 is the coefficient * 5
jmp     pack
pad

        half_fast:

; %if the least sign%ificant bit of the coefficient is 0, then we can do this
; the fast way. Sh%ift the coefficient by 1 bit and restore the exponent. %if
; the sh%ift produces zero, even easier.

mov     r0,-256         ; r0 is the coefficient mask
        and     r0,r1           ; r0 is the coefficient sh%ifted 8 bits
        jz      return
sar     r0,1            ; r0 is divided by 2
movzx   r1,r1_b         ; zero out r1 except lowest 8 bits
        or      r0,r1           ; mix in the exponent
ret

        pad; -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

dec64_signum: function_with_one_parameter
;(number: dec64) returns signature: dec64

; %if the number is nan, the result is nan.
; %if the number is less than zero, the result is -1.
; %if the number is zero, the result is 0.
; %if the number is greater than zero, the result is 1.

mov     r0,r1           ; r0 is the number
sar     r0,8            ; r0 is the coefficient
mov     r8,r0           ; r8 is the coefficient too
        neg     r0              ; r0 is -coefficient
        sar     r8,63           ; r8 is the extended sign (-1 %if negative else 0)
shr     r0,63           ; r0 is 1 %if the number was greater than 0
mov     r2,128          ; r2 is nan
        or      r0,r8           ; r0 is -1, 0, or 1
shl     r0,8            ; package the number with a zero exponent
        cmp     r1_b,r2_b       ; is the number nan?
cmove   r0,r2           ; %if so, replace the answer
        ret

pad; -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

dec64_not: function_with_one_parameter
;(boolean: dec64) returns notation: dec64

; %if the number is 1, the result is 0.
; %if the number is 0, the result is 1.
; Otherwise, the result is nan.

test    r1_b,r1_b
jnz     not_hard
test    r1,r1
jz      return_one
cmp     r1,100h
je      return_zero
jmp     return_nan
pad

        not_hard:

cmp     r1_b,80h
je      return_nan
call_with_one_parameter dec64_normal
test    r0_b,r0_b
jnz     return_nan
test    r0,r0
jz      return_one
cmp     r0,100h
je      return_zero
jmp     return_nan

pad; -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

dec64_neg: function_with_one_parameter
;(number: dec64) returns negation: dec64

; Negate a number. We need to negate the coefficient without changing the
; exponent.

cmp     r1_b,128        ; compare the exponent to nan
        je      return_nan      ; is the number nan?
mov     r2,r1           ; r2 is the number
mov     r0,-256         ; r0 is the coefficient mask
        sar     r2,8            ; r2 is the coefficient
cmovz   r1,r2           ; %if the coefficient is zero, then the exponent too
        xor     r0,r1           ; r0 has the exponent and complemented coefficient
        add     r0,256          ; r0 has the exponent and negated coefficient
        jo      neg_overflow    ; nice day %if it don't overflow
ret

        neg_overflow:

; The coefficient is -36028797018963968, which is the only coefficient that
; cannot be trivially negated. So we do this the hard way.

mov     r0,r2
movsx   r8,r1_b         ; r8 is the exponent
neg     r0
jmp     pack

pad; -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

dec64_abs: function_with_one_parameter
;(number: dec64) returns absolution: dec64

; Find the absolute value of a number. %if the number is negative, hand it off
; to dec64_neg. Otherwise, return the number unless it is nan or zero.

test    r1,r1           ; examine r1
js      dec64_neg       ; is the number negative?
mov     r10,r1          ; r10 is the number
mov     r0,r1           ; r0 is the number
mov     r2,128          ; r2 is nan
        and     r10,-256        ; r10 is the coefficient without an exponent
        cmovz   r0,r10          ; %if the coefficient is zero, the number is zero
        cmp     r1_b,r2_b       ; compare r1 to nan
cmovz   r0,r2           ; is the number nan?
ret                     ; return the number

pad; -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

dec64_equal: function_with_two_parameters
;(comparahend: dec64, comparator: dec64) returns comparison: dec64

; Compare two dec64 numbers. %if they are equal, return 1, otherwise return 0.
; Denormal zeroes are equal but denormal nans are not.

; %if the numbers are trivally equal, then return 1.

cmp     r1,r2           ; compare the two numbers
je      return_one

; %if the exponents match or %if their signs are d%ifferent, then return false.

mov     r0,r1           ; r0 is the first number
        xor     r0,r2           ; r0 the xor of the two numbers
        sets    r0_h            ; r0_h is 1 %if the signs are d%ifferent
        cmp     r1_b,r2_b       ; compare the two exponents
sete    r0_b            ; r0_b is 1 %if the exponents are the same
        or      r0_b,r0_h
        jnz     return_zero

; Do it the hard way by subtraction. Is the d%ifference zero?

call_with_two_parameters dec64_subtract ; r0 is r1 - r2
        cmp     r0_b,128        ; is the d%ifference nan?
je      return_zero
or      r0,r0           ; examine the d%ifference
        setz    r0_b            ; r0 is 1 %if the numbers are equal
movzx   r0,r0_b
shl     r0,8
ret

        pad; -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

dec64_less: function_with_two_parameters
;(comparahend: dec64, comparator: dec64) returns comparison: dec64

; Compare two dec64 numbers. %if either argument is any nan, then the result is
; nan. %if the first is less than the second, return 1, otherwise return 0.

; The other 3 comparison functions are easily implemented with dec64_less:

;    dec64_greater(a, b)          => dec64_less(b, a)
;    dec64_greater_or_equal(a, b) => dec64_not(dec64_less(a, b))
;    dec64_less_or_equal(a, b)    => dec64_not(dec64_less(b, a))

cmp     r1_b,128        ; is the first argument nan?
setne   r8_b            ; r8_b is 1 %if first is not nan
cmp     r2_b,128        ; is the second argument nan?
setne   r9_b            ; r9_b is 1 %if second is not nan
test    r8_b,r9_b       ; is either nan?
jz      return_nan

; %if the exponents are the same, or the coefficient signs are d%ifferent, then
; do a simple compare.

mov     r10,r1          ; r10 is the first number
        cmp     r1_b,r2_b       ; are the two exponents equal?
setne   r8_b            ; r8_b is 0 %if the exponents are equal
xor     r10,r2          ; r10 is the two numbers xor together
        setns   r10_b           ; r10_b is 0 %if the sign bits were d%ifferent
        test    r10_b,r8_b      ; exponents equal or sign bits d%ifferent
        jnz     less_slow       ; do it another way
        cmp     r1,r2           ; compare the numbers
        setl    r0_b            ; r0_b is 1 %if the first number is less
        movzx   r0,r0_b         ; r0 is 1 %if the first number is less
        shl     r0,8            ; that bit distinguishes true from false
ret
        pad

less_slow:

; Do it the hard way with a subtraction.

call_with_two_parameters dec64_subtract ; r0 is r1 - r2
        cmp     r0_b,128        ; is the d%ifference nan?
je      return_nan
shr     r0,63           ; r0 is 1 %if the first number is less
        shl     r0,8            ; that bit distinguishes true from false
ret

        pad; -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

dec64_is_integer: function_with_one_parameter
;(number: dec64) returns comparison: dec64

; %if the number contains a non-zero fractional part or %if it is nan, return
; false. Otherwise, return true.

cmp     r1_b,128        ; nan exponent?
je      return_zero     ; nan is not an integer
        mov     r0,r1           ; r0 is the number
sar     r0,8            ; r0 is the coefficient
cmovz   r1,r0           ; %if the coefficient is zero, so is the exponent
        movsx   r8,r1_b         ; r8 is the exponent
test    r1_b,r1_b       ; examine the exponent
        jns     return_one
        neg     r8              ; negate the exponent
        cmp     r8_b,17         ; extreme negative exponents can never be integer
        jae     return_zero
        mov     r9,power
        mov     r10,[r9+r8*8]  ; r10 is 10^-exponent
        cqo                     ; sign extend r0 into r2
        idiv    r10             ; divide r2:r0 by the power of ten
xor     r0,r0           ; r0 is zero
        test    r2,r2           ; examine the remainder
        setz    r0_b            ; %if the remainder is zero, then return one
        movzx   r0,r0_b
        shl     r0,8
ret

        pad; -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

dec64_is_any_nan: function_with_one_parameter
;(number: dec64) returns comparison: dec64

        cmp     r1_b,128        ; is r1 nan?
sete    r0_b            ; r0 is 1 %if r1 is nan
        movzx   r0,r0_b
        shl     r0,8
ret

        pad; -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

dec64_is_zero: function_with_one_parameter
;(number: dec64) returns comparison: dec64

        xor     r0,r0           ; r0 is zero
        test    r1,-256         ; coefficient mask
setz    r0_b            ; r0 is one %if the coefficient is zero
cmp     r1_b,128        ; is the number nan?
setnz   r0_h            ; r0_h is 1 %if the number is not nan
        and     r0_b,r0_h       ; r0 is one %if the coefficient is zero and not nan
        movzx   r0,r0_b
        shl     r0,8
ret

        pad; -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

dec64_int: function_with_one_parameter
;(number: dec64) returns integer: dec64

; Convert the number such that the exponent will be zero, discarding the
; fraction part. It will produce nan %if the result cannot be represented in
; 56 signed bits. This is used to extract an int56 from a dec64 for bitwise
; operations. It accepts a broader range than the safe integer range:
; -36028797018963968 thru 72057594037927935.

; Preserved: r11

        mov     r0,r1           ; r0 is the number
test    r1_b,r1_b       ; examine the exponent
        jz      return          ; nothing to do it the exponent is zero
        js      dec64_floor     ; shed the fraction part %if exponent is negative
        movzx   r8,r1_b         ; r8 is the exponent
mov     r2,18           ; r2 is 18
cmp     r1_b,18         ; is the exponent too enormous?
cmovae  r8,r2           ; r8 is min(exponent, 18)
and     r0,-256         ; r0 is the coefficient, sh%ifted 8
mov     r9,power
mov     r10,[r9+r8*8]  ; r10 is 10^exponent
        imul    r10             ; r0 is coefficient * 10^exponent
        sar     r2,1            ; sh%ift the lsb of the overflow into carry
adc     r2,0            ; %if r2 was 0 or -1, it is now 0
jnz     return_nan      ; was the coefficient too enormous?
ret

        pad; -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

dec64_normal: function_with_one_parameter
;(number: dec64) returns normalization: dec64

; Make the exponent as close to zero as possible without losing any signficance.
; Usually normalization is not needed since it does not materially change the
; value of a number.

mov     r0,r1           ; r0 is the number
cmp     r1_b,128        ; compare the exponent to nan
        jz      return_nan      ; %if exponent is nan, the result is nan
and     r0,-256         ; r0 is the coefficient sh%ifted 8 bits
        mov     r8,10           ; r8 is the divisor
cmovz   r1,r0           ; r1 is zero %if r0 is zero
        mov     r10,r0          ; r10 is the coefficient sh%ifted 8 bits
        test    r1_b,r1_b       ; examine the exponent
        jz      return          ; %if the exponent is zero, return r0
        jns     normal_multiply ; %if the exponent is positive
sar     r0,8            ; r0 is the coefficient
sar     r10,8           ; r10 is the coefficient
pad

        normal_divide:

; While the exponent is less than zero, divide the coefficient by 10 and
; increment the exponent.

cqo                     ; sign extend r0 into r2
        idiv    r8              ; divide r2:r0 by 10
test    r2,r2           ; examine the remainder
        jnz     normal_divide_done ; %if r2 is not zero, we are done
mov     r10,r0          ; r10 is the coefficient
add     r1_b,1          ; increment the exponent
        jnz     normal_divide   ; until the exponent is zero
        pad

normal_divide_done:

mov     r0,r10          ; r0 is the finished coefficient
        shl     r0,8            ; put it in position
mov     r0_b,r1_b       ; mix in the exponent
ret
        pad

normal_multiply:

; While the exponent is greater than zero, multiply the coefficient by 10 and
; decrement the exponent. %if the coefficient gets too large, wrap it up.

imul    r0,10           ; r0 is r0 * 10
jo      normal_multiply_done ; return zero %if overflow
        mov     r10,r0          ; r10 is the coefficient
sub     r1_b,1          ; decrement the exponent
        jnz     normal_multiply ; until the exponent is zero
        ret
pad

        normal_multiply_done:

mov     r0,r10          ; r0 is the finished positioned coefficient
mov     r0_b,r1_b       ; mix in the exponent
ret

        pad; -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

return:

; Return whatever is in r0.

ret

        pad; -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

return_nan:

; All of the dec64_ functions return only this form of nan.

mov     r0,128          ; nan
        ret

pad; -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

return_one:

mov     r0,256          ; one
        ret

pad; -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

return_zero:

xor     r0,r0           ; zero
        ret


*/