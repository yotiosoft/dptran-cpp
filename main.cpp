#include "settings.h"

using namespace std;

map< string, vector<string> > source_langs;
map< string, vector<string> > target_langs;

int get_all_langs() {
    // APIキー取得
    string api_key;
    if (get_key(api_key) != 0) {
        return 1;
    }

    // 言語コードを取得
    if (!get_lang_codes(source_langs, api_key, "source")) {
        return 1;
    }
    if (!get_lang_codes(target_langs, api_key, "target")) {
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

int dialogue_mode(string source_lang, string target_lang) {
    bool exit = false;

    // APIキー取得
    string api_key;
    if (get_key(api_key) != 0) {
        return 1;
    }
    
    // 対話
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

        if (w_count == 0) {
            cout << endl;
            return 0;
        }

        if (input_text.length() == 0)
            continue;

        // 翻訳
        if (translate(api_key, input_text, translated_text, source_lang, target_lang)) {
            std::cout << translated_text << std::endl;
        }
    }

    return 1;
}

int pipe_mode(string source_lang, string target_lang) {
    bool exit = false;

    // APIキー取得
    string api_key;
    if (get_key(api_key) != 0) {
        return 1;
    }
    
    // パイプから読み込み＆翻訳
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

        // 翻訳
        if (translate(api_key, input_text, translated_text, source_lang, target_lang)) {
            std::cout << translated_text << std::endl;
        }
    }

    return 1;
}

int help() {
    cout << "[コマンドオプション]" << endl;
    cout << "\t-f or -from\t"       << "原文の言語を指定（規定：自動検出）" << endl;
    cout << "\t-t or -to\t"         << "翻訳先の言語を指定（規定：日本語）" << endl;
    cout << "\t-p or -pipe\t"       << "パイプラインモードで実行" << endl;
    cout << "\t-s or -setting\t"    << "設定" << endl;
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

    cout << "[設定項目]" << endl;
    cout << "\t-s key\t\t"          << "DeepL APIキーの設定" << endl;
    cout << "\t-s default_lang\t"   << "標準の翻訳先言語の変更（既定: JA）" << endl;
    cout << "\t-s clear\t"          << "設定の初期化" << endl;
    cout << endl;

    cout << "[Examples]" << endl;
    cout << "1) 通常モードで日本語の原文を英語（イギリス英語）に翻訳" << endl;
    cout << "\tdptran -f JA -t EN こんにちは" << endl;
    cout << "2) 通常モードで英語の原文を日本語に翻訳" << endl;
    cout << "\tdptran -f EN Hello" << endl;
    cout << "3) 通常モードで原文の言語を指定せずに日本語に翻訳" << endl;
    cout << "\tdptran Hello" << endl;
    cout << "4) 対話モードで原文の言語を指定せずに日本語に翻訳" << endl;
    cout << "\tdptran" << endl;
    cout << "5) パイプラインモードによりcatコマンドで読み取った\"file.txt\"の内容を日本語に翻訳" << endl;
    cout << "\tcat file.txt | dptran -p" << endl;
    cout << "6) パイプラインモードによりmanページの内容を日本語に翻訳" << endl;
    cout << "\tman ls | col -b | dptran -p" << endl;

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

    string api_key;
    if (get_key(api_key) != 0) {
        return 1;
    }

    string get_data;
    string post_data = "auth_key=" + api_key;
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
            else if (str_argv == "-s" || str_argv == "-setting") {
                if (i+1 >= argc) {
                    cerr << "Error: 設定項目を指定してください" << endl;
                    return 1;
                }
                if (argv[i+1][0] == '-') {
                    cerr << "Error: 設定項目を指定してください" << endl;
                    return 1;
                }

                i++;
                string str_argv_next = string(argv[i]);
                if (str_argv_next == "key") {
                    if (i+1 >= argc) {
                        cerr << "Error: 設定内容を指定してください" << endl;
                        return 1;
                    }
                    return setting(KEY, argc, argv, i);
                }
                else if (str_argv_next == "default_lang") {
                    if (i+1 >= argc) {
                        cerr << "Error: 設定内容を指定してください" << endl;
                        return 1;
                    }
                    return setting(DEFAULT_LANG, argc, argv, i);
                }
                else if (str_argv_next == "clear") {
                    return setting(CLEAR, argc, argv, i);
                }

                cerr << "Error: " << str_argv_next << ": 設定項目名が無効です" << endl;
                return 1;
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
            else {
                cout << "Error: " << str_argv << ": オプションが無効です" << endl;
                return 1;
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

    // 翻訳先言語が指定されていない場合、翻訳先言語を取得
    if (!to_code_exists) {
        string default_lang;
        if (get_default_lang(default_lang) != 0) {
            return 1;
        }
        lang_to = default_lang;
    }

     // -pオプションの場合はパイプモードへ
    if (pipe) {
        return pipe_mode(lang_from, lang_to);
    }

    // 原文がない場合は対話モードへ
    if (!text_exists) {
        return dialogue_mode(lang_from, lang_to);
    }

    // 通常モード
    // APIキー取得
    string api_key;
    if (get_key(api_key) != 0) {
        return 1;
    }

    // 翻訳
    string translated_text;
    if (translate(api_key, source_text, translated_text, lang_from, lang_to)) {
        std::cout << translated_text << std::endl;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    return parse(argc, argv);
}
