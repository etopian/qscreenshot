#!/bin/sh
PACKAGER_INFO="your_first_name your_last_name <your e-mail>"
if [ -f ~/.rpmrc -o -d ~/rpm ]; then
        echo 'Delete any existing setup before running this script'
        exit 1
fi
cd

mkdir rpm
mkdir rpm/BUILD
mkdir rpm/RPMS
mkdir rpm/RPMS/i586
mkdir rpm/RPMS/x86_64
mkdir rpm/RPMS/noarch
mkdir rpm/SOURCES
mkdir rpm/SPECS
mkdir rpm/SRPMS
mkdir rpm/tmp

cat <<END >~/.rpmrc
buildarchtranslate: i386: i586
buildarchtranslate: i486: i586
buildarchtranslate: i586: i586
buildarchtranslate: i686: i586
END

cat <<END >~/.rpmmacros
%_topdir                %(echo $HOME)/rpm
%_tmppath               %(echo $HOME)/rpm/tmp

# If you want your packages to be GPG signed automatically, add these three lines
# replacing 'Mandrivalinux' with your GPG name. You may also use rpm --resign
# to sign the packages later.
%_signature             gpg
%_gpg_name              Mandrivalinux
%_gpg_path              ~/.gnupg

# Add your name and e-mail into the %packager field below. You may also want to
# also replace vendor with yourself.
%packager               $PACKAGER_INFO
%distribution           Mandriva Linux
%vendor                 Mandriva

# If you want your packages to have your own distsuffix instead of mdv, add it
# here like this
#%distsuffix             mdv
END
