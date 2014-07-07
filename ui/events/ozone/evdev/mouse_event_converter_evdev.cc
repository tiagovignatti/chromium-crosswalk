// Copyright 2014 The Chromium Authors. All rights reserved.
// Copyright 2014 Intel Corporation
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/ozone/evdev/mouse_event_converter_evdev.h"

#include <errno.h>
#include <linux/input.h>

#include "base/message_loop/message_loop.h"
#include "ui/events/event.h"
#include "ui/ozone/public/event_factory_ozone.h"

namespace ui {

MouseEventConverterEvdev::MouseEventConverterEvdev(
    int fd,
    base::FilePath path,
    CursorDelegateEvdev *cursor,
    const EventDispatchCallback& callback)
    : EventConverterEvdev(callback),
      fd_(fd),
      path_(path),
      cursor_(cursor) {
}

MouseEventConverterEvdev::~MouseEventConverterEvdev() {
  Stop();
  close(fd_);
}

void MouseEventConverterEvdev::Start() {
  base::MessageLoopForUI::current()->WatchFileDescriptor(
      fd_, true, base::MessagePumpLibevent::WATCH_READ, &controller_, this);
}

void MouseEventConverterEvdev::Stop() {
  controller_.StopWatchingFileDescriptor();
}

void MouseEventConverterEvdev::OnFileCanReadWithoutBlocking(int fd) {
  input_event inputs[4];
  ssize_t read_size = read(fd, inputs, sizeof(inputs));
  if (read_size < 0) {
    if (errno == EINTR || errno == EAGAIN)
      return;
    if (errno != ENODEV)
      PLOG(ERROR) << "error reading device " << path_.value();
    Stop();
    return;
  }

  CHECK_EQ(read_size % sizeof(*inputs), 0u);
  ProcessEvents(inputs, read_size / sizeof(*inputs));
}

void MouseEventConverterEvdev::OnFileCanWriteWithoutBlocking(int fd) {
  NOTREACHED();
}

void MouseEventConverterEvdev::ProcessEvents(const input_event* inputs,
                                           int count) {
  for (int i = 0; i < count; ++i) {
    const input_event& input = inputs[i];
    if (input.type == EV_REL)
      ConvertMouseMoveEvent(input);
    else if (input.type == EV_KEY)
      ConvertMouseButtonEvent(input);
  }
}

void MouseEventConverterEvdev::ConvertMouseMoveEvent(const input_event& input) {
  int x = 0, y = 0;

  if (input.code == REL_X)
    x = input.value;
  else if (input.code == REL_Y)
    y = input.value;
  else
    return;

  cursor_->MoveCursor(gfx::Vector2dF(x, y));

  MouseEvent mouse_event(ui::ET_MOUSE_MOVED,
                         cursor_->location(), cursor_->location(), 0, 0);
  DispatchEventToCallback(&mouse_event);
}

void MouseEventConverterEvdev::ConvertMouseButtonEvent(const input_event& input) {
  ui::EventType type;
  ui::EventFlags flags;

  if (input.code == BTN_LEFT)
    flags = ui::EF_LEFT_MOUSE_BUTTON;
  else if (input.code == BTN_RIGHT)
    flags = ui::EF_RIGHT_MOUSE_BUTTON;
  else if (input.code == BTN_MIDDLE)
    flags = ui::EF_MIDDLE_MOUSE_BUTTON;
  else
    return;

  if (input.value == 1)
    type = ui::ET_MOUSE_PRESSED;
  else if (input.value == 0)
    type = ui::ET_MOUSE_RELEASED;
  else
    return;

  MouseEvent mouse_event(type, cursor_->location(), cursor_->location(),
                         flags, flags);
  DispatchEventToCallback(&mouse_event);
}

}  // namespace ui
