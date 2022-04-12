#include "settings.h"

settings_struct settings_s;

// 設定切り替え
int setting(setting_type type, int argc, char *argv[]) {
    // 設定ファイル読み込み
    int ret;
    std::ifstream ifs(SETTING_FILE);
    if (!ifs.is_open()) {
        ifs.close();
        settings_init();
    }
    else {
        const string settings_str((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
        ifs.close();
        if ((ret = load_settings(settings_str)) != 0) {
            return ret;
        }
    }

    switch(type) {
        case CLEAR:
            settings_init();
            return 0;
        case KEY:
            return set_key(argc, argv);
    }
}

// 設定ファイル初期化
void settings_init() {
    settings_s.key = "";
}

// 設定ファイル読み込み
int load_settings(const string json_str) {
    picojson::value jv;
    const string err = picojson::parse(jv, json_str);
    if (!err.empty()) {
        cerr << "Error: settings: 設定ファイルの読み込みに失敗しました" << endl;
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
    ofs.open(SETTING_FILE, ios::out);
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
