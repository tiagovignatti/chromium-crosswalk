// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_FAVICON_BASE_SELECT_FAVICON_FRAMES_H_
#define COMPONENTS_FAVICON_BASE_SELECT_FAVICON_FRAMES_H_

#include <vector>

#include "base/basictypes.h"

class SkBitmap;

namespace gfx {
class ImageSkia;
class Size;
}

// Score which is smaller than the minimum score returned by
// SelectFaviconFrames() or SelectFaviconFrameIndices().
extern const float kSelectFaviconFramesInvalidScore;

// Takes a list of all bitmaps found in a .ico file, and creates an
// ImageSkia that's |desired_size_in_dip| x |desired_size_in_dip| big. This
// function adds a representation at every entry in |favicon_scales|.
// If |desired_size_in_dip| is 0, the largest bitmap is returned unmodified.
// |original_sizes| are the original sizes of the bitmaps. (For instance,
// WebContents::DownloadImage() does resampling if it is passed a max size.)
// If score is non-NULL, it receives a score between 0 (bad) and 1 (good)
// that describes how well |bitmaps| were able to produce an image at
// |desired_size_in_dip| for |favicon_scales|.
// The score is arbitrary, but it's best for exact size matches,
// and gets worse the more resampling needs to happen.
// If the resampling algorithm is modified, the resampling done in
// FaviconUtil::SelectFaviconFramesFromPNGs() should probably be modified too as
// it inspired by this method.
gfx::ImageSkia SelectFaviconFrames(const std::vector<SkBitmap>& bitmaps,
                                   const std::vector<gfx::Size>& original_sizes,
                                   const std::vector<float>& favicon_scales,
                                   int desired_size_in_dip,
                                   float* score);

// Takes a list of the pixel sizes of a favicon's favicon bitmaps and returns
// the indices of the best sizes to use to create an ImageSkia with
// ImageSkiaReps with edge sizes |desired_sizes|. If '0' is one of
// |desired_sizes|, the index of the largest size is returned. If |score| is
// non-NULL, |score| is set to a value between 0 (bad) and 1 (good) that
// describes how well the bitmap data with the sizes at |best_indices| will
// produce the ImageSkia. The score is arbitrary, but it's best for exact
// matches, and gets worse the more resampling needs to happen.
// TODO(pkotwicz): Change API so that |desired_sizes| being empty indicates
// that the index of the largest size is requested.
// TODO(pkotwicz): Remove callers of this method for which |frame_pixel_sizes|
// are the sizes of the favicon bitmaps after they were resized.
void SelectFaviconFrameIndices(const std::vector<gfx::Size>& frame_pixel_sizes,
                               const std::vector<int>& desired_sizes,
                               std::vector<size_t>* best_indices,
                               float* score);

#endif  // COMPONENTS_FAVICON_BASE_SELECT_FAVICON_FRAMES_H_
