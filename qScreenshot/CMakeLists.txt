project( qScreenshot )
cmake_minimum_required( VERSION 2.8.6 )
set ( CMAKE_BUILD_TYPE Release )
set ( CMAKE_AUTOMOC TRUE )
add_definitions ( -Wall )
find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR}
	${QT_QTGUI_INCLUDE_DIR}
	${QT_QTNETWORK_INCLUDE_DIR}
	${QT_QTWIDGETS_INCLUDE_DIR}
	src
	qxt/src/core
	qxt/src/gui
)

set ( qxt_all_SRCS
	qxt/src/core/qxtglobal.cpp
	qxt/src/gui/qxtglobalshortcut.cpp
	qxt/src/gui/qxtwindowsystem.cpp
)

set ( qScreenshot_SRCS
	src/aboutdlg.cpp
	src/common.cpp
	src/controller.cpp
	src/editserverdlg.cpp
	src/grabareawidget.cpp
	src/historydlg.cpp
	src/iconset.cpp
	src/main.cpp
	src/options.cpp
	src/optionsdlg.cpp
	src/optionswidget.cpp
	src/pixmapwidget.cpp
	src/proxysettingsdlg.cpp
	src/screenshoter.cpp
	src/screenshotmainwin.cpp
	src/screenshotoptions.cpp
	src/shortcutmanager.cpp
	src/server.cpp
	src/toolbar.cpp
	src/translator.cpp
	src/updateschecker.cpp
	src/uploader.cpp
)

set ( qScreenshot_UIS
	src/aboutdlg.ui
	src/editserverdlg.ui
	src/optionsdlg.ui
	src/optionswidget.ui
	src/proxysettingsdlg.ui
	src/screenshot.ui
	src/screenshotoptions.ui
)

set ( qScreenshot_RSCS
	src/screenshot.qrc
)

set ( qScreenshot_TRS
	src/lang/qscreenshot_ru.ts
)

if ( WIN32 AND Qt4_FOUND )
	set ( qScreenshot_W32_RSCS
		src/screenshot.rc
	)
	QT4_ADD_RESOURCES( RSCS
		${qScreenshot_RSCS}
		${qScreenshot_W32_RSCS}
	)
	QT4_WRAP_UI( UIS ${qScreenshot_UIS} )
	QT4_ADD_TRANSLATION( TRS ${qScreenshot_TRS} )
endif( WIN32 AND Qt4_FOUND )

if ( UNIX AND Qt4_FOUND )
	QT4_WRAP_UI( UIS ${qScreenshot_UIS} )
	QT4_ADD_TRANSLATION( TRS ${qScreenshot_TRS} )
	QT4_ADD_RESOURCES( RSCS ${qScreenshot_RSCS} )
	add_definitions ( -DHAVE_X11 )
	find_package( X11 REQUIRED )
	set ( qxt_SRCS
		${qxt_all_SRCS}
		qxt/src/gui/qxtglobalshortcut_x11.cpp
		qxt/src/gui/qxtwindowsystem_x11.cpp
	)
	add_executable (
		${PROJECT_NAME}
		${qxt_SRCS}
		${qScreenshot_SRCS}
		${UIS}
		${RSCS}
		${TRS} 
	)
	target_link_libraries (
		${PROJECT_NAME}
		${X11_LIBRARIES}
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_QTNETWORK_LIBRARY}
		${QT_QTWIDGETS_LIBRARY}
	)
	INSTALL ( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin )
	set ( DESKTOPFILES qscreenshot.desktop )
	set ( DATADIR_PATH ${CMAKE_INSTALL_PREFIX}/share )
	set ( APPS_PATH ${DATADIR_PATH}/applications )
	set ( ICON_PREFIX src/icons/logo/screenshot )
	set ( ICON_16 ${ICON_PREFIX}_16.png )
	set ( ICON_22 ${ICON_PREFIX}_22.png )
	set ( ICON_24 ${ICON_PREFIX}_24.png )
	set ( ICON_32 ${ICON_PREFIX}_32.png )
	set ( ICON_48 src/icons/screenshot.png )
	set ( ICONPATH_PREFIX ${DATADIR_PATH}/icons/hicolor )
	set ( APP_DATA_DIR ${DATADIR_PATH}/qscreenshot )
	set ( TRS_PATH ${APP_DATA_DIR}/translations )

	INSTALL ( FILES ${ICON_16} DESTINATION ${ICONPATH_PREFIX}/16x16/apps )
	INSTALL ( FILES ${ICON_22} DESTINATION ${ICONPATH_PREFIX}/22x22/apps )
	INSTALL ( FILES ${ICON_24} DESTINATION ${ICONPATH_PREFIX}/24x24/apps )
	INSTALL ( FILES ${ICON_32} DESTINATION ${ICONPATH_PREFIX}/32x32/apps )
	INSTALL ( FILES ${ICON_48} DESTINATION ${ICONPATH_PREFIX}/48x48/apps )
	INSTALL ( FILES ${TRS} DESTINATION ${TRS_PATH} )
	INSTALL ( FILES ${DESKTOPFILES} DESTINATION ${APPS_PATH} )
endif( UNIX AND Qt4_FOUND )

if ( APPLE AND Qt4_FOUND ) #not yet finished
	QT4_WRAP_UI( UIS ${qScreenshot_UIS} )
	QT4_ADD_TRANSLATION( TRS ${qScreenshot_TRS} )
	QT4_ADD_RESOURCES( RSCS ${qScreenshot_RSCS} )
	set( ICONFILE src/icons/logo/screenshot.icns )
	set( INFO_PLIST_FILE Info.plist )
	set_target_properties( ${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${INFO_PLIST_FILE} )
endif( APPLE AND Qt4_FOUND )
