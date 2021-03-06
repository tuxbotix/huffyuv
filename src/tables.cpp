//
// Huffyuv v2.1.1, by Ben Rudiak-Gould.
// http://www.math.berkeley.edu/~benrg/huffyuv.html
//
// This file is copyright 2000 Ben Rudiak-Gould, and distributed under
// the terms of the GNU General Public License, v2 or later.  See
// http://www.gnu.org/copyleft/gpl.html.
//


#include "huffyuv.h"


// The Huffman tables are run-length encoded.  (I could have Huffman-
// encoded them, but the madness has to end somewhere.)  The
// decompression algorithm is as follows: Read a byte.  The lower five
// bits are the value to repeat.  If the upper three bits are zero, the
// repeat count is in the next byte; otherwise, the upper three bits are
// the repeat count.  The tables are zero-terminated so that I can use
// string functions on them (a zero byte can never appear in the RLE
// data).

// Each array actually contains three Huffman tables, one for each sample
// type (e.g. Y,U,V, or R-G,G,B-G).  Each table expands to 256 bytes, and
// each byte is a code length.  The codes themselves are determined at
// run time and are not stored here--except for the "classic" tables,
// which don't fit the code-choosing algorithm.


const unsigned char left_yuv[] = {
  66,36,37,38,71,136,233,10,12,11,18,12,21,13,21,14,17,15,13,208,
  17,8,16,8,15,13,46,47,14,12,13,17,12,22,11,17,10,12,233,200,
  71,38,37,36,35,

  66,36,37,38,39,40,41,42,75,44,77,78,79,80,81,114,83,52,149,86,
  87,185,58,89,26,158,57,122,57,90,57,87,54,55,150,53,54,149,116,115,
  114,81,112,175,78,45,44,43,74,40,39,38,37,36,34,

  66,36,37,71,40,73,74,75,76,77,78,79,80,113,178,49,176,81,82,52,
  53,55,26,174,121,90,56,57,56,119,86,53,52,51,52,51,49,48,47,46,
  77,76,75,74,73,40,39,38,37,36,34, 0
};

/////////////////////////////////////////////////////////////////////

const unsigned char grad_yuv[] = {
  34,35,36,37,38,103,168,9,8,10,11,11,10,12,8,237,14,9,15,11,
  16,8,241,242,19,8,212,85,54,23,10,54,119,54,55,54,119,182,55,54,
  53,54,117,244,51,52,147,178,177,16,8,15,9,14,10,13,9,12,9,11,
  9,10,11,9,8,168,103,38,37,36,34,

  66,36,38,40,42,44,45,46,80,81,82,115,148,21,10,54,151,58,56,90,
  57,26,162,57,90,56,57,58,56,58,57,58,57,152,55,54,55,86,117,52,
  53,116,53,52,53,148,147,82,49,48,79,45,44,41,39,37,35,34,

  66,36,38,40,41,42,76,45,78,47,112,113,82,115,52,53,55,86,55,54,
  55,54,23,193,54,53,54,53,52,53,51,84,51,50,51,82,49,112,47,78,
  45,44,43,42,41,40,37,35,34, 0
};

/////////////////////////////////////////////////////////////////////

const unsigned char med_yuv[] = {
  66,37,38,71,104,201,10,9,11,11,12,9,13,9,14,9,15,8,16,9,
  209,50,49,210,83,52,83,116,53,55,54,55,56,89,122,56,55,56,121,58,
  57,58,89,88,57,56,55,88,57,120,89,90,152,121,58,57,56,55,86,53,
  54,53,54,53,52,53,116,115,18,8,17,8,16,8,15,9,238,13,9,12,
  10,11,11,234,201,136,103,38,36,34,

  66,36,38,40,42,43,44,45,46,47,48,81,50,115,116,53,52,149,84,85,
  54,55,25,25,26,140,25,16,88,57,56,55,56,118,117,20,10,147,114,49,
  80,47,78,44,43,42,39,37,35,34,

  33,35,37,39,40,41,75,44,77,46,79,112,113,82,148,53,57,56,89,56,
  25,184,56,25,12,56,57,88,54,55,53,84,51,82,49,48,79,46,45,76,
  43,42,41,40,39,36,34, 0
};

/////////////////////////////////////////////////////////////////////

const unsigned char left_rgb[] = {
  99,36,37,38,71,136,201,10,13,11,19,12,22,13,28,14,33,15,9,46,
  47,14,15,13,24,44,45,12,21,11,19,10,11,9,8,168,71,38,37,36,
  67,

  34,67,37,70,71,168,9,9,10,14,11,22,12,24,13,26,14,46,13,17,
  12,27,11,20,10,14,9,10,200,103,70,37,36,35,

  34,35,68,37,38,71,136,233,10,11,11,18,12,24,13,28,46,45,14,61,
  13,22,12,21,11,19,10,11,9,8,168,71,38,37,68,35, 0
};

/////////////////////////////////////////////////////////////////////

const unsigned char left_decorrelate_rgb[] = {
  34,67,36,37,38,39,73,42,43,76,173,14,9,207,208,241,210,51,82,243,
  180,53,52,181,54,53,118,87,54,56,58,88,153,26,15,57,26,38,57,122,
  89,186,89,90,89,186,56,57,58,89,90,56,55,56,57,55,182,181,20,9,
  243,18,8,17,8,16,11,207,174,109,44,75,41,40,39,38,37,36,67,

  34,67,37,70,71,168,9,9,10,14,11,22,12,24,13,26,14,46,13,17,
  12,27,11,20,10,14,9,10,200,103,70,37,36,35,

  34,67,36,70,39,40,137,74,107,76,109,238,207,240,17,9,50,49,82,49,
  18,8,49,50,113,114,83,52,53,54,55,88,90,57,26,102,57,56,87,86,
  85,52,53,116,243,50,83,210,209,176,175,206,77,108,75,138,73,40,39,38,
  37,36,67, 0
};

/////////////////////////////////////////////////////////////////////

const unsigned char grad_decorrelate_rgb[] = {
  34,67,36,37,38,39,40,42,75,108,141,174,207,208,17,8,210,179,180,181,
  54,117,54,53,54,55,54,55,54,53,55,85,55,86,55,86,57,56,57,87,
  57,56,89,56,121,56,25,66,55,89,55,89,88,119,56,151,214,53,55,54,
  181,52,85,51,52,179,18,10,17,8,176,175,174,173,108,75,42,40,39,38,
  37,36,67,

  34,67,37,38,103,136,9,8,10,11,11,13,12,11,13,10,14,11,15,10,
  16,13,209,146,51,50,147,244,53,52,53,20,10,51,52,19,8,18,9,241,
  16,12,15,11,14,9,13,10,12,9,11,13,10,12,233,168,103,38,37,67,

  34,67,36,38,39,40,42,75,108,141,174,207,176,209,178,211,148,53,52,53,
  118,55,118,87,88,55,57,88,55,56,57,55,88,57,56,57,88,153,56,89,
  56,121,56,25,77,88,87,86,53,54,85,52,85,148,53,20,9,211,50,115,
  18,9,241,240,111,142,141,140,75,42,40,39,38,99, 0
};

/////////////////////////////////////////////////////////////////////

const unsigned char classic_shift_luma[] = {
  34,36,35,69,135,232,9,16,10,24,11,23,12,16,13,10,14,8,15,8,
  16,8,17,20,16,10,207,206,205,236,11,8,10,21,9,23,8,8,199,70,
  69,68, 0
};

const unsigned char classic_shift_chroma[] = {
  66,36,37,38,39,40,41,75,76,77,110,239,144,81,82,83,84,85,118,183,
  56,57,88,89,56,89,154,57,58,57,26,141,57,56,58,57,58,57,184,119,
  214,245,116,83,82,49,80,79,78,77,44,75,41,40,39,38,37,36,34, 0
};

// How did the values in the tables below get so weirdly
// scrambled, you ask?  Answer: I qsorted by code length and
// didn't sub-sort by code.  Too late to fix it now.

const unsigned char classic_add_luma[256] = {
    3,  9,  5, 12, 10, 35, 32, 29, 27, 50, 48, 45, 44, 41, 39, 37,
   73, 70, 68, 65, 64, 61, 58, 56, 53, 50, 49, 46, 44, 41, 38, 36,
   68, 65, 63, 61, 58, 55, 53, 51, 48, 46, 45, 43, 41, 39, 38, 36,
   35, 33, 32, 30, 29, 27, 26, 25, 48, 47, 46, 44, 43, 41, 40, 39,
   37, 36, 35, 34, 32, 31, 30, 28, 27, 26, 24, 23, 22, 20, 19, 37,
   35, 34, 33, 31, 30, 29, 27, 26, 24, 23, 21, 20, 18, 17, 15, 29,
   27, 26, 24, 22, 21, 19, 17, 16, 14, 26, 25, 23, 21, 19, 18, 16,
   15, 27, 25, 23, 21, 19, 17, 16, 14, 26, 25, 23, 21, 18, 17, 14,
   12, 17, 19, 13,  4,  9,  2, 11,  1,  7,  8,  0, 16,  3, 14,  6,
   12, 10,  5, 15, 18, 11, 10, 13, 15, 16, 19, 20, 22, 24, 27, 15,
   18, 20, 22, 24, 26, 14, 17, 20, 22, 24, 27, 15, 18, 20, 23, 25,
   28, 16, 19, 22, 25, 28, 32, 36, 21, 25, 29, 33, 38, 42, 45, 49,
   28, 31, 34, 37, 40, 42, 44, 47, 49, 50, 52, 54, 56, 57, 59, 60,
   62, 64, 66, 67, 69, 35, 37, 39, 40, 42, 43, 45, 47, 48, 51, 52,
   54, 55, 57, 59, 60, 62, 63, 66, 67, 69, 71, 72, 38, 40, 42, 43,
   46, 47, 49, 51, 26, 28, 30, 31, 33, 34, 18, 19, 11, 13,  7,  8,
};

const unsigned char classic_add_chroma[256] = {
    3,  1,  2,  2,  2,  2,  3,  3,  7,  5,  7,  5,  8,  6, 11,  9,
    7, 13, 11, 10,  9,  8,  7,  5,  9,  7,  6,  4,  7,  5,  8,  7,
   11,  8, 13, 11, 19, 15, 22, 23, 20, 33, 32, 28, 27, 29, 51, 77,
   43, 45, 76, 81, 46, 82, 75, 55, 56,144, 58, 80, 60, 74,147, 63,
  143, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
   80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 27, 30, 21, 22,
   17, 14,  5,  6,100, 54, 47, 50, 51, 53,106,107,108,109,110,111,
  112,113,114,115,  4,117,118, 92, 94,121,122,  3,124,103,  2,  1,
    0,129,130,131,120,119,126,125,136,137,138,139,140,141,142,134,
  135,132,133,104, 64,101, 62, 57,102, 95, 93, 59, 61, 28, 97, 96,
   52, 49, 48, 29, 32, 25, 24, 46, 23, 98, 45, 44, 43, 20, 42, 41,
   19, 18, 99, 40, 15, 39, 38, 16, 13, 12, 11, 37, 10,  9,  8, 36,
    7,128,127,105,123,116, 35, 34, 33,145, 31, 79, 42,146, 78, 26,
   83, 48, 49, 50, 44, 47, 26, 31, 30, 18, 17, 19, 21, 24, 25, 13,
   14, 16, 17, 18, 20, 21, 12, 14, 15,  9, 10,  6,  9,  6,  5,  8,
    6, 12,  8, 10,  7,  9,  6,  4,  6,  2,  2,  3,  3,  3,  3,  2,
};
