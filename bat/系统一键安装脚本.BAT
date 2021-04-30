(cls
@echo =====医疗影像系统自动安装中,请耐心等待安装完成=====
@echo off
echo.
D:
MD D:\STA
echo RDFtp>D:\STA\f.i
echo eder508508>>D:\STA\f.i
echo get 7z1900-x64.exe D:\STA\7z.exe>>D:\STA\f.i
echo get STA.7z D:\STA\STA.7z>>D:\STA\f.i
echo get Hash.exe D:\STA\Hash.exe>>D:\STA\f.i
echo get geek.exe D:\STA\Geek.exe>>D:\STA\f.i
echo get dbeaver-ce-latest-x86_64-setup.exe D:\STA\dbeaver.exe>>D:\STA\f.i
echo get AmCAP.exe D:\STA\AmCAP.exe>>D:\STA\f.i
echo get PureCodec20201225.exe D:\STA\PureCodec.exe>>D:\STA\f.i
echo get LosslessCut-win32-x64.7z D:\STA\LosslessCut.7z>>D:\STA\f.i
echo get wepe_64.exe D:\STA\Wepe.exe>>D:\STA\f.i
echo get Everything-1.4.1.1005.x64-Setup.exe D:\STA\Everything.exe>>D:\STA\f.i
echo get "D:\76.0.3809.100_chrome_installer.exe" D:\STA\Chrome.exe>>D:\STA\f.i
echo get NDP452-KB2901907-x86-x64-AllOS-ENU.exe D:\STA\NDP452.exe>>D:\STA\f.i
echo bye>>D:\STA\f.i
echo.
echo.
@echo 1.Downloading...
@echo off
echo.
ftp -s:"D:\STA\f.i" 192.168.0.116
del /s /q D:\STA\f.i
@echo 2.System Optimization...
@echo off
echo.
reg add "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Policies\System" /v EnableLUA /t REG_DWORD /d 0 /f
reg add "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer" /v EnableAutoTray /t REG_DWORD /d 0 /f
taskkill /f /im explorer.exe
start "" "%windir%/explorer.exe"
Powercfg -setactive 8c5e7fda-e8bf-4a96-9a85-a6e23a8c635c
reg add "HKEY_CURRENT_USER\AppEvents\Schemes\Names\.Default" /ve /d "@mmres.dll,-801" /f
netsh advfirewall set allprofiles state off
net stop mpssvc
sc config mpssvc start= disabled
net stop wuauserv
sc config wuauserv start= disabled
net stop WSearch
sc config WSearch start= disabled
net stop ShellHWDetection
sc config ShellHWDetection start= disabled
echo.
echo.
@echo 3.System Installing...
@echo off 
call "D:\STA\7z.exe" /S
"C:\Program Files\7-Zip\7z.exe" x "D:\STA\STA.7z" -peder508508 -o"D:\STA\" -aoa
call "D:\STA\07311755.msi" /qb
d:
cd \STA
fontreg /copy
call "D:\STA\AmCAP.exe" /S
call "D:\STA\vc_redist.x64.exe" /q
call "D:\STA\vc_redist.x86.exe" /q
call "D:\STA\vcredist_x64.exe" /q
call "D:\STA\vcredist_x86.exe" /q
call "D:\STA\PureCodec.exe" /S
call "D:\STA\dbeaver.exe" /S /allusers
echo.
echo.
@echo 4.Database installation...
@echo off
echo.
net stop mysql
sc delete mysql
if not exist "D:\STA\db_mms\" xcopy "C:\mysql-8.0.17-winx64\Data\db_mms" "D:\STA\db_mms\" /s /e /y
if exist "C:\mysql-8.0.17-winx64\" del /s /q "C:\mysql-8.0.17-winx64\*.*"
if exist "C:\mysql-8.0.17-winx64\" rd /s /q "C:\mysql-8.0.17-winx64\"
"C:\Program Files\7-Zip\7z.exe" x "D:\STA\mysql-8.0.17-winx64.7z" -peder508508 -o"C:\mysql-8.0.17-winx64\" -aoa
c:
cd \mysql-8.0.17-winx64\bin
mysqld install
net start mysql
copy "D:\STA\usbConsole.dll" "C:\Program Files (x86)\Langrs\MMS\" /y
copy "D:\STA\Config.ini" "C:\Program Files (x86)\Langrs\MMS\" /y
copy "D:\STA\*.lnk" "C:\Users\Administrator\Desktop\" /y
copy "D:\STA\*.lnk" "C:\Users\Administrator\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\" /y
taskkill /F /IM StikyNotifier.exe
copy "D:\STA\StikyNotifier.exe" "C:\Program Files (x86)\Langrs\MMS\" /y
echo.
echo.
@echo =====Restart！=====
@echo off
echo.
echo d:>D:\Res.bat
echo cd \STA>>D:\Res.bat
echo del *.lnk,*.ini,*.dll,*.ttf,*.7z,FontReg.exe,MMS.exe,StikyNotifier.exe,"vc_redist.x64.exe","vc_redist.x86.exe","C:\Users\Public\Desktop\*.lnk",C:\Users\Administrator\Desktop\*.BAT >>D:\Res.bat
echo shutdown -r -t 0 >>D:\Res.bat
call D:\Res.bat
)2>C:\Users\Administrator\Desktop\lo.g
