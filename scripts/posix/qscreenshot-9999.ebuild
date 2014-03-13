EAPI="5"

inherit cmake-utils subversion

ESVN_REPO_URI="https://qscreenshot.googlecode.com/svn/trunk/qScreenshot/"
DESCRIPTION="Create and modify screenshots"
HOMEPAGE="http://code.google.com/p/qscreenshot/"
LICENSE="GPL-2"
SLOT="0"
KEYWORDS=""
DEPEND="
        dev-qt/qtcore
        dev-qt/qtgui
        x11-libs/libX11
"
RDEPEND="$DEPEND"

