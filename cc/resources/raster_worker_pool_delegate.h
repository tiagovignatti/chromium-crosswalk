// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_RESOURCES_RASTER_WORKER_POOL_DELEGATE_H_
#define CC_RESOURCES_RASTER_WORKER_POOL_DELEGATE_H_

#include <vector>

#include "cc/resources/raster_worker_pool.h"

namespace cc {

class RasterWorkerPoolDelegate : public RasterWorkerPoolClient {
 public:
  virtual ~RasterWorkerPoolDelegate();

  static scoped_ptr<RasterWorkerPoolDelegate> Create(
      RasterWorkerPoolClient* client,
      RasterWorkerPool** raster_worker_pools,
      size_t num_raster_worker_pools);

  void SetClient(RasterWorkerPoolClient* client);
  void Shutdown();
  void ScheduleTasks(RasterWorkerPool::RasterTask::Queue* raster_queue);
  void CheckForCompletedTasks();

  // Overriden from RasterWorkerPoolClient:
  virtual bool ShouldForceTasksRequiredForActivationToComplete() const OVERRIDE;
  virtual void DidFinishRunningTasks() OVERRIDE;
  virtual void DidFinishRunningTasksRequiredForActivation() OVERRIDE;

 private:
  RasterWorkerPoolDelegate(RasterWorkerPoolClient* client,
                           RasterWorkerPool** raster_worker_pools,
                           size_t num_raster_worker_pools);

  RasterWorkerPoolClient* client_;
  typedef std::vector<RasterWorkerPool*> RasterWorkerPoolVector;
  RasterWorkerPoolVector raster_worker_pools_;
  size_t did_finish_running_tasks_pending_count_;
  size_t did_finish_running_tasks_required_for_activation_pending_count_;
};

}  // namespace cc

#endif  // CC_RESOURCES_RASTER_WORKER_POOL_DELEGATE_H_
