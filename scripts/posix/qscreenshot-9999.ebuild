EAPI=5

inherit cmake-utils git-r3

EGIT_REPO_URI="https://code.google.com/p/qscreenshot/"
EGIT_MIN_CLONE_TYPE="shallow"
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

