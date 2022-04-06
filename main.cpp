#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <map>
#include <curl/curl.h>
#include "picojson/picojson.h"
#include "apikey.h"

using namespace std;

map< string, vector<string> > source_langs;
map< string, vector<string> > target_langs;

bool check_lang_code(map< string, vector<string> > langs, string lang_code, string& correct_code) {
    // 大文字に変換
    transform(lang_code.begin(), lang_code.end(), lang_code.begin(), ::toupper);
    
    // キーから検索
    auto itr = langs.begin();
    if ((itr = langs.find(lang_code)) != langs.end()) {
        correct_code = lang_code;
        return true;
    }

    // 言語名から検索
    transform(lang_code.begin(), lang_code.end(), lang_code.begin(), ::tolower);
    for (pair< string, vector<string> > lang : langs) {
        for (int i=0; i<lang.second.size(); i++) {
            string lang_name = lang.second[i];
            transform(lang_name.begin(), lang_name.end(), lang_name.begin(), ::tolower);

            if (lang_code == lang_name) {
                correct_code = lang.first;
                return true;
            }
        }
    }

    return false;
}

int dialogue_mode(int argc, char *argv[]) {
    return 0;
}

int normal_mode(int argc, char *argv[]) {
    string lang_from;
    string lang_to;

    cout << "argc=" << argc << endl;

    // 各引数をチェック
    for (int i=1; i<argc; i++) {
        string str_argv = string(argv[i]);

        // 1文字目が'-'ならオプション
        if (argv[i][0] == '-') {
            if (str_argv == "-h" || str_argv == "-help") {

            }
            else if (str_argv == "-f" || str_argv == "-from") {
                if (i+1 == argc) {
                    cerr << "Error: -fromオプションの後ろに翻訳元言語を指定してください" << endl;
                    return 1;
                }
                if (argv[i+1][0] == '-') {
                    cerr << "Error: -fromオプションの後ろに翻訳元言語を指定してください" << endl;
                    return 1;
                }
                
                i++;
                string str_argv_next = string(argv[i]);
                string correct_lang_code;
                cout << "argv: " << str_argv_next << endl;
                if (!check_lang_code(source_langs, str_argv_next, correct_lang_code)) {
                    cerr << "Error: " << str_argv_next << ": 言語コードが無効です" << endl;
                    return 1;
                }
                lang_from = correct_lang_code;
            }
            else if (str_argv == "-t" || str_argv == "-to") {
                if (i+1 >= argc) {
                    cerr << "Error: -fromオプションの後ろに翻訳元言語を指定してください" << endl;
                    return 1;
                }
                if (argv[i+1][0] == '-') {
                    cerr << "Error: -fromオプションの後ろに翻訳元言語を指定してください" << endl;
                    return 1;
                }
                
                i++;
                string str_argv_next = string(argv[i]);
                string correct_lang_code;
                cout << "argv: " << str_argv_next << endl;
                if (!check_lang_code(target_langs, str_argv_next, correct_lang_code)) {
                    cerr << "Error: " << str_argv_next << ": 言語コードが無効です" << endl;
                    return 1;
                }
                lang_to = correct_lang_code;
            }
            else {
                cout << str_argv << endl;
            }
        }
    }

    cout << "transform from " << lang_from << " to " << lang_to << endl;

    return 0;
}

// curlの初期設定
bool setup_curl(CURL **curl) {
    *curl = curl_easy_init();

    if (curl == nullptr) {
        cerr << "Error: failed to set up curl" << endl;
        return false;
    }

    return true;
}

// curlを解放
void cleanup_curl(CURL **curl) {
    cout << "cleanup for " << curl << endl;
    curl_easy_cleanup(*curl);
}

// curlで受信したときの動作
size_t curl_on_receive(char *ptr, size_t size, size_t nmemb, void *stream) {
    vector<char> *recv_buffer = (vector<char>*)stream;
    const size_t sizes = size * nmemb;
    recv_buffer->insert(recv_buffer->end(), (char*)ptr, (char*)ptr + sizes);
    return sizes;
}

// curlの接続設定
bool connect_curl(CURL **curl, string url, string post_data, string &res_string) {
    vector<char> res_data;
    const char *post_data_c = post_data.c_str();
    const char *url_c = url.c_str();

    curl_easy_setopt(*curl, CURLOPT_URL, url_c);
    curl_easy_setopt(*curl, CURLOPT_POST, 1);
    curl_easy_setopt(*curl, CURLOPT_POSTFIELDS, post_data_c);
    curl_easy_setopt(*curl, CURLOPT_POSTFIELDSIZE, strlen(post_data_c));
    curl_easy_setopt(*curl, CURLOPT_SSL_VERIFYPEER, 1);
    curl_easy_setopt(*curl, CURLOPT_WRITEFUNCTION, curl_on_receive);
    curl_easy_setopt(*curl, CURLOPT_WRITEDATA, &res_data);

    // 通信開始
    CURLcode res = curl_easy_perform(*curl);
    if (res != CURLE_OK) {
        cerr << "Error: curl_easy_perform failed: " << curl_easy_strerror(res) << endl;
        curl_easy_cleanup(*curl);
        return false;
    }

    cleanup_curl(curl);

    res_string = string(res_data.data());
    return true;
}

// 言語コードを取得
bool get_lang_codes(map< string, vector<string> > &langs, string type) {
    CURL *curl;

    // curlの初期設定
    if (!setup_curl(&curl)) {
        cleanup_curl(&curl);
        return 1;
    }

    string get_data;
    string post_data = "auth_key=" + API_KEY + "&type=" + type;
    cout << "before" << endl;
    if (!connect_curl(&curl, "https://api-free.deepl.com/v2/languages", post_data, get_data)) {
        cerr << "Error: 言語コードの取得に失敗しました" << endl;
        cleanup_curl(&curl);
        return false;
    }
    cout << "after" << endl;

    picojson::value v;
    string err = picojson::parse(v, get_data);
    if (!err.empty()) {
        cerr << "Error: picojson error - " << err << endl;
        return false;
    }

    cout << v << endl;
    
    picojson::array& ary = v.get<picojson::array>();
    for (picojson::value& el : ary) {
        picojson::object& el_obj = el.get<picojson::object>();
        
        string code = el_obj["language"].get<string>();
        string name = el_obj["name"].get<string>();
        
        langs[code].push_back(name);
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

    // target変換用の言語コードも用意
    if (target_langs.count("EN-GB")) {
        target_langs["EN-GB"].push_back("English");
        target_langs["EN-GB"].push_back("EN");
    }
    if (target_langs.count("PT-PT")) {
        target_langs["PT-PT"].push_back("Portuguese");
        target_langs["EN-GB"].push_back("PT");
    }
    
    cout << "source:" << source_langs.size() << endl;
    for (pair< string, vector<string> > e : source_langs) {
        for (string estr : e.second) {
            cout << e.first << " : " << estr << endl;
        }
    }
    cout << "target:" << target_langs.size() << endl;
    for (pair< string, vector<string> > e : target_langs) {
        for (string estr : e.second) {
            cout << e.first << " : " << estr << endl;
        }
    }

    string code;
    bool b = check_lang_code(source_langs, "JA", code);
    cout << "JA: " << b << " : " << code << endl;

    b = check_lang_code(source_langs, "Japanese", code);
    cout << "Japanese: " << b << " : " << code << endl;

    b = check_lang_code(source_langs, "JAPANESE", code);
    cout << "JAPANESE: " << b << " : " << code << endl;

    b = check_lang_code(source_langs, "JApaNese", code);
    cout << "JApaNese: " << b << " : " << code << endl;

    if (argc == 1) {
        // 対話モード
        return dialogue_mode(argc, argv);
    }
    else if (argc > 2) {
        // 通常モード
        return normal_mode(argc, argv);
    }
}
