/*
 * Copyright (c) 2014-2017, Siemens AG. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <log_test.h>
#include <embb/base/c/internal/unused.h>
#include <embb/base/log.h>

#include <string>

namespace embb {
namespace base {
namespace test {

LogTest::LogTest() {
  CreateUnit("Test all").Add(&LogTest::Test, this);
}

static void test_log_function(void * context, char const * msg) {
  std::string * logged_message = reinterpret_cast<std::string*>(context);
  *logged_message = msg;
}

void LogTest::Test() {
  using embb::base::Log;
  char const * test_msg = "hello";
  std::string logged_message;

  Log::SetLogFunction(&logged_message, test_log_function);

  Log::SetLogLevel(EMBB_LOG_LEVEL_TRACE);
  logged_message = "none";
  Log::Trace("chn", test_msg);
#ifdef EMBB_DEBUG
  PT_EXPECT(logged_message == "[chn] - [TRACE] hello");
#else
  PT_EXPECT(logged_message == "none");
#endif
  logged_message = "none";
  Log::Info("chn", test_msg);
#ifdef EMBB_DEBUG
  PT_EXPECT(logged_message == "[chn] - [INFO ] hello");
#else
  PT_EXPECT(logged_message == "none");
#endif
  logged_message = "none";
  Log::Warning("chn", test_msg);
  PT_EXPECT(logged_message == "[chn] - [WARN ] hello");
  logged_message = "none";
  Log::Error("chn", test_msg);
  PT_EXPECT(logged_message == "[chn] - [ERROR] hello");

  Log::SetLogLevel(EMBB_LOG_LEVEL_INFO);
  logged_message = "none";
  Log::Trace("chn", test_msg);
  PT_EXPECT(logged_message == "none");
  logged_message = "none";
  Log::Info("chn", test_msg);
#ifdef EMBB_DEBUG
  PT_EXPECT(logged_message == "[chn] - [INFO ] hello");
#else
  PT_EXPECT(logged_message == "none");
#endif
  logged_message = "none";
  Log::Warning("chn", test_msg);
  PT_EXPECT(logged_message == "[chn] - [WARN ] hello");
  logged_message = "none";
  Log::Error("chn", test_msg);
  PT_EXPECT(logged_message == "[chn] - [ERROR] hello");

  Log::SetLogLevel(EMBB_LOG_LEVEL_WARNING);
  logged_message = "none";
  Log::Trace("chn", test_msg);
  PT_EXPECT(logged_message == "none");
  logged_message = "none";
  Log::Info("chn", test_msg);
  PT_EXPECT(logged_message == "none");
  logged_message = "none";
  Log::Warning("chn", test_msg);
  PT_EXPECT(logged_message == "[chn] - [WARN ] hello");
  logged_message = "none";
  Log::Error("chn", test_msg);
  PT_EXPECT(logged_message == "[chn] - [ERROR] hello");

  Log::SetLogLevel(EMBB_LOG_LEVEL_ERROR);
  logged_message = "none";
  Log::Trace("chn", test_msg);
  PT_EXPECT(logged_message == "none");
  logged_message = "none";
  Log::Info("chn", test_msg);
  PT_EXPECT(logged_message == "none");
  logged_message = "none";
  Log::Warning("chn", test_msg);
  PT_EXPECT(logged_message == "none");
  logged_message = "none";
  Log::Error("chn", test_msg);
  PT_EXPECT(logged_message == "[chn] - [ERROR] hello");

  Log::SetLogLevel(EMBB_LOG_LEVEL_NONE);
  logged_message = "none";
  Log::Trace("chn", test_msg);
  PT_EXPECT(logged_message == "none");
  logged_message = "none";
  Log::Info("chn", test_msg);
  PT_EXPECT(logged_message == "none");
  logged_message = "none";
  Log::Warning("chn", test_msg);
  PT_EXPECT(logged_message == "none");
  logged_message = "none";
  Log::Error("chn", test_msg);
  PT_EXPECT(logged_message == "none");
}

} // namespace test
} // namespace base
} // namespace embb



