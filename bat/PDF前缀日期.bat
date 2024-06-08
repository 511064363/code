@echo off
setlocal enabledelayedexpansion

:: 获取当前日期，格式为YYYYMMDD
set CURRENT_DATE=%date:~0,4%%date:~5,2%%date:~8,2%

:: 循环遍历当前目录下的所有文件
for %%f in (*.pdf) do (
    :: 获取文件的原始名称
    set "original=%%f"
    
    :: 替换文件名中的空格为下划线，以避免文件名中出现空格
    set "newname=!original: =_!"
    
    :: 给文件名添加前缀
    set "newname=!CURRENT_DATE!_!newname!"
    
    :: 重命名文件
    ren "%%f" "!newname!"
)
