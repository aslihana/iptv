#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define INITIAL_BUFFER_SIZE 8192
#define LINE_SIZE 4096

typedef struct {
    char *data;
    size_t size;
} MemoryBuffer;

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t real_size = size * nmemb;
    MemoryBuffer *mem = (MemoryBuffer *)userp;

    char *ptr = realloc(mem->data, mem->size + real_size + 1);
    if (ptr == NULL) {
        fprintf(stderr, "Not enough memory\n");
        return 0;
    }

    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, real_size);
    mem->size += real_size;
    mem->data[mem->size] = '\0';

    return real_size;
}

int fetch_m3u(const char *url, MemoryBuffer *buffer) {
    CURL *curl;
    CURLcode res;

    buffer->data = malloc(1);
    buffer->size = 0;

    if (!buffer->data) {
        fprintf(stderr, "malloc failed\n");
        return 0;
    }

    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "curl_easy_init failed\n");
        free(buffer->data);
        return 0;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)buffer);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "m3u-parser/1.0");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "Download failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        free(buffer->data);
        return 0;
    }

    curl_easy_cleanup(curl);
    return 1;
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[len - 1] = '\0';
        len--;
    }
}

void parse_m3u(const char *content) {
    char *copy = strdup(content);
    if (!copy) {
        fprintf(stderr, "strdup failed\n");
        return;
    }

    char *line = strtok(copy, "\n");
    char current_name[LINE_SIZE] = {0};

    while (line != NULL) {
        trim_newline(line);

        if (strncmp(line, "#EXTINF:", 8) == 0) {
            char *comma = strrchr(line, ',');
            if (comma && *(comma + 1) != '\0') {
                strncpy(current_name, comma + 1, sizeof(current_name) - 1);
                current_name[sizeof(current_name) - 1] = '\0';
            } else {
                strncpy(current_name, "Unknown", sizeof(current_name) - 1);
                current_name[sizeof(current_name) - 1] = '\0';
            }
        } else if (line[0] != '#' && line[0] != '\0' && current_name[0] != '\0') {
            printf("%s -> %s\n", current_name, line);
            current_name[0] = '\0';
        }

        line = strtok(NULL, "\n");
    }

    free(copy);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <m3u_url>\n", argv[0]);
        return 1;
    }

    const char *url = argv[1];
    MemoryBuffer buffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    if (!fetch_m3u(url, &buffer)) {
        curl_global_cleanup();
        return 1;
    }

    parse_m3u(buffer.data);

    free(buffer.data);
    curl_global_cleanup();

    return 0;
}
