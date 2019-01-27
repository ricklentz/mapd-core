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

/*
 * @file    TargetValue.h
 * @author  Alex Suhan <alex@mapd.com>
 * @brief   High-level representation of SQL values.
 *
 * Copyright (c) 2014 MapD Technologies, Inc.  All rights reserved.
 */

#ifndef QUERYENGINE_TARGETVALUE_H
#define QUERYENGINE_TARGETVALUE_H

#include <glog/logging.h>
#include <boost/variant.hpp>

#include <cstdint>
#include <string>
#include <vector>

struct InternalTargetValue {
  int64_t i1;
  int64_t i2;

  enum class ITVType { Int, Pair, Str, Arr, Null };

  ITVType ty;

  explicit InternalTargetValue(const int64_t i1_) : i1(i1_), ty(ITVType::Int) {}

  explicit InternalTargetValue(const int64_t i1_, const int64_t i2_)
      : i1(i1_), i2(i2_), ty(ITVType::Pair) {}

  explicit InternalTargetValue(const std::string* s)
      : i1(reinterpret_cast<int64_t>(s)), ty(ITVType::Str) {}

  explicit InternalTargetValue(const std::vector<int64_t>* v)
      : i1(reinterpret_cast<int64_t>(v)), ty(ITVType::Arr) {}

  explicit InternalTargetValue() : ty(ITVType::Null) {}

  std::string strVal() const { return *reinterpret_cast<std::string*>(i1); }

  std::vector<int64_t> arrVal() const {
    return *reinterpret_cast<std::vector<int64_t>*>(i1);
  }

  bool isInt() const { return ty == ITVType::Int; }

  bool isPair() const { return ty == ITVType::Pair; }

  bool isNull() const { return ty == ITVType::Null; }

  bool isStr() const { return ty == ITVType::Str; }

  bool operator<(const InternalTargetValue& other) const {
    switch (ty) {
      case ITVType::Int:
        CHECK(other.ty == ITVType::Int);
        return i1 < other.i1;
      case ITVType::Pair:
        CHECK(other.ty == ITVType::Pair);
        if (i1 != other.i1) {
          return i1 < other.i1;
        }
        return i2 < other.i2;
      case ITVType::Str:
        CHECK(other.ty == ITVType::Str);
        return strVal() < other.strVal();
      case ITVType::Null:
        return false;
      default:
        abort();
    }
  }

  bool operator==(const InternalTargetValue& other) const {
    return !(*this < other || other < *this);
  }
};

struct GeoPointTargetValue {
  std::shared_ptr<std::vector<double>> coords;

  GeoPointTargetValue(const std::vector<double>& coords)
      : coords(std::make_shared<std::vector<double>>(coords)) {}
};

struct GeoLineStringTargetValue {
  std::shared_ptr<std::vector<double>> coords;

  GeoLineStringTargetValue(const std::vector<double>& coords)
      : coords(std::make_shared<std::vector<double>>(coords)) {}
};

struct GeoPolyTargetValue {
  std::shared_ptr<std::vector<double>> coords;
  std::shared_ptr<std::vector<int32_t>> ring_sizes;

  GeoPolyTargetValue(const std::vector<double>& coords,
                     const std::vector<int32_t>& ring_sizes)
      : coords(std::make_shared<std::vector<double>>(coords))
      , ring_sizes(std::make_shared<std::vector<int32_t>>(ring_sizes)) {}
};

struct GeoMultiPolyTargetValue {
  std::shared_ptr<std::vector<double>> coords;
  std::shared_ptr<std::vector<int32_t>> ring_sizes;
  std::shared_ptr<std::vector<int32_t>> poly_rings;

  GeoMultiPolyTargetValue(const std::vector<double>& coords,
                          const std::vector<int32_t>& ring_sizes,
                          const std::vector<int32_t>& poly_rings)
      : coords(std::make_shared<std::vector<double>>(coords))
      , ring_sizes(std::make_shared<std::vector<int32_t>>(ring_sizes))
      , poly_rings(std::make_shared<std::vector<int32_t>>(poly_rings)) {}
};

using NullableString = boost::variant<std::string, void*>;
using ScalarTargetValue = boost::variant<int64_t, double, float, NullableString>;
using GeoTargetValue = boost::variant<GeoPointTargetValue,
                                      GeoLineStringTargetValue,
                                      GeoPolyTargetValue,
                                      GeoMultiPolyTargetValue>;
using TargetValue =
    boost::variant<ScalarTargetValue, std::vector<ScalarTargetValue>, GeoTargetValue>;

#endif  // QUERYENGINE_TARGETVALUE_H
