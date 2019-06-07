; StringTheory installer
; (C) 2012 ETC Ltd

;LZMA Solid is smallest
;----------------------
  SetCompressor /SOLID lzma

;--------------------------------
;Includes

; Modern UI
  !include "MUI.nsh"
;FileFunc for calculating installed size
  !include "FileFunc.nsh"

;--------------------------------
;General

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "StringTheory"
!define PRODUCT_PUBLISHER "ETC Ltd."
!define PRODUCT_WEB_SITE "http://www.etcconnect.com"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\${PRODUCT_NAME}"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKCU"

;Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------
;MUI Settings

  !define MUI_ABORTWARNING
  !define MUI_ICON "install.ico"
  !define MUI_UNICON "uninstall.ico"

;--------------------------------
;Pages

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
; Set the outfile to be ETC_Product Name followed by version with _ delimiters
OutFile "${PRODUCT_NAME}_${PRODUCT_VERSION}.exe"
InstallDir "$PROGRAMFILES\ETC\StringTheory"
InstallDirRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

; --------------
Section "Application" SEC01

  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer

	; Application files (app, Qt, Plugins)
  File /r "..\deploy\"

  ; Shortcuts
  CreateDirectory "$SMPROGRAMS\ETC"
  CreateDirectory "$SMPROGRAMS\ETC\StringTheory"
  CreateShortCut "$SMPROGRAMS\ETC\StringTheory\StringTheory.lnk" "$INSTDIR\StringTheory.exe"

  ; Install VC Redist
  ExecWait '"$INSTDIR\vcredist_x86.exe" /q /norestart' ;Run the vcredist installer, invisible no restart
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  ; For Windows Add/Remove Programs
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\StringTheory.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "QuietUninstallString" "$\"$INSTDIR\Uninstall.exe$\" /S"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\StringTheory.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"

    ; Let Windows know the size
  ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  IntFmt $0 "0x%08X" $0
  WriteRegDWORD ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "EstimatedSize" "$0"
SectionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

;--------------------------------
;Uninstaller Section

Section Uninstall
; Delete installed files
  RMDir /r "$INSTDIR"

; Shortcuts
  Delete "$SMPROGRAMS\ETC\StringTheory\StringTheory.lnk"
  RMDir "$SMPROGRAMS\ETC\StringTheory"
  RMDir "$SMPROGRAMS\ETC"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
