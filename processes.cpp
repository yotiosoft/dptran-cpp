#include "processes.h"

// 言語コードを取得
bool get_lang_codes(map< string, vector<string> > &langs, string api_key, string type) {
    CURL *curl;

    // curlの初期設定
    if (!setup_curl(&curl)) {
        cleanup_curl(&curl);
        return 1;
    }

    string get_data;
    string post_data = "auth_key=" + api_key + "&type=" + type;
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

// 言語コードのチェック
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
int translate(string api_key, string str, string &translated_text, string source_lang_code, string target_lang_code) {
    CURL *curl;

    // curlの初期設定
    if (!setup_curl(&curl)) {
        cleanup_curl(&curl);
        return 1;
    }

    string get_data;
    string post_data;
    
    post_data = "auth_key=" + api_key + "&text=" + str + "&target_lang=" + target_lang_code;
    
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
