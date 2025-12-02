#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

// ANSI color codes
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"
#define RESET   "\033[0m"

struct string {
    char *ptr;
    size_t len;
};

void init_string(struct string *s) {
    s->len = 0;
    s->ptr = malloc(1);
    if (!s->ptr) exit(1);
    s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {
    size_t new_len = s->len + size * nmemb;
    s->ptr = realloc(s->ptr, new_len + 1);
    if (!s->ptr) return 0;
    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;
    return size * nmemb;
}

// Recursive function to print JSON keys/values with indentation and colors
void print_json(cJSON *item, int indent) {
    cJSON *child = NULL;
    for (child = item->child; child != NULL; child = child->next) {
        for (int i = 0; i < indent; i++) printf("  "); // indent

        if (cJSON_IsObject(child)) {
            printf(CYAN "%s:" RESET "\n", child->string);
            print_json(child, indent + 1);
        } else if (cJSON_IsArray(child)) {
            printf(CYAN "%s: [" RESET "\n", child->string);
            int size = cJSON_GetArraySize(child);
            for (int i = 0; i < size; i++) {
                cJSON *elem = cJSON_GetArrayItem(child, i);
                if (cJSON_IsObject(elem) || cJSON_IsArray(elem))
                    print_json(elem, indent + 1);
                else
                    printf("%*s" GREEN "%s" RESET "\n", (indent + 1) * 2, "", elem->valuestring ? elem->valuestring : "");
            }
            for (int i = 0; i < indent; i++) printf("  ");
            printf("]\n");
        } else {
            printf(YELLOW "%s" RESET ": " GREEN "%s" RESET "\n",
                   child->string,
                   child->valuestring ? child->valuestring : "");
        }
    }
}

// Detect public IP
char* detect_my_ip() {
    CURL *curl = curl_easy_init();
    if (!curl) return NULL;

    struct string s;
    init_string(&s);

    curl_easy_setopt(curl, CURLOPT_URL, "https://ident.me");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "C IP Info Client");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, RED "Failed to detect public IP: %s\n" RESET, curl_easy_strerror(res));
        free(s.ptr);
        return NULL;
    }

    for (size_t i = 0; i < s.len; i++) if (s.ptr[i] == '\n') s.ptr[i] = '\0';
    return s.ptr; // caller must free
}

// Fetch IP info, either colorized or raw JSON
void fetch_ip_info(const char *ip, int json_output) {
    CURL *curl = curl_easy_init();
    if (!curl) return;

    struct string s;
    init_string(&s);

    char url[128];
    snprintf(url, sizeof(url), "https://ipinfo.io/%s/json", ip);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "C IP Info Client");

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, RED "curl failed: %s\n" RESET, curl_easy_strerror(res));
    } else {
        if (json_output) {
            printf("%s\n", s.ptr); // raw JSON
        } else {
            cJSON *json = cJSON_Parse(s.ptr);
            if (json) {
                printf(MAGENTA "\n=== IP INFORMATION for %s ===\n" RESET, ip);
                print_json(json, 0);
                cJSON_Delete(json);
            } else {
                fprintf(stderr, RED "JSON parse error\n" RESET);
            }
        }
    }

    free(s.ptr);
    curl_easy_cleanup(curl);
}

// Help output
void print_help(const char *prog) {
    printf("Usage: %s [options] <IP1> [IP2 ...]\n", prog);
    printf("Options:\n");
    printf("  my_ip             Detect your public IP automatically\n");
    printf("  --json            Print raw JSON output instead of colorized format\n");
    printf("  -h, --help        Show this help message\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_help(argv[0]);
        return 1;
    }

    int json_output = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--json") == 0) {
            json_output = 1;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "my_ip") == 0) {
            char *myip = detect_my_ip();
            if (myip) {
                fetch_ip_info(myip, json_output);
                free(myip);
            }
        } else {
            fetch_ip_info(argv[i], json_output);
        }
    }

    return 0;
}
