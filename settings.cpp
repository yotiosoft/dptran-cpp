#include "settings.h"

settings_struct settings_s;

// 設定切り替え
int setting(setting_type type, int argc, char *argv[], int current_arg) {
    switch(type) {
        case CLEAR:
            settings_init();
            return 0;
        case KEY:
            argc -= current_arg + 1;
            argv = argv + current_arg + 1;
            return set_key(argc, argv);
        case DEFAULT_LANG:
            argc -= current_arg + 1;
            argv = argv + current_arg + 1;
            return set_default_lang(argc, argv);
    }
    return 1;
}

// 設定ファイル初期化
int settings_init() {
    settings_s.key = "";
    settings_s.default_lang = "JA";

    return save_settings();
}

// 設定ファイル読み込み
int load_settings() {
    // 設定ファイルがない場合は新規作成
    int ret;
    std::ifstream ifs(DPTRAN_DIR + SETTING_FILE);
    if (!ifs.is_open()) {
        ifs.close();
        return settings_init();
    }
    const string settings_str((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
    ifs.close();

    // jsonをパースして読み込み
    picojson::value jv;
    const string err = picojson::parse(jv, settings_str);
    if (!err.empty()) {
        cerr << "Error: settings: 設定ファイルのパースに失敗しました" << endl;
        return 1;
    }

    try {
        picojson::object &obj = jv.get<picojson::object>();
        settings_s.key = obj["key"].get<string>();
        settings_s.default_lang = obj["default_lang"].get<string>();
    }
    catch(int err) {
        cerr << "Error " << err << ": settings: 設定ファイルの項目の読み込みに失敗しました。設定を初期化します。" << endl;
        settings_init();
        return err;
    }

    return 0;
}

// 設定ファイルの保存
int save_settings() {
    picojson::object obj;
    obj.insert(make_pair("key", picojson::value(settings_s.key)));
    obj.insert(make_pair("default_lang", picojson::value(settings_s.default_lang)));

    ofstream ofs;
    ofs.open(DPTRAN_DIR + SETTING_FILE, ios::out);
    if (ofs.fail()) {
        cerr << "Error: 設定ファイルを保存できません" << endl;
        return 1;
    }
    ofs << picojson::value(obj) << endl;
    ofs.close();

    return 0;
}

// キー設定
int set_key(int argc, char *argv[]) {
    if (load_settings() != 0) {
        return 1;
    }
    
    if (argc == 0) {
        cerr << "Error: APIキーを指定してください" << endl;
        return 1;
    }

    settings_s.key = string(argv[0]);

    return save_settings();
}

// キーの読み込み
int get_key(string &api_key) {
    if (load_settings() != 0) {
        return 1;
    }

    if (settings_s.key.length() == 0) {
        cerr << "Error: ご利用前に、DeepL APIのAPIキーを取得して設定してください。無料で取得できます。" << endl
         << "DeepL API:\thttps://www.deepl.com/ja/docs-api/" << endl
         << "設定方法:\tdptran -s key <APIキー>" << endl;
        return 1;
    }

    api_key = settings_s.key;
    return 0;
}

// 標準の翻訳先言語の設定
int set_default_lang(int argc, char *argv[]) {
    if (argc == 0) {
        cerr << "Error: 翻訳先の言語を指定してください" << endl;
        return 1;
    }

    if (load_settings() != 0) {
        return 1;
    }

    string api_key;
    if (get_key(api_key) != 0) {
        return 1;
    }

    // 言語コードを取得
    map< string, vector<string> > s_target_langs;
    if (!get_lang_codes(s_target_langs, api_key, "target")) {
        return 1;
    }
    // target変換用の言語コードも用意
    if (s_target_langs.count("EN-GB")) {
        s_target_langs["EN-GB"].push_back("English");
        s_target_langs["EN-GB"].push_back("EN");
    }
    if (s_target_langs.count("PT-PT")) {
        s_target_langs["PT-PT"].push_back("Portuguese");
        s_target_langs["EN-GB"].push_back("PT");
    }

    // 言語コードのチェック
    string lang(argv[0]);
    string correct_lang_code;
    if (!check_lang_code(s_target_langs, lang, correct_lang_code)) {
        cerr << "Error: " << lang << ": 言語コードが無効です" << endl;
        return 1;
    }

    settings_s.default_lang = correct_lang_code;

    return save_settings();
}

// 標準の翻訳先言語の取得
int get_default_lang(string &default_lang) {
    if (load_settings() != 0) {
        return 1;
    }

    default_lang = settings_s.default_lang;

    return 0;
}
