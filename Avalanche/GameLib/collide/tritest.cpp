/***************************************************************************/
// A class to help compute different quantities about points lying inside of
// triangles
/***************************************************************************/
#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/tritest.h"

// This is what all the rest of this code is about
uint32 TriangleTest::signTest = 0x06c3761b;

#if 0
// Sign testing table.  Probaby should pack this at some point for better
// cache fit
bool TriangleTest::signTest[3][3][3] =
{
	true,		// [SIGN_NEG][SIGN_NEG][SIGN_NEG]
	true,    // [SIGN_NEG][SIGN_NEG][SIGN_ZERO]
	false,   // [SIGN_NEG][SIGN_NEG][SIGN_POS]

	true,    // [SIGN_NEG][SIGN_ZERO][SIGN_NEG]
	true,    // [SIGN_NEG][SIGN_ZERO][SIGN_ZERO]
	false,   // [SIGN_NEG][SIGN_ZERO][SIGN_POS]

	false,   // [SIGN_NEG][SIGN_POS][SIGN_NEG]
	false,   // [SIGN_NEG][SIGN_POS][SIGN_ZERO]
	false,   // [SIGN_NEG][SIGN_POS][SIGN_POS]

	true,    // [SIGN_ZERO][SIGN_NEG][SIGN_NEG]
	true,    // [SIGN_ZERO][SIGN_NEG][SIGN_ZERO]
	false,   // [SIGN_ZERO][SIGN_NEG][SIGN_POS]

	true,    // [SIGN_ZERO][SIGN_ZERO][SIGN_NEG]
	true,    // [SIGN_ZERO][SIGN_ZERO][SIGN_ZERO]
	true,    // [SIGN_ZERO][SIGN_ZERO][SIGN_POS]

	false,   // [SIGN_ZERO][SIGN_POS][SIGN_NEG]
	true,    // [SIGN_ZERO][SIGN_POS][SIGN_ZERO]
	true,    // [SIGN_ZERO][SIGN_POS][SIGN_POS]

	false,   // [SIGN_POS][SIGN_NEG][SIGN_NEG]
	false,   // [SIGN_POS][SIGN_NEG][SIGN_ZERO]
	false,   // [SIGN_POS][SIGN_NEG][SIGN_POS]

	false,   // [SIGN_POS][SIGN_ZERO][SIGN_NEG]
	true,    // [SIGN_POS][SIGN_ZERO][SIGN_ZERO]
	true,    // [SIGN_POS][SIGN_ZERO][SIGN_POS]

	false,   // [SIGN_POS][SIGN_POS][SIGN_NEG]
	true,    // [SIGN_POS][SIGN_POS][SIGN_ZERO]
	true     // [SIGN_POS][SIGN_POS][SIGN_POS]
};
#endif

#if 0
/***************************************************************************/
// Generate & Verify the test table
/***************************************************************************/
void GenerateTriangleTestTable(void)
{
#define SIGN_NEG 0
#define SIGN_ZERO 1
#define SIGN_POS 2

	// Try the manual way
	bool signTest[3][3][3];
	signTest[SIGN_NEG][SIGN_NEG][SIGN_NEG] = true;
	signTest[SIGN_NEG][SIGN_NEG][SIGN_ZERO] = true;
	signTest[SIGN_NEG][SIGN_NEG][SIGN_POS] = false;

	signTest[SIGN_NEG][SIGN_ZERO][SIGN_NEG] = true;
	signTest[SIGN_NEG][SIGN_ZERO][SIGN_ZERO] = true;
	signTest[SIGN_NEG][SIGN_ZERO][SIGN_POS] = false;

	signTest[SIGN_NEG][SIGN_POS][SIGN_NEG] = false;
	signTest[SIGN_NEG][SIGN_POS][SIGN_ZERO] = false;
	signTest[SIGN_NEG][SIGN_POS][SIGN_POS] = false;

	signTest[SIGN_ZERO][SIGN_NEG][SIGN_NEG] = true;
	signTest[SIGN_ZERO][SIGN_NEG][SIGN_ZERO] = true;
	signTest[SIGN_ZERO][SIGN_NEG][SIGN_POS] = false;

	signTest[SIGN_ZERO][SIGN_ZERO][SIGN_NEG] = true;
	signTest[SIGN_ZERO][SIGN_ZERO][SIGN_ZERO] = true; // ??
	signTest[SIGN_ZERO][SIGN_ZERO][SIGN_POS] = true;

	signTest[SIGN_ZERO][SIGN_POS][SIGN_NEG] = false;
	signTest[SIGN_ZERO][SIGN_POS][SIGN_ZERO] = true;
	signTest[SIGN_ZERO][SIGN_POS][SIGN_POS] = true;

	signTest[SIGN_POS][SIGN_NEG][SIGN_NEG] = false;
	signTest[SIGN_POS][SIGN_NEG][SIGN_ZERO] = false;
	signTest[SIGN_POS][SIGN_NEG][SIGN_POS] = false;

	signTest[SIGN_POS][SIGN_ZERO][SIGN_NEG] = false;
	signTest[SIGN_POS][SIGN_ZERO][SIGN_ZERO] = true;
	signTest[SIGN_POS][SIGN_ZERO][SIGN_POS] = true;

	signTest[SIGN_POS][SIGN_POS][SIGN_NEG] = false;
	signTest[SIGN_POS][SIGN_POS][SIGN_ZERO] = true;
	signTest[SIGN_POS][SIGN_POS][SIGN_POS] = true;

   // Try a logic way
	bool test[3][3][3];
	int right = 0;
   for (int s1 = SIGN_NEG; s1 <= SIGN_POS; s1++)
   	for (int s2 = SIGN_NEG; s2 <= SIGN_POS; s2++)
   		for (int s3 = SIGN_NEG; s3 <= SIGN_POS; s3++)
			{
            test[s1][s2][s3] = ((s1 == s2 && s1 == s3) ||
                						(s1 == SIGN_ZERO && s2 == s3) ||
                						(s2 == SIGN_ZERO && s1 == s3) ||
                						(s3 == SIGN_ZERO && s1 == s2) ||
					 						(s1 == SIGN_ZERO && s2 == SIGN_ZERO) ||
                						(s1 == SIGN_ZERO && s3 == SIGN_ZERO) ||
                						(s2 == SIGN_ZERO && s3 == SIGN_ZERO));

				if (test[s1][s2][s3] == signTest[s1][s2][s3])
					++right;
			}

	// right better be 27....  and it is

	// Build it into a nice packed bit vector
	uint32 signTestMask = 0;
   for (int a = SIGN_NEG; a <= SIGN_POS; a++)
   	for (int b = SIGN_NEG; b <= SIGN_POS; b++)
   		for (int c = SIGN_NEG; c <= SIGN_POS; c++)
			{
				if (signTest[a][b][c])
				{
					uint32 mask = 1 << (a + b * 3 + c * 9);
			 	 	signTestMask |= mask;
				}
			}

	// Dump it out
	FILE *fh = fopen("tritest.out", "wt");
	fprintf(fh, "bool signTest[3][3][3] = \n\t{\n");
	bool *dump = &signTest[0][0][0];
	for (int i = 0; i < 3 * 3 * 3; i++)
		fprintf(fh, "\t %s, \n", dump[i] ? "true" : "false");
	fprintf(fh, "\t};\n");
	fprintf(fh, "\n\nuint32 signTestMask = 0x%x;\n\n", signTestMask);
	fclose(fh);
}

#endif

