/**
 * @file Must NOT compile: toggle() on a double register. Exercise 1.2 b) asks for a static_assert
 *       that ensures the register type is integral, and ci/suite.mk checks that compiling this file
 *       fails with a static assertion, rather than compiling or failing for some other reason.
 */
#define main functionTemplateMain
#include "main.cpp"
#undef main

void toggleADouble()
{
    double reg{1.0};
    toggle(reg, 0U, 1U);
}
