; qscreenshot-install.nsi
; http://qscreenshot.googlecode.com/
; qScreenshot installation script, v0.1.0
; Written by zet <mailto:vladimir.shelukhin@gmail.com>
; Date: 2011-11-12

; -----------------------------------------------------------------------------
; Define your application information
!define PRODUCT_NAME "qScreenshot"
!define PRODUCT_VERSION "0.5"
!define COMPANY_NAME "qScreenshot Project"
!define PRODUCT_WEB_SITE "http://qscreenshot.googlecode.com/"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\qscreenshot.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; -----------------------------------------------------------------------------
; Main Install settings
Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
InstallDir "$PROGRAMFILES\qScreenshot"

;Get installation folder from registry if available
InstallDirRegKey HKLM "Software\${PRODUCT_NAME}" "InstallDir"

OutFile "setup\qscreenshot-0.5-win32-setup.exe"

; Use compression
SetCompressor /SOLID lzma

BrandingText "Copyright © 2011 ${COMPANY_NAME}"

; -----------------------------------------------------------------------------
; Version Information
VIAddVersionKey  "CompanyName"     "${COMPANY_NAME}"
VIAddVersionKey  "LegalCopyright"  "© 2011 ${COMPANY_NAME}"
VIAddVersionKey  "ProductName"     "${PRODUCT_NAME}"
VIAddVersionKey  "ProductVersion"  "${PRODUCT_VERSION}"
VIAddVersionKey  "FileDescription" "${PRODUCT_NAME} ${PRODUCT_VERSION}"
VIAddVersionKey  "FileVersion"     "${PRODUCT_VERSION}"
VIProductVersion "0.5.0.0"

; -----------------------------------------------------------------------------
; The installer will perform a CRC on itself before allowing an install
CRCCheck on

; -----------------------------------------------------------------------------
; Request application privileges for Windows Vista
;RequestExecutionLevel admin

; -----------------------------------------------------------------------------
XPStyle on

; -----------------------------------------------------------------------------
; Modern interface settings
!include "MUI.nsh"
;!include "UAC.nsh"

!define MUI_ABORTWARNING
!define MUI_ICON "setup\screenshot.ico"
!define MUI_UNICON "setup\un.ico"
!define MUI_COMPONENTSPAGE_SMALLDESC
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_WELCOMEFINISHPAGE_BITMAP "setup\screenshot.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "setup\screenshot.bmp"

; Language Selection Dialog Settings
!define MUI_LANGDLL_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_LANGDLL_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
;!define MUI_LANGDLL_REGISTRY_VALUENAME "NSIS:Language"

; Welcome page
!insertmacro MUI_PAGE_WELCOME

; License page
!insertmacro MUI_PAGE_LICENSE "setup\LICENSE.txt"

; Components page
!define MUI_COMPONENTSPAGE_NODESC
!insertmacro MUI_PAGE_COMPONENTS
;Page custom DisplayAdditionalTasksPage

; Directory page
!insertmacro MUI_PAGE_DIRECTORY

; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\qscreenshot.exe"
;!define MUI_FINISHPAGE_RUN_TEXT "&Run qScreenshot"
!define MUI_FINISHPAGE_RUN_NOTCHECKED
;!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\changelog_cp1251.txt"
;!define MUI_FINISHPAGE_SHOWREADME_TEXT "&Show Changelog"
;!define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
!define MUI_FINISHPAGE_LINK "qScreenshot Website"
!define MUI_FINISHPAGE_LINK_LOCATION "http://qscreenshot.googlecode.com/"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
;!insertmacro MUI_UNPAGE_COMPONENTS
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH
!define MUI_UNFINISHPAGE_NOAUTOCLOSE

; -----------------------------------------------------------------------------
; Set languages (first is default language)
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Russian"

; -----------------------------------------------------------------------------
; Reserve Files
  ; If you are using solid compression, files that are required before
  ; the actual installation should be stored first in the data block,
  ; because this will make your installer start faster.

;!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS
!insertmacro MUI_RESERVEFILE_LANGDLL

; -----------------------------------------------------------------------------
ShowInstDetails show
ShowUninstDetails show

; -----------------------------------------------------------------------------
;Install Types
InstType Typical
InstType Full

; -----------------------------------------------------------------------------
;Function DisplayAdditionalTasksPage
;  !insertmacro MUI_HEADER_TEXT "Select Additional Tasks" "Which additional tasks should be done?"
;  !insertmacro MUI_INSTALLOPTIONS_DISPLAY "AdditionalTasksPage.ini"
;FunctionEnd

; -----------------------------------------------------------------------------
;Installer Sections

Section "!qScreenshot Core Components" SectionqScreenshotCoreComponents

	; Set Section properties
	SectionIn 1 2 RO
	SetOverwrite on

	; Set Section Files and Shortcuts
	SetOutPath "$INSTDIR\"
	File "setup\libgcc_s_dw2-1.dll"
	File "setup\LICENSE.txt"
	File "setup\mingwm10.dll"
	File "setup\qscreenshot.exe"
	File "setup\QtCore4.dll"
	File "setup\QtGui4.dll"
	File "setup\QtNetwork4.dll"
	File "setup\QtSvg4.dll"
	File "setup\QtXml4.dll"
	File "setup\screenshot.ico"
	File "setup\version.txt"
  SetOutPath "$INSTDIR\iconengines\"
  File "setup\iconengines\qsvgicon4.dll"
  SetOutPath "$INSTDIR\imageformats\"
	File "setup\imageformats\qgif4.dll"
	File "setup\imageformats\qico4.dll"
	File "setup\imageformats\qjpeg4.dll"
	File "setup\imageformats\qmng4.dll"
	File "setup\imageformats\qsvg4.dll"
	File "setup\imageformats\qtiff4.dll"
  WriteIniStr "$INSTDIR\qScreenshot Website.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  DeleteRegValue HKCU "Software\Microsoft\Windows\CurrentVersion\Run" "${PRODUCT_NAME}"

SectionEnd


; Optional Start menu shortcut section
Section "Start Menu Shortcuts" SectionStartMenuShortcuts
	
	SectionIn 1 2
	SetOutPath "$INSTDIR\"
	SetShellVarContext all
	CreateDirectory "$SMPROGRAMS\qScreenshot"
	CreateShortCut "$SMPROGRAMS\qScreenshot\qScreenshot.lnk" "$INSTDIR\qscreenshot.exe"
  CreateShortCut "$SMPROGRAMS\qScreenshot\Uninstall.lnk" "$INSTDIR\uninstall.exe"
  CreateShortCut "$SMPROGRAMS\qScreenshot\Website.lnk" "$INSTDIR\${PRODUCT_NAME} Website.url"
	
SectionEnd

; Optional Desktop shortcut section
Section "Desktop Shortcut" SectionDesktopShortcut
	
	SectionIn 2
	SetOutPath "$INSTDIR\"
	SetShellVarContext all
	CreateShortCut "$DESKTOP\qScreenshot.lnk" "$INSTDIR\qscreenshot.exe"
	
SectionEnd

; Optional Quick Launch shortcut section
Section "Quick Launch Shortcut" SectionQuickLaunchShortcut
  
	SectionIn 2
	SetOutPath "$INSTDIR\"
	SetShellVarContext all
  CreateShortCut  "$QUICKLAUNCH\qScreenshot.lnk" "$INSTDIR\qscreenshot.exe"

SectionEnd

; Optional Start qScreenshot on system startup section
Section "Automatically start with Windows" SectionStartupRegistryString
	
	SectionIn 2
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Run" "${PRODUCT_NAME}" "$INSTDIR\qscreenshot.exe"

SectionEnd

Section -FinishSection

	SetOutPath "$INSTDIR\"
  WriteUninstaller "$INSTDIR\uninstall.exe"
	WriteRegExpandStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninstall.exe"
	WriteRegExpandStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "InstallLocation" "$INSTDIR"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "Software\${PRODUCT_NAME}" "" "$INSTDIR"
	DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "Software\Microsoft\Windows\CurrentVersion\App Paths\psi.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\qscreenshot.exe"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "${PRODUCT_NAME}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\qscreenshot.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "HelpLink" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${COMPANY_NAME}"
  WriteRegDWORD ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "NoModify" "1"
  WriteRegDWORD ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "NoRepair" "1"

SectionEnd

; -----------------------------------------------------------------------------
;Component Section Descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SectionqScreenshotCoreComponents} "qScreenshot core files (required)"
  !insertmacro MUI_DESCRIPTION_TEXT ${SectionStartMenuShortcuts} "qScreenshot Start Menu shortcuts"
	!insertmacro MUI_DESCRIPTION_TEXT ${SectionDesktopShortcut} "qScreenshot Desktop shortcut"
	!insertmacro MUI_DESCRIPTION_TEXT ${SectionQuickLaunchShortcut} "qScreenshot Quick Launch shortcut"
	!insertmacro MUI_DESCRIPTION_TEXT ${SectionStartupRegistryString} "Starts the qScreenshot automatically when Windows starts up"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

; =============================================================================
;Uninstall section
Section Uninstall

	;Remove qScreenshot items from registry...
	DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "SOFTWARE\${PRODUCT_NAME}"
	DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_DIR_REGKEY}"
	DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegValue HKCU "Software\Microsoft\Windows\CurrentVersion\Run" "${PRODUCT_NAME}"

	;Delete self
	Delete "$INSTDIR\uninstall.exe"

	;Delete qScreenshot Shortcuts
	SetShellVarContext all
	Delete "$DESKTOP\qScreenshot.lnk"
  Delete "$QUICKLAUNCH\qScreenshot.lnk"

  ;Cleanup qScreenshot Core Components
	Delete "$INSTDIR\qScreenshot Website.url"
	Delete "$INSTDIR\libgcc_s_dw2-1.dll"
	Delete "$INSTDIR\LICENSE.txt"
	Delete "$INSTDIR\mingwm10.dll"
	Delete "$INSTDIR\qscreenshot.exe"
	Delete "$INSTDIR\QtCore4.dll"
	Delete "$INSTDIR\QtGui4.dll"
	Delete "$INSTDIR\QtNetwork4.dll"
	Delete "$INSTDIR\QtSvg4.dll"
	Delete "$INSTDIR\QtXml4.dll"
	Delete "$INSTDIR\screenshot.ico"
	Delete "$INSTDIR\version.txt"
	Delete "$INSTDIR\iconengines\qsvgicon4.dll"
	Delete "$INSTDIR\imageformats\qgif4.dll"
	Delete "$INSTDIR\imageformats\qico4.dll"
	Delete "$INSTDIR\imageformats\qjpeg4.dll"
	Delete "$INSTDIR\imageformats\qmng4.dll"
	Delete "$INSTDIR\imageformats\qsvg4.dll"
	Delete "$INSTDIR\imageformats\qtiff4.dll"

  ;Remove remaining directories
	SetShellVarContext all
  RMDir /r "$SMPROGRAMS\qScreenshot"
	RMDir "$INSTDIR\iconengines\"
	RMDir "$INSTDIR\imageformats\"
	RMDir "$INSTDIR\"

SectionEnd

; -----------------------------------------------------------------------------
; On initialization
Function .onInit

	!insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd
