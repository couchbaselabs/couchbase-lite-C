CBL_C {
	global:
		CBL_Retain;
		CBL_Release;
		CBL_InstanceCount;
		CBL_DumpInstances;
		CBL_Now;
		CBLError_Message;
		CBLListener_Remove;
		kCBLTypeProperty;
		kCBLBlobType;
		kCBLBlobDigestProperty;
		kCBLBlobLengthProperty;
		kCBLBlobContentTypeProperty;
		FLDict_IsBlob;
		FLDict_GetBlob;
		FLSlot_SetBlob;
		CBLBlob_ContentType;
		CBLBlob_Length;
		CBLBlob_Digest;
		CBLBlob_Properties;
		CBLBlob_Content;
		CBLBlob_OpenContentStream;
		CBLBlob_NewWithData;
		CBLBlob_NewWithStream;
		CBLBlobReader_Read;
		CBLBlobReader_Close;
		CBLBlobWriter_New;
		CBLBlobWriter_Close;
		CBLBlobWriter_Write;
		CBLDatabaseConfiguration_Default;
		CBL_DatabaseExists;
		CBL_CopyDatabase;
		CBL_DeleteDatabase;
		CBLDatabase_Delete;
		CBLDatabase_Open;
		CBLDatabase_Close;
		CBLDatabase_Name;
		CBLDatabase_Path;
		CBLDatabase_Config;
		CBLDatabase_Count;
		CBLDatabase_LastSequence;
		CBLDatabase_Delete;
		CBLDatabase_BeginTransaction;
		CBLDatabase_EndTransaction;
		CBLDatabase_PerformMaintenance;
		CBLDatabase_AddChangeListener;
		CBLDatabase_AddChangeDetailListener;
		CBLDatabase_AddDocumentChangeListener;
		CBLDatabase_BufferNotifications;
		CBLDatabase_SendNotifications;
		CBLDatabase_CreateIndex;
		CBLDatabase_DeleteIndex;
		CBLDatabase_IndexNames;
		CBLDocument_ID;
		CBLDocument_RevisionID;
		CBLDocument_Sequence;
		CBLDocument_Create;
		CBLDocument_CreateWithID;
		CBLDocument_MutableCopy;
		CBLDocument_Properties;
		CBLDocument_MutableProperties;
		CBLDocument_SetProperties;
		CBLDocument_CreateJSON;
		CBLDocument_SetJSON;
		CBLDatabase_GetDocument;
		CBLDatabase_GetMutableDocument;
		CBLDatabase_SaveDocument;
		CBLDatabase_SaveDocumentWithConcurrencyControl;
		CBLDatabase_SaveDocumentWithConflictHandler;
		CBLDatabase_DeleteDocument;
		CBLDatabase_DeleteDocumentWithConcurrencyControl;
		CBLDatabase_DeleteDocumentByID;
		CBLDatabase_PurgeDocument;
		CBLDatabase_PurgeDocumentByID;
		CBLDatabase_GetDocumentExpiration;
		CBLDatabase_SetDocumentExpiration;
		CBL_Log;
		CBL_LogMessage;
		CBLLog_Callback;
		CBLLog_SetCallback;
		CBLLog_CallbackLevel;
		CBLLog_SetCallbackLevel;
		CBLLog_ConsoleLevel;
		CBLLog_SetConsoleLevel;
		CBLLog_ConsoleLevelOfDomain;
		CBLLog_SetConsoleLevelOfDomain;
		CBLLog_FileConfig;
		CBLLog_SetFileConfig;
		CBLQuery_New;
		CBLQuery_Parameters;
		CBLQuery_SetParameters;
		CBLQuery_Execute;
		CBLQuery_Explain;
		CBLQuery_ColumnCount;
		CBLQuery_ColumnName;
		CBLQuery_AddChangeListener;
		CBLQuery_CopyCurrentResults;
		CBLResultSet_Next;
		CBLResultSet_ValueAtIndex;
		CBLResultSet_ValueForKey;
		CBLResultSet_ResultArray;
		CBLResultSet_ResultDict;
		CBLResultSet_GetQuery;
		kCBLAuthDefaultCookieName;
		CBLEndpoint_NewWithURL;
		CBLEndpoint_Free;
		CBLAuth_NewPassword;
		CBLAuth_NewSession;
		CBLAuth_Free;
		CBLReplicator_New;
		CBLReplicator_Config;
		CBLReplicator_Start;
		CBLReplicator_Stop;
		CBLReplicator_SetHostReachable;
		CBLReplicator_SetSuspended;
		CBLReplicator_Status;
		CBLReplicator_PendingDocumentIDs;
		CBLReplicator_IsDocumentPending;
		CBLReplicator_AddChangeListener;
		CBLReplicator_AddDocumentReplicationListener;
		CBLDefaultConflictResolver;
		kFLNullValue;
		kFLEmptyArray;
		kFLEmptyDict;
		FLSlice_Equal;
		FLSlice_Compare;
		FLSlice_Copy;
		FLSlice_ToCString;
		FLSliceResult_New;
		_FLBuf_Retain;
		_FLBuf_Release;
		FLDoc_FromResultData;
		FLDoc_FromJSON;
		FLDoc_Release;
		FLDoc_Retain;
		FLDoc_GetData;
		FLDoc_GetRoot;
		FLDoc_GetSharedKeys;
		FLData_Dump;
		FLDump;
		FLDumpData;
		FLValue_FromData;
		FLValue_GetType;
		FLValue_IsInteger;
		FLValue_IsUnsigned;
		FLValue_IsDouble;
		FLValue_IsEqual;
		FLValue_AsBool;
		FLValue_AsData;
		FLValue_AsInt;
		FLValue_AsUnsigned;
		FLValue_AsFloat;
		FLValue_AsDouble;
		FLValue_AsString;
		FLValue_AsArray;
		FLValue_AsDict;
		FLValue_AsTimestamp;
		FLValue_ToString;
		FLValue_ToJSON;
		FLValue_ToJSONX;
		FLValue_ToJSON5;
		FLValue_FindDoc;
		FLValue_Retain;
		FLValue_Release;
		FLData_ConvertJSON;
		FLJSON5_ToJSON;
		FLArray_Count;
		FLArray_IsEmpty;
		FLArray_Get;
		FLArray_AsMutable;
		FLArray_MutableCopy;
		FLArrayIterator_Begin;
		FLArrayIterator_GetCount;
		FLArrayIterator_GetValue;
		FLArrayIterator_GetValueAt;
		FLArrayIterator_Next;
		FLMutableArray_New;
		FLMutableArray_GetSource;
		FLMutableArray_IsChanged;
		FLMutableArray_Append;
		FLMutableArray_Set;
		FLMutableArray_Insert;
		FLMutableArray_Remove;
		FLMutableArray_Resize;
		FLMutableArray_GetMutableArray;
		FLMutableArray_GetMutableDict;
		FLDict_Count;
		FLDict_IsEmpty;
		FLDict_Get;
		FLDict_GetWithKey;
		FLDict_AsMutable;
		FLDict_MutableCopy;
		FLDictIterator_Begin;
		FLDictIterator_GetCount;
		FLDictIterator_GetKey;
		FLDictIterator_GetKeyString;
		FLDictIterator_GetValue;
		FLDictIterator_Next;
		FLDictIterator_End;
		FLDictKey_Init;
		FLDictKey_GetString;
		FLMutableDict_New;
		FLMutableDict_GetSource;
		FLMutableDict_IsChanged;
		FLMutableDict_Set;
		FLMutableDict_Remove;
		FLMutableDict_RemoveAll;
		FLMutableDict_GetMutableArray;
		FLMutableDict_GetMutableDict;
		FLSlot_SetNull;
		FLSlot_SetBool;
		FLSlot_SetInt;
		FLSlot_SetUInt;
		FLSlot_SetFloat;
		FLSlot_SetDouble;
		FLSlot_SetString;
		FLSlot_SetData;
		FLSlot_SetValue;
		FLKeyPath_New;
		FLKeyPath_Free;
		FLKeyPath_Eval;
		FLKeyPath_EvalOnce;
		FLDeepIterator_New;
		FLDeepIterator_Free;
		FLDeepIterator_GetValue;
		FLDeepIterator_GetKey;
		FLDeepIterator_GetIndex;
		FLDeepIterator_GetDepth;
		FLDeepIterator_SkipChildren;
		FLDeepIterator_Next;
		FLDeepIterator_GetPath;
		FLDeepIterator_GetPathString;
		FLDeepIterator_GetJSONPointer;
	local:
		*;
};
