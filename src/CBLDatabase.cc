//
// CBLDatabase.cc
//
// Copyright © 2018 Couchbase. All rights reserved.
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

#include "CBLDatabase_Internal.hh"
#include "CBLPrivate.h"
#include "Internal.hh"
#include "Util.hh"
#include <sys/stat.h>
#include <unistd.h>

using namespace std;
using namespace fleece;
using namespace cbl_internal;


// Default location for databases: the current directory
static const char* defaultDbDir() {
    static const string kDir = getcwd(nullptr, 0);
    return kDir.c_str();
}

static slice effectiveDir(const char *inDirectory) {
    return slice(inDirectory ? inDirectory : defaultDbDir());
}

static C4DatabaseConfig2 asC4Config(const CBLDatabaseConfiguration *config) {
    C4DatabaseConfig2 c4config { effectiveDir(config->directory) };
#ifdef COUCHBASE_ENTERPRISE
    c4Config->encryptionKey.algorithm = config->encryptionKey.algorithm;
    static_assert(sizeof(CBLEncryptionKey::bytes) == sizeof(C4EncryptionKey::bytes));
    memcpy(c4Config->encryptionKey.bytes, config->encryptionKey.bytes, sizeof(CBLEncryptionKey::bytes));
#endif
    return c4config;
}


#pragma mark - STATIC "METHODS":


bool cbl_databaseExists(const char *name, const char *inDirectory) CBLAPI {
    return c4db_exists(slice(name), effectiveDir(inDirectory));
}


bool cbl_copyDatabase(const char* fromPath,
                const char* toName,
                const CBLDatabaseConfiguration *config,
                CBLError* outError) CBLAPI
{
    C4DatabaseConfig2 c4config = asC4Config(config);
    return c4db_copyNamed(slice(fromPath), slice(toName), &c4config, internal(outError));
}


bool cbl_deleteDatabase(const char *name,
                  const char *inDirectory,
                  CBLError* outError) CBLAPI
{
    return c4db_deleteNamed(slice(name), effectiveDir(inDirectory), internal(outError));
}


#pragma mark - LIFECYCLE & OPERATIONS:


CBLDatabase* cbl_db_open(const char *name,
                         const CBLDatabaseConfiguration *config,
                         CBLError *outError) CBLAPI
{
    C4DatabaseConfig2 c4config = asC4Config(config);
    C4Database *c4db = c4db_openNamed(slice(name), &c4config, internal(outError));
    if (!c4db)
        return nullptr;
    return retain(new CBLDatabase(c4db, name, (config->directory ?: "")));
}


bool cbl_db_close(CBLDatabase* db, CBLError* outError) CBLAPI {
    return !db || c4db_close(internal(db), internal(outError));
}

bool cbl_db_beginBatch(CBLDatabase* db, CBLError* outError) CBLAPI {
    return c4db_beginTransaction(internal(db), internal(outError));
}

bool cbl_db_endBatch(CBLDatabase* db, CBLError* outError) CBLAPI {
    return c4db_endTransaction(internal(db), true, internal(outError));
}

bool cbl_db_compact(CBLDatabase* db, CBLError* outError) CBLAPI {
    return c4db_compact(internal(db), internal(outError));
}

bool cbl_db_delete(CBLDatabase* db, CBLError* outError) CBLAPI {
    return c4db_delete(internal(db), internal(outError));
}


#pragma mark - ACCESSORS:


const char* cbl_db_name(const CBLDatabase* db) CBLAPI {
    return db->name.c_str();
}

const char* cbl_db_path(const CBLDatabase* db) CBLAPI {
    return db->path.c_str();
}

const CBLDatabaseConfiguration cbl_db_config(const CBLDatabase* db) CBLAPI {
    const char *dir = db->dir.empty() ? nullptr : db->dir.c_str();
    return {dir};
}

uint64_t cbl_db_count(const CBLDatabase* db) CBLAPI {
    return c4db_getDocumentCount(internal(db));
}

uint64_t cbl_db_lastSequence(const CBLDatabase* db) CBLAPI {
    return c4db_getLastSequence(internal(db));
}


#pragma mark - DATABASE LISTENERS:


CBLDatabase::~CBLDatabase() {
    c4dbobs_free(_observer);
    _docListeners.clear();
    c4db_release(c4db);
}


bool CBLDatabase::shouldNotifyNow() {
    if (_notificationsCallback) {
        if (!_notificationsAnnounced) {
            _notificationsAnnounced = true;
            _notificationsCallback(_notificationsContext, this);
        }
        return false;
    } else {
        return true;
    }
}


CBLListenerToken* CBLDatabase::addListener(CBLDatabaseChangeListener listener, void *context) {
    auto token = _listeners.add(listener, context);
    if (!_observer) {
        _observer = c4dbobs_create(c4db,
                                   [](C4DatabaseObserver* observer, void *context) {
                                       ((CBLDatabase*)context)->databaseChanged();
                                   },
                                   this);
    }
    return token;
}


void CBLDatabase::databaseChanged() {
    if (shouldNotifyNow())
        callDBListeners();
}


void CBLDatabase::callDBListeners() {
    static const uint32_t kMaxChanges = 100;
    while (true) {
        C4DatabaseChange c4changes[kMaxChanges];
        bool external;
        uint32_t nChanges = c4dbobs_getChanges(_observer, c4changes, kMaxChanges, &external);
        if (nChanges == 0)
            break;
        // Convert docID slices to C strings:
        const char* docIDs[kMaxChanges];
        size_t bufSize = 0;
        for (uint32_t i = 0; i < nChanges; ++i)
            bufSize += c4changes[i].docID.size + 1;
        char *buf = new char[bufSize], *next = buf;
        for (uint32_t i = 0; i < nChanges; ++i) {
            docIDs[i] = next;
            memcpy(next, (const char*)c4changes[i].docID.buf, c4changes[i].docID.size);
            next += c4changes[i].docID.size;
            *(next++) = '\0';
        }
        assert(next - buf == bufSize);
        // Call the listener(s):
        _listeners.call(this, nChanges, docIDs);
        delete [] buf;
    }
}


CBLListenerToken* cbl_db_addChangeListener(const CBLDatabase* constdb _cbl_nonnull,
                                     CBLDatabaseChangeListener listener _cbl_nonnull,
                                     void *context) CBLAPI
{
    return const_cast<CBLDatabase*>(constdb)->addListener(listener, context);
}


#pragma mark - DOCUMENT LISTENERS:


namespace cbl_internal {

    // Custom subclass of CBLListenerToken for document listeners.
    // (It implements the ListenerToken<> template so that it will work with Listeners<>.)
    template<>
    class ListenerToken<CBLDocumentChangeListener> : public CBLListenerToken {
    public:
        ListenerToken(CBLDatabase *db, const char *docID, CBLDocumentChangeListener callback, void *context)
        :CBLListenerToken((const void*)callback, context)
        ,_db(db)
        ,_docID(docID)
        ,_c4obs( c4docobs_create(internal(db),
                                 slice(docID),
                                 [](C4DocumentObserver* observer, C4String docID,
                                    C4SequenceNumber sequence, void *context)
                                 {
                                     ((ListenerToken*)context)->docChanged();
                                 },
                                 this) )
        { }

        ~ListenerToken() {
            c4docobs_free(_c4obs);
        }

        CBLDocumentChangeListener callback() const           {return (CBLDocumentChangeListener)_callback;}

        // this is called indirectly by CBLDatabase::sendNotifications
        void call(const CBLDatabase*, const char*) {
            if (_scheduled) {
                _scheduled = false;
                callback()(_context, _db, _docID.c_str());
            }
        }

    private:
        void docChanged() {
            _scheduled = true;
            if (_db->shouldNotifyNow())
                call(nullptr, nullptr);
        }

        CBLDatabase* _db;
        string _docID;
        C4DocumentObserver* _c4obs {nullptr};
        bool _scheduled {false};
    };

}


CBLListenerToken* CBLDatabase::addDocListener(const char* docID _cbl_nonnull,
                                              CBLDocumentChangeListener listener, void *context)
{
    auto token = new ListenerToken<CBLDocumentChangeListener>(this, docID, listener, context);
    _docListeners.add(token);
    return token;
}


CBLListenerToken* cbl_db_addDocumentChangeListener(const CBLDatabase* db _cbl_nonnull,
                                             const char* docID _cbl_nonnull,
                                             CBLDocumentChangeListener listener _cbl_nonnull,
                                             void *context) CBLAPI
{
    return const_cast<CBLDatabase*>(db)->addDocListener(docID, listener, context);

}


#pragma mark - SCHEDULING NOTIFICATIONS:


void CBLDatabase::bufferNotifications(CBLNotificationsReadyCallback callback, void *context) {
    _notificationsContext = context;
    _notificationsCallback = callback;
}


void CBLDatabase::sendNotifications() {
    if (!_notificationsAnnounced)
        return;
    _notificationsAnnounced = false;
    callDBListeners();
    _docListeners.call(this, nullptr);
}



void cbl_db_bufferNotifications(CBLDatabase *db,
                                CBLNotificationsReadyCallback callback,
                                void *context) CBLAPI
{
    db->bufferNotifications(callback, context);
}

void cbl_db_sendNotifications(CBLDatabase *db) CBLAPI {
    db->sendNotifications();
}

