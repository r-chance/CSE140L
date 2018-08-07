#include<stdio.h>
#include <inttypes.h>
#include <math.h>

double float2Int(char front, char back);

int main() {
  
    int8_t u, l;
  
    u = 0b00110101;
    l = 0b01010101;
  
    // printf("Upper is: %d\n", u);
    
    printf("Upper is: %#0x\n", u);
    printf("Lower is: %#0x\n", l);
    

    printf("int = %lf\n", float2Int(u,l));
    return 0;
  }

double float2Int(char front, char back) {
  int sign;// stores sign bit
  int8_t exponent;// stores exponent value
  int8_t exponent_shift = 0;
  int8_t hiddenBit;
  int mantissa;
  int8_t mantissa_shift = 0;
  // char done; // flag for completion
  double result = 0;

  // sign = (front >> 7) & 1;// extract sign bit
  if ((front & 0x80) == 0x80) {
    sign = -1;
  } else {
    sign = 1;
  }

  exponent = (front >> 2) & 0x1f;// extract exponent
  if (exponent == 0) {
    exponent_shift = -14;
    hiddenBit = 0;
  } else {
    exponent -= 15;// positive exponent shifts left, negative shifts right    
    exponent_shift = exponent;
    hiddenBit = 1;
  }  

  printf("Exponent (after normalization) is: %d\n", exponent_shift);
  
  
  mantissa = ((front & 3) << 8) | back;
  int x = mantissa;  
  for (int i = 0; i < 10; i++) {
    x = x << 1;
    x = (x & 0x3ff);
    printf("Shifted Mantissa is now: %d\n", x);
    if (x == 0) {
      mantissa_shift = i+1;
      break;
    }
  }
  
  mantissa_shift *= -1;

  printf("Mantissa is: %d\n", mantissa);
  printf("Mantissa is: %#0x\n", mantissa);
  printf("Mantissa shift is: %d\n", mantissa_shift);
  
  if (hiddenBit == 1) {
      result = ( 1 + (2<<mantissa_shift)) * (2<<exponent_shift);
//    result = (pow(2, 0) + pow(2, mantissa_shift)) * pow(2, exponent_shift);
  } else {
      result = 0;
//    result = pow(2, mantissa_shift) * pow(2, exponent_shift);
  }
  result *= sign;
  return result;
}


// int int2Float(char front, char back) {

//   // Store front and back in a separate register to access again later

//   char sign = (front >> 7) & 1;
//   char exponent = 7<<1;// max possible unbiased exponent
//   char mantissaUpper = front;// warning: sign bit still at msb
//   char mantissaLower = back;
//   char overflow;
//   char done;

//   // take 2's complement of number if negative
//   if(sign) {
//     mantissaLower = ~mantissaLower;// flip all bits
//     mantissaUpper = ~mantissaUpper;// flip all bits
//     if(mantissaLower=0xff) {// if overflow will occur in lower byte
//       if(mantissaUpper==0x7f){// if overflow occurs in upper byte(total overflow)
//         overflow = 1;
//       }
//       else { // add 1 to both lower and upper bytes (lower overflows)
//         mantissaLower += 1;
//         mantissaUpper += 1;
//       }
//     }
//     else {// no overflow in lower, add 1 to lower
//       mantissaLower += 1;
//     }
//   }
        
//   // mantissaUpper will now have a 0 at the MSB, shift left by 1
//   mantissaUpper = mantissaUpper << 1;
  
//   if(mantissaUpper != 0) { // if highest bit is in upper byte
//     while(!((mantissaUpper>>7)&1)) {// if hidden bit not at mbs
//       exponent -= 1;
//       mantissaUpper = mantissaUpper<<1;// double mantissa, reduce exponent
//     }
//   // We now have proper exponent when highest bit is in upper byte
//   // Reconstruct float with this information at the end, loss of precision possible
//   // if width of integer is more than 10 digits, i.e. highest bit at bit 1,2,3,4 (sign bit at 0)
  
//   }
  
//   else if(mantissaLower != 0) {// if highest bit is in lower byte
//     exponent -= 7;// adjust exponent for hidden bit residing in lower byte
//     while(!((mantissaLower >> 7)&1)) {// if hidden bit not at msb
//       exponent -= 1;
//       mantissaLower = mantissaLower << 1;
//     }

//     // We now have proper exponent when highest bit is in lower byte
//     // Reconstruct float with this information, no loss of precision possible
    
//   }
//   else {// if no "1" is present in either byte (except sign bit)
//     if(!sign) {// if a non-negative, value = 0, graceful underflow for rounding ** DO THIS**
//       exponent = 0;
//       mantissaUpper = 0;
//       mantissaLower = 0;
//       done = 1;
//     }
//     else {// otherwise value is max negative number
//      exponent = 15;
//      mantissaUpper = 0x2;;
//      mantissaLower = 0xff;
//      done = 1;
//     }
//   }
//   // Exponent should have correct value at this point
//   // Shift integer by 10-exponent to put in correct location, assuming exponent <= 10
//   if(!done) {
    
//   }

// }


// int float2float(char frontA, char backA, char frontB, char backB) {

//   char expA = (frontA>>2)&0x1f;
//   char expB = (frontB>>2)&0x1f;
//   char mantissaUpperA = frontA & 0x2;
//   char mantissaUpperB = frontB & 0x2;
//   char mantissaLowerA = backA;
//   char mantissaLowerB = backB;
//   char diff = expA-expB;

//   if(diff >= 0) {// if exponent of A is greater than or equal to exponent of B
//     while(
  
  


// }



