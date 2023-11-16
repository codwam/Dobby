
#include "dobby.h"

#include "logging/logging.h"

#include <stdio.h>
#include <unistd.h>

#include <iostream>


#define kMyTest 0

#if kMyTest == 0
static void (*orig_test)();

static void test() {
    printf("test\n");
}

static void replaced_test() {
    printf("replace_test\n");
}

__attribute__((constructor)) static void ctor_test() {
    Logger::Shared()->setLogLevel(LOG_LEVEL_INFO);

    test();
    DobbyHook((void *)&test, (void *)&replaced_test, (void **)&orig_test);
    test();
    orig_test();
}

#elif kMyTest == 1
uintptr_t getCallFirstArg(RegisterContext *ctx) {
    uintptr_t result;
    result = ctx->general.regs.x0;
    return result;
}

void format_integer_manually(char *buf, uint64_t integer) {
    uint64_t tmp = 0;
    for (tmp = integer; tmp > 0; tmp = (tmp >> 4)) {
        buf[0] += (tmp % 16);
        buf--;
    }
}

static void* dbiTest(size_t size) {
    printf("test size: 0x%zx\n", size);
    return NULL;
}

// [ATTENTION]:
// printf will call 'malloc' internally, and will crash in a loop.
// so, use 'puts' is a better choice.
void malloc_handler(RegisterContext *ctx, const HookEntryInfo *info) {
    size_t size_ = 0;
    size_        = getCallFirstArg(ctx);
    char buffer[] = "[-] function malloc first arg: 0x00000000.";
    format_integer_manually(strchr(buffer, '.') - 1, size_);
    puts(buffer);
//    info->target_address();
}

__attribute__((constructor)) static void ctor_test() {
    log_set_level(-1);
    
//#define kDBIFunc malloc // malloc 本身的分析有点麻烦
#define kDBIFunc dbiTest
    
    void *tmp = NULL;
    {
        tmp = kDBIFunc(0x11111111);
        if(tmp) free(tmp);
    }
    DobbyInstrument((void *)kDBIFunc, malloc_handler);
    {
        tmp = kDBIFunc(0x22222222);
        if(tmp) free(tmp);
    }
#undef kDBIFunc
}

#elif kMyTest == 2
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

static void* symbolTest(size_t size);

#ifdef __cplusplus
}
#endif

static void* symbolTest(size_t size) {
    printf("symbol size: 0x%zx\n", size);
    return NULL;
}

void symbol_handler(void *address, DobbyRegisterContext *ctx) {
    printf("%s address: %p\n", __func__, address);
}

__attribute__((constructor)) static void ctor_test() {
    Logger::Shared()->setLogLevel(LOG_LEVEL_INFO);
    
    // hook
    void *func = DobbySymbolResolver(NULL, "open");
    DobbyInstrument(func, symbol_handler);
    
    func = DobbySymbolResolver(NULL, "symbolTest"); // 加上 extern "C" 和 去掉 static 才能找到.
    if (func == NULL) {
        func = DobbySymbolResolver(NULL, "__ZL10symbolTestm");
    }
    DobbyInstrument(func, symbol_handler);
    // test
    int fd = open("not found", O_RDONLY);
    if (fd != -1) {
        close(fd);
    }
    symbolTest(0x1111);
}
#endif


int main(int argc, char const *argv[]) {

  std::cout << "Start..." << std::endl;

//  sleep(100);
  return 0;
}
