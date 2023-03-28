SetCompressor /SOLID lzma
;Требуем админские права, чтобы установиться в %PROGRAMFILES% (виртуализация не катит, она срабатывает при установке в startmenu, в ProgramFiles виртуализировать не хочет. Возможно это глюк NSIS)
;Ярлыки ставим для всех пользователей.
;В программе надо сделать, чтобы все настройки сохранялись не в директорию установки (programfiles), а в общие доки пользователей (%appdata% в контексте ShellVars:all)
RequestExecutionLevel admin
!define PRODUCT_NAME "XILab"
!searchparse /file src/main.h `#define XILAB_VERSION "` VER_MAJOR `.` VER_MINOR `.` VER_RELEASE `"`
!define PRODUCT_VERSION "${VER_MAJOR}.${VER_MINOR}.${VER_RELEASE}"

!define PRODUCT_MANUFACTURER "XIMC"
!define URL_INFO "http://www.standasupport.com"
!define URL_UPDATE "http://www.standasupport.com/projects/support/wiki/Software"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\XILab.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME} ${PRODUCT_VERSION}"

!include "MUI2.nsh"
OutFile "${PRODUCT_NAME}-${PRODUCT_VERSION}-win32_win64.exe"
Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
!include x64.nsh



;---MUI settings---;
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
;-WelcomePage
!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange.bmp"
!define MUI_WELCOMEPAGE_TITLE "Welcome to the ${PRODUCT_NAME} ${PRODUCT_VERSION} Setup Wizard"
!define MUI_WELCOMEPAGE_TEXT "This wizard will guide you through the installation of ${PRODUCT_NAME} ${PRODUCT_VERSION}.$\n"
;--FinishPage
!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_FUNCTION RunXilab
Function RunXilab
SetOutPath $INSTDIR
Exec "$INSTDIR\Xilab.exe"
FunctionEnd

!define MUI_FINISHPAGE_RUN_TEXT "Run XILab"
;!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\changelog.txt"
;!define MUI_FINISHPAGE_SHOWREADME_TEXT "View changelog"

Var StartMenuFolder

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Applications $StartMenuFolder
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_LANGUAGE "English"


;--Поиск и деинсталляция старых версий XILab
Section -SecUnEarlier
  StrCpy $0 0
loop:
  EnumRegKey $1 HKLM Software\Microsoft\Windows\CurrentVersion\Uninstall $0
  StrCmp $1 "" done
  IntOp $0 $0 + 1
  StrCpy $2 $1 5
  StrCpy $3 $1 3 6
  StrCmp $2 "XILab" 0 loop
  StrCmp $3 "1.4" loop 0
  Call MakeUninstall
  Goto loop
done:
SectionEnd

!define RELEASEFILES ""
!define SETUPFILES $%DEPDIR%
!define QWTFILES "C:\Qwt\msvc2013\qwt-6.0.0\"
Section "Application files" SecMain_32
  !undef RELEASEFILES
  !define RELEASEFILES ".\Win32\release - usermode"
  !define QTFILES32 "C:\Qt\msvc2013\4.8.6\" ;$%QTDIR32%

  SetShellVarContext all
  SetOutPath "$INSTDIR"
  SetOverwrite on
  ;--Files--Для сборки в других системах обновить эти пути и строку реестра в функции vcredist_test--;
  File  "${RELEASEFILES}\libximc.dll"
  File  "${RELEASEFILES}\xibridge.dll"
  File  "${RELEASEFILES}\bindy.dll"
  File  "${QTFILES32}\bin\QtCore4.dll"
  File  "${QTFILES32}\bin\QtGui4.dll"
  File  "${QTFILES32}\bin\QtSvg4.dll"
  File  "${QTFILES32}\bin\QtScript4.dll"
  File  "${QTFILES32}\bin\QtNetwork4.dll"
  File  "${QTFILES32}\bin\QtXml4.dll"
  File  "${QWTFILES}\lib32\qwt.dll"
  File  "xilabdefault.cfg"
  File  "default_keyfile.sqlite"
  File  "qt.conf"

  CreateDirectory "$INSTDIR\imageformats"
  File  /oname=imageformats\qgif4.dll "${QTFILES32}\plugins\imageformats\qgif4.dll"
  File  /oname=imageformats\qsvg4.dll "${QTFILES32}\plugins\imageformats\qsvg4.dll"

  CreateDirectory "$INSTDIR\iconengines"
  File  /oname=iconengines\qsvgicon4.dll "${QTFILES32}\plugins\iconengines\qsvgicon4.dll"

  CreateDirectory "$INSTDIR\driver"
  File  /oname=driver\Standa_8SMC4-5.inf "driver\Standa_8SMC4-5.inf"
  File  /oname=driver\standa_8smc4-5.cat "driver\standa_8smc4-5.cat"
  File  /oname=$WINDIR\Inf\Standa_8SMC4-5.inf "driver\Standa_8SMC4-5.inf"
  File  /oname=$WINDIR\Inf\standa_8smc4-5.cat "driver\standa_8smc4-5.cat"
  File  /oname=XILab.exe "${RELEASEFILES}\XILab [release - usermode].exe"
  
  CreateDirectory "$INSTDIR\scripts"
  SetOutPath "$INSTDIR\scripts"
  File /r "xiresource\scripts\*.*"

  CreateDirectory "$INSTDIR\profiles"
  SetOutPath "$INSTDIR\profiles"
  File /r "xiresource\profiles\*.cfg"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Applications
  CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
  SetOutPath "$INSTDIR"
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\XILab.lnk" "$INSTDIR\XILab.exe"
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\uninst.exe"
  CreateShortCut "$DESKTOP\XILab.lnk" "$INSTDIR\XILab.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "Application files" SecMain_64
  ${DisableX64FSRedirection}
  !undef RELEASEFILES
  !define RELEASEFILES ".\x64\release - usermode"
  !define QTFILES64 "C:\Qt\msvc2013\4.8.6_x64\" ;$%QTDIR64%

  SetShellVarContext all
  SetOutPath "$INSTDIR"
  SetOverwrite on
  ;--Files--Для сборки в других системах обновить эти пути и строку реестра в функции vcredist_test--;
  File  "${RELEASEFILES}\libximc.dll"
  File  "${RELEASEFILES}\xibridge.dll"
  File  "${RELEASEFILES}\bindy.dll"
  File  "${QTFILES64}\bin\QtCore4.dll"
  File  "${QTFILES64}\bin\QtGui4.dll"
  File  "${QTFILES64}\bin\QtSvg4.dll"
  File  "${QTFILES64}\bin\QtScript4.dll"
  File  "${QTFILES64}\bin\QtNetwork4.dll"
  File  "${QTFILES64}\bin\QtXml4.dll"
  File  "${QWTFILES}\lib64\qwt.dll"
  File  "xilabdefault.cfg"
  File  "default_keyfile.sqlite"
  File  "qt.conf"

  CreateDirectory "$INSTDIR\imageformats"
  File  /oname=imageformats\qgif4.dll "${QTFILES64}\plugins\imageformats\qgif4.dll"
  File  /oname=imageformats\qsvg4.dll "${QTFILES64}\plugins\imageformats\qsvg4.dll"

  CreateDirectory "$INSTDIR\iconengines"
  File  /oname=iconengines\qsvgicon4.dll "${QTFILES64}\plugins\iconengines\qsvgicon4.dll"

  CreateDirectory "$INSTDIR\driver"
  File  /oname=driver\Standa_8SMC4-5.inf "driver\Standa_8SMC4-5.inf"
  File  /oname=driver\standa_8smc4-5.cat "driver\standa_8smc4-5.cat"
  File  /oname=$WINDIR\Inf\Standa_8SMC4-5.inf "driver\Standa_8SMC4-5.inf"
  File  /oname=$WINDIR\Inf\standa_8smc4-5.cat "driver\standa_8smc4-5.cat"
  File  /oname=XILab.exe "${RELEASEFILES}\XILab [release - usermode].exe"
  
  CreateDirectory "$INSTDIR\scripts"
  SetOutPath "$INSTDIR\scripts"
  File /r "xiresource\scripts\*.*"

  CreateDirectory "$INSTDIR\profiles"
  SetOutPath "$INSTDIR\profiles"
  File /r "xiresource\profiles\*.cfg"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Applications
  CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
  SetOutPath "$INSTDIR"
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\XILab.lnk" "$INSTDIR\XILab.exe"
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\uninst.exe"
  CreateShortCut "$DESKTOP\XILab.lnk" "$INSTDIR\XILab.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "Drivers" SecDrivers
  SetOutPath "$TEMP"
  SetOverwrite on

  ShowWindow $HWNDPARENT ${SW_MINIMIZE} ; A hack to show popunder "Operation completed succesfully" dialog on Windows8 after executing next line
  ExecWait 'InfDefaultInstall "$INSTDIR\driver\Standa_8SMC4-5.inf"'
  ShowWindow $HWNDPARENT ${SW_NORMAL}
${If} ${RunningX64}
  Call vcredist_test_64
${Else}
  Call vcredist_test_32
${EndIf}
  StrCmp $0 "" 0 vcredist_done

${If} ${RunningX64}
  File   "${SETUPFILES}\vcredist_x64_2013.exe"
  ExecWait "$TEMP\vcredist_x64_2013.exe /install /passive /norestart"
${Else}
  File   "${SETUPFILES}\vcredist_x86_2013.exe"
  ExecWait "$TEMP\vcredist_x86_2013.exe /install /passive /norestart"
${EndIf}

  vcredist_done:
SectionEnd

Section -SecReg
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\XILab.exe"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "StartMenuFolder" "$StartMenuFolder"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayName" "${PRODUCT_NAME} ${PRODUCT_VERSION}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\XILab.exe"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_MANUFACTURER}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${URL_INFO}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "URLUpdateInfo" "${URL_UPDATE}"
SectionEnd

;возвращает непустую строку (DisplayName), если vcredist_x86 12.0.21005 найден
Function vcredist_test_32
  ReadRegStr $0 HKLM SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{13A4EE12-23EA-3371-91EE-EFB36DDFFF3E} "DisplayName"
FunctionEnd

;возвращает непустую строку (DisplayName), если vcredist_x64 12.0.21005 найден
Function vcredist_test_64
  ReadRegStr $0 HKLM SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{A749D8E6-B613-3BE3-8F5F-045C84EBA29B} "DisplayName"
FunctionEnd

;удаляет XILab, в качестве аргумента принимает имя подключа из директории реестра Uninstall в регистре $1
Function MakeUninstall
  ReadRegStr $R4 HKLM Software\Microsoft\Windows\CurrentVersion\Uninstall\$1 "UninstallString"
;  MessageBox MB_OK "$2$\nXILab found$\nCall $R4"
  ExecWait "$R4 /S"
  Sleep 1500
FunctionEnd


;--Uninstaller--;
Section Uninstall
${If} ${RunningX64}
  SetRegView 64
${EndIf}
  SetShellVarContext all
  ReadRegStr $StartMenuFolder HKLM "${PRODUCT_UNINST_KEY}" "StartMenuFolder"
  ; удаление хлама от старых версий
  Delete "$INSTDIR\ftd2xx.dll"
  Delete "$INSTDIR\libxidcusb.dll"
  Delete "$INSTDIR\Standa_8SMC4-5.inf"
  RMDir "$INSTDIR\ftdi_drivers\i386"
  RMDir "$INSTDIR\ftdi_drivers\amd64"
  RMDir "$INSTDIR\ftdi_drivers"

  ; удаление новой версии
  Delete "$INSTDIR\imageformats\qgif4.dll"
  Delete "$INSTDIR\imageformats\qsvg4.dll"
  RMDir "$INSTDIR\imageformats"

  Delete "$INSTDIR\iconengines\qsvgicon4.dll"
  RMDir "$INSTDIR\iconengines"

  Delete "$INSTDIR\driver\Standa_8SMC4-5.inf"
  Delete "$INSTDIR\driver\standa_8smc4-5.cat"
  RMDir "$INSTDIR\driver"

  Delete "$INSTDIR\scripts\cyclic.txt"
  Delete "$INSTDIR\scripts\script_one.txt"
  Delete "$INSTDIR\scripts\script_two.txt"
  Delete "$INSTDIR\scripts\set_zero.txt"
  Delete "$INSTDIR\scripts\homing_test.txt"
  Delete "$INSTDIR\scripts\random_shift.txt"
  Delete "$INSTDIR\scripts\csv_file_read_save.txt"
  Delete "$INSTDIR\scripts\cyclic_single_axis.txt"
  Delete "$INSTDIR\scripts\list_axis_serials.txt"
  Delete "$INSTDIR\scripts\move_and_sleep.csv"
  Delete "$INSTDIR\scripts\move_and_wait.txt"
  Delete "$INSTDIR\scripts\bitmask.txt"
  
  Delete "$INSTDIR\scripts\miscellaneous\autotester_script.txt"
  Delete "$INSTDIR\scripts\miscellaneous\Border_crossing_test.txt"
  Delete "$INSTDIR\scripts\miscellaneous\circle.txt"
  Delete "$INSTDIR\scripts\miscellaneous\closedloop_tuning.txt"
  Delete "$INSTDIR\scripts\miscellaneous\homing_test_with_extio.txt"
  Delete "$INSTDIR\scripts\miscellaneous\probabilistic_tests.txt"
  Delete "$INSTDIR\scripts\miscellaneous\several_shifts_with_calibration.txt"
  Delete "$INSTDIR\scripts\miscellaneous\sync_test.txt"
  Delete "$INSTDIR\scripts\miscellaneous\synch_bug_test.txt"
  RMDir "$INSTDIR\scripts\miscellaneous"
  
  RMDir "$INSTDIR\scripts"

  RMDir "$INSTDIR\profiles" ; Doesn't do anything as long as there are profiles remaining in that directory

  Delete "$INSTDIR\libximc.dll"
  Delete "$INSTDIR\bindy.dll"
  Delete "$INSTDIR\xibridge.dll"
  Delete "$INSTDIR\keyfile.bin"
  Delete "$INSTDIR\keyfile.sqlite"
  Delete "$INSTDIR\default_keyfile.sqlite"
  Delete "$INSTDIR\QtCore4.dll"
  Delete "$INSTDIR\QtGui4.dll"
  Delete "$INSTDIR\QtScript4.dll"
  Delete "$INSTDIR\QtSvg4.dll" 
  Delete "$INSTDIR\QtNetwork4.dll"
  Delete "$INSTDIR\QtXml4.dll"
  Delete "$INSTDIR\qwt5.dll"
  Delete "$INSTDIR\qwt.dll"
  Delete "$INSTDIR\xilabdefault.cfg"
  Delete "$INSTDIR\qt.conf"
  Delete "$INSTDIR\changelog.txt"
  Delete "$INSTDIR\XILab.exe"
  Delete "$INSTDIR\uninst.exe"
  RMDir "$INSTDIR"
  ;
  Delete "$DESKTOP\XILab.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\XILab.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"
  ;
  DeleteRegKey HKLM "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
;  SetAutoClose true
SectionEnd

Function .onInit
${If} ${RunningX64}
  SetRegView 64
  StrCpy $INSTDIR "$PROGRAMFILES64\${PRODUCT_NAME}"

  Push $0 ; This and corresponding Pop are recommended by 4.2.2 in the manual
 
  SectionGetFlags ${SecMain_64} $0
  IntOp $0 $0 | ${SF_SELECTED}
  SectionSetFlags ${SecMain_64} $0
 
  SectionGetFlags ${SecMain_32} $0
  IntOp $0 $0 & ${SECTION_OFF}
  SectionSetFlags ${SecMain_32} $0
 
  Pop $0
${Else}
  StrCpy $INSTDIR "$PROGRAMFILES32\${PRODUCT_NAME}"

  Push $0
 
  SectionGetFlags ${SecMain_32} $0
  IntOp $0 $0 | ${SF_SELECTED}
  SectionSetFlags ${SecMain_32} $0
 
  SectionGetFlags ${SecMain_64} $0
  IntOp $0 $0 & ${SECTION_OFF}
  SectionSetFlags ${SecMain_64} $0
 
  Pop $0
${EndIf}
FunctionEnd
