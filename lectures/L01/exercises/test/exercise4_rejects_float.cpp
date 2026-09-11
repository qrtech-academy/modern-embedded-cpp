/**
 * @file Must NOT compile: clear() and toggle() on a float register. Exercise 4.1 asks for a
 *       static_assert that rejects a non-integral type, and ci/suite.mk checks that compiling
 *       this file fails with a static assertion, rather than compiling or failing for some other
 *       reason.
 */
#define main exercise4Main
#include "main.cpp"
#undef main

void clearAFloat()
{
    float reg{1.0F};
    clear(reg, 0U);
}
