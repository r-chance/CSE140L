#include<stdio.h>


int float2Int(char front, char back) {
  char sign;// stores sign bit
  char overflow;// indicates an overflow in conversion
  char exponent= 0;// stores exponent value
  char mantissaUpper = 0;// stores upper byte with hidden bit in place
  char mantissaLower = 0;// stores lower byte
  char ejected=0;// stores bits that are discared while shifting
  char intUpper=0;// stores upper bit of final int result
  char intLower=0;// stores lower bit of final int result
  char done; // flag for completion

  sign = (front >> 7) & 1;// extract sign bit
  exponent = (front >> 2) & 0x1f;// extract exponent
  exponent -= 25;// positive exponent shifts left, negative shifts right
  mantissaUpper = 1 << 2;// set hidden bit in place
  mantissaUpper = (front & 3) | mantissaUpper;
  mantissaLower = back;
  
  if(exponent >= 5) { // this is an overflow
    overflow = 1; // set overflow bit
  }

  else if(exponent >= 0) { // shift left on positive exponent
    intUpper = mantissaUpper << exponent;
    intUpper = (mantissaLower >> (8-exponent)) | intUpper;// shift lower bits
                                                          // to upper word(lsr)
    intLower = intLower << exponent; // shift lower bits in lower word
  }

  else { // shift right on negative exponent
    ejected = (1 << exponent) -1;// mask for capturing ejected bits
    ejected = ejected & mantissaLower;// capture ejected bits
    intLower = mantissaLower >> exponent;// lsr by exponent value
    intUpper = mantissaUpper & ((1 << exponent)-1);//capture ejected upper bits
    intLower = intUpper << (8-exponent) | intLower;// construct lower byte
    intUpper = mantissaUpper >> exponent;// lsr to form upper byte
    if(ejected == (1 << (exponent - 1))) {// if halfway between integers
      if(intLower & 1) {// if an odd number
        if(intLower == 0xff) {// check overflows
          if(intUpper == 0x7f) {// complete overflow
            overflow = 1;
          }
          else {// overflow in lower byte only
            intLower = 0;
            intUpper += 1;
          }
        }
      }
    }
    else if(ejected > (1 << (exponent - 1))) { // round up
      if(intLower == 0xff) { // check overflows
        if(intUpper == 0x7f) { // a complete overflow
          overflow = 1;
        }
        else { // lower byte overflow only
          intLower = 0;
          intUpper += 1;
        }
      }
      else {// no overflow
        intLower += 1;
      }
    }
  }

  if(sign) {
    intLower = ~intLower;
    intUpper = ~intUpper;
    if(intLower == 0xff) {// carry out if lower byte all 1's
      intLower = 0;
      intUpper += 1;
    }
    else {
      intLower += 1;
    }
  }
  printf("upper = %d, lower = %d\n", intUpper, intLower);
  return (intUpper<<8) | (intLower);
}


int int2Float(char front, char back) {

  // Store front and back in a separate register to access again later

  char sign = (front >> 7) & 1;
  char exponent = 7<<1;// max possible unbiased exponent
  char mantissaUpper = front;// warning: sign bit still at msb
  char mantissaLower = back;
  char overflow;
  char done;
  char shiftamt;

  // take 2's complement of number if negative
  if(sign) {
    mantissaLower = ~mantissaLower;// flip all bits
    mantissaUpper = ~mantissaUpper;// flip all bits
    if(mantissaLower=0xff) {// if overflow will occur in lower byte
      mantissaLower = 0;
      mantissaUpper += 1;
    }      
    else {// no overflow in lower, add 1 to lower
      mantissaLower += 1;
    }
  }
        
  // mantissaUpper will now have a 0 at the MSB, shift left by 1, exp for 14
  mantissaUpper = mantissaUpper << 1;
  
  if(mantissaUpper != 0) { // if highest bit is in upper byte
    while(!((mantissaUpper>>7)&1)) {// if hidden bit not at mbs
      exponent -= 1;
      mantissaUpper = mantissaUpper<<1;// double mantissa, reduce exponent
    }
  // We now have proper exponent when highest bit is in upper byte
  // Reconstruct float with this information at the end, loss of precision possible
  // if width of integer is more than 10 digits, i.e. highest bit at bit 1,2,3,4 (sign bit at 0)
  
  }
  
  else if(mantissaLower != 0) {// if highest bit is in lower byte
    exponent -= 7;// adjust exponent for hidden bit residing in lower byte
    while(!((mantissaLower >> 7)&1)) {// if hidden bit not at msb
      exponent -= 1;
      mantissaLower = mantissaLower << 1;
    }

    // We now have proper exponent when highest bit is in lower byte
    // Reconstruct float with this information, no loss of precision possible
    
  }
  else {// if no "1" is present in either byte (except sign bit)
    mantissaUpper = 0;
    mantissaLower = 0;
    if(!sign) {
      exponent = 0;
    }    
    else {
      exponent = 15;
    }
    
  }
  // Exponent should have correct value at this point
  shiftamt = 10-exponent;

  if(exponent <= 10) {
    mantissaUpper = (mantissaUpper << (10-exponent))& 3;
    mantissaUpper = mantissaUpper | 

    overflow =  
    0000000I IIIIIIII
    SEEEEEMM MMMMMMMM

}

/*
int float2float(char frontA, char backA, char frontB, char backB) {

  char expA = (frontA>>2)&0x1f;
  char expB = (frontB>>2)&0x1f;
  char mantissaUpperA = frontA & 0x2;
  char mantissaUpperB = frontB & 0x2;
  char mantissaLowerA = backA;
  char mantissaLowerB = backB;
  char diff = expA-expB;

  if(diff >= 0) {// if exponent of A is greater than or equal to exponent of B
    while(
  
  


}*/


int main() {

  char u, l;

  u = 0b01011000;
  l = 0b10000000;

  printf("int = %d", float2Int(u,l));
  return 0;
}
