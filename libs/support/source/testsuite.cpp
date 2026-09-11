/**
 * @file The main() every test binary in the course shares: run the tests the binary was built
 *       with, and report the result as its exit status.
 */
#include <cstdio>

#include "qacademy/test/test.hpp"

/**
 * @brief Run all test cases.
 *
 * @return 0 on success, or 1 if a test failed or none was registered.
 */
int main()
{
    // Line-buffered, so each result line is written as soon as it is printed. Under make, standard
    // output is a pipe and fully buffered, and a test that crashes the binary would otherwise take
    // the results of every test before it down with it.
    std::setvbuf(stdout, nullptr, _IOLBF, 0);
    return qacademy::test::runAllTests() ? 0 : 1;
}
