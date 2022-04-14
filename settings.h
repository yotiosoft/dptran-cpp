#include "processes.h"
#include <fstream>

using namespace std;

#ifdef _WIN32
    #define DPTRAN_DIR      string(get_home_dir()) + "/dptran_bin/"
#else
    #define DPTRAN_DIR      string(getenv("HOME")) + "/dptran_bin/"
#endif
#define SETTING_FILE    "settings.json"

enum setting_type {
    CLEAR,
    KEY,
    DEFAULT_LANG
};

typedef struct {
    string key;
    string default_lang;
} settings_struct;

// 設定切り替え
int setting(setting_type type, int argc, char *argv[], int current_arg);

// 設定ファイル初期化
int settings_init();

// 設定ファイル読み込み
int load_settings();

// 設定ファイルの保存
int save_settings();

// キー設定
int set_key(int argc, char *argv[]);

// キーの読み込み
int get_key(string &api_key);

// 標準の翻訳先言語の設定
int set_default_lang(int argc, char *argv[]);

// 標準の翻訳先言語の取得
int get_default_lang(string &default_lang);

// 環境変数の取得
string get_home_dir();
int get_env_var(string env_name, string &ret);
