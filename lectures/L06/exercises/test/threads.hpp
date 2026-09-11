/**
 * @file Helpers the L06 tests share for starting, waiting for and stopping threads.
 *
 *       Two rules shape every test in this suite. A failed EXPECT_* throws, and destroying a
 *       std::thread that has not been joined calls std::terminate, so no test expects anything
 *       while one of its threads is still running: it records what it saw, stops and joins the
 *       threads, and only then checks. And a thread that fails to stop must not hang the suite,
 *       so joinWithin() gives up waiting after a deadline, reports that, and keeps nudging the
 *       thread until it does finish.
 */
#pragma once

#include <atomic>
#include <chrono>
#include <thread>

namespace l06
{
/** Milliseconds, the unit every duration in the exercises is given in. */
using Milliseconds = std::chrono::milliseconds;

/**
 * @brief Poll a condition until it holds or the time runs out.
 *
 * @tparam Predicate A callable returning bool.
 *
 * @param[in] predicate The condition to wait for.
 * @param[in] timeout How long to wait at most.
 *
 * @return True if the condition held in time, false otherwise.
 */
template<typename Predicate>
bool waitUntil(Predicate&& predicate, const Milliseconds timeout)
{
    const auto deadline{std::chrono::steady_clock::now() + timeout};
    while (!predicate())
    {
        if (std::chrono::steady_clock::now() > deadline) { return predicate(); }
        std::this_thread::sleep_for(Milliseconds{1});
    }
    return true;
}

/**
 * @brief Join a thread, and report whether it finished within a deadline.
 *
 *        A thread that is still running at the deadline is nudged, repeatedly, until it finishes,
 *        so a test can report "did not stop in time" instead of hanging on join() for ever.
 *
 * @tparam Nudge A callable that gives a stuck thread a reason to finish, e.g. new data and a
 *               notification.
 *
 * @param[in, out] thread The thread to join.
 * @param[in] timeout How long it may take to finish.
 * @param[in] nudge What to do, over and over, while it has not.
 *
 * @return True if the thread finished within the deadline, false otherwise.
 */
template<typename Nudge>
bool joinWithin(std::thread& thread, const Milliseconds timeout, Nudge&& nudge)
{
    std::atomic<bool> joined{false};
    std::thread joiner{[&thread, &joined]
                       {
                           thread.join();
                           joined.store(true);
                       }};
    const bool inTime{waitUntil([&joined] { return joined.load(); }, timeout)};
    while (!joined.load())
    {
        nudge();
        std::this_thread::sleep_for(Milliseconds{10});
    }
    joiner.join();
    return inTime;
}

/**
 * @brief Join a thread that needs no nudging, and report whether it finished within a deadline.
 *
 * @param[in, out] thread The thread to join.
 * @param[in] timeout How long it may take to finish.
 *
 * @return True if the thread finished within the deadline, false otherwise.
 */
inline bool joinWithin(std::thread& thread, const Milliseconds timeout)
{
    return joinWithin(thread, timeout, [] {});
}

/**
 * @brief The time elapsed since a given moment, in milliseconds.
 *
 * @param[in] start The moment to measure from.
 *
 * @return The elapsed time.
 */
inline long long elapsedSince(const std::chrono::steady_clock::time_point start)
{
    return std::chrono::duration_cast<Milliseconds>(std::chrono::steady_clock::now() - start)
        .count();
}
} // namespace l06
