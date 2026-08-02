#include <cassert>

#include "press_modes.h"

static bool transition(
    double_tap_tracker_t& tracker,
    bool& previous,
    bool current,
    uint64_t now,
    uint64_t threshold = 100,
    bool enabled = true) {
    bool result = update_double_tap(tracker, current, previous, now, threshold, enabled);
    previous = current;
    return result;
}

int main() {
    double_tap_tracker_t tracker;
    bool previous = false;

    // A normal double-tap emits once, on the second release.
    assert(!transition(tracker, previous, true, 0));
    assert(!transition(tracker, previous, false, 10));
    assert(!transition(tracker, previous, true, 50));
    assert(transition(tracker, previous, false, 60));
    assert(!transition(tracker, previous, false, 70));
    assert(tracker.phase == double_tap_phase_t::IDLE);

    // A third tap starts a new sequence instead of extending the old one.
    assert(!transition(tracker, previous, true, 100));
    assert(!transition(tracker, previous, false, 110));

    // The release-to-press window is strict: exactly the threshold is too late.
    assert(!transition(tracker, previous, true, 210));
    assert(!transition(tracker, previous, false, 220));
    assert(!transition(tracker, previous, true, 230));
    assert(transition(tracker, previous, false, 240));

    // A press whose duration reaches the threshold is not a tap.
    assert(!transition(tracker, previous, true, 300));
    assert(!transition(tracker, previous, false, 400));

    // A long second press cancels the candidate without emitting.
    assert(!transition(tracker, previous, true, 500));
    assert(!transition(tracker, previous, false, 510));
    assert(!transition(tracker, previous, true, 550));
    assert(!transition(tracker, previous, false, 650));

    // An expired candidate is discarded before the next press.
    assert(!transition(tracker, previous, true, 700));
    assert(!transition(tracker, previous, false, 710));
    assert(!transition(tracker, previous, false, 810));
    assert(!transition(tracker, previous, true, 820));
    assert(!transition(tracker, previous, false, 830));

    // Disabling the mode resets any partially armed sequence.
    assert(!transition(tracker, previous, false, 900, 100, false));
    assert(!transition(tracker, previous, true, 930));
    assert(!transition(tracker, previous, false, 940));

    return 0;
}
