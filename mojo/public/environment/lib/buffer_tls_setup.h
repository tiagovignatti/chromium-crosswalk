// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MOJO_PUBLIC_ENVIRONMENT_STANDALONE_BUFFER_TLS_SETUP_H_
#define MOJO_PUBLIC_ENVIRONMENT_STANDALONE_BUFFER_TLS_SETUP_H_

namespace mojo {
namespace internal {

void SetUpCurrentBuffer();
void TearDownCurrentBuffer();

}  // namespace internal
}  // namespace mojo

#endif  // MOJO_PUBLIC_ENVIRONMENT_STANDALONE_BUFFER_TLS_SETUP_H_
