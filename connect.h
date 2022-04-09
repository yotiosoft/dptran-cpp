#include "header.h"

using namespace std;

// curlの初期設定
bool setup_curl(CURL **curl);
// curlを解放
void cleanup_curl(CURL **curl);
// curlで受信したときの動作
size_t curl_on_receive(char *ptr, size_t size, size_t nmemb, void *stream);
// curlの接続設定
bool connect_curl(CURL **curl, string url, string post_data, string &res_string);