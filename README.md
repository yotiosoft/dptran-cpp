# dptran

A tool to run DeepL translations on command line.  
To use dptran, you need to obtain your DeepL API key.  

**Note: This tool is still under development.**

## How to install?

### for Windows

comming soon..

### for macOS & Linux

To run, you need the following library:  

- libcurl-devel


To install dptran, run the following:

```bash
$ git clone https://github.com/YotioSoft/dptran.git
$ cd ./dptran
$ make
$ bash ./install.sh
```

After executing the above command, restart the terminal to make the dptran command available.  
For Apple-Chip macOS, please use the clang (Xcode) compiler to build because it can't be built with g++.

## How to use?

### Setting API key

Please be sure to get your DeepL API key (it's free) and set it up on dptran before using the service.

```bash
$ dptran -s key [API key]
```

### Translate in normal mode

You can specify the source language with the ``-f`` option and the target language with the ``-t`` option.  
If you omit the ``-destination language`` option, the translation will be done in Japanese. 

```bash
$ dptran Hello
こんにちは
$ dptran -t FR Hello
Bonjour
```


For more information about language codes, see help:  

```bash
$ dptran -h
```

### Translate in interactive mode

Multiple source texts can be translated interactively.  
Exit with ``:q``.

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

### Translate in pipeline mode

Example: Translate man page content  

```bash
$ man ls | col -b | dptran -p
```

Can be run in pipeline mode with the ``-p`` option.  
You can translate the execution result of other commands.

### Show help

```bash
$ dptran -h
```

### Displays the number of characters remaining to be translated

You can view the number of remaining characters that can be translated by DeepL API.  
DeepL API's free plan allows you to translate up to 500,000 characters per month.

```bash
$ dptran -r
Character count:		16965
Character limit (per month):	500000
```

## Change default target language

It is set to Japanese (JA) by default.  
You can change it with ``-s default_lang``.  
For example, to change it to English (EN), run the following:

```bash
$ dptran -s default_lang EN
```

## Reset settings

You can reset all settings.  
Note: The API key will be reset as well. If you wish to use dptran again, please set the API key again.  

```bash
$ dptran -s clear
```



## How to uninstall?

### for Windows

comming soon..

### for macOS & Linux

```bash
$ bash ./uninstall.sh
```

After executing the above command, reboot the terminal to complete uninstallation.
