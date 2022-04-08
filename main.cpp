#include <stdio.h>
#include <iostream>
#include <string>
#include <cstring>
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
    cout << "\r";

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
        cleanup_curl(curl);
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
    cout << "接続中.." << flush;
    if (!connect_curl(&curl, "https://api-free.deepl.com/v2/languages", post_data, get_data)) {
        cout << "\r" << string(8, ' ');
        cout << "\r";
        cerr << "Error: 言語コードの取得に失敗しました" << endl;
        cleanup_curl(&curl);
        return false;
    }
    cout << "\r" << string(8, ' ');
    cout << "\r";

    picojson::value v;
    string err = picojson::parse(v, get_data);
    if (!err.empty()) {
        cerr << "Error: picojson error - " << err << endl;
        return false;
    }
    
    picojson::array& ary = v.get<picojson::array>();
    for (picojson::value& el : ary) {
        picojson::object& el_obj = el.get<picojson::object>();
        
        string code = el_obj["language"].get<string>();
        string name = el_obj["name"].get<string>();
        
        langs[code].push_back(name);
    }
    
    return true;
}

int get_all_langs() {
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

    return 0;
}

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

// 翻訳文を取得
int translate(string str, string &translated_text, string source_lang_code, string target_lang_code) {
    CURL *curl;

    // curlの初期設定
    if (!setup_curl(&curl)) {
        cleanup_curl(&curl);
        return 1;
    }

    string get_data;
    string post_data;
    
    if (target_lang_code != "") {
        post_data = "auth_key=" + API_KEY + "&text=" + str + "&target_lang=" + target_lang_code;
    }
    else {
        cerr << "Error: 翻訳先の指定（-tオプション）は入力必須です" << endl;
        return 1;
    }
    if (source_lang_code != "") {
        post_data += "&source_lang=" + source_lang_code;
    }
    
    cout << "翻訳中.." << flush;
    if (!connect_curl(&curl, "https://api-free.deepl.com/v2/translate", post_data, get_data)) {
        cout << "\r" << string(8, ' ');
        cout << "\r";
        cerr << "Error: 翻訳結果の取得に失敗しました" << endl;
        cleanup_curl(&curl);
        return false;
    }
    cout << "\r" << string(8, ' ');
    cout << "\r";

    picojson::value v;
    string err = picojson::parse(v, get_data);
    if (!err.empty()) {
        cerr << "Error: picojson error - " << err << endl;
        return false;
    }

    translated_text = "";
    picojson::object& v_obj = v.get<picojson::object>();
    picojson::array& v_array = v_obj["translations"].get<picojson::array>();
    for (auto v_element : v_array) {
        picojson::object& v_element_obj = v_element.get<picojson::object>();
        translated_text += v_element_obj["text"].get<string>();
    }
    
    return true;
}

int dialogue_mode(string source_lang, string target_lang) {
    bool exit = false;
    
    while (1) {
        string input_text = "";
        string input_text_buf = "";
        string translated_text = "";
        
        // 入力待ち
        cout << "> ";

        //scanf("%[^\n]", input_text_buf);
        //input_text = string(input_text_buf);
        
        int w_count = 0;
        while (getline(std::cin, input_text_buf)) {
            w_count++;

            if (input_text_buf[0] == '\0') {
                break;
            }
            if (input_text.length() > 0)
                input_text += '\n';
            input_text += input_text_buf;

            // 終了コード
            if (input_text == ":q" || input_text == ":quit") {
                return 0;
            }
        }
        /*
        while (fgets(input_text_buf, 1024, stdin) != NULL) {
            input_text += string(input_text_buf);
        };*/

        if (w_count == 0) {
            cout << endl;
            return 0;
        }

        if (input_text.length() == 0)
            continue;

        //std::cout << input_text << std::endl;

        // 翻訳
        if (translate(input_text, translated_text, source_lang, target_lang)) {
            std::cout << translated_text << std::endl;
        }
    }

    return 1;
}

int pipe_mode(string source_lang, string target_lang) {
    bool exit = false;
    
    while (1) {
        string input_text = "";
        string input_text_buf = "";
        string translated_text = "";

        //scanf("%[^\n]", input_text_buf);
        //input_text = string(input_text_buf);
        
        int w_count = 0;
        while (getline(std::cin, input_text_buf)) {
            w_count ++;

            if (input_text_buf[0] == '\0') {
                break;
            }
            if (input_text.length() > 0)
                input_text += '\n';
            input_text += input_text_buf;

            // 終了コード
            if (input_text == ":q" || input_text == ":quit") {
                return 0;
            }
        }
        
        if (w_count == 0)
            return 0;

        if (input_text.length() == 0)
            return 0;

        //std::cout << input_text << std::endl;

        // 翻訳
        if (translate(input_text, translated_text, source_lang, target_lang)) {
            std::cout << translated_text << std::endl;
        }
    }

    return 1;
}

int help() {
    cout << "[コマンドオプション]" << endl;
    cout << "\t-f or -from\t"       << "原文の言語を指定（未指定の場合、自動検出）" << endl;
    cout << "\t-t or -to\t"         << "翻訳先の言語を指定（翻訳実行時は指定必須）" << endl;
    cout << "\t-p or -pipe\t"       << "パイプラインモードで実行" << endl;
    cout << "\t-h or -help\t"       << "ヘルプを表示" << endl;
    cout << "\t-v or -version\t"    << "バージョン情報を表示" << endl;
    cout << "\t-r or -remain\t"     << "DeepLへ送信可能な残りの文字数を表示" << endl;
    cout << endl;

    cout << "[機能]" << endl;
    cout << "<通常モード>" << endl;
    cout << "\t1回の実行につき1文を翻訳します。" << endl
     << "\tコマンド実行時に入力した原文が翻訳されます。" << endl;
    cout << "<対話モード>" << endl;
    cout << "\t複数の文を対話形式で翻訳します。複数行に渡る原文が翻訳可能です。" << endl
     << "\tコマンド実行時に翻訳文を入力しなかった場合、対話モードで起動します。" << endl
     << "\t※原文を入力し終えたら、2回Enterキー（Returnキー）を押下してください。" << endl
     << "\t※「:q」または「:quit」を入力すると終了します。" << endl;
    cout << "<パイプラインモード>" << endl;
    cout << "\t他のコマンドの実行結果をパイプラインで受け取り翻訳できます。" << endl
     << "\t-pオプションで利用可能です。" << endl;
    cout << endl;

    cout << "[言語コード]" << endl;
    cout << "<Source Languages>" << endl;
    for (pair< string, vector<string> > e : source_langs) {
        cout << "\t" << e.first << " : " << e.second[0] << endl;
    }
    
    cout << "<Target Languages>" << endl;
    for (pair< string, vector<string> > e : target_langs) {
        cout << "\t" << e.first << " : " << e.second[0] << endl;
    }
    cout << "\t" << "EN : " << "English (British)" << endl;
    cout << "\t" << "PT : " << "Portuguese (European)" << endl;
    cout << endl;

    cout << "[Usage Example]" << endl;
    cout << "1) 通常モードで日本語の原文を英語（イギリス英語）に変換" << endl;
    cout << "\tdptran -f JA -t EN こんにちは" << endl;
    cout << "2) 通常モードで英語の原文を日本語に変換" << endl;
    cout << "\tdptran -f EN -t JA Hello" << endl;
    cout << "3) 通常モードで原文の言語を指定せずに日本語に変換" << endl;
    cout << "\tdptran -t JA Hello" << endl;
    cout << "4) 対話モードで原文の言語を指定せずに日本語に変換" << endl;
    cout << "\tdptran -t JA" << endl;

    return 0;
}

int version() {
    cout << "dptran ver.1.0" << endl;
    cout << "(c) YotioSoft 2022" << endl;
    cout << endl;
    cout << "This application uses DeepL API v2 for translations." << endl;
    cout << "Translations are provided by DeepL." << endl;

    return 0;
}

int remain() {
    CURL *curl;

    // curlの初期設定
    if (!setup_curl(&curl)) {
        cleanup_curl(&curl);
        return 1;
    }

    string get_data;
    string post_data = "auth_key=" + API_KEY;
    cout << "接続中.." << flush;
    if (!connect_curl(&curl, "https://api-free.deepl.com/v2/usage", post_data, get_data)) {
        cout << "\r" << string(8, ' ');
        cout << "\r";
        cerr << "Error: データの取得に失敗しました" << endl;
        cleanup_curl(&curl);
        return false;
    }
    cout << "\r" << string(8, ' ');
    cout << "\r";

    picojson::value v;
    string err = picojson::parse(v, get_data);
    if (!err.empty()) {
        cerr << "Error: picojson error - " << err << endl;
        return false;
    }

    picojson::object& obj = v.get<picojson::object>();
    cout << "Character count:\t\t" << int(obj["character_count"].get<double>()) << endl;
    cout << "Character limit (per month):\t" << int(obj["character_limit"].get<double>()) << endl;
    
    return true;
}

int parse(int argc, char *argv[]) {
    string lang_from = "";
    string lang_to = "";

    bool to_code_exists = false;
    bool text_exists = false;
    bool got_langs = false;
    bool pipe = false;
    string source_text;

    // 各引数をチェック
    for (int i=1; i<argc; i++) {
        string str_argv = string(argv[i]);

        // 1文字目が'-'ならオプション
        if (argv[i][0] == '-') {
            if (str_argv == "-f" || str_argv == "-from") {
                if (i+1 == argc) {
                    cerr << "Error: -fromオプションの後ろに翻訳元言語を指定してください" << endl;
                    return 1;
                }
                if (argv[i+1][0] == '-') {
                    cerr << "Error: -fromオプションの後ろに翻訳元言語を指定してください" << endl;
                    return 1;
                }

                // 言語コードを取得
                if (!got_langs) {
                    if (get_all_langs() > 0) {
                        return 1;
                    }
                    got_langs = true;
                }
                
                i++;
                string str_argv_next = string(argv[i]);
                string correct_lang_code;
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

                // 言語コードを取得
                if (!got_langs) {
                    if (get_all_langs() > 0) {
                        return 1;
                    }
                    got_langs = true;
                }
                
                i++;
                string str_argv_next = string(argv[i]);
                string correct_lang_code;
                if (!check_lang_code(target_langs, str_argv_next, correct_lang_code)) {
                    cerr << "Error: " << str_argv_next << ": 言語コードが無効です" << endl;
                    return 1;
                }
                lang_to = correct_lang_code;

                to_code_exists = true;
            }
            else if (str_argv == "-p" || str_argv == "-pipe") {
                pipe = true;
            }
            else if (str_argv == "-h" || str_argv == "-help") {
                // 言語コードを取得
                if (!got_langs) {
                    if (get_all_langs() > 0) {
                        return 1;
                    }
                    got_langs = true;
                }
                return help();
            }
            else if (str_argv == "-v" || str_argv == "-version") {
                return version();
            }
            else if (str_argv == "-r" || str_argv == "-remain") {
                return remain();
            }
        }
        // それ以外は翻訳文として扱う
        else {
            if (source_text.length() > 0)
                source_text += " ";
            source_text += argv[i];
            text_exists = true;
        }
    }

    if (!to_code_exists) {
        cerr << "Error: 翻訳先の指定（-tオプション）は入力必須です" << endl;
        return 1;
    }

     // -pオプションの場合はパイプモードへ
    if (pipe) {
        return pipe_mode(lang_from, lang_to);
    }

    // 原文がない場合は対話モードへ
    if (!text_exists) {
        return dialogue_mode(lang_from, lang_to);
    }

    string translated_text;
    if (translate(source_text, translated_text, lang_from, lang_to)) {
        std::cout << translated_text << std::endl;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    return parse(argc, argv);
}
