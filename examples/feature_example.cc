
#include "dobby.h"

#include "logging/logging.h"

#include <stdio.h>
#include <unistd.h>

#include <iostream>

const char* ret_string = "ret_value";

#define kLogMethod(methodName) printf("--->\n%s called. address: %p\n", __func__, &methodName);
#define kLogParameter2(arg0, arg1) printf("arg0: %d, arg1: %s\n<---\n", arg0, arg1);

#define kHookLogMethod(address) printf("%s called. address: %p\n", __func__, address);
#define kHookParameter2(ctx) printf("arg0: %d, arg1: %s\n", (int) ctx->general.regs.x0, (char *) ctx->general.regs.x1);


// MARK: - DobbyHook Demo

static void (*orig_dobbyHookTest)(int arg0, const char* arg1);

static void dobbyHookTest(int arg0, const char* arg1) {
    kLogMethod(dobbyHookTest)
    kLogParameter2(arg0, arg1)
}

static void replaced_dobbyHookTest(int arg0, const char* arg1) {
    kLogMethod(dobbyHookTest)
    kLogParameter2(arg0, arg1)
}

static void DobbyHook_test() {
    printf("\n\n--- DobbyHook Demo ---\n");
    dobbyHookTest(111, ret_string);
    // 
    DobbyHook((void *)&dobbyHookTest, (void *)&replaced_dobbyHookTest, (void **)&orig_dobbyHookTest);
    dobbyHookTest(222, ret_string);
    //
    DobbyDestroy((void *)&dobbyHookTest);
    dobbyHookTest(333, ret_string);
}

// MARK: - DobbyInstrument Demo

static void* dobbyInstrumentTest(int arg0, const char* arg1) {
    kLogMethod(dobbyInstrumentTest)
    kLogParameter2(arg0, arg1)
    return (void *) arg1;
}

void dobbyInstrumentTest_handler(void *address, DobbyRegisterContext *ctx) {
    kHookLogMethod(address)
    kHookParameter2(ctx)
}

static void DobbyInstrument_test() {
    printf("\n\n--- DobbySymbolResolver Demo ---\n");
    dobbyInstrumentTest(111, ret_string);
    //
    DobbyInstrument((void *)dobbyInstrumentTest, dobbyInstrumentTest_handler);
    dobbyInstrumentTest(222, ret_string);
    //
    DobbyDestroy((void *)dobbyInstrumentTest);
    dobbyInstrumentTest(333, ret_string);
}

// MARK: - DobbySymbolResolver Demo

// 加上 extern "C" 和 去掉 static 才能被 DobbySymbolResolver 直接找到.
#ifdef __cplusplus
extern "C" {
#endif

static void* dobbySymbolResolverTest(int arg0, const char* arg1);

#ifdef __cplusplus
}
#endif

static void* dobbySymbolResolverTest(int arg0, const char* arg1) {
    kLogMethod(dobbySymbolResolverTest)
    kLogParameter2(arg0, arg1);
    return (void *) arg1;
}

void dobbySymbolResolverTest_handler(void *address, DobbyRegisterContext *ctx) {
    kHookLogMethod(address);
    kHookParameter2(ctx);
}

static void DobbySymbolResolver_test() {
    printf("\n\n--- DobbySymbolResolver Demo ---\n");
    dobbySymbolResolverTest(111, ret_string);
    //
    void *func = DobbySymbolResolver(NULL, "dobbySymbolResolverTest_handler");
    if (func == NULL) {
        func = DobbySymbolResolver(NULL, "__ZL23dobbySymbolResolverTestiPKc");
    }
    DobbyInstrument(func, dobbySymbolResolverTest_handler);
    dobbySymbolResolverTest(222, ret_string);
    //
    DobbyDestroy(func);
    dobbySymbolResolverTest(333, ret_string);
}

// MARK: - Main

__attribute__((constructor)) static void ctor() {
    std::cout << "Testing..." << std::endl;
}

int main(int argc, char const *argv[]) {
    Logger::Shared()->setLogLevel(LOG_LEVEL_INFO);
    
    DobbyHook_test();
    DobbyInstrument_test();
    DobbySymbolResolver_test();
    
    std::cout << "\n\nStarting..." << std::endl;
    
    sleep(100);
    return 0;
}
