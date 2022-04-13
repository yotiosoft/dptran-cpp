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
    }
}

// 設定ファイル初期化
int settings_init() {
    settings_s.key = "";

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
    }
    catch(int err) {
        cerr << "Error: settings: 設定ファイルの項目の読み込みに失敗しました" << endl;
        return err;
    }

    return 0;
}

// 設定ファイルの保存
int save_settings() {
    picojson::object obj;
    obj.insert(make_pair("key", picojson::value(settings_s.key)));

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
        cerr << "Error: ご利用前に、DeepL APIのAPIキーを取得して設定してください。無料で取得可能です。" << endl
         << "DeepL API:\thttps://www.deepl.com/ja/docs-api/" << endl
         << "設定方法:\tdptran -s key <APIキー>" << endl;
        return 1;
    }

    api_key = settings_s.key;
    return 0;
}
