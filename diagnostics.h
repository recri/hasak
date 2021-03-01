/*
** diagnostics
*/
void ioreport(const char *tag, int usage, int max, int over, int under) {
  Serial.print(tag);
  Serial.print(" usage "); Serial.print(usage);
  Serial.print(" max usage "); Serial.print(max);
  Serial.print(" overruns: "); Serial.print(over);
  Serial.print(" underruns "); Serial.print(under);
  Serial.println();
}
void ireport(const char *tag, AudioInputSample &sample) {
  ioreport(tag, sample.processorUsage(), sample.processorUsage(), sample.overruns(), sample.underruns());
}

void ireset(AudioInputSample &sample) {
  sample.reset();
  sample.processorUsageMaxReset();
}

void oreport(const char *tag, AudioOutputSample &sample) {
  ioreport(tag, sample.processorUsage(), sample.processorUsage(), sample.overruns(), sample.underruns());
}

void oreset(AudioOutputSample &sample) {
  sample.reset();
  sample.processorUsageMaxReset();
}

void preport(const char *tag, int16_t *p, int n) {
  int count = 0;
  int words = 0;
  int delay = 1.0/AUDIO_SAMPLE_RATE * 1e6;
  Serial.printf("%s %dus ", tag, delay);
  words += 2;
  for (int i = 0; i < n; i += 1) {
    if (*p != 0) {
      if (count >= 0) count += 1;
      else {
	Serial.printf("%d", count);
	count = 1;
	words += 1;
      }
    } else {
      if (count <= 0) count -= 1;
      else {
	Serial.printf("+%d", count);
	count = -1;
	words += 1;
      }
    }
    if (words > 10) { Serial.printf("\n"); words = 0; }
    delayMicroseconds(delay);
  }
  if (count > 0) Serial.printf("+%d\n", count);
  else Serial.printf("%d\n", count);
}

void mreport(const char *tag, AudioStream &str) {
  Serial.print(tag);
  Serial.print(" usage "); Serial.print(str.processorUsage());
  Serial.print(" max "); Serial.print(str.processorUsageMax());
  Serial.println();
}

void mreset(AudioStream &str) {
  str.processorUsageMaxReset();
}

// #include "src/dspmath/window.h"
static const double dtwo_pi = 2*3.14159265358979323846;		/* 2*pi */
static float cosine_series1(const int size, const int k, const double a0, const double a1) {
  return a0 - a1 * cos((dtwo_pi * k) / (size-1));
}    
static float cosine_series2(const int size, const int k, const double a0, const double a1, const double a2) {
  return cosine_series1(size, k, a0, a1) + a2 * cos((2 * dtwo_pi * k) / (size-1));
}
static float cosine_series3(const int size, const int k, const double a0, const double a1, const double a2, const double a3) {
  return cosine_series2(size, k, a0, a1, a2) - a3 * cos((3 * dtwo_pi * k) / (size-1));
}

static int16_t hann(int k, int size) { return (1<<15)*cosine_series1(size, k, 0.50, 0.50); }
static int16_t blackman_harris(int k, int size) { return (1<<15)*cosine_series3(size, k, 0.3587500, 0.4882900, 0.1412800, 0.0116800); }


// interpolation tables

#define WINDOW_TABLE_LENGTH 128

//
// Step function from a RaisedCosine window. Mathematica code:
//
// RC[x_] = 1/2 (1 + Cos[2 Pi x + Pi])
// RCI[y_] = Integrate[RC[x], {x, 0, y}]
// Tab = Table[Round[2^31  RCI[i/129]/RCI[1]], {i, 1, 128}];
// Do[{l = Table[PaddedForm[Tab[[j]], 12], {j, 8*i + 1, 8*i + 8}], Print[Row[l, ","], ","]}, {i, 0, 15}]
//
const int32_t hann_window_table[WINDOW_TABLE_LENGTH+1] = {
         6581,        52632,       177529,       420460,       820335,      1415690,      2244599,      3344580,
      4752510,      6504535,      8635984,     11181287,     14173892,     17646185,     21629417,     26153625,
     31247563,     36938634,     43252825,     50214644,     57847064,     66171467,     75207593,     84973495,
     95485495,    106758145,    118804192,    131634549,    145258270,    159682527,    174912590,    190951822,
    207801665,    225461638,    243929338,    263200447,    283268743,    304126109,    325762560,    348166263,
    371323562,    395219017,    419835436,    445153916,    471153890,    497813174,    525108021,    553013175,
    581501933,    610546210,    640116599,    670182449,    700711932,    731672120,    763029066,    794747877,
    826792808,    859127335,    891714249,    924515745,    957493504,    990608794,   1023822553,   1057095487,
   1090388161,   1123661095,   1156874854,   1189990144,   1222967903,   1255769399,   1288356313,   1320690840,
   1352735771,   1384454582,   1415811528,   1446771716,   1477301199,   1507367049,   1536937438,   1565981715,
   1594470473,   1622375627,   1649670474,   1676329758,   1702329732,   1727648212,   1752264631,   1776160086,
   1799317385,   1821721088,   1843357539,   1864214905,   1884283201,   1903554310,   1922022010,   1939681983,
   1956531826,   1972571058,   1987801121,   2002225378,   2015849099,   2028679456,   2040725503,   2051998153,
   2062510153,   2072276055,   2081312181,   2089636584,   2097269004,   2104230823,   2110545014,   2116236085,
   2121330023,   2125854231,   2129837463,   2133309756,   2136302361,   2138847664,   2140979113,   2142731138,
   2144139068,   2145239049,   2146067958,   2146663313,   2147063188,   2147306119,   2147431016,   2147477067,
   2147483648
};


// Blackman-Harris Window. Mathematica code.
//
// a0 = 0.3587500000000000000000000000000000000000000000000000;
// a1 = 0.4882900000000000000000000000000000000000000000000000;
// a2 = 0.1412800000000000000000000000000000000000000000000000;
// a3 = 0.0116800000000000000000000000000000000000000000000000;
//
// BH[x_] = a0 - a1 Cos[2 Pi x] + a2 Cos[4 Pi x] - a3 Cos[6 Pi x];
// BHIHI[y_] = Integrate[BH[x], {x, 0, y}]
// Tab = Table[Round[2^31 BHI[i/129]/BHI[1]], {i, 1, 128}];
// Do[{l = Table[PaddedForm[Tab[[j]], 12], {j, 8*i + 1, 8*i + 8}], Print[Row[l, ","], ","]}, {i, 0, 15}]
//
const int32_t blackman_harris_window_table[WINDOW_TABLE_LENGTH+1] = {
         3305,         9778,        22804,        46197,        84420,       142798,       227731,       346909,
       509525,       726474,      1010566,      1376713,      1842122,      2426465,      3152045,      4043938,
      5130118,      6441557,      8012296,      9879493,     12083427,     14667468,     17678014,     21164373,
     25178607,     29775326,     35011439,     40945845,     47639087,     55152951,     63550020,     72893188,
     83245126,     94667721,    107221476,    120964892,    135953824,    152240832,    169874519,    188898887,
    209352690,    231268820,    254673713,    279586805,    306020025,    333977346,    363454400,    394438165,
    426906721,    460829093,    496165178,    532865761,    570872623,    610118743,    650528584,    692018482,
    734497114,    777866051,    822020396,    866849483,    912237655,    958065083,   1004208652,   1050542864,
   1096940784,   1143274996,   1189418565,   1235245993,   1280634165,   1325463252,   1369617597,   1412986534,
   1455465166,   1496955064,   1537364905,   1576611025,   1614617887,   1651318470,   1686654555,   1720576927,
   1753045483,   1784029248,   1813506302,   1841463623,   1867896843,   1892809935,   1916214828,   1938130958,
   1958584761,   1977609129,   1995242816,   2011529824,   2026518756,   2040262172,   2052815927,   2064238522,
   2074590460,   2083933628,   2092330697,   2099844561,   2106537803,   2112472209,   2117708322,   2122305041,
   2126319275,   2129805634,   2132816180,   2135400221,   2137604155,   2139471352,   2141042091,   2142353530,
   2143439710,   2144331603,   2145057183,   2145641526,   2146106935,   2146473082,   2146757174,   2146974123,
   2147136739,   2147255917,   2147340850,   2147399228,   2147437451,   2147460844,   2147473870,   2147480343,
   2147483648
};

static int16_t interp(const int32_t table[WINDOW_TABLE_LENGTH+1], int k, int size) {
  uint32_t i, pos, inc, index, scale;
  int32_t val1, val2, val;
  pos = k * (0xFFFFFFFFu/size);
  index = pos >> 25;
  val1 = table[index]>>16;
  val2 = table[index+1]>>16;
  scale = (pos >> 8) & 0xFFFF;
  val2 *= scale;
  val1 *= 0x10000 - scale;
  return (val1 + val2) >> 16;
}
  
static int16_t hann_interp(int k, int size) { return interp(hann_window_table, k, size); }
static int16_t blackman_harris_interp(int k, int size) { return interp(blackman_harris_window_table, k, size); }
elapsedMicros usecs;

void benchmark_time(const char *name, int16_t (*window)(int, int), int size, int16_t *value) {
  usecs = 0;
  for (int i = 0; i < size; i += 1) { value[i] = (int16_t)((1<<15)*window(2*size-1, i)); }
  unsigned long t = usecs;
  Serial.printf("benchmark_time(%s, ..., %d) = %lu microseconds, %f us/element\n", name, size, t, t/((float)size));
}

void benchmark_windows() {
  int16_t values[1024];
  benchmark_time("hann", hann, 1024, values);
  benchmark_time("blackman-harris", blackman_harris, 1024, values);
  benchmark_time("hann interp", hann_interp, 1024, values);
  benchmark_time("blackman-harris interp", blackman_harris_interp, 1024, values);
}

void diagnostics_loop() {
  if (Serial.available()) {
    switch (Serial.read()) {
    case '?': 
      Serial.println("hasak monitor usage:");
      Serial.println(" 0 -> audio library resource usage");
      Serial.println(" 1 -> probe1 trace");
      Serial.println(" 2 -> probe2 trace");
      Serial.println(" r -> reset usage counts");
      Serial.println(" w -> benchmark window computations");
      break;
    case '0':
      Serial.print(" cpu usage "); Serial.print(AudioProcessorUsage()); 
      Serial.print(" max "); Serial.print(AudioProcessorUsageMax());
      // Serial.println();
      Serial.print(" mem usage "); Serial.print(AudioMemoryUsage());
      Serial.print(" max "); Serial.print(AudioMemoryUsageMax());
      Serial.println();
      ireport("left paddle", l_pad);
      ireport("right paddle", r_pad);
      ireport("straight key", s_key);
      ireport("ptt switch", ptt_sw);
      mreport("paddle", paddle);
      mreport("arbiter", arbiter);
      mreport("key_ramp", key_ramp);
      mreport("sidetone", sine_I);
      mreport("I_ramp", I_ramp);
      mreport("and_not_kyr", and_not_kyr);
      mreport("ptt_delay", ptt_delay);
      oreport("key out", key_out); 
      oreport("ptt out", ptt_out);
      break;
    case '1': preport("probe1", &_probe1, 5*44100); break;
    case '2': preport("probe2", &_probe2, 5*44100); break;
    case 'r':
      AudioProcessorUsageMaxReset();
      AudioMemoryUsageMaxReset();
      ireset(l_pad);
      ireset(r_pad);
      ireset(s_key);
      ireset(ptt_sw);
      mreset(paddle);
      mreset(arbiter);
      mreset(key_ramp);
      mreset(sine_I);
      mreset(I_ramp);
      mreset(and_not_kyr);
      mreset(ptt_delay);
      oreset(key_out);
      oreset(ptt_out);
      break;
    case 'w':
      benchmark_windows();
      break;
    default:
      //Serial.print("buttonpolls: "); 
      //Serial.println(buttonpolls); 
      break;
    }
  }
}  
