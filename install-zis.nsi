; example1.nsi
;
; This script is perhaps one of the simplest NSIs you can make. All of the
; optional settings are left to their default settings. The instalelr simply
; prompts the user asking them where to install, and drops of notepad.exe
; there. If your Windows directory is not C:\windows, change it below.
;

; The name of the installer
Name "ZIS - Zionist in Space 0.99.5" ; @RELEASE@

; The file to write
OutFile "install-ZIS-0.99.5.exe" ; @RELEASE@

LicenseData LICENSE

; The default installation directory
InstallDir "$PROGRAMFILES\Eli Games\ZIS"

; The text to prompt the user to enter a directory
DirText "This will install ZIS 0.99.5 on your computer. Choose a directory" ; @RELEASE@

; The stuff to install
Section "ZIS (required)"
; Set output path to the installation directory.
SetOutPath $INSTDIR
; Put file there
File "ZIS.exe"
File "set50.exe"
File "help.html"
File "bundle\cwsdpmi.exe"

; Use the source!!!
Section "Source code (recommended)"
CreateDirectory $INSTDIR\Source
SetOutPath $INSTDIR\Source
File "game.c"
File "ai.c"
File "zis.h"
File "set50.c"
File "Makefile"
File "help.html.m4"
File "install-zis.nsi"
File "release.py"
File "LICENSE"
File "README"
File "INSTALL"

; Write the uninstall keys for Windows
WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\ZIS" "DisplayName" "Eli's ZIS (remove only)"
WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\ZIS" "UninstallString" '"$INSTDIR\uninstall.exe"'

Section "Start Menu Shortcuts"
SetOutPath $INSTDIR
CreateDirectory "$SMPROGRAMS\ZIS"
CreateShortCut "$SMPROGRAMS\ZIS\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
CreateShortCut "$SMPROGRAMS\ZIS\ZIS.pif" "$INSTDIR\ZIS.exe" "" "$INSTDIR\ZIS.exe" 0 SW_SHOWMAXIMIZED
CreateShortCut "$SMPROGRAMS\ZIS\Help.lnk" "$INSTDIR\help.html" "" "$INSTDIR\help.html" 0

UninstallText "This will uninstall ZIS. Hit next to continue."
UninstallExeName "uninstall.exe"

; special uninstall section.
Section "Uninstall"
; remove files
Delete $INSTDIR\*.*

; remove shortcuts, if any.
Delete "$SMPROGRAMS\ZIS\*.*"
RMDir "$SMPROGRAMS\ZIS"

MessageBox MB_YESNO|MB_ICONQUESTION "Do you want to keep the source code?$\n(You can remove the directory manually when you want.)" IDYES 2

Delete "$INSTDIR\Source\*.*"
; remove directories used.
RMDir "$INSTDIR\Source"

RMDir "$INSTDIR"

; remove registry keys
DeleteRegKey HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\ZIS"

; eof
