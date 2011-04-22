#!/bin/bash
set -e

debuild -us -uc
debuild -S -us -uc
su -c pbuilder build ../qscreenshot-0.2.dsc
