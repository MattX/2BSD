# define SEOS 1
# define SCOMMENT 2
# define SLABEL 3
# define SUNKNOWN 4
# define SHOLLERITH 5
# define SICON 6
# define SRCON 7
# define SDCON 8
# define SBITCON 9
# define SOCTCON 10
# define SHEXCON 11
# define STRUE 12
# define SFALSE 13
# define SNAME 14
# define SNAMEEQ 15
# define SFIELD 16
# define SSCALE 17
# define SINCLUDE 18
# define SLET 19
# define SASSIGN 20
# define SAUTOMATIC 21
# define SBACKSPACE 22
# define SBLOCK 23
# define SCALL 24
# define SCHARACTER 25
# define SCLOSE 26
# define SCOMMON 27
# define SCOMPLEX 28
# define SCONTINUE 29
# define SDATA 30
# define SDCOMPLEX 31
# define SDIMENSION 32
# define SDO 33
# define SDOUBLE 34
# define SELSE 35
# define SELSEIF 36
# define SEND 37
# define SENDFILE 38
# define SENDIF 39
# define SENTRY 40
# define SEQUIV 41
# define SEXTERNAL 42
# define SFORMAT 43
# define SFUNCTION 44
# define SGOTO 45
# define SASGOTO 46
# define SCOMPGOTO 47
# define SARITHIF 48
# define SLOGIF 49
# define SIMPLICIT 50
# define SINQUIRE 51
# define SINTEGER 52
# define SINTRINSIC 53
# define SLOGICAL 54
# define SOPEN 55
# define SPARAM 56
# define SPAUSE 57
# define SPRINT 58
# define SPROGRAM 59
# define SPUNCH 60
# define SREAD 61
# define SREAL 62
# define SRETURN 63
# define SREWIND 64
# define SSAVE 65
# define SSTATIC 66
# define SSTOP 67
# define SSUBROUTINE 68
# define STHEN 69
# define STO 70
# define SUNDEFINED 71
# define SWRITE 72
# define SLPAR 73
# define SRPAR 74
# define SEQUALS 75
# define SCOLON 76
# define SCOMMA 77
# define SCURRENCY 78
# define SPLUS 79
# define SMINUS 80
# define SSTAR 81
# define SSLASH 82
# define SPOWER 83
# define SCONCAT 84
# define SAND 85
# define SOR 86
# define SNEQV 87
# define SEQV 88
# define SNOT 89
# define SEQ 90
# define SLT 91
# define SGT 92
# define SLE 93
# define SGE 94
# define SNE 95
# define SDECODE 96
# define SENCODE 97

# line 99 "gram.in"
#include "defs"
#include "string_defs"

static int nstars;
static int ndim;
static int vartype;
static ftnint varleng;
static struct { ptr lb, ub; } dims[8];
static struct labelblock *labarray[MAXLABLIST];
static int lastwasbranch = NO;
static int thiswasbranch = NO;
extern ftnint yystno;

ftnint convci();
double convcd();
struct addrblock *nextdata(), *mkbitcon();
struct constblock *mklogcon(), *mkaddcon(), *mkrealcon();
struct constblock *mkstrcon(), *mkcxcon();
struct listblock *mklist();
struct listblock *mklist();
struct impldoblock *mkiodo();
struct extsym *comblock();

#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
#ifndef YYSTYPE
#define YYSTYPE int
#endif
YYSTYPE yylval, yyval;
# define YYERRCODE 256
short yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
-1, 20,
	1, 31,
	-2, 205,
-1, 24,
	1, 35,
	-2, 205,
-1, 148,
	1, 219,
	-2, 170,
-1, 166,
	77, 240,
	-2, 170,
-1, 228,
	76, 156,
	-2, 123,
-1, 242,
	73, 205,
	-2, 202,
-1, 269,
	1, 262,
	-2, 127,
-1, 273,
	1, 271,
	77, 271,
	-2, 129,
-1, 338,
	76, 157,
	-2, 125,
-1, 349,
	1, 242,
	14, 242,
	73, 242,
	77, 242,
	-2, 171,
-1, 402,
	90, 0,
	91, 0,
	92, 0,
	93, 0,
	94, 0,
	95, 0,
	-2, 137,
-1, 428,
	1, 265,
	77, 265,
	-2, 127,
-1, 430,
	1, 267,
	77, 267,
	-2, 127,
-1, 432,
	1, 269,
	77, 269,
	-2, 127,
-1, 479,
	77, 265,
	-2, 127,
	};
# define YYNPROD 276
# define YYLAST 1314
short yyact[]={

 218, 447, 449, 391, 448, 291, 390, 441, 262, 310,
 440, 249, 233, 277, 191, 347, 187, 348, 237, 308,
 105, 271,   5, 199, 114, 296,  17, 198, 209, 202,
 126, 268, 181, 197, 118, 195, 117,  95, 180, 116,
 316, 270, 314, 315, 316, 110, 160, 161, 314, 315,
 316, 102,  96,  97,  98, 446, 260, 254, 255, 256,
 445, 510, 256, 306, 486,  12, 160, 161, 254, 255,
 256, 257, 101, 153, 272, 153, 127, 101, 463,  10,
  53,  43,  70,  86,  14,  58,  67,  91,  36,  63,
  44,  40,  65,  69,  30,  64,  33,  32,  11,  88,
  34,  18,  39,  37,  27,  16,  54,  55,  56,  47,
  51,  41,  89,  61,  38,  66,  90,  28,  59,  85,
  13, 512,  81,  62,  49,  87,  26,  71,  60,  15,
 183, 184,  68,  84, 301, 182, 495, 185, 208, 300,
 118, 491, 117, 220, 293, 190, 101, 111, 147, 436,
 153, 435, 101, 253, 153, 434, 496,  82,  83, 419,
 239, 241, 362, 226, 153, 229, 225, 224, 221, 426,
 160, 161, 314, 315, 316, 322, 321, 320, 319, 318,
 153, 323, 325, 324, 327, 326, 328, 413, 278, 279,
 280, 281, 282, 160, 161, 254, 255, 256, 257, 410,
 285, 286, 212, 275, 276, 264, 388, 215, 369, 261,
 368, 295, 289, 313, 234, 238, 238, 288, 287, 293,
 294, 461, 162, 164, 462, 266, 171, 119, 367, 302,
 305, 304, 353, 265, 438, 313, 334, 439, 420, 313,
 393, 419, 383, 394, 267, 384, 253, 336, 377, 153,
 205, 378, 172, 344, 153, 153, 153, 153, 153, 253,
 253, 112, 109, 346, 128, 129, 130, 131, 313, 133,
 108, 107, 106, 313, 478,   4, 160, 161, 254, 255,
 256, 257, 350, 104, 253, 351, 312, 342, 330, 168,
 343, 508, 214, 332, 333, 411, 372, 373, 374, 228,
 396, 396, 335, 338, 375, 341, 395, 227, 513, 502,
 380, 330, 501, 313, 376, 345, 371, 500, 363, 492,
 379, 364, 365, 498, 493, 387, 354, 488, 186, 412,
 408, 313, 471, 313, 313, 418, 313, 297, 188, 313,
 303, 243, 313, 240, 247, 230, 313, 228, 196, 330,
 211, 207, 153, 253, 153, 313, 416, 253, 253, 253,
 253, 253, 101, 421, 163, 424, 170, 153, 101, 169,
 139, 425, 160, 161, 314, 315, 316, 322, 273, 273,
 273, 340, 468, 216, 437, 101, 392, 450, 398, 399,
 400, 401, 402, 403, 404, 405, 406, 407, 459, 313,
 313, 313, 313, 313, 313, 313, 313, 313, 313, 429,
 431, 433, 387, 135, 464,  99, 101, 148, 234, 166,
  29, 264, 206, 253, 217, 210, 134, 201, 253,  93,
 470, 100, 103, 473, 474, 425, 477, 475,   6, 251,
 115, 476, 246, 313, 480, 481, 482, 450, 485,  80,
 483,  79, 121, 487,  78,  77, 150, 442, 150,  76,
  75, 165,  74, 269, 269, 269, 313, 490, 313, 489,
 465, 467,  73, 313,  72,  57, 429, 431, 433, 153,
  50, 232, 273,  48,  46,  45, 238, 450, 472, 506,
 494, 505, 503,  42, 200, 466,  31, 151, 313, 151,
 253, 317, 339, 152, 337, 313, 331, 204, 313, 103,
 103, 103, 103, 509, 442, 389, 200, 203, 189, 382,
 192, 193, 194, 381, 452, 386, 385, 511, 132, 331,
 514, 298,  52, 150,  35, 307, 113, 150,  25,  24,
  23,  22, 192, 222, 223,  21, 238, 150, 263,  20,
  19, 497, 263, 160, 161, 254, 255, 256, 242, 504,
 244, 290,  92, 150,   9,   8, 507, 370,   7,   3,
   2,   1,   0,   0, 151, 273, 273, 273, 151,   0,
 299,   0,   0, 238,   0, 200,   0,   0, 151, 156,
 157, 158, 159,   0, 451,   0, 154, 155, 101, 103,
 423,   0, 292,   0, 151, 160, 161, 254, 255, 256,
 257,   0, 160, 161, 314, 315, 316, 322, 321, 115,
   0, 309, 311, 323, 325, 324, 327, 326, 328,   0,
   0,   0, 150,   0,   0, 192,   0, 150, 150, 150,
 150, 150, 273, 273, 273, 263,   0,   0, 263, 263,
   0,   0,   0, 245, 451,   0,   0, 258,   0,   0,
 428, 430, 432,   0,   0,   0,   0, 259, 156, 157,
 158, 159,   0, 151,   0, 154, 155, 101, 151, 151,
 151, 151, 151, 283,   0, 200, 160, 161, 314, 315,
 316, 322, 321, 320, 451,   0,   0, 323, 325, 324,
 327, 326, 328, 469,   0,   0,   0, 484,   0, 192,
 160, 161, 314, 315, 316, 322, 321, 320, 319, 318,
   0, 323, 325, 324, 327, 326, 328, 479, 430, 432,
   0, 156, 157, 158, 159, 150, 248, 150, 154, 155,
 101,   0, 160, 161, 284,   0, 263,   0, 331,   0,
 150,   0, 352,   0, 414,   0,   0, 356, 357, 358,
 359, 360, 156, 157, 158, 159,   0,   0, 444, 154,
 155, 101, 200,   0,   0,   0, 151,   0, 151,   0,
   0,   0, 160, 161, 314, 315, 316, 322,   0,   0,
   0, 151, 292, 323, 325, 324, 327, 326, 328, 248,
 453,   0, 460,   0,   0, 160, 161, 417, 309,   0,
 263, 454, 160, 161, 314, 315, 316, 322, 321, 320,
 319, 318,   0, 323, 325, 324, 327, 326, 328,   0,
 236, 231,   0,   0,   0,   0, 160, 161, 235,   0,
 460,   0,   0,   0,   0,   0, 219,   0, 460, 460,
 460,   0,   0,   0,   0, 422,   0, 422,   0,   0,
 453,   0, 150,   0, 453,   0,   0,   0,   0,   0,
 427, 454,   0,   0,   0, 454,   0,   0,  53,  43,
   0,  86,   0,  58, 361,  91,   0,   0,  44, 160,
 161, 254, 255, 256, 257,   0,   0,  88,   0,   0,
 453,   0,   0, 151,  54,  55,  56,  47,   0,   0,
  89, 454,   0,   0,  90,   0,  59,  85,   0,   0,
  81,   0,  49,  87,   0,   0,  60, 415, 122,   0,
   0,  84, 160, 161, 314, 315, 316, 322, 321, 320,
 319, 318,   0, 323, 325, 324, 327, 326, 328,   0,
   0,   0, 409,   0,   0,  82,  83, 160, 161, 314,
 315, 316, 322, 321, 320, 319, 318,   0, 323, 325,
 324, 327, 326, 328,   0,   0,   0,   0, 397,   0,
   0,   0, 499, 160, 161, 314, 315, 316, 322, 321,
 320, 319, 318,  94, 323, 325, 324, 327, 326, 328,
 366,   0, 160, 161, 314, 315, 316, 322, 321, 320,
 319, 318,   0, 323, 325, 324, 327, 326, 328,   0,
   0,   0,   0,   0, 120, 349, 123, 124, 125,   0,
 160, 161, 254, 255, 256, 257,   0, 136, 137,   0,
   0, 138,   0, 140, 141, 142,   0,   0, 143, 144,
 145,   0, 146, 156, 157, 158, 159,   0,   0,   0,
 154, 155, 101,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0, 173, 174, 175, 176, 177, 178, 179,
 156, 157, 158, 159,   0,   0,   0, 154, 155, 101,
   0, 156, 157, 158, 159,   0,   0,   0, 154, 155,
 101,   0, 156, 157, 158, 159,   0,   0,   0, 154,
 155, 101,   0,   0,   0, 156, 157, 158, 159,   0,
   0, 236, 154, 155, 101,   0,   0, 160, 161, 443,
   0,   0, 156, 157, 158, 159,   0, 219,   0, 154,
 155, 101,   0, 156, 157, 158, 159,   0, 236,   0,
 154, 155, 101,   0, 160, 161, 235,   0,   0, 236,
   0,   0,   0,   0, 219, 160, 161, 355,   0,   0,
 236,   0,   0,   0,   0, 219, 160, 161, 329,   0,
   0,   0,   0, 236,   0,   0, 219,   0,   0, 160,
 161,   0,   0,   0, 156, 157, 158, 159,   0, 219,
 274, 154, 155, 101, 252,   0, 160, 161,   0,   0,
   0, 213, 156, 157, 158, 159, 219, 160, 161, 154,
 155, 101,   0,   0,   0,   0,   0, 219, 156, 157,
 158, 159, 458, 457, 456, 154, 155, 101, 156, 157,
 158, 159,   0,   0,   0, 154, 155, 101,  70,   0,
   0,   0,  67,   0,   0,  63,   0,   0,  65,  69,
   0,  64, 248,   0,   0,   0,   0,   0, 160, 161,
 250,   0,   0,   0,   0,   0,   0,   0,   0,  61,
 149,  66,   0,   0,   0,   0, 160, 161, 167,  62,
   0,   0,   0,  71,   0,   0, 455,   0,  68,   0,
   0,   0, 160, 161,   0,   0, 149,   0,   0,   0,
   0,   0, 160, 161 };
short yypact[]={

-1000,  19, 437,  61,-1000,-1000,-1000,-1000,-1000,-1000,
 424,-1000,-1000,-1000,-1000,-1000,-1000, 371, 402, 206,
 195, 194, 193, 185,  70, 184,  58,-1000,-1000,-1000,
-1000, 859,-1000,-1000,-1000,  -5,-1000,-1000,-1000,-1000,
-1000,-1000, 402,-1000,-1000,-1000,-1000,-1000, 297,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,1233, 291,1207, 296, 296, 291, 175,-1000,
-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,-1000, 402, 402, 402, 402,-1000,
-1000,-1000, 265,-1000, 402, -48, 402, 402, 402, 275,
 354,-1000,-1000, 173,-1000,-1000,-1000,-1000, 408, 278,
 419,-1000,-1000, 277,-1000,-1000,-1000,1138,  58, 402,
 402, 275, 354,-1000, 232, 274, 419,-1000, 272, 757,
1110,1110, 270, 419, 402, 268, 402,-1000,-1000,1189,
-1000,-1000, 114, 726,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,1189, 132, 156,-1000,-1000, 348,1110,
1127,1127,1127,-1000,-1000,-1000,-1000,-1000,-1000, 663,
-1000,-1000,-1000, 265, 265, 402,  -5,-1000, 138,  -5,
  70,-1000, 264,-1000,-1000,-1000, 402,-1000,  57,-1000,
-1000, 354,-1000, 267,1227,  58, -19, 402, 402,-1000,
-1000,1110,  91,1097,-1000,-1000,-1000,-1000,1110,1110,
-1000, 402,-1000,-1000,-1000,-1000,-1000,1110,1110, 311,
1110,-1000, 213,-1000,  91, 419,1110,-1000,  91,-1000,
1110,-1000,  70, 419,-1000, 951, 208,-1000, 726, 155,
-1000,1086,-1000, 726, 726, 726, 726, 726, -21, 810,
  85, 348,-1000,-1000, 348, 348,  85, 923,-1000, 151,
 133, 131,  91,-1000,1127,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-1000, 114,-1000,-1000,-1000, 265, 264,-1000,
 174,-1000,-1000,-1000, 264, 402,-1000,-1000, 168,-1000,
-1000, 354, 129, 372,-1000,-1000,-1000, 166,-1000, 231,
-1000, 225, 904,1110,1110,1110,1110,1110,1110,1110,
1110,1110,1110,-1000,-1000,-1000,-1000,-1000,-1000, 256,
 878, 122, -43, 703,-1000,  91, 219, 255,  91, 110,
 402, 853,-1000,1075,-1000, 733, 262, 164,-1000,-1000,
-1000,1189, 526,1189,  91,-1000, -24, -21, -21, -21,
 474,-1000, 348,  85,  92,  85, 726,1127,1127,1127,
  78,  74,  72,-1000,-1000,-1000,-1000,-1000,  63,-1000,
-1000, 160,1048,-1000, 402, -22,1223,-1000, 354, 147,
-1000,  -2,-1000,-1000, 402,1110,1110, 313, -39, -43,
 -43, -43, 293, 607, 607, 533, 703, -33,-1000,-1000,
1110,1110, 259,1110,-1000, 419,-1000,-1000, 419, 419,
  70,-1000, 114,-1000,-1000,-1000, 348, 197,-1000,-1000,
-1000,-1000,-1000,-1000,1127,1127,1127,-1000,-1000,1048,
-1000,-1000, 631,-1000,-1000,-1000,1223,-1000,-1000, -17,
 584,-1000,-1000,-1000,-1000,1110,-1000,-1000,-1000, 253,
 226,-1000, 372, 372,-1000,  91,  64,  91,-1000, 245,
 250,1110,  91,  59,  82,-1000,1110, 249, 726, 245,
 243, 238, 235,-1000,1048,-1000,1223,-1000,-1000,-1000,
-1000,1110,-1000,-1000, 215, 419,-1000,  91,-1000, -13,
-1000,-1000,-1000,-1000,  91,-1000,-1000,  91,1110,  44,
-1000, 234, 419,-1000,-1000 };
short yypgo[]={

   0, 571, 570, 569, 568, 565, 564, 562, 993,  37,
  38,  32,  16,  26, 413, 561,   5, 550, 549, 545,
 541, 540, 539, 538, 536, 227, 535,  30,  25, 534,
 532,  74,  14,  39,  20,  35, 531, 383, 528,  33,
  27, 526, 525,   1,   4,   2,   0, 207, 524,  24,
  19,  23,   9, 523, 519,  10,   7,  15,  17,  28,
  29, 517, 515, 507,   6,   3, 504, 502, 292, 424,
 501,  18, 503, 344, 420, 496, 495, 493, 485, 484,
 483, 481, 480,  12, 475, 474, 148, 472, 462,  56,
 461, 460, 289, 459,  31, 455, 454, 451,  13, 449,
 442,  11, 439,   8,  41,  21 };
short yyr1[]={

   0,   1,   1,   2,   2,   2,   2,   2,   2,   2,
   3,   4,   4,   4,   4,   4,   4,   9,  11,  14,
  10,  10,  12,  12,  12,  15,  15,  16,  16,   7,
   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,
   5,  17,  17,  13,  29,  30,  30,  30,  30,  30,
  30,  30,  30,  30,  30,  30,  27,  27,  27,  18,
  18,  18,  18,  33,  33,  19,  19,  20,  20,  21,
  21,  35,  36,  36,  22,  22,  38,  39,  42,  41,
  41,  43,  43,  44,  44,  44,  44,  24,  24,  49,
  49,  26,  26,  50,  32,  51,  51,  40,  40,  28,
  28,  54,  53,  53,  55,  55,  56,  56,  57,  57,
  58,  59,  23,  23,  60,  63,  61,  62,  62,  64,
  64,  65,  25,  66,  66,  67,  67,  31,  31,  31,
  68,  68,  68,  68,  68,  68,  68,  68,  68,  68,
  68,  68,  68,  68,  46,  46,  70,  70,  70,  70,
  70,  70,  37,  37,  37,  37,  71,  71,  45,  45,
  69,  69,  69,  69,  69,  69,  47,  48,  48,  48,
  72,  72,  73,  73,  73,  73,  73,  73,  73,  73,
   6,   6,   6,   6,   6,   6,   6,  75,  52,  74,
  74,  74,  74,  74,  74,  74,  74,  74,  74,  74,
  77,  78,  78,  78,  78,  34,  34,  80,  81,  81,
  83,  83,  82,  82,  76,  76,   8,  79,  84,  84,
  84,  84,  84,  84,  84,  84,  84,  84,  84,  84,
  84,  84,  85,  97,  97,  97,  87,  99,  99,  99,
  90,  90,  86,  86, 100, 100, 101, 101, 101, 101,
 102,  92,  88,  95,  91,  93,  96,  96,  89,  89,
 103, 103,  94,  94,  94, 105, 105, 105, 105, 105,
 105, 104, 104, 104, 104,  98 };
short yyr2[]={

   0,   0,   3,   2,   2,   2,   3,   3,   2,   1,
   1,   3,   3,   4,   4,   5,   3,   0,   1,   1,
   0,   1,   0,   2,   3,   1,   3,   1,   1,   1,
   1,   1,   1,   1,   1,   1,   1,   1,   2,   1,
   5,   5,   5,   2,   1,   1,   1,   1,   1,   1,
   1,   1,   1,   1,   1,   1,   0,   2,   4,   3,
   4,   5,   3,   1,   3,   3,   3,   3,   3,   3,
   3,   3,   1,   3,   3,   3,   0,   4,   0,   2,
   3,   1,   3,   1,   2,   1,   1,   1,   3,   1,
   1,   1,   3,   3,   2,   1,   5,   1,   3,   0,
   3,   0,   2,   3,   1,   3,   1,   1,   1,   3,
   1,   1,   3,   3,   4,   0,   2,   1,   3,   1,
   3,   1,   0,   0,   1,   1,   3,   1,   3,   1,
   1,   1,   3,   3,   3,   3,   2,   3,   3,   3,
   3,   3,   2,   3,   1,   1,   1,   1,   1,   1,
   1,   1,   1,   6,   4,   9,   0,   1,   1,   1,
   1,   1,   1,   1,   1,   1,   5,   1,   1,   1,
   1,   3,   1,   1,   3,   3,   3,   3,   2,   3,
   1,   4,   2,   2,   6,   2,   2,   5,   3,   4,
   5,   2,   1,   1,  10,   1,   3,   4,   3,   3,
   1,   3,   3,   7,   7,   0,   1,   3,   1,   3,
   1,   2,   1,   1,   1,   3,   0,   1,   2,   2,
   2,   2,   3,   4,   4,   3,   2,   3,   2,   3,
   1,   3,   3,   1,   1,   1,   3,   1,   1,   1,
   1,   1,   3,   3,   3,   3,   1,   1,   2,   2,
   1,   7,   3,   3,   3,   3,   4,   4,   1,   3,
   1,   5,   1,   1,   1,   3,   3,   3,   3,   3,
   3,   1,   5,   5,   5,   0 };
short yychk[]={

-1000,  -1,  -2,  -3, 256,   3,   1,  -4,  -5,  -6,
  18,  37,   4,  59,  23,  68,  44, -13,  40, -17,
 -18, -19, -20, -21, -22, -23,  65,  43,  56, -74,
  33, -75,  36,  35,  39, -29,  27,  42,  53,  41,
  30,  50, -77,  20,  29, -78, -79,  48, -80,  63,
 -82,  49, -30,  19,  45,  46,  47, -84,  24,  57,
  67,  52,  62,  28,  34,  31,  54,  25,  71,  32,
  21,  66, -85, -87, -88, -91, -93, -95, -96, -97,
 -99,  61,  96,  97,  72,  58,  22,  64,  38,  51,
  55,  26,  -7,   5,  -8,  -9,  -9,  -9,  -9,  44,
 -14,  14, -11, -14,  77, -34,  77,  77,  77,  77,
 -34,  77,  77, -24, -49, -14, -33,  84,  82, -25,
  -8, -74,  69,  -8,  -8,  -8, -27,  81, -25, -25,
 -25, -25, -38, -25, -37, -14,  -8,  -8,  -8,  73,
  -8,  -8,  -8,  -8,  -8,  -8,  -8, -86, -73,  73,
 -37, -69, -72, -46,  12,  13,   5,   6,   7,   8,
  79,  80, -86,  73, -86, -90, -73,  81, -92,  73,
 -92, -86,  77,  -8,  -8,  -8,  -8,  -8,  -8,  -8,
 -10, -11, -10, -11, -11,  -9, -25, -12,  73, -14,
 -33, -32, -14, -14, -14, -35,  73, -39, -40, -51,
 -37,  73, -60, -61, -63,  77,  14,  73, -58, -59,
   6,  73, -31,  73, -68, -47, -37, -69, -46,  89,
 -32, -33, -14, -14, -35, -39, -60,  75,  73, -59,
  73,  74, -81, -83, -31,  81,  73, -71, -31, -71,
  73, -58, -14,  73, -14, -72,-100, -73,  73,-101,
  81,-102,  15, -46,  81,  82,  83,  84, -72, -72,
 -89,  77,-103, -37,  73,  77, -89, -31, -94, -68,
-104,-105, -31, -47,  73, -94, -94, -98, -98, -98,
 -98, -98, -98, -72,  81, -12, -12, -11, -27,  74,
 -15, -16, -14,  81, -27, -34, -28,  73, -36, -37,
  82,  77, -40,  73, -13, -49,  82, -26, -50, -14,
 -52, -14, -31, -46,  81,  82,  83, -70,  88,  87,
  86,  85,  84,  90,  92,  91,  94,  93,  95,  81,
 -31, -68, -31, -31, -32, -31, -71, -66, -31, -67,
  70, -31,  74,  77, -58, -31, -34, -57, -58,  74,
  74,  77, -72,  77, -31,  81, -72, -72, -72, -72,
 -72,  74,  77, -89, -89, -89,  77,  77,  77,  77,
 -68,-104,-105, -98, -98, -12, -28,  74,  77, -28,
 -32, -53, -54,  74,  77, -41, -42, -51,  77, -62,
 -64, -65,  14,  74,  77,  75,  75,  74, -31, -31,
 -31, -31, -31, -31, -31, -31, -31, -31,  74,  74,
  77,  76,  74,  77, -14,  74, -83,  74,  73,  77,
  74,-101, -72,  74,-101,-103,  77, -72, -68,-104,
 -68,-104, -68,-104,  77,  77,  77, -16,  74,  77,
 -55, -56, -31,  81, -37,  82,  77, -43, -44, -45,
 -46, -47, -48, -14, -69,  73,  11,  10,   9, -52,
 -14,  74,  77,  80, -50, -31, -76, -31,  69, -68,
 -71,  73, -31, -58, -57, -58, -34, -52,  77, -68,
 -52, -52, -52, -55,  76, -43,  81, -45,  74, -64,
 -65,  77,  74,  74, -71,  77,  74, -31,  74, -72,
  74,  74,  74, -56, -31, -44, -45, -31,  76, -58,
  74, -71,  77,  74, -58 };
short yydef[]={

   1,  -2,   0,   0,   9,  10,   2,   3,   4,   5,
   0, 216,   8,  17,  17,  17,  17,   0,   0,  30,
  -2,  32,  33,  34,  -2,  36,  37,  39, 122, 180,
 216,   0, 216, 216, 216,  56, 122, 122, 122, 122,
  76, 122,   0, 216, 216, 192, 193, 216, 195, 216,
 216, 216,  44, 200, 216, 216, 216, 217, 216, 212,
 213,  45,  46,  47,  48,  49,  50,  51,  52,  53,
  54,  55,   0,   0,   0,   0,   0,   0, 230, 216,
 216, 216, 216, 216, 216, 216, 233, 234, 235, 237,
 238, 239,   6,  29,   7,  20,  20,   0,   0,  17,
 122,  19,  22,  18,   0,   0, 206,   0,   0,   0,
   0, 206, 115,  38,  87,  89,  90,  63,   0,   0,
   0, 182, 183,   0, 185, 186,  43,   0,   0,   0,
   0,   0,   0, 115,   0, 152,   0, 191,   0,   0,
 156, 156,   0,   0,   0,   0,   0, 218,  -2,   0,
 172, 173,   0,   0, 160, 161, 162, 163, 164, 165,
 144, 145, 220,   0, 221,   0,  -2, 241,   0,   0,
 226, 228,   0, 275, 275, 275, 275, 275, 275,   0,
  11,  21,  12,  22,  22,   0,  56,  16,   0,  56,
 205,  62,  99,  66,  68,  70,   0,  75,   0,  97,
  95,   0, 113,   0,   0,   0,   0,   0,   0, 110,
 111,   0,  57,   0, 127, 129, 130, 131,   0,   0,
  59,   0,  65,  67,  69,  74, 112,   0,  -2,   0,
   0, 196,   0, 208, 210,   0,   0, 198, 157, 199,
   0, 201,  -2,   0, 207, 246,   0, 170,   0,   0,
 247,   0, 250,   0,   0,   0,   0,   0, 178, 246,
 222,   0, 258, 260,   0,   0, 225,   0, 227,  -2,
 263, 264,   0,  -2,   0, 229, 231, 232, 236, 252,
 254, 255, 253, 275, 275,  13,  14,  22,  99,  23,
   0,  25,  27,  28,  99,   0,  94, 101,   0,  72,
  78,   0,   0,   0, 116,  88,  64,   0,  91,   0,
 181,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0, 146, 147, 148, 149, 150, 151,   0,
   0, 127, 136, 142,  60, 189,   0,   0,  -2, 124,
   0,   0, 197,   0, 211,   0,   0,   0, 108,  -2,
 243,   0,   0,   0, 248, 249, 174, 175, 176, 177,
 179, 242,   0, 224,   0, 223,   0,   0,   0,   0,
 127,   0,   0, 256, 257,  15,  41,  24,   0,  42,
  61,   0,   0,  71,   0,   0,   0,  98,   0,   0,
 117, 119, 121,  40,   0,   0,   0,   0, 132, 133,
 134, 135,  -2, 138, 139, 140, 141, 143,  58, 128,
   0, 156, 154,   0, 190,   0, 209, 187,   0,   0,
 205, 245, 246, 171, 244, 259,   0,   0,  -2, 266,
  -2, 268,  -2, 270,   0,   0,   0,  26, 100,   0,
 102, 104, 107, 106,  73,  77,   0,  79,  81,  83,
   0,  85,  86, 158, 159,   0, 167, 168, 169,   0,
 152, 114,   0,   0,  92,  93, 188, 214, 184, 127,
   0, 156, 126,   0,   0, 109,   0,   0,   0,  -2,
   0,   0,   0, 103,   0,  80,   0,  84,  96, 118,
 120,   0, 166, 153,   0,   0, 203, 204, 261,   0,
 272, 273, 274, 105, 107,  82,  83, 215, 156,   0,
 251,   0,   0, 155, 194 };
#
# define YYFLAG -1000
# define YYERROR goto yyerrlab
# define YYACCEPT return(0)
# define YYABORT return(1)

/*	parser for yacc output	*/

int yydebug = 0; /* 1 for debugging */
YYSTYPE yyv[YYMAXDEPTH]; /* where the values are stored */
int yychar = -1; /* current input token number */
int yynerrs = 0;  /* number of errors */
short yyerrflag = 0;  /* error recovery flag */

yyparse() {

	short yys[YYMAXDEPTH];
	short yyj, yym;
	register YYSTYPE *yypvt;
	register short yystate, *yyps, yyn;
	register YYSTYPE *yypv;
	register short *yyxi;

	yystate = 0;
	yychar = -1;
	yynerrs = 0;
	yyerrflag = 0;
	yyps= &yys[-1];
	yypv= &yyv[-1];

 yystack:    /* put a state and value onto the stack */

	if( yydebug  ) printf( "state %d, char 0%o\n", yystate, yychar );
		if( ++yyps> &yys[YYMAXDEPTH] ) { error("yacc stack overflow",0,0,YYERR ); return(1); }
		*yyps = yystate;
		++yypv;
		*yypv = yyval;

 yynewstate:

	yyn = yypact[yystate];

	if( yyn<= YYFLAG ) goto yydefault; /* simple state */

	if( yychar<0 ) if( (yychar=yylex())<0 ) yychar=0;
	if( (yyn += yychar)<0 || yyn >= YYLAST ) goto yydefault;

	if( yychk[ yyn=yyact[ yyn ] ] == yychar ){ /* valid shift */
		yychar = -1;
		yyval = yylval;
		yystate = yyn;
		if( yyerrflag > 0 ) --yyerrflag;
		goto yystack;
		}

 yydefault:
	/* default state action */

	if( (yyn=yydef[yystate]) == -2 ) {
		if( yychar<0 ) if( (yychar=yylex())<0 ) yychar = 0;
		/* look through exception table */

		for( yyxi=yyexca; (*yyxi!= (-1)) || (yyxi[1]!=yystate) ; yyxi += 2 ) ; /* VOID */

		while( *(yyxi+=2) >= 0 ){
			if( *yyxi == yychar ) break;
			}
		if( (yyn = yyxi[1]) < 0 ) return(0);   /* accept */
		}

	if( yyn == 0 ){ /* error */
		/* error ... attempt to resume parsing */

		switch( yyerrflag ){

		case 0:   /* brand new error */

			error("syntax error",0,0,YYERR);
		yyerrlab:
			++yynerrs;

		case 1:
		case 2: /* incompletely recovered error ... try again */

			yyerrflag = 3;

			/* find a state where "error" is a legal shift action */

			while ( yyps >= yys ) {
			   yyn = yypact[*yyps] + YYERRCODE;
			   if( yyn>= 0 && yyn < YYLAST && yychk[yyact[yyn]] == YYERRCODE ){
			      yystate = yyact[yyn];  /* simulate a shift of "error" */
			      goto yystack;
			      }
			   yyn = yypact[*yyps];

			   /* the current yyps has no shift onn "error", pop stack */

			   if( yydebug ) printf( "error recovery pops state %d, uncovers %d\n", *yyps, yyps[-1] );
			   --yyps;
			   --yypv;
			   }

			/* there is no state on the stack with an error shift ... abort */

	yyabort:
			return(1);


		case 3:  /* no shift yet; clobber input char */

			if( yydebug ) printf( "error recovery discards char %d\n", yychar );

			if( yychar == 0 ) goto yyabort; /* don't discard EOF, quit */
			yychar = -1;
			goto yynewstate;   /* try again in the same state */

			}

		}

	/* reduction by production yyn */

		if( yydebug ) printf("reduce %d\n",yyn);
		yyps -= yyr2[yyn];
		yypvt = yypv;
		yypv -= yyr2[yyn];
		yyval = yypv[1];
		yym=yyn;
			/* consult goto table to find next state */
		yyn = yyr1[yyn];
		yyj = yypgo[yyn] + *yyps + 1;
		if( yyj>=YYLAST || yychk[ yystate = yyact[yyj] ] != -yyn ) yystate = yyact[yypgo[yyn]];
		switch(yym){
			
case 3:
# line 145 "gram.in"
{ lastwasbranch = NO; } break;
case 5:
# line 148 "gram.in"
{ if(yypvt[-1] && (yypvt[-1]->labelno==dorange))
			enddo(yypvt[-1]->labelno);
		  if(lastwasbranch && thislabel==NULL)
			error("statement cannot be reached",0,0,WARN);
		  lastwasbranch = thiswasbranch;
		  thiswasbranch = NO;
		} break;
case 6:
# line 156 "gram.in"
{ doinclude( yypvt[-0] ); } break;
case 7:
# line 158 "gram.in"
{ lastwasbranch = NO;  endproc(); } break;
case 8:
# line 160 "gram.in"
{ error("unclassifiable statement", 0,0,EXECERR);  flline(); } break;
case 9:
# line 162 "gram.in"
{ flline();  needkwd = NO;  inioctl = NO; 
		  yyerrok; yyclearin; } break;
case 10:
# line 167 "gram.in"
{
		if(yystno != 0)
			{
			yyval = thislabel =  mklabel(yystno);
			if( ! headerdone )
				puthead(NULL, procclass);
			if(thislabel->labdefined)
				error("label %s already defined",
					convic(thislabel->stateno),0,EXECERR);
			else	{
				if(thislabel->blklevel!=0 && thislabel->blklevel<blklevel
				    && thislabel->labtype!=LABFORMAT)
					error("there is a branch to label %s from outside block",
					      convic( (ftnint) (thislabel->stateno) ) ,0,WARN1);
				thislabel->blklevel = blklevel;
				thislabel->labdefined = YES;
				if(thislabel->labtype != LABFORMAT)
					putlabel(thislabel->labelno);
				}
			}
		else    yyval = thislabel = NULL;
		} break;
case 11:
# line 192 "gram.in"
{ startproc(yypvt[-0], CLMAIN); } break;
case 12:
# line 194 "gram.in"
{ startproc(yypvt[-0], CLBLOCK); } break;
case 13:
# line 196 "gram.in"
{ entrypt(CLPROC, TYSUBR, (ftnint) 0,  yypvt[-1], yypvt[-0]); } break;
case 14:
# line 198 "gram.in"
{ entrypt(CLPROC, TYUNKNOWN, (ftnint) 0, yypvt[-1], yypvt[-0]); } break;
case 15:
# line 200 "gram.in"
{ entrypt(CLPROC, yypvt[-4], varleng, yypvt[-1], yypvt[-0]); } break;
case 16:
# line 202 "gram.in"
{ if(parstate==OUTSIDE || procclass==CLMAIN
			|| procclass==CLBLOCK)
				error("misplaced entry statement", 0,0,EXECERR);
		  entrypt(CLENTRY, 0, (ftnint) 0, yypvt[-1], yypvt[-0]);
		} break;
case 17:
# line 210 "gram.in"
{ newproc(); } break;
case 18:
# line 214 "gram.in"
{ yyval = newentry(yypvt[-0]); } break;
case 19:
# line 218 "gram.in"
{ yyval = mkname(toklen, token); } break;
case 20:
# line 221 "gram.in"
{ yyval = NULL; } break;
case 22:
# line 226 "gram.in"
{ yyval = 0; } break;
case 23:
# line 228 "gram.in"
{ yyval = 0; } break;
case 24:
# line 230 "gram.in"
{yyval = yypvt[-1]; } break;
case 25:
# line 234 "gram.in"
{ yyval = (yypvt[-0] ? mkchain(yypvt[-0],0) : 0 ); } break;
case 26:
# line 236 "gram.in"
{ if(yypvt[-0]) yypvt[-2] = yyval = hookup(yypvt[-2], mkchain(yypvt[-0],0)); } break;
case 27:
# line 240 "gram.in"
{ yypvt[-0]->vstg = STGARG; } break;
case 28:
# line 242 "gram.in"
{ yyval = 0;  substars = YES; } break;
case 29:
# line 248 "gram.in"
{
		char *s;
		s = copyn(toklen+1, token);
		s[toklen] = '\0';
		yyval = s;
		} break;
case 37:
# line 263 "gram.in"
{ saveall = YES; } break;
case 39:
# line 266 "gram.in"
{ fmtstmt(thislabel); setfmt(thislabel); } break;
case 41:
# line 271 "gram.in"
{ settype(yypvt[-3], yypvt[-4], yypvt[-1]);
		  if(ndim>0) setbound(yypvt[-3],ndim,dims);
		} break;
case 42:
# line 275 "gram.in"
{ settype(yypvt[-2], yypvt[-4], yypvt[-1]);
		  if(ndim>0) setbound(yypvt[-2],ndim,dims);
		} break;
case 43:
# line 281 "gram.in"
{ varleng = yypvt[-0]; } break;
case 44:
# line 285 "gram.in"
{ varleng = (yypvt[-0]<0 || yypvt[-0]==TYLONG ? 0 : typesize[yypvt[-0]]); } break;
case 45:
# line 288 "gram.in"
{ yyval = TYLONG; } break;
case 46:
# line 289 "gram.in"
{ yyval = TYREAL; } break;
case 47:
# line 290 "gram.in"
{ yyval = TYCOMPLEX; } break;
case 48:
# line 291 "gram.in"
{ yyval = TYDREAL; } break;
case 49:
# line 292 "gram.in"
{ yyval = TYDCOMPLEX; } break;
case 50:
# line 293 "gram.in"
{ yyval = TYLOGICAL; } break;
case 51:
# line 294 "gram.in"
{ yyval = TYCHAR; } break;
case 52:
# line 295 "gram.in"
{ yyval = TYUNKNOWN; } break;
case 53:
# line 296 "gram.in"
{ yyval = TYUNKNOWN; } break;
case 54:
# line 297 "gram.in"
{ yyval = - STGAUTO; } break;
case 55:
# line 298 "gram.in"
{ yyval = - STGBSS; } break;
case 56:
# line 302 "gram.in"
{ yyval = varleng; } break;
case 57:
# line 304 "gram.in"
{
		  if( ! ISICON(yypvt[-0]) )
			{
			yyval = 0;
			dclerr("length must be an integer constant", 0);
			}
		  else yyval = yypvt[-0]->const.ci;
		} break;
case 58:
# line 313 "gram.in"
{ yyval = 0; } break;
case 59:
# line 317 "gram.in"
{ incomm( yyval = comblock(0, 0) , yypvt[-0] ); } break;
case 60:
# line 319 "gram.in"
{ yyval = yypvt[-1];  incomm(yypvt[-1], yypvt[-0]); } break;
case 61:
# line 321 "gram.in"
{ yyval = yypvt[-2];  incomm(yypvt[-2], yypvt[-0]); } break;
case 62:
# line 323 "gram.in"
{ incomm(yypvt[-2], yypvt[-0]); } break;
case 63:
# line 327 "gram.in"
{ yyval = comblock(0, 0); } break;
case 64:
# line 329 "gram.in"
{ yyval = comblock(toklen, token); } break;
case 65:
# line 333 "gram.in"
{ setext(yypvt[-0]); } break;
case 66:
# line 335 "gram.in"
{ setext(yypvt[-0]); } break;
case 67:
# line 339 "gram.in"
{ setintr(yypvt[-0]); } break;
case 68:
# line 341 "gram.in"
{ setintr(yypvt[-0]); } break;
case 71:
# line 349 "gram.in"
{
		struct equivblock *p;
		if(nequiv >= MAXEQUIV)
			error("too many equivalences",0,0,FATAL);
		p  =  & eqvclass[nequiv++];
		p->eqvinit = 0;
		p->eqvbottom = 0;
		p->eqvtop = 0;
		p->equivs = yypvt[-1];
		} break;
case 72:
# line 362 "gram.in"
{ yyval = ALLOC(eqvchain); yyval->eqvitem = yypvt[-0]; } break;
case 73:
# line 364 "gram.in"
{ yyval = ALLOC(eqvchain); yyval->eqvitem = yypvt[-0]; yyval->nextp = yypvt[-2]; } break;
case 76:
# line 372 "gram.in"
{ if(parstate == OUTSIDE)
			{
			newproc();
			startproc(0, CLMAIN);
			}
		  if(parstate < INDATA)
			{
			enddcl();
			parstate = INDATA;
			}
		} break;
case 77:
# line 386 "gram.in"
{ ftnint junk;
		  if(nextdata(&junk,&junk) != NULL)
			{
			error("too few initializers",0,0,ERR);
			curdtp = NULL;
			}
		  frdata(yypvt[-3]);
		  frrpl();
		} break;
case 78:
# line 397 "gram.in"
{ toomanyinit = NO; } break;
case 81:
# line 402 "gram.in"
{ dataval(NULL, yypvt[-0]); } break;
case 82:
# line 404 "gram.in"
{ dataval(yypvt[-2], yypvt[-0]); } break;
case 84:
# line 409 "gram.in"
{ if( yypvt[-1]==OPMINUS && ISCONST(yypvt[-0]) )
			consnegop(yypvt[-0]);
		  yyval = yypvt[-0];
		} break;
case 89:
# line 422 "gram.in"
{ int k;
		  yypvt[-0]->vsave = 1;
		  k = yypvt[-0]->vstg;
		if( ! ONEOF(k, M(STGUNKNOWN)|M(STGBSS)|M(STGINIT)) )
			dclerr("can only save static variables", yypvt[-0]);
		} break;
case 90:
# line 429 "gram.in"
{ yypvt[-0]->extsave = 1; } break;
case 93:
# line 437 "gram.in"
{ if(yypvt[-2]->vclass == CLUNKNOWN)
			{ yypvt[-2]->vclass = CLPARAM;
			  yypvt[-2]->paramval = yypvt[-0];
			}
		  else dclerr("cannot make %s parameter", yypvt[-2]);
		} break;
case 94:
# line 446 "gram.in"
{ if(ndim>0) setbounds(yypvt[-1], ndim, dims); } break;
case 95:
# line 450 "gram.in"
{ ptr np;
		  vardcl(np = yypvt[-0]->namep);
		  if(np->vstg == STGBSS)
			np->vstg = STGINIT;
		  else if(np->vstg == STGCOMMON)
			extsymtab[np->vardesc.varno].extinit = YES;
		  else if(np->vstg==STGEQUIV)
			eqvclass[np->vardesc.varno].eqvinit = YES;
		  else if(np->vstg != STGINIT)
			dclerr("inconsistent storage classes", np);
		  yyval = mkchain(yypvt[-0], 0);
		} break;
case 96:
# line 463 "gram.in"
{ chainp p; struct impldoblock *q;
		q = ALLOC(impldoblock);
		q->tag = TIMPLDO;
		q->varnp = yypvt[-1]->datap;
		p = yypvt[-1]->nextp;
		if(p)  { q->implb = p->datap; p = p->nextp; }
		if(p)  { q->impub = p->datap; p = p->nextp; }
		if(p)  { q->impstep = p->datap; p = p->nextp; }
		frchain( & (yypvt[-1]) );
		yyval = mkchain(q, 0);
		q->datalist = hookup(yypvt[-3], yyval);
		} break;
case 97:
# line 478 "gram.in"
{ curdtp = yypvt[-0]; curdtelt = 0; } break;
case 98:
# line 480 "gram.in"
{ yyval = hookup(yypvt[-2], yypvt[-0]); } break;
case 99:
# line 484 "gram.in"
{ ndim = 0; } break;
case 101:
# line 488 "gram.in"
{ ndim = 0; } break;
case 104:
# line 493 "gram.in"
{ dims[ndim].lb = 0;
		  dims[ndim].ub = yypvt[-0];
		  ++ndim;
		} break;
case 105:
# line 498 "gram.in"
{ dims[ndim].lb = yypvt[-2];
		  dims[ndim].ub = yypvt[-0];
		  ++ndim;
		} break;
case 106:
# line 505 "gram.in"
{ yyval = 0; } break;
case 108:
# line 510 "gram.in"
{ nstars = 1; labarray[0] = yypvt[-0]; } break;
case 109:
# line 512 "gram.in"
{ if(nstars < MAXLABLIST)  labarray[nstars++] = yypvt[-0]; } break;
case 110:
# line 516 "gram.in"
{ if(yypvt[-0]->labinacc)
			error("illegal branch to inner block, statement %s",
				convic( (ftnint) (yypvt[-0]->stateno) ),0,WARN1);
		  else if(yypvt[-0]->labdefined == NO)
			yypvt[-0]->blklevel = blklevel;
		  yypvt[-0]->labused = YES;
		} break;
case 111:
# line 526 "gram.in"
{ yyval = mklabel( convci(toklen, token) ); } break;
case 115:
# line 536 "gram.in"
{ needkwd = 1; } break;
case 116:
# line 537 "gram.in"
{ vartype = yypvt[-0]; } break;
case 119:
# line 545 "gram.in"
{ setimpl(vartype, varleng, yypvt[-0], yypvt[-0]); } break;
case 120:
# line 547 "gram.in"
{ setimpl(vartype, varleng, yypvt[-2], yypvt[-0]); } break;
case 121:
# line 551 "gram.in"
{ if(toklen!=1 || token[0]<'a' || token[0]>'z')
			{
			dclerr("implicit item must be single letter", 0);
			yyval = 0;
			}
		  else yyval = token[0];
		} break;
case 122:
# line 561 "gram.in"
{ switch(parstate)	
			{
			case OUTSIDE:	newproc();
					startproc(0, CLMAIN);
			case INSIDE:	parstate = INDCL;
			case INDCL:	break;

			default:
				dclerr("declaration among executables", 0);
			}
		} break;
case 123:
# line 574 "gram.in"
{ yyval = 0; } break;
case 125:
# line 579 "gram.in"
{ yyval = mkchain(yypvt[-0], 0); } break;
case 126:
# line 581 "gram.in"
{ yyval = hookup(yypvt[-2], mkchain(yypvt[-0],0) ); } break;
case 128:
# line 586 "gram.in"
{ yyval = yypvt[-1]; } break;
case 132:
# line 593 "gram.in"
{ yyval = mkexpr(yypvt[-1], yypvt[-2], yypvt[-0]); } break;
case 133:
# line 595 "gram.in"
{ yyval = mkexpr(OPSTAR, yypvt[-2], yypvt[-0]); } break;
case 134:
# line 597 "gram.in"
{ yyval = mkexpr(OPSLASH, yypvt[-2], yypvt[-0]); } break;
case 135:
# line 599 "gram.in"
{ yyval = mkexpr(OPPOWER, yypvt[-2], yypvt[-0]); } break;
case 136:
# line 601 "gram.in"
{ if(yypvt[-1] == OPMINUS)
			yyval = mkexpr(OPNEG, yypvt[-0], 0);
		  else 	yyval = yypvt[-0];
		} break;
case 137:
# line 606 "gram.in"
{ yyval = mkexpr(yypvt[-1], yypvt[-2], yypvt[-0]); } break;
case 138:
# line 608 "gram.in"
{ yyval = mkexpr(OPEQV, yypvt[-2],yypvt[-0]); } break;
case 139:
# line 610 "gram.in"
{ yyval = mkexpr(OPNEQV, yypvt[-2], yypvt[-0]); } break;
case 140:
# line 612 "gram.in"
{ yyval = mkexpr(OPOR, yypvt[-2], yypvt[-0]); } break;
case 141:
# line 614 "gram.in"
{ yyval = mkexpr(OPAND, yypvt[-2], yypvt[-0]); } break;
case 142:
# line 616 "gram.in"
{ yyval = mkexpr(OPNOT, yypvt[-0], 0); } break;
case 143:
# line 618 "gram.in"
{ yyval = mkexpr(OPCONCAT, yypvt[-2], yypvt[-0]); } break;
case 144:
# line 621 "gram.in"
{ yyval = OPPLUS; } break;
case 145:
# line 622 "gram.in"
{ yyval = OPMINUS; } break;
case 146:
# line 625 "gram.in"
{ yyval = OPEQ; } break;
case 147:
# line 626 "gram.in"
{ yyval = OPGT; } break;
case 148:
# line 627 "gram.in"
{ yyval = OPLT; } break;
case 149:
# line 628 "gram.in"
{ yyval = OPGE; } break;
case 150:
# line 629 "gram.in"
{ yyval = OPLE; } break;
case 151:
# line 630 "gram.in"
{ yyval = OPNE; } break;
case 152:
# line 634 "gram.in"
{ yyval = mkprim(yypvt[-0], 0, 0, 0); } break;
case 153:
# line 636 "gram.in"
{ yyval = mkprim(yypvt[-5], 0, yypvt[-3], yypvt[-1]); } break;
case 154:
# line 638 "gram.in"
{ yyval = mkprim(yypvt[-3], mklist(yypvt[-1]), 0, 0); } break;
case 155:
# line 640 "gram.in"
{ yyval = mkprim(yypvt[-8], mklist(yypvt[-6]), yypvt[-3], yypvt[-1]); } break;
case 156:
# line 644 "gram.in"
{ yyval = 0; } break;
case 158:
# line 649 "gram.in"
{ if(yypvt[-0]->vclass == CLPARAM)
			yyval = cpexpr(yypvt[-0]->paramval);
		} break;
case 160:
# line 655 "gram.in"
{ yyval = mklogcon(1); } break;
case 161:
# line 656 "gram.in"
{ yyval = mklogcon(0); } break;
case 162:
# line 657 "gram.in"
{ yyval = mkstrcon(toklen, token); } break;
case 163:
# line 658 "gram.in"
 { yyval = mkintcon( convci(toklen, token) ); } break;
case 164:
# line 659 "gram.in"
 { yyval = mkrealcon(TYREAL, convcd(toklen, token)); } break;
case 165:
# line 660 "gram.in"
 { yyval = mkrealcon(TYDREAL, convcd(toklen, token)); } break;
case 166:
# line 664 "gram.in"
{ yyval = mkcxcon(yypvt[-3],yypvt[-1]); } break;
case 167:
# line 668 "gram.in"
{ yyval = mkbitcon(4, toklen, token); } break;
case 168:
# line 670 "gram.in"
{ yyval = mkbitcon(3, toklen, token); } break;
case 169:
# line 672 "gram.in"
{ yyval = mkbitcon(1, toklen, token); } break;
case 171:
# line 677 "gram.in"
{ yyval = yypvt[-1]; } break;
case 174:
# line 683 "gram.in"
{ yyval = mkexpr(yypvt[-1], yypvt[-2], yypvt[-0]); } break;
case 175:
# line 685 "gram.in"
{ yyval = mkexpr(OPSTAR, yypvt[-2], yypvt[-0]); } break;
case 176:
# line 687 "gram.in"
{ yyval = mkexpr(OPSLASH, yypvt[-2], yypvt[-0]); } break;
case 177:
# line 689 "gram.in"
{ yyval = mkexpr(OPPOWER, yypvt[-2], yypvt[-0]); } break;
case 178:
# line 691 "gram.in"
{ if(yypvt[-1] == OPMINUS)
			yyval = mkexpr(OPNEG, yypvt[-0], 0);
		  else	yyval = yypvt[-0];
		} break;
case 179:
# line 696 "gram.in"
{ yyval = mkexpr(OPCONCAT, yypvt[-2], yypvt[-0]); } break;
case 181:
# line 700 "gram.in"
{
		if(yypvt[-1]->labdefined)
			error("no backward DO loops",0,0,EXECERR);
		yypvt[-1]->blklevel = blklevel+1;
		exdo(yypvt[-1]->labelno, yypvt[-0]);
		} break;
case 182:
# line 707 "gram.in"
{ exendif();  thiswasbranch = NO; } break;
case 184:
# line 710 "gram.in"
{ exelif(yypvt[-2]); } break;
case 185:
# line 712 "gram.in"
{ exelse(); } break;
case 186:
# line 714 "gram.in"
{ exendif(); } break;
case 187:
# line 718 "gram.in"
{ exif(yypvt[-1]); } break;
case 188:
# line 722 "gram.in"
{ yyval = mkchain(yypvt[-2], yypvt[-0]); } break;
case 189:
# line 726 "gram.in"
{ exequals(yypvt[-2], yypvt[-0]); } break;
case 190:
# line 728 "gram.in"
{ exassign(yypvt[-0], yypvt[-2]); } break;
case 193:
# line 732 "gram.in"
{ inioctl = NO; } break;
case 194:
# line 734 "gram.in"
{ exarif(yypvt[-6], yypvt[-4], yypvt[-2], yypvt[-0]);  thiswasbranch = YES; } break;
case 195:
# line 736 "gram.in"
{ excall(yypvt[-0], 0, 0, labarray); } break;
case 196:
# line 738 "gram.in"
{ excall(yypvt[-2], 0, 0, labarray); } break;
case 197:
# line 740 "gram.in"
{ if(nstars < MAXLABLIST)
			excall(yypvt[-3], mklist(yypvt[-1]), nstars, labarray);
		  else
			error("too many alternate returns",0,0,ERR);
		} break;
case 198:
# line 746 "gram.in"
{ exreturn(yypvt[-0]);  thiswasbranch = YES; } break;
case 199:
# line 748 "gram.in"
{ exstop(yypvt[-2], yypvt[-0]);  thiswasbranch = yypvt[-2]; } break;
case 200:
# line 752 "gram.in"
{ if(parstate == OUTSIDE)
			{
			newproc();
			startproc(0, CLMAIN);
			}
		} break;
case 201:
# line 761 "gram.in"
{ exgoto(yypvt[-0]);  thiswasbranch = YES; } break;
case 202:
# line 763 "gram.in"
{ exasgoto(yypvt[-0]);  thiswasbranch = YES; } break;
case 203:
# line 765 "gram.in"
{ exasgoto(yypvt[-4]);  thiswasbranch = YES; } break;
case 204:
# line 767 "gram.in"
{ if(nstars < MAXLABLIST)
			putcmgo(fixtype(yypvt[-0]), nstars, labarray);
		  else
			error("computed GOTO list too long",0,0,ERR);
		} break;
case 207:
# line 779 "gram.in"
{ nstars = 0; yyval = yypvt[-0]; } break;
case 208:
# line 783 "gram.in"
{ yyval = (yypvt[-0] ? mkchain(yypvt[-0],0) : 0); } break;
case 209:
# line 785 "gram.in"
{ if(yypvt[-0])
			if(yypvt[-2]) yyval = hookup(yypvt[-2], mkchain(yypvt[-0],0));
			else yyval = mkchain(yypvt[-0],0);
		} break;
case 211:
# line 793 "gram.in"
{ if(nstars<MAXLABLIST) labarray[nstars++] = yypvt[-0]; yyval = 0; } break;
case 212:
# line 797 "gram.in"
{ yyval = 0; } break;
case 213:
# line 799 "gram.in"
{ yyval = 1; } break;
case 214:
# line 803 "gram.in"
{ yyval = mkchain(yypvt[-0], 0); } break;
case 215:
# line 805 "gram.in"
{ yyval = hookup(yypvt[-2], mkchain(yypvt[-0],0) ); } break;
case 216:
# line 809 "gram.in"
{ if(parstate == OUTSIDE)
			{
			newproc();
			startproc(0, CLMAIN);
			}
		  if(parstate < INDATA) enddcl();
		} break;
case 217:
# line 820 "gram.in"
{ endio(); } break;
case 219:
# line 825 "gram.in"
{ ioclause(IOSUNIT, yypvt[-0]); endioctl(); } break;
case 221:
# line 828 "gram.in"
{ doio(NULL); } break;
case 222:
# line 830 "gram.in"
{ doio(yypvt[-0]); } break;
case 223:
# line 832 "gram.in"
{ doio(yypvt[-0]); } break;
case 224:
# line 834 "gram.in"
{ doio(yypvt[-0]); } break;
case 225:
# line 836 "gram.in"
{ doio(yypvt[-0]); } break;
case 226:
# line 838 "gram.in"
{ doio(NULL); } break;
case 227:
# line 840 "gram.in"
{ doio(yypvt[-0]); } break;
case 228:
# line 842 "gram.in"
{ doio(NULL); } break;
case 229:
# line 844 "gram.in"
{ doio(yypvt[-0]); } break;
case 230:
# line 846 "gram.in"
{ doio(NULL); } break;
case 231:
# line 848 "gram.in"
{ doio(yypvt[-0]); } break;
case 233:
# line 855 "gram.in"
{ iostmt = IOREWIND; } break;
case 234:
# line 857 "gram.in"
{ iostmt = IOREWIND; } break;
case 235:
# line 859 "gram.in"
{ iostmt = IOENDFILE; } break;
case 237:
# line 866 "gram.in"
{ iostmt = IOINQUIRE; } break;
case 238:
# line 868 "gram.in"
{ iostmt = IOOPEN; } break;
case 239:
# line 870 "gram.in"
{ iostmt = IOCLOSE; } break;
case 240:
# line 874 "gram.in"
{
		ioclause(IOSUNIT, NULL);
		ioclause(IOSFMT, yypvt[-0]);
		endioctl();
		} break;
case 241:
# line 880 "gram.in"
{
		ioclause(IOSUNIT, NULL);
		ioclause(IOSFMT, NULL);
		endioctl();
		} break;
case 242:
# line 888 "gram.in"
{ ioclause(IOSUNIT, yypvt[-1]); endioctl(); } break;
case 243:
# line 890 "gram.in"
{ endioctl(); } break;
case 246:
# line 898 "gram.in"
{ ioclause(IOSPOSITIONAL, yypvt[-0]); } break;
case 247:
# line 900 "gram.in"
{ ioclause(IOSPOSITIONAL, NULL); } break;
case 248:
# line 902 "gram.in"
{ ioclause(yypvt[-1], yypvt[-0]); } break;
case 249:
# line 904 "gram.in"
{ ioclause(yypvt[-1], NULL); } break;
case 250:
# line 908 "gram.in"
{ yyval = iocname(); } break;
case 251:
# line 912 "gram.in"
{					/* E-D */
		iosetecdc( yypvt[-5] );			/* E-D */
		ioclause( IOSUNIT, yypvt[-1] );		/* E-D */
		ioclause(  IOSFMT, yypvt[-3] );		/* E-D */
		endioctl();				/* E-D */
		} break;
case 252:
# line 921 "gram.in"
{ iostmt = IOREAD; } break;
case 253:
# line 925 "gram.in"
{ iostmt = IOWRITE; } break;
case 254:
# line 929 "gram.in"
{ iostmt = IOREAD; } break;
case 255:
# line 933 "gram.in"
{ iostmt = IOWRITE; } break;
case 256:
# line 937 "gram.in"
{
		iostmt = IOWRITE;
		ioclause(IOSUNIT, NULL);
		ioclause(IOSFMT, yypvt[-1]);
		endioctl();
		} break;
case 257:
# line 944 "gram.in"
{
		iostmt = IOWRITE;
		ioclause(IOSUNIT, NULL);
		ioclause(IOSFMT, NULL);
		endioctl();
		} break;
case 258:
# line 953 "gram.in"
{ yyval = mkchain(yypvt[-0],0); } break;
case 259:
# line 955 "gram.in"
{ yyval = hookup(yypvt[-2], mkchain(yypvt[-0],0)); } break;
case 261:
# line 960 "gram.in"
{ yyval = mkiodo(yypvt[-1],yypvt[-3]); } break;
case 262:
# line 964 "gram.in"
{ yyval = mkchain(yypvt[-0], 0); } break;
case 263:
# line 966 "gram.in"
{ yyval = mkchain(yypvt[-0], 0); } break;
case 265:
# line 971 "gram.in"
{ yyval = mkchain(yypvt[-2], mkchain(yypvt[-0], 0) ); } break;
case 266:
# line 973 "gram.in"
{ yyval = mkchain(yypvt[-2], mkchain(yypvt[-0], 0) ); } break;
case 267:
# line 975 "gram.in"
{ yyval = mkchain(yypvt[-2], mkchain(yypvt[-0], 0) ); } break;
case 268:
# line 977 "gram.in"
{ yyval = mkchain(yypvt[-2], mkchain(yypvt[-0], 0) ); } break;
case 269:
# line 979 "gram.in"
{ yyval = hookup(yypvt[-2], mkchain(yypvt[-0], 0) ); } break;
case 270:
# line 981 "gram.in"
{ yyval = hookup(yypvt[-2], mkchain(yypvt[-0], 0) ); } break;
case 272:
# line 986 "gram.in"
{ yyval = mkiodo(yypvt[-1], mkchain(yypvt[-3], 0) ); } break;
case 273:
# line 988 "gram.in"
{ yyval = mkiodo(yypvt[-1], mkchain(yypvt[-3], 0) ); } break;
case 274:
# line 990 "gram.in"
{ yyval = mkiodo(yypvt[-1], yypvt[-3]); } break;
case 275:
# line 994 "gram.in"
{ startioctl(); } break;
		}
		goto yystack;  /* stack new state and value */

	}
