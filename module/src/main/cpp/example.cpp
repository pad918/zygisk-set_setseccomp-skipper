#include "zygisk.hpp"
#include <android/log.h>
#include <sys/prctl.h>
#include <string.h>

#define LOGD(...) __android_log_print(ANDROID_LOG_INFO, "bypass_prctl", __VA_ARGS__)
#define TARGET_APP_NAME "com.example.libtests"

#if defined(__aarch64__)
#include "And64InlineHook.hpp"

static int (*orig_prctl)(int, unsigned long, unsigned long, unsigned long, unsigned long);

static int hook_prctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) {
    if (option == 22) {
        LOGD("Seccomp caught! Blocking filter...");
        return 0; 
    }
    return orig_prctl(option, arg2, arg3, arg4, arg5);
}
#endif

class MyModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        const char *process = env->GetStringUTFChars(args->nice_name, nullptr);
        LOGD("LOADING THE PRCTL HOOKED PROCESS %s\n", process);
        if (process && strcmp(process, TARGET_APP_NAME) == 0) {

/* KernelSU does not load it unless it is built for all
architecutres, but "A64HookFunction" only works on AArch64
thus, we simple remove the logic on all other platforms. */ 

#if defined(__aarch64__)
            LOGD("Injecting And64InlineHook...");
            A64HookFunction((void *)prctl, (void *)hook_prctl, (void **)&orig_prctl);
#else
            LOGD("Hooking skipped.");
#endif

        }
        if (process) env->ReleaseStringUTFChars(args->nice_name, process);
    }

private:
    zygisk::Api *api;
    JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(MyModule)