Forked google-breakpad r1076 from trunk on 03/11/2012
http://code.google.com/p/google-breakpad

To update google-breakpad:
svn co http://google-breakpad.googlecode.com/svn/trunk/ google-breakpad

Check the root .hgignore to see and update uncommitted files:
* autotools compilation files (replaced here by cmake)
* test data: crash dumps
* unused libs: testing, third_party, tools
* unsupported OSes such as Android, iOS, Solaris

svn diff
Index: src/common/windows/guid_string.cc
===================================================================
--- src/common/windows/guid_string.cc	(revision 1076)
+++ src/common/windows/guid_string.cc	(working copy)
@@ -35,6 +35,8 @@
 
 #include "common/windows/string_utils-inl.h"
 
+#include <Guiddef.h>//CF
+
 #include "common/windows/guid_string.h"
 
 namespace google_breakpad {
Index: src/common/windows/guid_string.h
===================================================================
--- src/common/windows/guid_string.h	(revision 1076)
+++ src/common/windows/guid_string.h	(working copy)
@@ -32,7 +32,7 @@
 #ifndef COMMON_WINDOWS_GUID_STRING_H__
 #define COMMON_WINDOWS_GUID_STRING_H__
 
-#include <Guiddef.h>
+//#include <Guiddef.h>//CF
 
 #include <string>
 
Index: src/client/minidump_file_writer.cc
===================================================================
--- src/client/minidump_file_writer.cc	(revision 1076)
+++ src/client/minidump_file_writer.cc	(working copy)
@@ -44,6 +44,21 @@
 #include "third_party/lss/linux_syscall_support.h"
 #endif
 
+//CF from http://comments.gmane.org/gmane.comp.lib.gnulib.bugs/12109
+#ifdef __MINGW32__
+#if !defined getpagesize
+long
+getpagesize (void)
+{
+#ifdef _ALPHA_
+	return 8192;
+#else
+	return 4096;
+#endif
+}
+#endif
+#endif
+
 namespace google_breakpad {
 
 const MDRVA MinidumpFileWriter::kInvalidMDRVA = static_cast<MDRVA>(-1);
Index: src/client/windows/crash_generation/crash_generation_client.h
===================================================================
--- src/client/windows/crash_generation/crash_generation_client.h	(revision 1076)
+++ src/client/windows/crash_generation/crash_generation_client.h	(working copy)
@@ -30,8 +30,8 @@
 #ifndef CLIENT_WINDOWS_CRASH_GENERATION_CRASH_GENERATION_CLIENT_H_
 #define CLIENT_WINDOWS_CRASH_GENERATION_CRASH_GENERATION_CLIENT_H_
 
-#include <windows.h>
-#include <dbghelp.h>
+#include <Windows.h>
+#include <DbgHelp.h>
 #include <string>
 #include <utility>
 #include "client/windows/common/ipc_protocol.h"
Index: src/client/windows/crash_generation/minidump_generator.cc
===================================================================
--- src/client/windows/crash_generation/minidump_generator.cc	(revision 1076)
+++ src/client/windows/crash_generation/minidump_generator.cc	(working copy)
@@ -30,7 +30,9 @@
 #include "client/windows/crash_generation/minidump_generator.h"
 
 #include <assert.h>
+#ifndef __MINGW32__ //CF
 #include <avrfsdk.h>
+#endif
 
 #include <algorithm>
 #include <iterator>
@@ -80,7 +82,9 @@
       HANDLE Process,
       ULONG Flags,
       ULONG ResourceType,
+#ifndef __MINGW32__ //CF
       AVRF_RESOURCE_ENUMERATE_CALLBACK ResourceCallback,
+#endif
       PVOID EnumerationContext);
 
   // Handle to dynamically loaded verifier.dll.
@@ -91,10 +95,10 @@
 
   // Handle value to look for.
   ULONG64 handle_;
-
+#ifndef __MINGW32__ //CF
   // List of handle operations for |handle_|.
   std::list<AVRF_HANDLE_OPERATION> operations_;
-
+#endif
   // Minidump stream data.
   std::vector<char> stream_;
 };
@@ -144,7 +148,9 @@
   // and record only the operations for that handle value.
   if (enumerate_resource_(process_handle,
                           0,
+#ifndef __MINGW32__ //CF
                           AvrfResourceHandleTrace,
+#endif
                           &RecordHandleOperations,
                           this) != ERROR_SUCCESS) {
     // The handle tracing must have not been enabled.
@@ -152,6 +158,7 @@
   }
 
   // Now that |handle_| is initialized, purge all irrelevant operations.
+#ifndef __MINGW32__ //CF
   std::list<AVRF_HANDLE_OPERATION>::iterator i = operations_.begin();
   std::list<AVRF_HANDLE_OPERATION>::iterator i_end = operations_.end();
   while (i != i_end) {
@@ -178,7 +185,7 @@
             stdext::checked_array_iterator<AVRF_HANDLE_OPERATION*>(
                 reinterpret_cast<AVRF_HANDLE_OPERATION*>(stream_data + 1),
                 operations_.size()));
-
+#endif
   return true;
 }
 
@@ -186,7 +193,9 @@
   if (stream_.empty()) {
     return false;
   } else {
+#ifndef __MINGW32__ //CF
     user_stream->Type = HandleOperationListStream;
+#endif
     user_stream->BufferSize = static_cast<ULONG>(stream_.size());
     user_stream->Buffer = &stream_.front();
     return true;
@@ -221,12 +230,15 @@
     void* resource_description,
     void* enumeration_context,
     ULONG* enumeration_level) {
+#ifndef __MINGW32__ //CF
   AVRF_HANDLE_OPERATION* description =
       reinterpret_cast<AVRF_HANDLE_OPERATION*>(resource_description);
+#endif
   HandleTraceData* self =
       reinterpret_cast<HandleTraceData*>(enumeration_context);
 
   // Remember the last invalid handle operation.
+#ifndef __MINGW32__ //CF
   if (description->OperationType == OperationDbBADREF) {
     self->handle_ = description->Handle;
   }
@@ -235,7 +247,8 @@
   self->operations_.push_back(*description);
 
   *enumeration_level = HeapEnumerationEverything;
-  return ERROR_SUCCESS;
+#endif
+	return ERROR_SUCCESS;
 }
 
 }  // namespace
@@ -308,11 +321,18 @@
   wstring full_dump_file_path;
   if (full_memory_dump) {
     full_dump_file_path.assign(dump_file_path);
-    full_dump_file_path.resize(full_dump_file_path.size() - 4);  // strip .dmp
-    full_dump_file_path.append(TEXT("-full.dmp"));
+	full_dump_file_path.resize(full_dump_file_path.size() - 4);  // strip .dmp
+#ifndef __MINGW32__ //CF
+	full_dump_file_path.append(TEXT("-full.dmp"));
+#else
+	full_dump_file_path += L"-full.dmp";
+#endif
   }
-
+#ifdef __MINGW32__ //CF
+  HANDLE dump_file = CreateFileW(dump_file_path.c_str(),
+#else
   HANDLE dump_file = CreateFile(dump_file_path.c_str(),
+#endif
                                 GENERIC_WRITE,
                                 0,
                                 NULL,
@@ -326,7 +346,11 @@
 
   HANDLE full_dump_file = INVALID_HANDLE_VALUE;
   if (full_memory_dump) {
+#ifdef __MINGW32__ //CF
+    full_dump_file = CreateFileW(full_dump_file_path.c_str(),
+#else
     full_dump_file = CreateFile(full_dump_file_path.c_str(),
+#endif
                                 GENERIC_WRITE,
                                 0,
                                 NULL,
@@ -530,7 +554,11 @@
   create_uuid(&id);
   wstring id_str = GUIDString::GUIDToWString(&id);
 
+  #ifndef __MINGW32__ //CF
   *file_path = dump_path_ + TEXT("\\") + id_str + TEXT(".dmp");
+  #else
+  *file_path = dump_path_ + L"\\" + id_str + L".dmp";
+  #endif
   return true;
 }
 
Index: src/client/windows/crash_generation/crash_generation_client.cc
===================================================================
--- src/client/windows/crash_generation/crash_generation_client.cc	(revision 1076)
+++ src/client/windows/crash_generation/crash_generation_client.cc	(working copy)
@@ -276,7 +276,11 @@
   }
 
   for (int i = 0; i < kPipeConnectMaxAttempts; ++i) {
-    HANDLE pipe = CreateFile(pipe_name,
+	#ifdef __MINGW32__ //CF
+    HANDLE pipe = CreateFileW(pipe_name,
+	#else
+    HANDLE pipe = CreateFile(pipe_name,							 
+    #endif
                              pipe_access,
                              0,
                              NULL,
@@ -294,7 +298,11 @@
     }
 
     // Cannot continue retrying if wait on pipe fails.
-    if (!WaitNamedPipe(pipe_name, kPipeBusyWaitTimeoutMs)) {
+#ifdef __MINGW32__ //CF
+	if (!WaitNamedPipeW(pipe_name, kPipeBusyWaitTimeoutMs)) {
+#else
+	if (!WaitNamedPipe(pipe_name, kPipeBusyWaitTimeoutMs)) {
+#endif
       break;
     }
   }
@@ -372,7 +380,11 @@
 HANDLE CrashGenerationClient::DuplicatePipeToClientProcess(const wchar_t* pipe_name,
                                                            HANDLE hProcess) {
   for (int i = 0; i < kPipeConnectMaxAttempts; ++i) {
+#ifdef __MINGW32__ //CF
+    HANDLE local_pipe = CreateFileW(pipe_name, kPipeDesiredAccess,
+#else
     HANDLE local_pipe = CreateFile(pipe_name, kPipeDesiredAccess,
+#endif
                                    0, NULL, OPEN_EXISTING,
                                    kPipeFlagsAndAttributes, NULL);
     if (local_pipe != INVALID_HANDLE_VALUE) {
@@ -390,8 +402,11 @@
     if (GetLastError() != ERROR_PIPE_BUSY) {
       return INVALID_HANDLE_VALUE;
     }
-
+#ifdef __MINGW32__ //CF
+    if (!WaitNamedPipeW(pipe_name, kPipeBusyWaitTimeoutMs)) {
+#else
     if (!WaitNamedPipe(pipe_name, kPipeBusyWaitTimeoutMs)) {
+#endif
       return INVALID_HANDLE_VALUE;
     }
   }
Index: src/client/windows/crash_generation/minidump_generator.h
===================================================================
--- src/client/windows/crash_generation/minidump_generator.h	(revision 1076)
+++ src/client/windows/crash_generation/minidump_generator.h	(working copy)
@@ -32,6 +32,7 @@
 
 #include <windows.h>
 #include <dbghelp.h>
+#include <iostream> //CF
 #include <list>
 #include "google_breakpad/common/minidump_format.h"
 
Index: src/client/windows/handler/exception_handler.cc
===================================================================
--- src/client/windows/handler/exception_handler.cc	(revision 1076)
+++ src/client/windows/handler/exception_handler.cc	(working copy)
@@ -27,8 +27,6 @@
 // (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 // OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
-#include <ObjBase.h>
-
 #include <algorithm>
 #include <cassert>
 #include <cstdio>
@@ -138,7 +136,9 @@
 #if _MSC_VER >= 1400  // MSVC 2005/8
   previous_iph_ = NULL;
 #endif  // _MSC_VER >= 1400
-  previous_pch_ = NULL;
+#ifndef __MINGW32__  //CF
+previous_pch_ = NULL;
+#endif
   handler_thread_ = NULL;
   is_shutdown_ = false;
   handler_start_semaphore_ = NULL;
@@ -201,12 +201,13 @@
                                      &thread_id);
       assert(handler_thread_ != NULL);
     }
-
+#ifndef __MINGW32__ //CF
     dbghelp_module_ = LoadLibrary(L"dbghelp.dll");
     if (dbghelp_module_) {
       minidump_write_dump_ = reinterpret_cast<MiniDumpWriteDump_type>(
           GetProcAddress(dbghelp_module_, "MiniDumpWriteDump"));
     }
+	
 
     // Load this library dynamically to not affect existing projects.  Most
     // projects don't link against this directly, it's usually dynamically
@@ -216,7 +217,7 @@
       uuid_create_ = reinterpret_cast<UuidCreate_type>(
           GetProcAddress(rpcrt4_module_, "UuidCreate"));
     }
-
+#endif
     // set_dump_path calls UpdateNextID.  This sets up all of the path and id
     // strings, and their equivalent c_str pointers.
     set_dump_path(dump_path);
@@ -260,10 +261,10 @@
     if (handler_types & HANDLER_INVALID_PARAMETER)
       previous_iph_ = _set_invalid_parameter_handler(HandleInvalidParameter);
 #endif  // _MSC_VER >= 1400
-
+#ifndef __MINGW32__ //CF
     if (handler_types & HANDLER_PURECALL)
       previous_pch_ = _set_purecall_handler(HandlePureVirtualCall);
-
+#endif
     LeaveCriticalSection(&handler_stack_critical_section_);
   }
 }
@@ -287,10 +288,10 @@
     if (handler_types_ & HANDLER_INVALID_PARAMETER)
       _set_invalid_parameter_handler(previous_iph_);
 #endif  // _MSC_VER >= 1400
-
+#ifndef __MINGW32__ //CF
     if (handler_types_ & HANDLER_PURECALL)
       _set_purecall_handler(previous_pch_);
-
+#endif
     if (handler_stack_->back() == this) {
       handler_stack_->pop_back();
     } else {
@@ -422,7 +423,9 @@
 #if _MSC_VER >= 1400  // MSVC 2005/8
     _set_invalid_parameter_handler(handler_->previous_iph_);
 #endif  // _MSC_VER >= 1400
+#ifndef __MINGW32__ //CF
     _set_purecall_handler(handler_->previous_pch_);
+#endif
   }
 
   ~AutoExceptionHandler() {
@@ -431,8 +434,9 @@
 #if _MSC_VER >= 1400  // MSVC 2005/8
     _set_invalid_parameter_handler(ExceptionHandler::HandleInvalidParameter);
 #endif  // _MSC_VER >= 1400
+#ifndef __MINGW32__ //CF
     _set_purecall_handler(ExceptionHandler::HandlePureVirtualCall);
-
+#endif
     --ExceptionHandler::handler_stack_index_;
     LeaveCriticalSection(&ExceptionHandler::handler_stack_critical_section_);
   }
@@ -601,6 +605,7 @@
 }
 #endif  // _MSC_VER >= 1400
 
+#ifndef __MINGW32__ //CF
 // static
 void ExceptionHandler::HandlePureVirtualCall() {
   // This is an pure virtual function call, not an exception.  It's safe to
@@ -619,9 +624,7 @@
   EXCEPTION_RECORD exception_record = {};
   CONTEXT exception_context = {};
   EXCEPTION_POINTERS exception_ptrs = { &exception_record, &exception_context };
-
   ::RtlCaptureContext(&exception_context);
-
   exception_record.ExceptionCode = STATUS_NONCONTINUABLE_EXCEPTION;
 
   // We store pointers to the the expression and function strings,
@@ -647,7 +650,6 @@
     success = current_handler->WriteMinidumpOnHandlerThread(&exception_ptrs,
                                                             &assertion);
   }
-
   if (!success) {
     if (current_handler->previous_pch_) {
       // The handler didn't fully handle the exception.  Give it to the
@@ -659,13 +661,13 @@
       return;
     }
   }
-
   // The handler either took care of the invalid parameter problem itself,
   // or passed it on to another handler.  "Swallow" it by exiting, paralleling
   // the behavior of "swallowing" exceptions.
   exit(0);
 }
-
+#endif 
+	
 bool ExceptionHandler::WriteMinidumpOnHandlerThread(
     EXCEPTION_POINTERS* exinfo, MDRawAssertionInfo* assertion) {
   EnterCriticalSection(&handler_critical_section_);
@@ -711,8 +713,9 @@
   EXCEPTION_RECORD exception_record = {};
   CONTEXT exception_context = {};
   EXCEPTION_POINTERS exception_ptrs = { &exception_record, &exception_context };
-
+#ifndef __MINGW32__ //CF
   ::RtlCaptureContext(&exception_context);
+#endif
   exception_record.ExceptionCode = STATUS_NONCONTINUABLE_EXCEPTION;
 
   return WriteMinidumpForException(&exception_ptrs);
@@ -761,7 +764,9 @@
   if (child_thread_handle != NULL) {
     last_suspend_count = SuspendThread(child_thread_handle);
     if (last_suspend_count != kFailedToSuspendThread) {
+		#ifndef __MINGW32__ //CF
       ctx.ContextFlags = CONTEXT_ALL;
+		#endif
       if (GetThreadContext(child_thread_handle, &ctx)) {
         memset(&ex, 0, sizeof(ex));
         ex.ExceptionCode = EXCEPTION_BREAKPOINT;
@@ -862,14 +867,15 @@
   case IncludeThreadCallback:
   case ThreadCallback:
     return TRUE;
-
+#ifndef __MINGW32__ //CF
     // Stop receiving cancel callbacks.
   case CancelCallback:
     callback_output->CheckCancel = FALSE;
     callback_output->Cancel = FALSE;
     return TRUE;
+#endif
   }
-  // Ignore other callback types.
+	// Ignore other callback types.
   return FALSE;
 }
 
@@ -881,7 +887,11 @@
     bool write_requester_stream) {
   bool success = false;
   if (minidump_write_dump_) {
+#ifdef __MINGW32__ //CF
+    HANDLE dump_file = CreateFileW(next_minidump_path_c_,
+#else
     HANDLE dump_file = CreateFile(next_minidump_path_c_,
+#endif
                                   GENERIC_WRITE,
                                   0,  // no sharing
                                   NULL,
Index: src/client/windows/handler/exception_handler.h
===================================================================
--- src/client/windows/handler/exception_handler.h	(revision 1076)
+++ src/client/windows/handler/exception_handler.h	(working copy)
@@ -300,9 +300,11 @@
                                      uintptr_t reserved);
 #endif  // _MSC_VER >= 1400
 
+	#ifndef __MINGW32__ //CF
   // This function will be called by the CRT when a pure virtual
   // function is called.
   static void HandlePureVirtualCall();
+	#endif
 
   // This is called on the exception thread or on another thread that
   // the user wishes to produce a dump from.  It calls
@@ -403,9 +405,11 @@
   _invalid_parameter_handler previous_iph_;
 #endif  // _MSC_VER >= 1400
 
+	#ifndef __MINGW32__ //CF
   // The CRT allows you to override the default handler for pure
   // virtual function calls.
   _purecall_handler previous_pch_;
+	#endif
 
   // The exception handler thread.
   HANDLE handler_thread_;
