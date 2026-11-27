#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "qb.h"

void qb_compile_to_obj(const char *);
void qb_compile_to_exe();

#define qb_log_error(e, ...)    fprintf(stderr, "\033[0;31m%-5s\033[90m %s:%-3d \e[0m" e "\n", "ERROR", __FILE__, __LINE__, ##__VA_ARGS__);
#define qb_log_trace(e, ...)    fprintf(stderr, "\e[0;34m%-5s\033[90m %s:%-3d \e[0m" e "\n", "TRACE", __FILE__, __LINE__, ##__VA_ARGS__);
#define qb_log_info(e, ...)     fprintf(stderr, "\e[0;32m%-5s\033[90m %s:%-3d \e[0m" e "\n", "INFO", __FILE__, __LINE__, ##__VA_ARGS__);
#define qb_log_debug(e, ...)    fprintf(stderr, "\e[0;36m%-5s\033[90m %s:%-3d \e[0m" e "\n", "DEBUG", __FILE__, __LINE__, ##__VA_ARGS__);

int main(int argc, char *argv[]) {
	#ifndef QB_TARGET
    qb_log_error("no target defined");
    #endif
    
    char *src_files = strdup(QB_SRC_FILES);
    if (!src_files) {
        qb_log_error("failed to duplicate QB_SRC_FILES");
        exit(1);
    }
    
    char *src_files_it = strtok(src_files, " ");
    
    while (src_files_it != NULL) {
        qb_compile_to_obj(src_files_it);
        src_files_it = strtok(NULL, " ");
    }
    qb_compile_to_exe();

    free(src_files);

    return 0;
}

void qb_compile_to_obj(const char* src) {
    char *sh = NULL;
    char *obj = NULL;
    char *src_c = strdup(src);
    if (!src_c) {
        qb_log_error("files to copy string \"%s\" to src_c", src);
        goto clean_error;
    }
    
    const char *cc = "gcc";
    const char* flags = QB_CFLAGS;
    
    if (strstr(src, ".cpp") || strstr(src, ".cc") || strstr(src, ".cxx") || strstr(src, ".c++") || strstr(src, ".C")) {
        cc = "g++";
        flags = QB_CXXFLAGS;
    }

    char *fname = strrchr(src_c, '.');
    if (fname != NULL) {
        *fname = '\0';
    }

    if (strrchr(src_c, '/')) {
        if (asprintf(&obj, "%s%s.o", QB_BUILD_DIR, strrchr(src_c, '/')) == -1) {
	        qb_log_error("failed to format output directory");
        	goto clean_error;

        }
    } else {
        if (asprintf(&obj, "%s/%s.o", QB_BUILD_DIR, src_c) == -1) {
	        qb_log_error("failed to format output directory");
        	goto clean_error;
        }
    }


    #ifdef QB_INCLUDE
    int result = asprintf(&sh, "%s -I%s %s -c %s -o %s", cc, QB_INCLUDE, flags, src, obj);
    #else
    int result = asprintf(&sh, "%s %s -c %s -o %s", cc, flags, src, obj);
    #endif
    
    if (result == -1) {
        qb_log_error("failed to format compile command");
        goto clean_error;
    }

    qb_log_trace("%s", sh);

    if (system(sh)) {
        qb_log_error("failed to run command \"%s\"", sh);
        goto clean_error;
    }

    free(obj);
    free(sh);
    free(src_c);

    return;

 clean_error:
    free(obj);
    free(sh);
    free(src_c);
    exit(1);
}

void qb_compile_to_exe() {
    DIR *obj = opendir(QB_BUILD_DIR);
    if (!obj) {
        qb_log_error("error reading source folder: \"%s\"", QB_BUILD_DIR);
        exit(1);
    }

    char *sh = NULL;
    char *files = malloc(1);
    if (!files) {
        qb_log_error("error allocating obj files");
        exit(1);
    }
    
    files[0] = '\0';
    
    struct dirent *entry;
    while ((entry = readdir(obj)) != NULL) {
        if (entry->d_name[0] != '.' && entry->d_type == DT_REG) {
            char *temp = realloc(files, strlen(files) + strlen(entry->d_name) + strlen(QB_BUILD_DIR) + 3);
            if (!temp) {
                free(files);
                qb_log_error("error allocating obj files");
                exit(1);
            }
            files = temp;

            strcat(files, " ");
            strcat(files, QB_BUILD_DIR);
            strcat(files, "/");
            strcat(files, entry->d_name);

            qb_log_info("%s", files); 
        }
    }

    if (!asprintf(&sh, "g++%s -o %s %s", files, QB_TARGET, QB_LDFLAGS)) {
        free(files);
        free(sh);
        qb_log_error("failed to format compile command");
        exit(1);
    }

    qb_log_info("%s", sh);
    
    if (system(sh)) {
        free(files);
        free(sh);
        exit(1);
    }
    
    free(files);
    free(sh);

    if (closedir(obj) == -1) {
        qb_log_error("error closing directory \"%s\"", QB_BUILD_DIR);
        exit(1);
    }
}
