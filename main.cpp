#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <curl/curl.h>
#include "picojson/picojson.h"
#include "apikey.h"

std::map<std::string, std::string> source_langs;
std::map<std::string, std::string> target_langs;

bool check_lang_code(std::string lang_code) {
    //if (lang_code == "")
    return true;
}

int dialogue_mode(int argc, char *argv[]) {
    return 0;
}

int normal_mode(int argc, char *argv[]) {
    std::string lang_from;
    std::string lang_to;

    // 各引数をチェック
    for (int i=1; i<argc; i++) {
        // 1文字目が'-'ならオプション
        if (argv[i][0] == '-') {
            if (argv[i] == "-h" || argv[i] == "-help") {

            }
            else if (argv[i] == "-f" || argv[i] == "-from") {
                if (i+1 >= argc) {
                    std::cerr << "Error: -fromオプションの後ろに翻訳元言語を指定してください" << std::endl;
                    return 1;
                }
                if (argv[i+1][0] == '-') {
                    std::cerr << "Error: -fromオプションの後ろに翻訳元言語を指定してください" << std::endl;
                    return 1;
                }
                
                i++;
                lang_from = argv[i];
            }
            else if (argv[i] == "-t" || argv[i] == "-to") {
                if (i+1 >= argc) {
                    std::cerr << "Error: -fromオプションの後ろに翻訳元言語を指定してください" << std::endl;
                    return 1;
                }
                if (argv[i+1][0] == '-') {
                    std::cerr << "Error: -fromオプションの後ろに翻訳元言語を指定してください" << std::endl;
                    return 1;
                }
                
                i++;
                lang_from = argv[i];
            }
        }
    }

    return 0;
}

// curlの初期設定
bool setup_curl(CURL **curl) {
    std::cout << "setup for " << curl << std::endl;

    *curl = curl_easy_init();

    if (curl == nullptr) {
        std::cerr << "Error: failed to set up curl" << std::endl;
        return false;
    }

    return true;
}

// curlを解放
void cleanup_curl(CURL **curl) {
    std::cout << "cleanup for " << curl << std::endl;
    curl_easy_cleanup(*curl);
}

// curlで受信したときの動作
size_t curl_on_receive(char *ptr, size_t size, size_t nmemb, void *stream) {
    std::vector<char> *recv_buffer = (std::vector<char>*)stream;
    const size_t sizes = size * nmemb;
    recv_buffer->insert(recv_buffer->end(), (char*)ptr, (char*)ptr + sizes);
    return sizes;
}

// curlの接続設定
bool connect_curl(CURL **curl, std::string url, std::string post_data, std::string &res_string) {
    std::cout << "connect for " << curl << " to " << url << std::endl;

    std::vector<char> res_data;
    const char *post_data_c = post_data.c_str();
    const char *url_c = url.c_str();

    curl_easy_setopt(*curl, CURLOPT_URL, url_c);
    curl_easy_setopt(*curl, CURLOPT_POST, 1);
    curl_easy_setopt(*curl, CURLOPT_POSTFIELDS, post_data_c);
    curl_easy_setopt(*curl, CURLOPT_POSTFIELDSIZE, strlen(post_data_c));
    curl_easy_setopt(*curl, CURLOPT_SSL_VERIFYPEER, 1);
    curl_easy_setopt(*curl, CURLOPT_WRITEFUNCTION, curl_on_receive);
    curl_easy_setopt(*curl, CURLOPT_WRITEDATA, &res_data);

    std::cout << "connect B" << std::endl;

    // 通信開始
    CURLcode res = curl_easy_perform(*curl);
    if (res != CURLE_OK) {
        std::cerr << "Error: curl_easy_perform failed: " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(*curl);
        return false;
    }

    cleanup_curl(curl);
    std::cout << "done." << std::endl;

    res_string = std::string(res_data.data());
    return true;
}

// 言語コードを取得
bool get_lang_codes(std::map<std::string, std::string> &langs, std::string type) {
    CURL *curl;

    // curlの初期設定
    if (!setup_curl(&curl)) {
        cleanup_curl(&curl);
        return 1;
    }

    std::string get_data;
    std::string post_data = "auth_key=" + API_KEY + "&type=" + type;
    std::cout << "before" << std::endl;
    if (!connect_curl(&curl, "https://api-free.deepl.com/v2/languages", post_data, get_data)) {
        std::cerr << "Error: 言語コードの取得に失敗しました" << std::endl;
        cleanup_curl(&curl);
        return false;
    }
    std::cout << "after" << std::endl;

    picojson::value v;
    std::string err = picojson::parse(v, get_data);
    if (!err.empty()) {
        std::cerr << "Error: picojson error - " << err << std::endl;
        return false;
    }

    std::cout << v << std::endl;
    
    picojson::array& ary = v.get<picojson::array>();
    for (picojson::value& el : ary) {
        picojson::object& el_obj = el.get<picojson::object>();
        
        std::string code = el_obj["language"].get<std::string>();
        std::string name = el_obj["name"].get<std::string>();
        
        langs[code] = name;
    }
    
    return true;
}

// 翻訳文を取得


int main(int argc, char *argv[]) {
    // 言語コードを取得
    if (!get_lang_codes(source_langs, "source")) {
        return 1;
    }
    if (!get_lang_codes(target_langs, "target")) {
        return 1;
    }
    
    std::cout << "source:" << source_langs.size() << std::endl;
    for (auto e : source_langs) {
        std::cout << e.first << " : " << e.second << std::endl;
    }
    std::cout << "target:" << source_langs.size() << std::endl;
    for (auto e : target_langs) {
        std::cout << e.first << " : " << e.second << std::endl;
    }

    if (argc == 1) {
        // 対話モード
        return dialogue_mode(argc, argv);
    }
    else if (argc > 2) {
        // 通常モード
        return normal_mode(argc, argv);
    }
}
