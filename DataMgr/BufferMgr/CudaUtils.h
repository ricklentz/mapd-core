/*
 * Copyright 2017 MapD Technologies, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CUDAUTILS_H
#define CUDAUTILS_H

namespace CudaUtils {

template <typename T>
void allocGpuMem(T*& devMem,
                 const std::size_t numElems,
                 const std::size_t elemSize,
                 const int gpuNum);

template <typename T>
void allocPinnedHostMem(T*& hostMem,
                        const std::size_t numElems,
                        const std::size_t elemSize);

template <typename T>
void copyToGpu(T* devMem,
               const T* hostMem,
               const std::size_t numElems,
               const std::size_t elemSize,
               const int gpuNum);

template <typename T>
void copyToHost(T* hostMem,
                const T* devMem,
                const std::size_t numElems,
                const std::size_t elemSize,
                const int gpuNum);

template <typename T>
void copyGpuToGpu(T* dstMem,
                  const T* srcMem,
                  const std::size_t numElems,
                  const std::size_t elemSize,
                  const int dstGpuNum);

template <typename T>
void gpuFree(T*& devMem);

template <typename T>
void hostFree(T*& hostMem);
}  // namespace CudaUtils

#endif
