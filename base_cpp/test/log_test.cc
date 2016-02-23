/*
 * Copyright (c) 2014-2015, Siemens AG. All rights reserved.
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

#include <cstring>

namespace embb {
namespace base {
namespace test {

LogTest::LogTest() {
  CreateUnit("Test all").Add(&LogTest::Test, this);
}

static char const * logged_message;

static void test_log_function(void * context, char const * msg) {
  EMBB_UNUSED(context);
  logged_message = msg;
}

void LogTest::Test() {
  using embb::base::Log;
  char const * test_msg = "hello";
  char const * null = 0;

  Log::SetLogFunction(0, test_log_function);

  Log::SetLogLevel(EMBB_LOG_LEVEL_TRACE);
  logged_message = null;
  Log::Trace("chn", test_msg);
#ifdef EMBB_DEBUG
  PT_ASSERT_NE(logged_message, null);
  PT_EXPECT(0 == strcmp(logged_message, "[chn] - [TRACE] hello"));
#else
  PT_EXPECT_EQ(null, logged_message);
#endif
  logged_message = null;
  Log::Info("chn", test_msg);
#ifdef EMBB_DEBUG
  PT_ASSERT_NE(logged_message, null);
  PT_EXPECT(0 == strcmp(logged_message, "[chn] - [INFO ] hello"));
#else
  PT_EXPECT_EQ(null, logged_message);
#endif
  logged_message = null;
  Log::Warning("chn", test_msg);
  PT_ASSERT_NE(logged_message, null);
  PT_EXPECT(0 == strcmp(logged_message, "[chn] - [WARN ] hello"));
  logged_message = null;
  Log::Error("chn", test_msg);
  PT_ASSERT_NE(logged_message, null);
  PT_EXPECT(0 == strcmp(logged_message, "[chn] - [ERROR] hello"));

  Log::SetLogLevel(EMBB_LOG_LEVEL_INFO);
  logged_message = null;
  Log::Trace("chn", test_msg);
  PT_EXPECT_EQ(null, logged_message);
  logged_message = null;
  Log::Info("chn", test_msg);
#ifdef EMBB_DEBUG
  PT_ASSERT_NE(logged_message, null);
  PT_EXPECT(0 == strcmp(logged_message, "[chn] - [INFO ] hello"));
#else
  PT_EXPECT_EQ(null, logged_message);
#endif
  logged_message = null;
  Log::Warning("chn", test_msg);
  PT_ASSERT_NE(logged_message, null);
  PT_EXPECT(0 == strcmp(logged_message, "[chn] - [WARN ] hello"));
  logged_message = null;
  Log::Error("chn", test_msg);
  PT_ASSERT_NE(logged_message, null);
  PT_EXPECT(0 == strcmp(logged_message, "[chn] - [ERROR] hello"));

  Log::SetLogLevel(EMBB_LOG_LEVEL_WARNING);
  logged_message = null;
  Log::Trace("chn", test_msg);
  PT_EXPECT_EQ(null, logged_message);
  logged_message = null;
  Log::Info("chn", test_msg);
  PT_EXPECT_EQ(null, logged_message);
  logged_message = null;
  Log::Warning("chn", test_msg);
  PT_ASSERT_NE(logged_message, null);
  PT_EXPECT(0 == strcmp(logged_message, "[chn] - [WARN ] hello"));
  logged_message = null;
  Log::Error("chn", test_msg);
  PT_ASSERT_NE(logged_message, null);
  PT_EXPECT(0 == strcmp(logged_message, "[chn] - [ERROR] hello"));

  Log::SetLogLevel(EMBB_LOG_LEVEL_ERROR);
  logged_message = null;
  Log::Trace("chn", test_msg);
  PT_EXPECT_EQ(null, logged_message);
  logged_message = null;
  Log::Info("chn", test_msg);
  PT_EXPECT_EQ(null, logged_message);
  logged_message = null;
  Log::Warning("chn", test_msg);
  PT_EXPECT_EQ(null, logged_message);
  logged_message = null;
  Log::Error("chn", test_msg);
  PT_ASSERT_NE(logged_message, null);
  PT_EXPECT(0 == strcmp(logged_message, "[chn] - [ERROR] hello"));

  Log::SetLogLevel(EMBB_LOG_LEVEL_NONE);
  logged_message = null;
  Log::Trace("chn", test_msg);
  PT_EXPECT_EQ(null, logged_message);
  logged_message = null;
  Log::Info("chn", test_msg);
  PT_EXPECT_EQ(null, logged_message);
  logged_message = null;
  Log::Warning("chn", test_msg);
  PT_EXPECT_EQ(null, logged_message);
  logged_message = null;
  Log::Error("chn", test_msg);
  PT_EXPECT_EQ(null, logged_message);
}

} // namespace test
} // namespace base
} // namespace embb



