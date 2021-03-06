/////////////////////////////////////////////////////////////////////////////////
//
// SourceFile: TestServer.h
//
// Marlin Server: Internet server/client
// 
// Copyright (c) 2015-2018 ir. W.E. Huisman
// All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#pragma once
#include "TestPorts.h"

extern bool doDetails;

// Forward declaration
class  HTTPServer;
class  ThreadPool;

// In TestServer.cpp
void xerror();
void xprintf(const char* p_format, ...);
void qprintf(const char* p_format, ...);
void WaitForKey();

// In detail files
int Test_CrackURL(void);
int Test_HTTPTime(void);
int TestWebServiceServer (HTTPServer* p_server,CString p_contract,int p_loglevel);
int TestJsonServer       (HTTPServer* p_server,CString p_contract,int p_loglevel); 
int TestBaseSite         (HTTPServer* P_server);
int TestPushEvents       (HTTPServer* p_server);
int TestInsecure         (HTTPServer* p_server);
int TestBodySigning      (HTTPServer* p_server);
int TestBodyEncryption   (HTTPServer* p_server);
int TestMessageEncryption(HTTPServer* p_server);
int TestReliable         (HTTPServer* p_server);
int TestReliableBA       (HTTPServer* p_server);
int TestCookies          (HTTPServer* p_server);
int TestToken            (HTTPServer* p_server);
int TestSubSites         (HTTPServer* p_server);
int StopSubsites         (HTTPServer* p_server);
int TestJsonData         (HTTPServer* p_server);
int TestFilter           (HTTPServer* p_server);
int TestPatch            (HTTPServer* p_server);
int TestFormData         (HTTPServer* p_server);
int TestClientCertificate(HTTPServer* p_server,bool p_standalone);
int TestCompression      (HTTPServer* p_server);
int TestAsynchrone       (HTTPServer* p_server);
int TestSecureSite       (HTTPServer* p_server);
int TestStreams          (HTTPServer* p_server);
int TestForms            (HTTPServer* p_server);
int TestWebSocket        (HTTPServer* p_server);
int TestThreadPool       (ThreadPool* p_pool);

int AfterTestBaseSite();
int AfterTestFilter();
int AfterTestAsynchrone();
int AfterTestBodyEncryption();
int AfterTestBodySigning();
int AfterTestClientCert();
int AfterTestCompression();
int AfterTestContract();
int AfterTestCookies();
int AfterTestEvents();
int AfterTestFormData();
int AfterTestInsecure();
int AfterTestJsonData();
int AfterTestMessageEncryption();
int AfterTestPatch();
int AfterTestReliable();
int AfterTestSubsites();
int AfterTestToken();
int AfterTestSecureSite();
int AfterTestThreadpool();
int AfterTestTime();
int AfterTestCrackURL();
int AfterTestWebSocket();
