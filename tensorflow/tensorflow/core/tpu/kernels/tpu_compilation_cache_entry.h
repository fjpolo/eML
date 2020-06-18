/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#ifndef EXPERIMENTAL_BRAIN_TPU_1VM_MINIEXECUTOR_TPU_COMPILATION_CACHE_ENTRY_H_
#define EXPERIMENTAL_BRAIN_TPU_1VM_MINIEXECUTOR_TPU_COMPILATION_CACHE_ENTRY_H_

#include "tensorflow/compiler/xla/service/hlo.pb.h"
#include "tensorflow/core/lib/core/refcount.h"
#include "tensorflow/core/tpu/kernels/tpu_executable_info.pb.h"
#include "tensorflow/core/tpu/kernels/tpu_program.h"

namespace tensorflow {
namespace tpu {

class CompilationCacheEntry {
 public:
  explicit CompilationCacheEntry(
      std::unique_ptr<const TpuProgram> tpu_program)
      : tpu_program_(std::move(tpu_program)) {}

  // Constructor for an empty entry.
  CompilationCacheEntry()
      : tpu_program_(nullptr) {}

  const TPUExecutableInfoProto* get_executable_info() const {
    return &tpu_program_->executable_info();
  }

  const TPUHostTransferInfoProto* get_host_transfer_info() const {
    return &tpu_program_->host_transfer_info();
  }

  const xla::HloProto* get_hlo_metadata() const {
    return &tpu_program_->hlo_metadata();
  }

  // TODO(henrytan,jiawenhao): When should we expect more than one
  // XLA_TpuProgram* per TpuProgram? Remove the program_count CHECK below then.
  const XLA_TpuProgram* get_tpu_program() const {
    CHECK_EQ(tpu_program_->program_count(), 1);
    return tpu_program_->tpu_programs()[0];
  }

 private:
  std::unique_ptr<const TpuProgram> tpu_program_;
};

// Base class for a reference to a cached proto. A unique_ptr to a
// CompilationCacheEntryRef is returned by all the cache Lookup methods below,
// and ensures the underlying proto is not garbage-collected until the client
// discards the ptr.
class CompilationCacheEntryRef {
 public:
  virtual ~CompilationCacheEntryRef() = default;

  // Returns a CompilationCacheEntry that should not be used beyond the lifetime
  // of the CompilationCacheEntryRef.
  virtual CompilationCacheEntry get() = 0;
};

// Base class that holds references to compiled protos so that the protos are
// not garbage-collected before being used by execute ops. Use
// TpuCompilationCache::MakePerStepRefHolder to create an instance of a concrete
// ref holder object.
class CompilationRefHolder : public ResourceBase {
 public:
  ~CompilationRefHolder() override = default;
};

}  // namespace tpu
}  // namespace tensorflow

#endif  // EXPERIMENTAL_BRAIN_TPU_1VM_MINIEXECUTOR_TPU_COMPILATION_CACHE_ENTRY_H_
