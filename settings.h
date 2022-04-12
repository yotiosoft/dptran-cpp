#include "header.h"
#include "picojson/picojson.h"
#include <fstream>

using namespace std;

#define SETTING_FILE    "settings.json"

enum setting_type {
    CLEAR,
    KEY
};

typedef struct {
    string key;
} settings_struct;

// 設定切り替え
int setting(setting_type type, int argc, char *argv[]);

// 設定ファイル初期化
void settings_init();

// 設定ファイル読み込み
int load_settings(const string json_str);

// 設定ファイルの保存
int save_settings();

// キー設定
int set_key(int argc, char *argv[]);
