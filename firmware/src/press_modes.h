#ifndef _PRESS_MODES_H_
#define _PRESS_MODES_H_

#include <stdint.h>

enum class double_tap_phase_t : uint8_t {
    IDLE,
    FIRST_PRESS,
    WAITING_FOR_SECOND_PRESS,
    SECOND_PRESS,
};

struct double_tap_tracker_t {
    double_tap_phase_t phase = double_tap_phase_t::IDLE;
    uint64_t pressed_at = 0;
    uint64_t first_release_at = 0;
};

// Returns true for exactly one update: the release that completes the second
// short press of a double-tap sequence.
inline bool update_double_tap(
    double_tap_tracker_t& tracker,
    bool current,
    bool previous,
    uint64_t now,
    uint64_t threshold,
    bool enabled) {
    if (!enabled) {
        tracker.phase = double_tap_phase_t::IDLE;
        return false;
    }

    if ((tracker.phase == double_tap_phase_t::WAITING_FOR_SECOND_PRESS) &&
        (now - tracker.first_release_at >= threshold)) {
        tracker.phase = double_tap_phase_t::IDLE;
    }

    if (current && !previous) {
        bool is_second_press =
            (tracker.phase == double_tap_phase_t::WAITING_FOR_SECOND_PRESS) &&
            (now - tracker.first_release_at < threshold);
        tracker.pressed_at = now;
        tracker.phase = is_second_press ? double_tap_phase_t::SECOND_PRESS : double_tap_phase_t::FIRST_PRESS;
        return false;
    }

    if (!current && previous &&
        ((tracker.phase == double_tap_phase_t::FIRST_PRESS) ||
         (tracker.phase == double_tap_phase_t::SECOND_PRESS))) {
        bool is_short_press = now - tracker.pressed_at < threshold;
        if (!is_short_press) {
            tracker.phase = double_tap_phase_t::IDLE;
        } else if (tracker.phase == double_tap_phase_t::SECOND_PRESS) {
            tracker.phase = double_tap_phase_t::IDLE;
            return true;
        } else {
            tracker.first_release_at = now;
            tracker.phase = double_tap_phase_t::WAITING_FOR_SECOND_PRESS;
        }
    }

    return false;
}

#endif
