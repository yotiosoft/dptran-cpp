#include "connect.h"
#include "picojson/picojson.h"
#include "apikey.h"

// 言語コードを取得
bool get_lang_codes(map< string, vector<string> > &langs, string api_key,  string type);
// 言語コードのチェック
bool check_lang_code(map< string, vector<string> > langs, string lang_code, string& correct_code);
// 翻訳文を取得
int translate(string api_key, string str, string &translated_text, string source_lang_code, string target_lang_code);
