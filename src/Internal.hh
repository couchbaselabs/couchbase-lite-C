//
//  Internal.hh
//
// Copyright (c) 2019 Couchbase, Inc All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once
#include "c4Base.hh"
#include "c4.h"
#include "fleece/slice.hh"
#include "fleece/Fleece.hh"
#include "RefCounted.hh"
#include "InstanceCounted.hh"


struct CBLRefCounted : public fleece::RefCounted, fleece::InstanceCountedIn<CBLRefCounted> {
protected:
    using Value = fleece::Value;
    using Dict = fleece::Dict;
    using Array = fleece::Array;
    using Doc = fleece::Doc;
    using Encoder = fleece::Encoder;
    using MutableDict = fleece::MutableDict;
    using alloc_slice = fleece::alloc_slice;
    using slice = fleece::slice;

    template <class T> using Retained = fleece::Retained<T>;
    template <class T> using RetainedConst = fleece::RetainedConst<T>;
};


namespace cbl_internal {
    // Converting C4Error <--> CBLError
    static inline       C4Error* internal(      CBLError *error) {return (C4Error*)error;}
    static inline const C4Error& internal(const CBLError &error) {return (const C4Error&)error;}

    static inline       CBLError* external(      C4Error *error) {return (CBLError*)error;}
    static inline const CBLError& external(const C4Error &error) {return (const CBLError&)error;}

    template <typename T>
    T* validated(T *obj, CBLError *outError) {
        if (obj->validate(outError))
            return retain(obj);
        delete obj;
        return nullptr;
    }

    template <typename T>
    static inline void writeOptionalKey(fleece::Encoder &enc, const char *propName, T value) {
        if (value)
            enc[fleece::slice(propName)] = value;
    }
}

using namespace cbl_internal;
