project( qScreenshot )
cmake_minimum_required( VERSION 2.8.11 )
set( CMAKE_AUTOMOC ON )
set( USE_QT5 OFF CACHE BOOL "Use Qt5" )
include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	src
	qxt/src/core
	qxt/src/gui
)
set( qxt_all_SRCS
	qxt/src/core/qxtglobal.cpp
	qxt/src/gui/qxtglobalshortcut.cpp
	qxt/src/gui/qxtwindowsystem.cpp
)
set( qScreenshot_SRCS
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
set( qScreenshot_UIS
	src/aboutdlg.ui
	src/editserverdlg.ui
	src/optionsdlg.ui
	src/optionswidget.ui
	src/proxysettingsdlg.ui
	src/screenshot.ui
	src/screenshotoptions.ui
)

set( qScreenshot_RSCS
	src/screenshot.qrc
)
set( qScreenshot_TRS
	src/lang/qscreenshot_ru.ts
)

if( UNIX AND NOT( APPLE OR CYGWIN ) )
	set( DATADIR_PATH ${CMAKE_INSTALL_PREFIX}/share )
	set( APP_DATA_DIR ${DATADIR_PATH}/qscreenshot )
	add_definitions( -Wall -DHAVE_X11 -DQSCREENSHOT_DATADIR="${APP_DATA_DIR}" )
	find_package( X11 REQUIRED )
	set( qxt_SRCS
		${qxt_all_SRCS}
		qxt/src/gui/qxtglobalshortcut_x11.cpp
		qxt/src/gui/qxtwindowsystem_x11.cpp
	)
endif( UNIX AND NOT( APPLE OR CYGWIN ) )

if( ${WIN32} )
	set( qxt_SRCS
		${qxt_all_SRCS}
		qxt/src/gui/qxtglobalshortcut_win.cpp
		qxt/src/gui/qxtwindowsystem_win.cpp
	)
endif( ${WIN32} )

if( ${USE_QT5} )
	find_package( Qt5Widgets REQUIRED )
	find_package( Qt5Network REQUIRED )
	find_package( Qt5LinguistTools )
	find_package( Qt5PrintSupport REQUIRED )
	set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
	include_directories(
		${Qt5Widgets_INCLUDES}
		${Qt5Network_INCLUDES}
		${Qt5Gui_PRIVATE_INCLUDE_DIRS}
		${Qt5PrintSupport_INCLUDES}
	)
	add_definitions(
		-DHAVE_QT5
		${Qt5Widgets_DEFINITIONS}
		${Qt5Network_DEFINITIONS}
	)
	set(QT_DEPLIBS
		Qt5::Widgets
		Qt5::Network
		Qt5::PrintSupport
	)
	if( UNIX AND NOT( APPLE OR CYGWIN ) )
		find_package( Qt5X11Extras REQUIRED )
		include_directories( ${Qt5X11Extras_INCLUDES} )
		add_definitions( ${Qt5X11Extras_DEFINITIONS} )
		set(QT_DEPLIBS
			${QT_DEPLIBS}
			Qt5::X11Extras
		)
	endif( UNIX AND NOT( APPLE OR CYGWIN ) )
	qt5_add_resources( RSCS ${qScreenshot_RSCS} )
	qt5_add_translation( TRS ${qScreenshot_TRS} )
	qt5_wrap_ui( UIS ${qScreenshot_UIS} )
else( ${USE_QT5} )
	find_package( Qt4 REQUIRED )
	include( ${QT_USE_FILE} )
	include_directories (
		${QT_QTCORE_INCLUDE_DIR}
		${QT_QTGUI_INCLUDE_DIR}
		${QT_QTNETWORK_INCLUDE_DIR}
		${QT_QTWIDGETS_INCLUDE_DIR}
	)
	QT4_WRAP_UI( UIS ${qScreenshot_UIS} )
	QT4_ADD_TRANSLATION( TRS ${qScreenshot_TRS} )
	QT4_ADD_RESOURCES( RSCS ${qScreenshot_RSCS} )
	set (QT_DEPLIBS
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_QTNETWORK_LIBRARY}
		${QT_QTWIDGETS_LIBRARY}
	)
endif( ${USE_QT5} )


if( UNIX AND NOT( APPLE OR CYGWIN ) )
	add_executable(
		${PROJECT_NAME}
		${qxt_SRCS}
		${qScreenshot_SRCS}
		${UIS}
		${RSCS}
		${TRS} 
	)
	target_link_libraries(
		${PROJECT_NAME}
		${X11_LIBRARIES}
		${QT_DEPLIBS}
	)
	INSTALL( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin )
	set( DESKTOPFILES qscreenshot.desktop )
	set( APPS_PATH ${DATADIR_PATH}/applications )
	set( ICON_PREFIX src/icons/logo/screenshot )
	set( ICON_16 ${ICON_PREFIX}_16.png )
	set( ICON_22 ${ICON_PREFIX}_22.png )
	set( ICON_24 ${ICON_PREFIX}_24.png )
	set( ICON_32 ${ICON_PREFIX}_32.png )
	set( ICON_48 src/icons/screenshot.png )
	set( ICONPATH_PREFIX ${DATADIR_PATH}/icons/hicolor )
	set( TRS_PATH ${APP_DATA_DIR}/translations )
	INSTALL( FILES ${ICON_16} DESTINATION ${ICONPATH_PREFIX}/16x16/apps )
	INSTALL( FILES ${ICON_22} DESTINATION ${ICONPATH_PREFIX}/22x22/apps )
	INSTALL( FILES ${ICON_24} DESTINATION ${ICONPATH_PREFIX}/24x24/apps )
	INSTALL( FILES ${ICON_32} DESTINATION ${ICONPATH_PREFIX}/32x32/apps )
	INSTALL( FILES ${ICON_48} DESTINATION ${ICONPATH_PREFIX}/48x48/apps )
	INSTALL( FILES ${TRS} DESTINATION ${TRS_PATH} )
	INSTALL( FILES ${DESKTOPFILES} DESTINATION ${APPS_PATH} )
endif( UNIX AND NOT( APPLE OR CYGWIN ) )

if( ${WIN32} )
	add_executable(
		${PROJECT_NAME}
		WIN32
		${qxt_SRCS}
		${qScreenshot_SRCS}
		${UIS}
		${RSCS}
		${TRS}
		src/screenshot.rc
	)
	target_link_libraries(
		${PROJECT_NAME}
		${QT_DEPLIBS}
	)
	INSTALL( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} )
	INSTALL( FILES ${TRS} DESTINATION ${CMAKE_INSTALL_PREFIX}/translations )
	INSTALL( FILES LICENSE.txt DESTINATION ${CMAKE_INSTALL_PREFIX} )
endif( ${WIN32} )

