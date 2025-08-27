#include <android/log.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "dobby.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "auditpatch", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "auditpatch", __VA_ARGS__)

static int (*old_vasprintf)(char **strp, const char *fmt, va_list ap) = NULL;

static bool is_in_quotes(const char *str, const char *pos) {
    bool in_quote = false;
    for (const char *p = str; p < pos; p++) {
        if (*p == '"') {
            in_quote = !in_quote;
        }
    }
    return in_quote;
}

static int my_vasprintf(char **strp, const char *fmt, va_list ap) {
    int result = old_vasprintf(strp, fmt, ap);
    if (result > 0 && *strp) {
        const char *source_contexts[] = {
            "tcontext=u:r:su:s0",
            "tcontext=u:r:magisk:s0",
            "tcontext=u:object_r:proc_modules:s0"
        };
        size_t source_contexts_len = sizeof(source_contexts) / sizeof(source_contexts[0]);
        for (size_t i = 0; i < source_contexts_len; i++) {
            const char *source = source_contexts[i];
            char *pos = strstr(*strp, source);
            if (pos && !is_in_quotes(*strp, pos)) {
                char *line_start = pos;
                while (line_start > *strp && *(line_start - 1) != '\n') line_start--;
                char *line_end = strchr(pos, '\n');
                if (line_end) line_end++;
                else line_end = *strp + strlen(*strp);
                size_t new_len = strlen(*strp) - (line_end - line_start);
                char *new_str = malloc(new_len + 1);
                strncpy(new_str, *strp, line_start - *strp);
                strcpy(new_str + (line_start - *strp), line_end);
                free(*strp);
                *strp = new_str;
                return (int)new_len;
            }
        }
    }
    return result;
}

__attribute__((constructor))
void init(void) {
    void *addr = DobbySymbolResolver("libc.so", "vasprintf");
    if (!addr) {
        LOGE("Failed to find vasprintf symbol");
        return;
    }

    if (DobbyHook(addr, (void*)my_vasprintf, (void**)&old_vasprintf) == 0) {
        LOGI("vasprintf hooked successfully in logd!");
    } else {
        LOGE("vasprintf hook failed in logd");
    }
}
