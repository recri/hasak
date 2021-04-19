// Morse code for ASCII characters 33 - 90, from ITU-R M.1677-1
// read from bit0 to bit7
// 0 = dot, 1 = dash
// highest bit set indicated end-of-character (not to be sent as a dash!)
// borrowed from Christoph's keyer, along with the decoding code
// https://github.com/dl1ycf/TeensyWinkeyEmulator
//
// 0x01 is a nul, a character that is not coded
// it's also the end state of decoding a character.
// void decode(uint8_t code, char buffer[8]) {
//   int i = 0;
//   while (code != 1) {
//     buffer[i++] = code & 1 ? '-' : '.';
//     code >>= 1;
//   }
//   buffer[i] = 0;
// }
// uint8_t encode(const char *buffer) {
//   int i = 0;
//   uint8_t code = 1;
//   while (buffer[i] != 0) i += 1;
//   while (--i >= 0) {
//     code <<= 1;
//     code |= buffer[i] == '-' ? 
//   }
//   return code;
// }
//
#define MORSE_NIL  0x01		// no character

#define MORSE_A	   0x06		// A    .-
#define MORSE_B	   0x11		// B    .---
#define MORSE_C	   0x15		// C    -.-.
#define MORSE_D	   0x09		// D    -..
#define MORSE_E	   0x02		// E    .
#define MORSE_F	   0x14		// F    ..-.
#define MORSE_G	   0x0b		// G    --.
#define MORSE_H	   0x10		// H    ....
#define MORSE_I	   0x04		// I    ..
#define MORSE_J	   0x1e		// J    .---
#define MORSE_K	   0x0d		// K    -.-
#define MORSE_L	   0x12		// L    .-..
#define MORSE_M	   0x07		// M    -- 
#define MORSE_N	   0x05		// N    -.
#define MORSE_O	   0x0f		// O    ---
#define MORSE_P	   0x16		// P    .--.
#define MORSE_Q	   0x1b		// Q    --.-
#define MORSE_R	   0x0a		// R    .-.
#define MORSE_S	   0x08		// S    ...
#define MORSE_T	   0x03		// T    -
#define MORSE_U	   0x0c		// U    ..-
#define MORSE_V	   0x18		// V    ...-
#define MORSE_W	   0x0e		// W    .--
#define MORSE_X	   0x19		// X    -..-
#define MORSE_Y	   0x1d		// Y    -.--
#define MORSE_Z	   0x13		// Z    --..

#define MORSE_0	   0x3f		// 0    -----
#define MORSE_1	   0x3e		// 1    .----
#define MORSE_2	   0x3c		// 2    ..---
#define MORSE_3	   0x38		// 3    ...--
#define MORSE_4	   0x30		// 4    ....-
#define MORSE_5	   0x20		// 5    .....
#define MORSE_6	   0x21		// 6    -....
#define MORSE_7	   0x23		// 7    --...
#define MORSE_8	   0x27		// 8    ---..
#define MORSE_9	   0x2f		// 9    ----.

#define MORSE_DQUOTE 0x52	// "    .-..-.
#define MORSE_SQUOTE 0x5e	// '    .----.    
#define MORSE_LPAREN 0x36	// (    -.--.
#define MORSE_RPAREN 0x6d	// )    -.--.-
#define MORSE_PLUS   0x2a	// +    .-.-.
#define MORSE_COMMA  0x73	// ,    --..--
#define MORSE_MINUS  0x61	// -    -....-
#define MORSE_PERIOD 0x6A	// .    .-.-.-
#define MORSE_SLASH  0x29	// /    -..-.
#define MORSE_COLON  0x78	// :    ---...
#define MORSE_EQUAL  0x31	// =    -...-
#define MORSE_QUERY  0x4c	// ?    ..--..
#define MORSE_ATSIGN 0x56	// @    .--.-.

static const unsigned char morse[64] = {
   MORSE_NIL,			//33 !    not in ITU-R M.1677-1
   MORSE_DQUOTE,		//34 "    .-..-.
   MORSE_NIL,			//35 #    not in ITU-R M.1677-1
   MORSE_NIL,			//36 $    not in ITU-R M.1677-1
   MORSE_NIL,			//37 %    not in ITU-R M.1677-1
   MORSE_NIL,			//38 &    not in ITU-R M.1677-1
   MORSE_SQUOTE,		//39 '    .----.    
   MORSE_LPAREN,		//40 (    -.--.
   MORSE_RPAREN,		//41 )    -.--.-
   MORSE_NIL,			//42 *    not in ITU-R M.1677-1
   MORSE_PLUS,			//43 +    .-.-.
   MORSE_COMMA,			//44 ,    --..--
   MORSE_MINUS,			//45 -    -....-
   MORSE_PERIOD,		//46 .    .-.-.-
   MORSE_SLASH,			//47 /    -..-.
   MORSE_0,			//48 0    -----
   MORSE_1,			//49 1    .----
   MORSE_2,			//50 2    ..---
   MORSE_3,			//51 3    ...--
   MORSE_4,			//52 4    ....-
   MORSE_5,			//53 5    .....
   MORSE_6,			//54 6    -....
   MORSE_7,			//55 7    --...
   MORSE_8,			//56 8    ---..
   MORSE_9,			//57 9    ----.
   MORSE_COLON,			//58 :    ---...
   MORSE_NIL,			//59 ;    not in ITU-R M.1677-1
   MORSE_NIL,			//60 <    not in ITU-R M.1677-1
   MORSE_EQUAL,			//61 =    -...-
   MORSE_NIL,			//62 >    not in ITU-R M.1677-1
   MORSE_QUERY,			//63 ?    ..--..
   MORSE_ATSIGN,		//64 @    .--.-.
   MORSE_A,			//65 A    .-
   MORSE_B,			//66 B    .---
   MORSE_C,			//67 C    -.-.
   MORSE_D,			//68 D    -..
   MORSE_E,			//69 E    .
   MORSE_F,			//70 F    ..-.
   MORSE_G,			//71 G    --.
   MORSE_H,			//72 H    ....
   MORSE_I,			//73 I    ..
   MORSE_J,			//74 J    .---
   MORSE_K,			//75 K    -.-
   MORSE_L,			//76 L    .-..
   MORSE_M,			//77 M    -- 
   MORSE_N,			//78 N    -.
   MORSE_O,			//79 O    ---
   MORSE_P,			//80 P    .--.
   MORSE_Q,			//81 Q    --.-
   MORSE_R,			//82 R    .-.
   MORSE_S,			//83 S    ...
   MORSE_T,			//84 T    -
   MORSE_U,			//85 U    ..-
   MORSE_V,			//86 V    ...-
   MORSE_W,			//87 W    .--
   MORSE_X,			//88 X    -..-
   MORSE_Y,			//89 Y    -.--
   MORSE_Z,			//90 Z    --..
   MORSE_NIL,			//91 [    not in ITU-R M.1677-1
   MORSE_NIL,			//92 \    not in ITU-R M.1677-1
   MORSE_NIL,			//93 ]    not in ITU-R M.1677-1
   MORSE_NIL,			//94 ^    not in ITU-R M.1677-1
   MORSE_NIL,			//95 _    not in ITU-R M.1677-1
   MORSE_NIL			//96 `    not in ITU-R M.1677-1
};
