#!/bin/sh
set -e -x

CURDIR="$PWD"

QDIR="$HOME/.cache/qemu"

if [ -n "$RTEMS" -a "$TEST" = "YES" ]
then
  git clone --quiet --branch vme --depth 10 https://github.com/mdavidsaver/qemu.git "$HOME/.build/qemu"
  cd "$HOME/.build/qemu"

  HEAD=`git log -n1 --pretty=format:%H`
  echo "HEAD revision $HEAD"

  [ -e "$HOME/.cache/qemu/built" ] && BUILT=`cat "$HOME/.cache/qemu/built"`
  echo "Cached revision $BUILT"

  if [ "$HEAD" != "$BUILT" ]
  then
    echo "Building QEMU"
    git submodule --quiet update --init

    install -d "$HOME/.build/qemu/build"
    cd         "$HOME/.build/qemu/build"

    "$HOME/.build/qemu/configure" --prefix="$HOME/.cache/qemu/usr" --target-list=i386-softmmu --disable-werror
    make -j2
    make install

    echo "$HEAD" > "$HOME/.cache/qemu/built"
  fi
fi

cd "$CURDIR"

cat << EOF > $CURDIR/configure/RELEASE.local
EPICS_BASE=$HOME/.source/epics-base
EOF

install -d "$HOME/.source"
cd "$HOME/.source"

add_base_module() {
  MODULE=$1
  BRANCH=$2
  ( cd epics-base/modules && \
  git clone --quiet --depth 5 --branch $MODULE/$BRANCH https://github.com/${REPOBASE:-epics-base}/epics-base.git $MODULE && \
  cd $MODULE && git log -n1 )
}

add_gh_module() {
  MODULE=$1
  REPOOWNER=$2
  REPONAME=$3
  BRANCH=$4
  ( cd epics-base/modules && \
  git clone --quiet --depth 5 --branch $BRANCH https://github.com/$REPOOWNER/$REPONAME.git $MODULE && \
  cd $MODULE && git log -n1 )
}

add_gh_flat() {
  MODULE=$1
  REPOOWNER=$2
  REPONAME=$3
  BRANCH=$4
  MODULE_UC=$(echo $MODULE | tr 'a-z' 'A-Z')
  ( git clone --quiet --depth 5 --branch $BRANCH https://github.com/$REPOOWNER/$REPONAME.git $MODULE && \
  cd $MODULE && git log -n1 )
  cat < $CURDIR/configure/RELEASE.local > $MODULE/configure/RELEASE.local
  cat << EOF >> $CURDIR/configure/RELEASE.local
${MODULE_UC}=$HOME/.source/$MODULE
EOF
}

if [ "$BRBASE" ]
then
  git clone --quiet --depth 5 --branch "$BRBASE" https://github.com/${REPOBASE:-epics-base}/epics-base.git epics-base
  (cd epics-base && git log -n1 )
  add_gh_flat pvData ${REPOPVD:-epics-base} pvDataCPP ${BRPVD:-master}
  add_gh_flat pvAccess ${REPOPVA:-epics-base} pvAccessCPP ${BRPVA:-master}
  add_gh_flat normativeTypes ${REPONT:-epics-base} normativeTypesCPP ${BRNT:-master}
  add_gh_flat pvaClient ${REPOPVAC:-epics-base} pvaClientCPP ${BRPVAC:-master}
  add_gh_flat pvDatabase ${REPOPVDB:-epics-base} pvDatabaseCPP ${BRPVDB:-master}
  add_gh_flat pva2pva ${REPOQSRV:-epics-base} pva2pva ${BRQSRV:-master}
else
  git clone --quiet --depth 5 --branch core/"${BRCORE:-master}" https://github.com/${REPOBASE:-epics-base}/epics-base.git epics-base
  ( cd epics-base && git log -n1 )
  add_base_module libcom "${BRLIBCOM:-master}"
  add_base_module ca "${BRCA:-master}"
  add_base_module database "${BRDATABASE:-master}"
  add_gh_module pvData ${REPOPVD:-epics-base} pvDataCPP ${BRPVD:-master}
  add_gh_module pvAccess ${REPOPVA:-epics-base} pvAccessCPP ${BRPVA:-master}
  add_gh_module normativeTypes ${REPONT:-epics-base} normativeTypesCPP ${BRNT:-master}
  add_gh_module pvaClient ${REPOPVAC:-epics-base} pvaClientCPP ${BRPVAC:-master}
  add_gh_module pvDatabase ${REPOPVDB:-epics-base} pvDatabaseCPP ${BRPVDB:-master}
  add_gh_module pva2pva ${REPOQSRV:-epics-base} pva2pva ${BRQSRV:-master}
fi

if [ -e $CURDIR/configure/RELEASE.local ]
then
  cat $CURDIR/configure/RELEASE.local
fi

EPICS_HOST_ARCH=`sh epics-base/startup/EpicsHostArch`

# requires wine and g++-mingw-w64-i686
if [ "$WINE" = "32" ]
then
  echo "Cross mingw32"
  sed -i -e '/CMPLR_PREFIX/d' epics-base/configure/os/CONFIG_SITE.linux-x86.win32-x86-mingw
  cat << EOF >> epics-base/configure/os/CONFIG_SITE.linux-x86.win32-x86-mingw
CMPLR_PREFIX=i686-w64-mingw32-
EOF
  cat << EOF >> epics-base/configure/CONFIG_SITE
CROSS_COMPILER_TARGET_ARCHS+=win32-x86-mingw
EOF
fi

if [ "$STATIC" = "YES" ]
then
  echo "Build static libraries/executables"
  cat << EOF >> epics-base/configure/CONFIG_SITE
SHARED_LIBRARIES=NO
STATIC_BUILD=YES
EOF
fi

case "$CMPLR" in
clang)
  echo "Host compiler is clang"
  cat << EOF >> epics-base/configure/os/CONFIG_SITE.Common.$EPICS_HOST_ARCH
GNU         = NO
CMPLR_CLASS = clang
CC          = clang
CCC         = clang++
EOF

  # hack
  sed -i -e 's/CMPLR_CLASS = gcc/CMPLR_CLASS = clang/' epics-base/configure/CONFIG.gnuCommon

  clang --version
  ;;
*)
  echo "Host compiler is default"
  gcc --version
  ;;
esac

cat <<EOF >> epics-base/configure/CONFIG_SITE
USR_CPPFLAGS += $USR_CPPFLAGS
USR_CFLAGS += $USR_CFLAGS
USR_CXXFLAGS += $USR_CXXFLAGS
EOF

# set RTEMS to eg. "4.9" or "4.10"
# requires qemu, bison, flex, texinfo, install-info
if [ -n "$RTEMS" ]
then
  echo "Cross RTEMS${RTEMS} for pc386"
  install -d /home/travis/.cache
  curl -L "https://github.com/mdavidsaver/rsb/releases/download/travis-20160306-2/rtems${RTEMS}-i386-trusty-20190306-2.tar.gz" \
  | tar -C /home/travis/.cache -xj

  sed -i -e '/^RTEMS_VERSION/d' -e '/^RTEMS_BASE/d' epics-base/configure/os/CONFIG_SITE.Common.RTEMS
  cat << EOF >> epics-base/configure/os/CONFIG_SITE.Common.RTEMS
RTEMS_VERSION=$RTEMS
RTEMS_BASE=/home/travis/.cache/rtems${RTEMS}-i386
EOF
  cat << EOF >> epics-base/configure/CONFIG_SITE
CROSS_COMPILER_TARGET_ARCHS+=RTEMS-pc386
EOF

  # find local qemu-system-i386
  export PATH="$HOME/.cache/qemu/usr/bin:$PATH"
  echo -n "Using QEMU: "
  type qemu-system-i386 || echo "Missing qemu"
  EXTRA=RTEMS_QEMU_FIXUPS=YES
fi

make -j2 -C epics-base $EXTRA

if [ "$BRBASE" ]
then
  make -j2 -C pvData $EXTRA
  make -j2 -C pvAccess $EXTRA
  make -j2 -C normativeTypes $EXTRA
  make -j2 -C pvaClient $EXTRA
  make -j2 -C pvDatabase $EXTRA
  make -j2 -C pva2pva $EXTRA
fi
