#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include "apikey.h"

CURL *curl;

bool check_lang_code(std::string lang_code) {
    if (lang_code == "")
}

int dialogue_mode(int argc, char *argv[]) {

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
}

// curlの初期設定
bool setup_curl() {
    curl = curl_easy_init();

    if (curl == nullptr) {
        std::cerr << "Error: failed to set up curl" << std::endl;
        return false;
    }

    std::vector<char> res_data;
    curl_easy_setopt(curl, CURLOPT_URL, "")

    return true;
}

int main(int argc, char *argv[]) {
    // curlの初期設定
    if (!setup_curl()) {
        return 1;
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
