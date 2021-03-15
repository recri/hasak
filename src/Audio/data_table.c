#include "data_table.h"

const int16_t hann_ramp[257] = {
     0,     1,     5,    11,    20,    31,    45,    61,
    79,   100,   124,   150,   178,   209,   242,   278,
   316,   357,   400,   445,   493,   543,   596,   651,
   708,   768,   830,   895,   961,  1031,  1102,  1176,
  1252,  1330,  1411,  1494,  1579,  1666,  1756,  1848,
  1942,  2038,  2137,  2237,  2340,  2445,  2552,  2661,
  2772,  2885,  3000,  3117,  3236,  3358,  3481,  3606,
  3733,  3862,  3993,  4125,  4260,  4396,  4535,  4675,
  4816,  4960,  5105,  5252,  5401,  5551,  5703,  5857,
  6012,  6169,  6327,  6487,  6648,  6811,  6975,  7140,
  7308,  7476,  7646,  7817,  7989,  8163,  8338,  8514,
  8691,  8869,  9049,  9230,  9411,  9594,  9778,  9963,
 10149, 10335, 10523, 10712, 10901, 11091, 11282, 11474,
 11667, 11860, 12054, 12249, 12444, 12640, 12836, 13033,
 13230, 13428, 13627, 13826, 14025, 14224, 14424, 14624,
 14825, 15025, 15226, 15427, 15628, 15830, 16031, 16232,
 16434, 16635, 16837, 17038, 17239, 17440, 17641, 17842,
 18043, 18243, 18443, 18643, 18842, 19041, 19239, 19438,
 19635, 19833, 20029, 20225, 20421, 20616, 20810, 21004,
 21197, 21389, 21580, 21771, 21961, 22150, 22338, 22525,
 22711, 22897, 23081, 23264, 23447, 23628, 23808, 23987,
 24165, 24342, 24517, 24691, 24864, 25036, 25206, 25375,
 25543, 25710, 25874, 26038, 26200, 26360, 26520, 26677,
 26833, 26987, 27140, 27291, 27441, 27588, 27735, 27879,
 28022, 28163, 28302, 28439, 28575, 28708, 28840, 28970,
 29098, 29224, 29348, 29470, 29591, 29709, 29825, 29939,
 30051, 30161, 30269, 30375, 30479, 30580, 30680, 30777,
 30872, 30965, 31056, 31145, 31231, 31315, 31397, 31476,
 31553, 31628, 31701, 31771, 31839, 31905, 31968, 32029,
 32088, 32144, 32198, 32249, 32298, 32345, 32389, 32431,
 32470, 32507, 32542, 32574, 32603, 32631, 32655, 32678,
 32697, 32715, 32730, 32742, 32752, 32759, 32764, 32767,
 32767
};
const int16_t blackman_harris_ramp[257] = {
     2,     2,     2,     3,     3,     4,     5,     5,
     7,     8,     9,    11,    13,    15,    17,    19,
    22,    24,    27,    31,    34,    38,    42,    46,
    51,    56,    61,    67,    73,    79,    86,    93,
   101,   109,   117,   126,   136,   146,   157,   168,
   180,   192,   206,   219,   234,   249,   265,   282,
   300,   318,   338,   358,   379,   401,   424,   448,
   474,   500,   527,   556,   585,   616,   648,   682,
   717,   753,   790,   829,   869,   911,   955,  1000,
  1046,  1094,  1144,  1195,  1249,  1304,  1360,  1419,
  1480,  1542,  1606,  1673,  1741,  1811,  1884,  1958,
  2035,  2114,  2195,  2279,  2364,  2452,  2543,  2635,
  2730,  2828,  2928,  3030,  3135,  3243,  3353,  3466,
  3581,  3699,  3819,  3943,  4069,  4197,  4329,  4463,
  4600,  4739,  4882,  5027,  5175,  5326,  5479,  5636,
  5795,  5957,  6122,  6290,  6461,  6634,  6811,  6990,
  7172,  7356,  7544,  7734,  7927,  8123,  8321,  8522,
  8726,  8933,  9142,  9354,  9568,  9785, 10004, 10226,
 10450, 10677, 10906, 11137, 11371, 11607, 11845, 12085,
 12327, 12571, 12817, 13065, 13315, 13566, 13820, 14075,
 14331, 14589, 14849, 15110, 15372, 15635, 15900, 16166,
 16432, 16700, 16968, 17237, 17507, 17777, 18048, 18319,
 18591, 18862, 19134, 19406, 19678, 19949, 20220, 20491,
 20762, 21032, 21301, 21569, 21837, 22104, 22369, 22634,
 22897, 23158, 23419, 23677, 23934, 24189, 24443, 24694,
 24943, 25190, 25435, 25677, 25916, 26153, 26388, 26619,
 26848, 27073, 27296, 27515, 27731, 27944, 28153, 28358,
 28560, 28758, 28952, 29142, 29328, 29510, 29688, 29861,
 30030, 30195, 30355, 30511, 30662, 30808, 30949, 31085,
 31217, 31343, 31465, 31581, 31692, 31798, 31899, 31994,
 32084, 32169, 32248, 32321, 32389, 32452, 32509, 32560,
 32606, 32646, 32680, 32709, 32732, 32749, 32761, 32766,
 32766
};
const int16_t linear_ramp[257] = {
     0,   128,   256,   385,   513,   641,   769,   898,
  1026,  1154,  1282,  1411,  1539,  1667,  1795,  1924,
  2052,  2180,  2308,  2437,  2565,  2693,  2821,  2950,
  3078,  3206,  3334,  3463,  3591,  3719,  3847,  3976,
  4104,  4232,  4360,  4489,  4617,  4745,  4873,  5002,
  5130,  5258,  5386,  5515,  5643,  5771,  5899,  6028,
  6156,  6284,  6412,  6541,  6669,  6797,  6925,  7054,
  7182,  7310,  7438,  7567,  7695,  7823,  7951,  8080,
  8208,  8336,  8464,  8593,  8721,  8849,  8977,  9106,
  9234,  9362,  9490,  9618,  9747,  9875, 10003, 10131,
 10260, 10388, 10516, 10644, 10773, 10901, 11029, 11157,
 11286, 11414, 11542, 11670, 11799, 11927, 12055, 12183,
 12312, 12440, 12568, 12696, 12825, 12953, 13081, 13209,
 13338, 13466, 13594, 13722, 13851, 13979, 14107, 14235,
 14364, 14492, 14620, 14748, 14877, 15005, 15133, 15261,
 15390, 15518, 15646, 15774, 15903, 16031, 16159, 16287,
 16416, 16544, 16672, 16800, 16929, 17057, 17185, 17313,
 17442, 17570, 17698, 17826, 17955, 18083, 18211, 18339,
 18468, 18596, 18724, 18852, 18980, 19109, 19237, 19365,
 19493, 19622, 19750, 19878, 20006, 20135, 20263, 20391,
 20519, 20648, 20776, 20904, 21032, 21161, 21289, 21417,
 21545, 21674, 21802, 21930, 22058, 22187, 22315, 22443,
 22571, 22700, 22828, 22956, 23084, 23213, 23341, 23469,
 23597, 23726, 23854, 23982, 24110, 24239, 24367, 24495,
 24623, 24752, 24880, 25008, 25136, 25265, 25393, 25521,
 25649, 25778, 25906, 26034, 26162, 26291, 26419, 26547,
 26675, 26804, 26932, 27060, 27188, 27317, 27445, 27573,
 27701, 27830, 27958, 28086, 28214, 28342, 28471, 28599,
 28727, 28855, 28984, 29112, 29240, 29368, 29497, 29625,
 29753, 29881, 30010, 30138, 30266, 30394, 30523, 30651,
 30779, 30907, 31036, 31164, 31292, 31420, 31549, 31677,
 31805, 31933, 32062, 32190, 32318, 32446, 32575, 32703,
 32703
};
const int16_t sine_table[257] = {
     0,   804,  1608,  2410,  3212,  4011,  4808,  5602,
  6393,  7179,  7962,  8739,  9512, 10278, 11039, 11793,
 12539, 13279, 14010, 14732, 15446, 16151, 16846, 17530,
 18204, 18868, 19519, 20159, 20787, 21403, 22005, 22594,
 23170, 23731, 24279, 24811, 25329, 25832, 26319, 26790,
 27245, 27683, 28105, 28510, 28898, 29268, 29621, 29956,
 30273, 30571, 30852, 31113, 31356, 31580, 31785, 31971,
 32137, 32285, 32412, 32521, 32609, 32678, 32728, 32757,
 32767, 32757, 32728, 32678, 32609, 32521, 32412, 32285,
 32137, 31971, 31785, 31580, 31356, 31113, 30852, 30571,
 30273, 29956, 29621, 29268, 28898, 28510, 28105, 27683,
 27245, 26790, 26319, 25832, 25329, 24811, 24279, 23731,
 23170, 22594, 22005, 21403, 20787, 20159, 19519, 18868,
 18204, 17530, 16846, 16151, 15446, 14732, 14010, 13279,
 12539, 11793, 11039, 10278,  9512,  8739,  7962,  7179,
  6393,  5602,  4808,  4011,  3212,  2410,  1608,   804,
     0,  -804, -1608, -2410, -3212, -4011, -4808, -5602,
 -6393, -7179, -7962, -8739, -9512,-10278,-11039,-11793,
-12539,-13279,-14010,-14732,-15446,-16151,-16846,-17530,
-18204,-18868,-19519,-20159,-20787,-21403,-22005,-22594,
-23170,-23731,-24279,-24811,-25329,-25832,-26319,-26790,
-27245,-27683,-28105,-28510,-28898,-29268,-29621,-29956,
-30273,-30571,-30852,-31113,-31356,-31580,-31785,-31971,
-32137,-32285,-32412,-32521,-32609,-32678,-32728,-32757,
-32767,-32757,-32728,-32678,-32609,-32521,-32412,-32285,
-32137,-31971,-31785,-31580,-31356,-31113,-30852,-30571,
-30273,-29956,-29621,-29268,-28898,-28510,-28105,-27683,
-27245,-26790,-26319,-25832,-25329,-24811,-24279,-23731,
-23170,-22594,-22005,-21403,-20787,-20159,-19519,-18868,
-18204,-17530,-16846,-16151,-15446,-14732,-14010,-13279,
-12539,-11793,-11039,-10278, -9512, -8739, -7962, -7179,
 -6393, -5602, -4808, -4011, -3212, -2410, -1608,  -804,
     0
};
