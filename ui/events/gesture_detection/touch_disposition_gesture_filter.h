// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_GESTURE_DETECTION_TOUCH_DISPOSITION_GESTURE_FILTER_H_
#define UI_EVENTS_GESTURE_DETECTION_TOUCH_DISPOSITION_GESTURE_FILTER_H_

#include <queue>

#include "ui/events/event_constants.h"
#include "ui/events/gesture_detection/bitset_32.h"
#include "ui/events/gesture_detection/gesture_detection_export.h"
#include "ui/events/gesture_detection/gesture_event_data_packet.h"

namespace ui {

// Interface with which the |TouchDispositionGestureFilter| forwards gestures
// for a given touch event.
class GESTURE_DETECTION_EXPORT TouchDispositionGestureFilterClient {
 public:
  virtual void ForwardGestureEvent(const GestureEventData&) = 0;
};

// Given a stream of touch-derived gesture packets, produces a refined gesture
// sequence based on the ack dispositions of the generating touch events.
class GESTURE_DETECTION_EXPORT TouchDispositionGestureFilter {
 public:
  explicit TouchDispositionGestureFilter(
      TouchDispositionGestureFilterClient* client);
  ~TouchDispositionGestureFilter();

  // To be called upon production of touch-derived gestures by the platform,
  // *prior* to the generating touch being forward to the renderer.  In
  // particular, |packet| contains [0, n] gestures that correspond to a given
  // touch event. It is imperative that a single packet is received for
  // *each* touch event, even those that did not produce a gesture.
  enum PacketResult {
    SUCCESS,              // Packet successfully queued.
    INVALID_PACKET_ORDER, // Packets were received in the wrong order, i.e.,
                          // TOUCH_BEGIN should always precede other packets.
    INVALID_PACKET_TYPE,  // Packet had an invalid type.
  };
  PacketResult OnGesturePacket(const GestureEventDataPacket& packet);

  // To be called upon receipt of *all* touch event acks.
  void OnTouchEventAck(bool event_consumed);

  // Whether there are any active gesture sequences still queued in the filter.
  bool IsEmpty() const;

 private:
  // A single GestureSequence corresponds to all gestures created
  // between the first finger down and the last finger up, including gestures
  // generated by timeouts from a statinoary finger.
  typedef std::queue<GestureEventDataPacket> GestureSequence;

  // Utility class for maintaining the touch and gesture handling state for the
  // current gesture sequence.
  class GestureHandlingState {
   public:
    GestureHandlingState();

    // To be called on each touch event ack.
    void OnTouchEventAck(bool event_consumed, bool is_touch_start_event);

    // Returns true iff the gesture should be dropped.
    bool Filter(EventType type);

   private:
    // True iff the sequence has had any touch down event consumed.
    bool start_touch_consumed_;
    // True iff the most recently ack'ed touch event was consumed.
    bool current_touch_consumed_;
    // If the previous gesture of a given type was dropped instead of being
    // dispatched, its type will occur in this set.
    BitSet32 last_gesture_of_type_dropped_;
  };

  void FilterAndSendPacket(const GestureEventDataPacket& packet);
  void SendGesture(const GestureEventData& gesture);
  void CancelTapIfNecessary();
  void CancelFlingIfNecessary();
  void EndScrollIfNecessary();
  GestureSequence& Head();
  GestureSequence& Tail();

  TouchDispositionGestureFilterClient* client_;
  std::queue<GestureSequence> sequences_;

  GestureHandlingState state_;

  // Bookkeeping for inserting synthetic Gesture{Tap,Fling}Cancel events
  // when necessary, e.g., GestureTapCancel when scrolling begins, or
  // GestureFlingCancel when a user taps following a GestureFlingStart.
  int ending_event_motion_event_id_;
  bool needs_tap_ending_event_;
  bool needs_fling_ending_event_;
  bool needs_scroll_ending_event_;

  DISALLOW_COPY_AND_ASSIGN(TouchDispositionGestureFilter);
};

}  // namespace ui

#endif  // UI_EVENTS_GESTURE_DETECTION_TOUCH_DISPOSITION_GESTURE_FILTER_H_
