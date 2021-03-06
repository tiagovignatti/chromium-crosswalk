// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/simple_message_box.h"

#include "ui/base/win/message_box_win.h"

namespace chrome {

MessageBoxResult NativeShowMessageBox(HWND parent,
                                      const base::string16& title,
                                      const base::string16& message,
                                      MessageBoxType type) {
  UINT flags = MB_SETFOREGROUND;
  if (type == MESSAGE_BOX_TYPE_QUESTION) {
    flags |= MB_YESNO;
  } else if (type == MESSAGE_BOX_TYPE_OK_CANCEL) {
    flags |= MB_OKCANCEL;
  } else {
    flags |= MB_OK;
  }
  flags |= ((type == MESSAGE_BOX_TYPE_INFORMATION) ?
      MB_ICONINFORMATION : MB_ICONWARNING);
  int result = ui::MessageBox(parent, message, title, flags);
  return (result == IDYES || result == IDOK) ?
      MESSAGE_BOX_RESULT_YES : MESSAGE_BOX_RESULT_NO;
}

}  // namespace chrome
