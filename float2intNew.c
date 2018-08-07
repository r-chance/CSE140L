#include<stdio.h>


int float2Int(char front, char back) {
  char sign;// stores sign bit
  char overflow;// indicates an overflow in conversion
  char exponent;// stores exponent value
  char mantissaUpper;// stores upper byte with hidden bit in place
  char mantissaLower;// stores lower byte
  char ejected;// stores bits that are discared while shifting
  char ejectedUpper;// stores ejected bits from top byte
  char intUpper;// stores upper bit of final int result
  char intLower;// stores lower bit of final int result
  char done; // flag for completion

  char roundingBit;
  char sticky;

  sign = (front >> 7) & 1;// extract sign bit
  exponent = (front >> 2) & 0x1f;// extract exponent
  exponent -= 25;// positive exponent shifts left, negative shifts right
  mantissaUpper = 1 << 2;// set hidden bit in place
  mantissaUpper = (front & 3) | mantissaUpper;
  mantissaLower = back;
  
  if(exponent >= 5) { // this is an overflow
    overflow = 1; // set overflow bit
  }

  else if(exponent >= 0) { // shift left on positive exponent, no rounding required
    intUpper = mantissaUpper << exponent;
    intUpper = (((unsigned char)mantissaLower >> (8-exponent))) | intUpper;// shift lower bits
                                                          // to upper word(lsr)
    intLower = mantissaLower << exponent; // shift lower bits in lower word
  }

  else { // shift right on negative exponent, rounding required
    exponent = -exponent;
    ejected = (unsigned char)((1 << exponent)-1);// mask for capturing ejected bits
    ejected = ejected & mantissaLower;// captures all ejected bits in lower byte

    if(exponent > 8) {// if upper bits are ejected as well
      exponent -= 8;
      sticky = ejected;// entire lower byte is sticky
      mantissaLower = mantissaUpper;
      roundingBit = (1<<(exponent-1))&mantissaLower;// capture rounding bit
      ejectedUpper = ((1 << (exponent-1))-1) & mantissaLower;// capture upper ejected bits, minus rounding bit
      intLower = ((unsigned char)mantissaLower >> exponent);
      intUpper = 0;
    }
    else { // if only lower bits are ejected; sticky is ejected OR sticky
      roundingBit = (1<<(exponent-1))&mantissaLower;// captures rounding bit for large R shift values
      sticky = ((1<<(exponent-1))-1) & mantissaLower;// captures sticky of large R shift values
      ejectedUpper = mantissaUpper & ((1<<exponent)-1);
      ejectedUpper = ejectedUpper << (8-exponent);
      intUpper = ((unsigned char)mantissaUpper >> exponent);
      intLower = ((unsigned char)mantissaLower >> exponent);

      intLower = intLower | ejectedUpper;
      ejectedUpper = 0;

    }

    // check for rounding
    if(roundingBit) {
      if((sticky|| ejectedUpper || (intLower&1))) {// If number should be rounded
       
        if(intLower == 0xff) {// if overflow on rounding
          intLower++;
          intUpper++;
        }
        else {
          intLower++;
        }
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
//  printf("IntUpper = %d, IntLower = %d\n", (unsigned char)intUpper, (unsigned char)intLower);
 // printf("Int = %d\n", (intUpper<<8)|(unsigned char)intLower);
  return (intUpper<<8)|(unsigned char)(intLower);
}

          
int int2Float(char front, char back) {

  // Store front and back in a separate register to access again later

  char sign = (front >> 7) & 1;
  char exponent = 7<<1;// max possible unbiased exponent
  char mantissaUpper;// warning: sign bit still at msb
  char mantissaLower;
  char overflow;
  char done;
  char ejected;// stores ejected bits 
  char ejectedUpper;// stores ejected bits from lower byte
  char intUpper = front;
  char intLower = back;

  // take 2's complement of number if negative
  if(sign) {
    intLower = ~intLower;// flip all bits
    intUpper = ~intUpper;// flip all bits
    if(intLower==0xff) {// if overflow will occur in lower byte
      if(intUpper==0x7f){// if overflow occurs in upper byte(total overflow)
        overflow = 1;
      }
      else { // add 1 to both lower and upper bytes (lower overflows)
        intLower += 1;
        intUpper += 1;
      }
    }
    else {// no overflow in lower, add 1 to lower
      intLower += 1;
    }
  }

  mantissaUpper = intUpper;
  mantissaLower = intLower;        

  // intUpper will now have a 0 at the MSB, shift left by 1
  intUpper = intUpper << 1;
  
  // Find exponent
  if(intUpper != 0) { // if highest bit is in upper byte
    while(!((intUpper>>7)&1)) {// if hidden bit not at mbs
      exponent -= 1;
      intUpper = intUpper<<1;// double mantissa, reduce exponent
    }

    mantissaUpper = mantissaUpper << (16-exponent);// clear hidden bit
    mantissaUpper = (unsigned char)mantissaUpper >> (16-exponent);  
  }
  
  else if(intLower != 0) {// if highest bit is in lower byte
    exponent -= 7;// adjust exponent for hidden bit residing in lower byte
    while(!((intLower >> 7)&1)) {// if hidden bit not at msb
      exponent -= 1;
      intLower = intLower << 1;
    }
    
    mantissaLower = mantissaLower << (8-exponent);// clear hidden bit
    mantissaLower = (unsigned char)mantissaLower >> (8-exponent);// return to proper location

  }
  else {// if no "1" is present in either byte (except sign bit)
    if(!sign) {// value is 0
      exponent = 0;
      mantissaUpper = 0;
      mantissaLower = 0;
      done = 1;
    }
    else {// otherwise value is max negative number
     mantissaUpper = 1<<7; // move sign bit into place
  //   mantissaUpper = mantissaUpper | (30<<2);// 
     mantissaUpper = mantissaUpper | 0x3;
     mantissaLower = 0xff;
     exponent += 15;
     done = 1;
    }
  }

  // Shift integer by exponent-10 to put in correct location
  if(!done) {
    
    if(exponent-10>=0) {// exponent-10 is positive, R shift, possible loss of precision
      ejected = (1 << (exponent-10))-1;// mask for capturing ejected bits
      ejectedUpper = mantissaUpper & ejected;// capture ejected bits from upper byte
      ejected = ejected & mantissaLower;// stores lower ejected bits
      mantissaLower = (unsigned char)mantissaLower >> (exponent-10);// shift lower byte
      mantissaLower = mantissaLower | (ejectedUpper << (8-(exponent-10)));// add upper ejected to lower byte
      mantissaUpper = (unsigned char)mantissaUpper >> (exponent-10);
      mantissaUpper = mantissaUpper | (sign << 7);// add in sign bit
      mantissaUpper = mantissaUpper | ((exponent+15) << 2);
      done = 1;
    }
  
    else {// exponent-10 is negative, L shift
      ejected = (1 << (10-exponent))-1;// mask for capturing ejected bits
      ejected = ((unsigned char)mantissaLower >> (8-(10-exponent)))& ejected;// capture ejected bits from lower byte
      mantissaLower = (unsigned char)intLower << (10-exponent);
      mantissaUpper = mantissaUpper << (10-exponent);
      mantissaUpper = mantissaUpper | ejected;// place ejected bits into upper byte
      mantissaUpper = mantissaUpper | (sign << 7);// add in sign bit
      mantissaUpper = mantissaUpper | ((exponent+15) << 2);
//printf("exponent = %d\n", exponent);
      done = 1;
    }
  }

  printf("MantissaUpper = %d, MantissaLower = %d\n", mantissaUpper, mantissaLower);
  return ((unsigned char)mantissaUpper<<8)| (unsigned char)mantissaLower;


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

  char uf, lf, ui, li;

  uf = 0b01100000;
  lf = 0b00000100;

  ui = 0b00000010;
  li = 0b00000010;
int2Float(ui,li);
  printf("int = %d\n", float2Int(uf,lf));
  printf("floatUpper = %d\n", int2Float(ui,li));

  return 0;
}
