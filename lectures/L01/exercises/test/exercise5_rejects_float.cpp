/**
 * @file Must NOT compile: lowestSetBit() on a float register. Exercise 5.1 asks for a
 *       static_assert that rejects a non-integral type, and ci/suite.mk checks that compiling
 *       this file fails with a static assertion, rather than compiling or failing for some other
 *       reason.
 */
#define main exercise5Main
#include "main.cpp"
#undef main

bool lowestSetBitOfAFloat() { return lowestSetBit(1.0F).has_value(); }
