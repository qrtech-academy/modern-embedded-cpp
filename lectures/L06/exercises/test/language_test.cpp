/**
 * @file The suite's always-on tests: claims Appendix A makes about threads, mutexes, atomics,
 *       condition variables and futures, checked.
 *
 *       Every lecture's suite has one binary that is built whether or not you have written
 *       anything, because qacademy::test::runAllTests() reports failure when it has no tests to
 *       run. These are the appendix's statements in forms whose outcome is guaranteed, however the
 *       threads happen to be scheduled: a count that must come out exact, a thread identity that
 *       must match, an invariant no reader may ever see broken.
 */
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <future>
#include <mutex>
#include <thread>
#include <type_traits>
#include <vector>

#include "qacademy/test/test.hpp"
#include "threads.hpp"

/**
 * @brief Appendix A.2: std::thread copies its arguments, and std::ref is how a thread gets a
 *        reference to the caller's variable instead.
 */
TEST(Language, StdRefPassesAReferenceToAThread)
{
    std::uint32_t counter{};
    std::thread thread{[](std::uint32_t& value) { value = 42U; }, std::ref(counter)};
    thread.join();
    EXPECT_EQ(counter, 42U);
}

/**
 * @brief Appendix A.2: a std::thread can be moved but not copied, and once joined it is no longer
 *        joinable.
 */
TEST(Language, AThreadIsMoveOnlyAndJoinedOnce)
{
    EXPECT_FALSE(std::is_copy_constructible<std::thread>::value);
    EXPECT_TRUE(std::is_move_constructible<std::thread>::value);

    std::thread thread{[] {}};
    EXPECT_TRUE(thread.joinable());
    thread.join();
    EXPECT_FALSE(thread.joinable());
}

/**
 * @brief Appendix A.6: increments of an atomic counter from several threads are never lost.
 */
TEST(Language, AtomicIncrementsSumExactly)
{
    constexpr unsigned threadCount{4U};
    constexpr std::uint32_t iterations{20000U};
    std::atomic<std::uint32_t> counter{};
    std::vector<std::thread> threads{};
    for (unsigned i{}; i < threadCount; ++i)
    {
        threads.emplace_back(
            [&counter]
            {
                for (std::uint32_t j{}; j < iterations; ++j)
                {
                    counter++;
                }
            });
    }
    for (auto& thread : threads)
    {
        thread.join();
    }
    EXPECT_EQ(counter.load(), threadCount * iterations);
}

/**
 * @brief Appendix A.5: a lock guard unlocks its mutex when it goes out of scope, and while it
 *        holds the mutex no other thread can take it.
 */
TEST(Language, ALockGuardUnlocksAtTheEndOfItsScope)
{
    std::mutex mutex{};
    bool takenWhileLocked{true};
    {
        std::lock_guard<std::mutex> lock{mutex};
        std::thread other{[&mutex, &takenWhileLocked]
                          {
                              takenWhileLocked = mutex.try_lock();
                              if (takenWhileLocked) { mutex.unlock(); }
                          }};
        other.join();
    }
    EXPECT_FALSE(takenWhileLocked);
    EXPECT_TRUE(mutex.try_lock());
    mutex.unlock();
}

/**
 * @brief Appendix A.6: a mutex protects several related variables together, which an atomic per
 *        variable cannot. A writer moves a unit from one variable to the other, and a reader
 *        holding the same mutex never sees the sum change.
 */
TEST(Language, AMutexKeepsRelatedVariablesConsistent)
{
    constexpr int total{1000};
    std::mutex mutex{};
    int from{total};
    int to{};
    std::atomic<bool> done{false};
    bool alwaysConsistent{true};

    std::thread writer{[&]
                       {
                           for (int i{}; i < total; ++i)
                           {
                               std::lock_guard<std::mutex> lock{mutex};
                               --from;
                               ++to;
                           }
                           done.store(true);
                       }};
    while (!done.load())
    {
        std::lock_guard<std::mutex> lock{mutex};
        if (total != from + to) { alwaysConsistent = false; }
    }
    writer.join();
    EXPECT_TRUE(alwaysConsistent);
    EXPECT_EQ(to, total);
}

/**
 * @brief Appendix A.7: std::launch::async runs the task in a new thread, and
 *        std::launch::deferred runs it on the calling thread, when get() is called.
 */
TEST(Language, TheLaunchPolicyDecidesWhichThreadRunsTheTask)
{
    const auto whoRuns{[] { return std::this_thread::get_id(); }};
    auto asyncFuture{std::async(std::launch::async, whoRuns)};
    auto deferredFuture{std::async(std::launch::deferred, whoRuns)};

    EXPECT_TRUE(std::future_status::deferred == deferredFuture.wait_for(l06::Milliseconds{0}));
    EXPECT_TRUE(std::this_thread::get_id() != asyncFuture.get());
    EXPECT_TRUE(std::this_thread::get_id() == deferredFuture.get());
}

/**
 * @brief Appendix A.7: a future holds the task's return value, and wait_for() reports when it is
 *        ready.
 */
TEST(Language, AFutureDeliversTheResult)
{
    std::promise<void> release{};
    auto gate{release.get_future()};
    auto future{std::async(std::launch::async,
                           [&gate]
                           {
                               gate.wait();
                               return 6 * 7;
                           })};
    // Recorded first and checked last: a failure while the task still waits on the gate would
    // leave the future's destructor waiting for it for ever.
    const auto beforeRelease{future.wait_for(l06::Milliseconds{10})};
    release.set_value();
    const auto afterRelease{future.wait_for(l06::Milliseconds{5000})};
    EXPECT_TRUE(std::future_status::timeout == beforeRelease);
    EXPECT_TRUE(std::future_status::ready == afterRelease);
    EXPECT_EQ(future.get(), 42);
}

/**
 * @brief Appendix A.8: a thread waiting on a condition variable with a predicate wakes when the
 *        predicate's state is changed under the mutex and the variable is notified.
 */
TEST(Language, AConditionVariableWakesAWaitingThread)
{
    std::mutex mutex{};
    std::condition_variable cv{};
    bool ready{false};
    bool woke{false};

    std::thread waiter{[&]
                       {
                           std::unique_lock<std::mutex> lock{mutex};
                           cv.wait(lock, [&ready] { return ready; });
                           woke = true;
                       }};
    {
        std::lock_guard<std::mutex> lock{mutex};
        ready = true;
    }
    cv.notify_one();
    EXPECT_TRUE(l06::joinWithin(waiter, l06::Milliseconds{5000}));
    EXPECT_TRUE(woke);
}
