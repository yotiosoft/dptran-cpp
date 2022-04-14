set DEST="%USERPROFILE%\dptran_bin"
set PROGRAM="dptran"

md %DEST% > NUL 2>&1
copy . %DEST% /Y
setx PATH=%PATH%;%DEST%
echo "インストール完了. ターミナルを再起動してください"
