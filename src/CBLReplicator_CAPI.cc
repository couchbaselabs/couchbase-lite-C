//
// CBLReplicator_CAPI.cc
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

#include "CBLReplicator.h"
#include "CBLReplicator_Internal.hh"

#ifdef COUCHBASE_ENTERPRISE
#include "CBLCertificate_Internal.hh"
#include "CBLURLEndpointListener_Internal.hh"
#endif


const FLString kCBLAuthDefaultCookieName = FLSTR("SyncGatewaySession");


CBLEndpoint* CBLEndpoint_NewWithURL(FLString url) noexcept {
    try {
        return new CBLURLEndpoint(url);
    } catchAndWarn()
}

#ifdef COUCHBASE_ENTERPRISE
CBLEndpoint* CBLEndpoint_NewWithLocalDB(CBLDatabase* db) noexcept {
    try {
        return new CBLLocalEndpoint(db);
    } catchAndWarn()
}
#endif

void CBLEndpoint_Free(CBLEndpoint *endpoint) noexcept {
    delete endpoint;
}

CBLAuthenticator* CBLAuth_NewPassword(FLString username, FLString password) noexcept {
    try {
        return new BasicAuthenticator(username, password);
    } catchAndWarn()
}

CBLAuthenticator* CBLAuth_NewSession(FLString sessionID, FLString cookieName) noexcept {
    try {
        return new SessionAuthenticator(sessionID, cookieName);
    } catchAndWarn()
}

void CBLAuth_Free(CBLAuthenticator *auth) noexcept {
    delete auth;
}

CBLReplicator* CBLReplicator_New(const CBLReplicatorConfiguration* conf, CBLError *outError) noexcept {
    try {
        return retain(new CBLReplicator(*conf));
    } catchAndBridge(outError)
}

const CBLReplicatorConfiguration* CBLReplicator_Config(CBLReplicator* repl) noexcept {
    return repl->configuration();
}

CBLReplicatorStatus CBLReplicator_Status(CBLReplicator* repl) noexcept {
    return repl->status();
}

void CBLReplicator_Start(CBLReplicator* repl, bool reset) noexcept        {repl->start(reset);}
void CBLReplicator_Stop(CBLReplicator* repl) noexcept                     {repl->stop();}
void CBLReplicator_SetHostReachable(CBLReplicator* repl, bool r) noexcept {repl->setHostReachable(r);}
void CBLReplicator_SetSuspended(CBLReplicator* repl, bool sus) noexcept   {repl->setSuspended(sus);}

FLDict CBLReplicator_PendingDocumentIDs(CBLReplicator *repl, CBLError *outError) noexcept {
    try {
        auto result = FLDict_Retain(repl->pendingDocumentIDs());
        if (!result)
            if (outError) outError->code = 0;
        return result;
    } catchAndBridge(outError)
}

bool CBLReplicator_IsDocumentPending(CBLReplicator *repl, FLString docID, CBLError *outError) noexcept {
    try {
        bool result = repl->isDocumentPending(docID);
        if (!result)
            if (outError) outError->code = 0;
        return result;
    } catchAndBridge(outError)
}

CBLListenerToken* CBLReplicator_AddChangeListener(CBLReplicator* repl,
                                                  CBLReplicatorChangeListener listener,
                                                  void *context) noexcept
{
    return retain(repl->addChangeListener(listener, context));
}

CBLListenerToken* CBLReplicator_AddDocumentReplicationListener(CBLReplicator* repl,
                                                    CBLDocumentReplicationListener listener,
                                                    void *context) noexcept
{
    return retain(repl->addDocumentListener(listener, context));
}


#pragma mark - CBLURLEndpointListener:


#ifdef COUCHBASE_ENTERPRISE

CBLURLEndpointListener* CBLURLEndpointListener_New(CBLURLEndpointListenerConfiguration* config) noexcept {
    try {
        return new CBLURLEndpointListener(config);
    } catchAndWarn()
}

bool CBLURLEndpointListener_Start(CBLURLEndpointListener* listener, CBLError *outError) noexcept {
    try {
        return listener->start();
    } catchAndBridge(outError)
}

void CBLURLEndpointListener_Stop(CBLURLEndpointListener* listener) noexcept {
    listener->stop();
}

uint16_t CBLURLEndpointListener_GetPort(CBLURLEndpointListener* listener) noexcept {
    return listener->port();
}

FLMutableArray CBLURLEndpointListener_GetURLs(CBLURLEndpointListener* listener) noexcept {
    return listener->URLs();
}

CBLConnectionStatus CBLURLEndpointListener_GetStatus(CBLURLEndpointListener* listener) noexcept {
    return listener->status();
}



CBLCertificate* CBLCertificate_NewFromData(FLSlice certData, CBLError* outError) noexcept {
    try {
        return move(CBLCertificate::fromData(certData)).detach();
    } catchAndBridge(outError);
}

FLSliceResult CBLCertificate_PEMData(CBLCertificate *cert) noexcept {
    try {
        return FLSliceResult(cert->PEMData());
    } catchAndWarn();
}

FLSliceResult CBLCertificate_DERData(CBLCertificate *cert) noexcept {
    try {
        return FLSliceResult(cert->DERData());
    } catchAndWarn();
}

CBLCertificate* CBLCertificate_NextInChain(CBLCertificate *cert) noexcept {
    try {
        return move(cert->nextInChain()).detach();
    } catchAndWarn();
}



CBLTLSIdentity* CBLTLSIdentity_NewFromData(FLSlice privateKeyData,
                                           CBLCertificate *certificate,
                                           CBLError* outError) noexcept
{
    try {
        return move(CBLTLSIdentity::fromPrivateKeyData(privateKeyData, certificate)).detach();
    } catchAndBridge(outError);
}

CBLTLSIdentity* CBLTLSIdentity_GenerateAnonymous(CBLError* outError) noexcept {
    try {
        return move(CBLTLSIdentity::generateAnonymous()).detach();
    } catchAndBridge(outError);
}

CBLCertificate* CBLTLSIdentity_GetCertificate(CBLTLSIdentity *identity) noexcept {
    return identity->certificate();
}

FLSliceResult CBLTLSIdentity_PrivateKeyData(CBLTLSIdentity *identity) noexcept {
    try {
        return FLSliceResult(identity->privateKeyData());
    } catchAndWarn();
}

#endif
