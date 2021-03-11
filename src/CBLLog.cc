//
// CBLLog.cc
//
// Copyright © 2019 Couchbase. All rights reserved.
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

#include "CBLLog.h"
#include "CBLPrivate.h"
#include "c4Base.h"
#include "Internal.hh"
#include "fleece/slice.hh"
#include <atomic>
#include <cstdlib>
#include "betterassert.hh"

using namespace std;
using namespace fleece;


static const C4LogDomain kC4Domains[5] = {
    kC4DefaultLog, kC4DatabaseLog, kC4QueryLog, kC4SyncLog, kC4WebSocketLog
};

static atomic<CBLLogCallback> sCallback = nullptr;
static atomic<CBLLogLevel>    sCallbackLevel = CBLLogInfo;


static void updateCallback();


CBLLogLevel CBLLog_ConsoleLevelOfDomain(CBLLogDomain domain) CBLAPI {
    precondition((domain <= kCBLLogDomainNetwork));
    return CBLLogLevel(c4log_getLevel(kC4Domains[domain]));
}


void CBLLog_SetConsoleLevelOfDomain(CBLLogDomain domain, CBLLogLevel level) CBLAPI {
    precondition((domain <= kCBLLogDomainNetwork));
    precondition((level <= CBLLogNone));
    if (domain == kCBLLogDomainAll) {
        c4log_setCallbackLevel(C4LogLevel(level));
        for (int i = 0; i < 5; ++i)
            c4log_setLevel(kC4Domains[i], C4LogLevel(level));
    } else {
        c4log_setLevel(kC4Domains[domain], C4LogLevel(level));
    }
}


CBLLogLevel CBLLog_ConsoleLevel() CBLAPI {
    return CBLLog_ConsoleLevelOfDomain(kCBLLogDomainAll);
}


void CBLLog_SetConsoleLevel(CBLLogLevel level) CBLAPI {
    CBLLog_SetConsoleLevelOfDomain(kCBLLogDomainAll, level);
}


CBLLogLevel CBLLog_CallbackLevel() CBLAPI {
    return sCallbackLevel;
}


void CBLLog_SetCallbackLevel(CBLLogLevel level) CBLAPI {
    if (sCallbackLevel.exchange(level) != level)
        updateCallback();
}


CBLLogCallback CBLLog_Callback() CBLAPI {
    return sCallback;
}


void CBLLog_SetCallback(CBLLogCallback callback) CBLAPI {
    auto oldCallback = sCallback.exchange(callback);
    if ((callback != nullptr) != (oldCallback != nullptr))
        updateCallback();
}


static void updateCallback() {
    C4LogCallback c4Callback = nullptr;
    if (sCallback) {
        c4Callback = [](C4LogDomain domain, C4LogLevel level, const char *msg, va_list) noexcept {
            // Map C4LogDomain to CBLLogDomain:
            CBLLogCallback callback = sCallback;
            if (!callback)
                return;
            CBLLogDomain cblDomain = kCBLLogDomainAll;
            for (int d = 0; d < 5; ++d) {
                if (kC4Domains[d] == domain) {
                    cblDomain = CBLLogDomain(d);
                    break;
                }
            }
            callback(cblDomain, CBLLogLevel(level), slice(msg));
        };
    }
    c4log_writeToCallback(C4LogLevel(sCallbackLevel.load()), c4Callback, true);
}


void CBL_Log(CBLLogDomain domain, CBLLogLevel level, const char *format _cbl_nonnull, ...) CBLAPI {
    precondition((domain <= kCBLLogDomainNetwork));
    precondition((level <= CBLLogNone));
    char *message = nullptr;
    va_list args;
    va_start(args, format);
    vasprintf(&message, format, args);
    va_end(args);
    C4LogToAt(kC4Domains[domain], C4LogLevel(level), "%s", message);
    free(message);
}


void CBL_LogMessage(CBLLogDomain domain, CBLLogLevel level, FLString message) CBLAPI {
    precondition((domain <= kCBLLogDomainNetwork));
    precondition((level <= CBLLogNone));
    c4slog(kC4Domains[domain], C4LogLevel(level), message);
}


static CBLLogFileConfiguration sLogFileConfig;
static alloc_slice sLogFileDir;


const CBLLogFileConfiguration* CBLLog_FileConfig() CBLAPI {
    if (sLogFileConfig.directory.buf)
        return &sLogFileConfig;
    else
        return nullptr;
}


bool CBLLog_SetFileConfig(CBLLogFileConfiguration config, CBLError *outError) CBLAPI {
    sLogFileDir = config.directory;     // copy string to the heap
    config.directory = sLogFileDir;     // and put the heap copy in the struct
    sLogFileConfig = config;

    alloc_slice buildInfo = c4_getBuildInfo();
    string header = "Generated by Couchbase Lite for C / " + string(buildInfo);

    C4LogFileOptions c4opt = {};
    c4opt.log_level         = C4LogLevel(config.level);
    c4opt.base_path         = config.directory;
    c4opt.max_size_bytes    = config.maxSize;
    c4opt.max_rotate_count  = config.maxRotateCount;
    c4opt.use_plaintext     = config.usePlaintext;
    c4opt.header            = slice(header);

    return c4log_writeToBinaryFile(c4opt, internal(outError));
}


extern "C" CBL_CORE_API std::atomic_int gC4ExpectExceptions;

void CBLLog_BeginExpectingExceptions() CBLAPI {
    ++gC4ExpectExceptions;
    c4log_warnOnErrors(false);
}

void CBLLog_EndExpectingExceptions() CBLAPI {
    if (--gC4ExpectExceptions == 0)
        c4log_warnOnErrors(true);
}

