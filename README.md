# dptran

コマンドライン上でDeepL翻訳を実行するCLIツールです。

## インストール方法

### for Windows

準備中

### for macOS & Linux

下記のライブラリが必要です。  

- libcurl-devel

```bash
$ git clone https://github.com/YotioSoft/dptran.git
$ cd ./dptran
$ make
$ bash ./install.sh
```

上記のコマンドを実行後、端末を再起動することでdptranコマンドが利用可能になります。

## ご利用方法

### APIキーの設定

ご利用の前に、必ずDeepL APIキー（無料で取得可能）を取得して設定するようお願いいたします。

```bash
$ dptran -s key [取得したAPIキー]
```

### 通常モードで翻訳

```bash
$ dptran Hello
こんにちは
$ dptran -t FR Hello
Bonjour
```

``-f``オプションで原文の言語を、``-t``オプションで翻訳先の言語を指定できます。翻訳先の言語の指定を省略した場合は日本語に翻訳されます。言語コードについてはヘルプをご覧ください。  

```bash
$ dptran -h
```

### 対話モードで翻訳

```bash
$ dptran
> Hello

こんにちは
> Ich stehe jeden Tag um 7 Uhr auf.

毎日7時に起きています。
> Seriously, Hiro, you need to improve your English.

マジでヒロさん、英語力アップしてください。
> 今天玩儿得真开心！

今日は素晴らしい時間を過ごせました
> :q
```

複数の原文を対話形式で翻訳できます。``:q``で終了します。

### パイプラインモードで翻訳

例）manページの内容を日本語に翻訳  

```bash
$ man ls | col -b | dptran -p
```

``-p``オプションによりパイプラインモードに切り替わります。他のコマンドの実行結果を翻訳できます。

### ヘルプを表示

```bash
$ dptran -h
```

### 翻訳可能な残り文字数を表示

```bash
$ dptran -r
Character count:		16965
Character limit (per month):	500000
```

DeepL APIで翻訳可能な残りの文字数が表示できます。DeepL APIのフリープランの場合、1ヶ月あたり50万文字まで翻訳可能です。

## デフォルトの翻訳先言語の変更

既定では日本語（JA)に設定されています。  
``-s default_lang``で変更可能です。例えば、英語（EN）に変更するには以下のようにします。

```bash
$ dptran -s default_lang EN
```

## 設定のリセット

全設定をリセットします。  
注：APIキーも含めてリセットされます。再びご利用の場合は、もう一度APIキーを設定するようお願いいたします。  

```bash
$ dptran -s clear
```



## アンインストール方法

### for Windows

準備中

### for macOS & Linux

```bash
$ bash ./uninstall.sh
```

上記のコマンドを実行後、端末を再起動することでアンインストール完了です。