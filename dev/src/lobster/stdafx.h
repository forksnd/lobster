// Copyright 2014 Wouter van Oortmerssen. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#ifdef _MSC_VER
    #define _HAS_STD_BYTE 0  // clashes with windows headers.
    #define _CRT_SECURE_NO_WARNINGS
    #define _SCL_SECURE_NO_WARNINGS
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
    #ifndef NDEBUG
        #define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
        #define new DEBUG_NEW
    #endif
    #include "StackWalker\StackWalkerHelpers.h"
    #include <intrin.h>
#endif

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <float.h>
#include <limits.h>
#include <csetjmp>

#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <list>
#include <span>
#include <set>
#include <algorithm>
#include <iterator>
#include <functional>
#include <array>
#include <type_traits>
#include <memory>
#include <optional>
#include <charconv>
#include <cinttypes>

#if defined(__has_include) && __has_include(<string_view>)
    #include <string_view>
#else
    #include <experimental/string_view>
#endif

#include <thread>
#include <future>

#include <sstream>
#include <iostream>
#include <iomanip>

using namespace std;

#include "flatbuffers/flatbuffers.h"
#include "flatbuffers/flexbuffers.h"

// Our universally used headers.
#include "wentropy.h"

#include "tools.h"
#include "dllist.h"
#include "rng_hash.h"
#include "string_tools.h"
#include "accumulator.h"
#include "resource_manager.h"
#include "small_vector.h"
#include "stack_vector.h"
#include "packed_vector.h"
#include "varint.h"

#include "platform.h"
#include "slaballoc.h"
#include "geom.h"
#include "unicode.h"

using namespace geom;

#ifndef VM_JIT_MODE
    #if defined(BUILD_CONTEXT_compiled_lobster) || \
        defined(__IOS__) || \
        defined(__ANDROID__) || \
        defined(__EMSCRIPTEN__)
        #define VM_JIT_MODE 0
    #else
        #define VM_JIT_MODE 1
    #endif
#endif

#ifndef LOBSTER_ENGINE
    // By default, build Lobster assuming it comes with the default engine.
    // Build systems can override this for a console-only build.
    #define LOBSTER_ENGINE 1
#endif

#ifndef LOBSTER_FRAME_PROFILER
    #if LOBSTER_ENGINE && defined(_WIN32)
        // 1 == use builtin Tracy compatible emulation.
        // 2 == use full Tracy version.
        #define LOBSTER_FRAME_PROFILER 1
    #else
        #define LOBSTER_FRAME_PROFILER 0
    #endif
#endif
#if LOBSTER_FRAME_PROFILER
    // These are too expensive to always have on, but can give maximum info automatically.
    #define LOBSTER_FRAME_PROFILER_BUILTINS 0
    #define LOBSTER_FRAME_PROFILER_FUNCTIONS 0   // Only works with --runtime-stack-trace on.
    #define LOBSTER_FRAME_PROFILER_GLOBAL 0      // Save additional copy to debug hard crashes.
    #if LOBSTER_FRAME_PROFILER == 2
        // Not used by default because of several out-standing bugs with Tracy.
        // https://github.com/wolfpld/tracy/issues/422
        // https://github.com/wolfpld/tracy/issues/419
        #define LOBSTER_FRAME_PROFILE_THIS_SCOPE ZoneScoped;
        #define LOBSTER_FRAME_PROFILE_GPU TracyGpuZone(__FUNCTION__)
        #define TRACY_ENABLE 1
        #define TRACY_ON_DEMAND 1
        #define TRACY_ONLY_LOCALHOST 1
        #undef new
        #include "Tracy.hpp"
        #include "TracyC.h"
        #if defined(_MSC_VER) && !defined(NDEBUG)
            #define new DEBUG_NEW
        #endif
    #else
        #define LOBSTER_FRAME_PROFILE_THIS_SCOPE
        #define LOBSTER_FRAME_PROFILE_GPU
        // Emulate Tracy with our own minimal API.
        struct ___tracy_source_location_data {
            const char *name;
            const char *function;
            const char *file;
            uint32_t line;
            uint32_t color;
        };
        struct ___tracy_c_zone_context {
            double start_time;
        };
        ___tracy_c_zone_context ___tracy_emit_zone_begin(
            const struct ___tracy_source_location_data *srcloc, int active);
        void ___tracy_emit_zone_end(___tracy_c_zone_context ctx);
        struct ProfStat {
            double time;
        };
        extern unordered_map<const struct ___tracy_source_location_data *, ProfStat> prof_stats;
    #endif
#else
    #define LOBSTER_FRAME_PROFILER_BUILTINS 0
    #define LOBSTER_FRAME_PROFILER_FUNCTIONS 0
    #define LOBSTER_FRAME_PROFILER_GLOBAL 0
    #define LOBSTER_FRAME_PROFILE_THIS_SCOPE
    #define LOBSTER_FRAME_PROFILE_GPU
#endif

#define LOBSTER_RENDERDOC 0
