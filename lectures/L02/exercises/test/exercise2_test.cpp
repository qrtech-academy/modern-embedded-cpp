/**
 * @file Tests for Exercise Set 2: the program in exercise2/main.cpp that controls an LED with a
 *       button and then blinks it, using the classes from Exercise Set 1.
 *
 *       The set asks for behaviour rather than new declarations, and states the behaviour as
 *       output, so the test runs your program and compares what it prints. The classes themselves
 *       are tested by exercise1.
 */
#include "qacademy/test/test.hpp"
#include "support/output.hpp"

// Your program, with its main() renamed so this file can run it. A main() may leave out its
// return statement and any other function may not, so that one warning is silenced here.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#define main exercise2Main
#include "main.cpp"
#undef main
#pragma GCC diagnostic pop

using support::captureOutput;

/**
 * @brief Exercises 2.1 and 2.2: the LED follows the button, released and then pressed, and the
 *        blink loop starts from off, so the first of its six toggles turns the LED on.
 */
TEST(Program, PrintsTheExampleOutput)
{
    EXPECT_OUTPUT(captureOutput([] { exercise2Main(); }), "Button released -> LED: Off\n"
                                                          "Button pressed  -> LED: On\n"
                                                          "Blink 1: On\n"
                                                          "Blink 2: Off\n"
                                                          "Blink 3: On\n"
                                                          "Blink 4: Off\n"
                                                          "Blink 5: On\n"
                                                          "Blink 6: Off\n");
}
