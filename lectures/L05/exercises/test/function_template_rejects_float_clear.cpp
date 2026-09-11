/**
 * @file Must NOT compile: clear() on a float register. Exercise 1.1 a) asks for a static_assert
 *       with std::is_integral that rejects a non-integral type, and ci/suite.mk checks that
 *       compiling this file fails with a static assertion, rather than compiling or failing for
 *       some other reason. std::is_arithmetic would let a float through to the &= that follows.
 */
#define main functionTemplateMain
#include "main.cpp"
#undef main

void clearAFloat()
{
    float reg{1.0F};
    clear(reg, 0U);
}
