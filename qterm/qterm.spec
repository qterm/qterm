# This spec file was generated using Kpp
# If you find any problems with this spec file please report
# the error to ian geiser <geiseri@msoe.edu>
Summary:   %%info%%
Name:      qterm
Version:   0.3.4
Release:   %%release%%
Copyright: GPL
Vendor:    kingson fiasco hooey<xiaokiangwang@yahoo.com.cn yinshouyi@peoplemail.com.cn hephooey@hotmail.com>
URL:       http://qterm.gnuchina.org

Packager:  kingson fiasco hooey<xiaokiangwang@yahoo.com.cn yinshouyi@peoplemail.com.cn hephooey@hotmail.com>
Group:     internet
Source:    qterm-0.3.4.tar.gz
BuildRoot: %%buildroot%%

%description
QTerm is a BBS client in Linux

%prep
%setup
CFLAGS="$RPM_OPT_FLAGS" CXXFLAGS="$RPM_OPT_FLAGS" ./configure \
                 \
                $LOCALFLAGS
%build
# Setup for parallel builds
numprocs=`egrep -c ^cpu[0-9]+ /proc/stat || :`
if [ "$numprocs" = "0" ]; then
  numprocs=1
fi

make -j$numprocs

%install
make install-strip DESTDIR=$RPM_BUILD_ROOT

cd $RPM_BUILD_ROOT
find . -type d | sed '1,2d;s,^\.,\%attr(-\,root\,root) \%dir ,' > $RPM_BUILD_DIR/file.list.qterm
find . -type f | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.qterm
find . -type l | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.qterm

%clean
rm -rf $RPM_BUILD_ROOT/*
rm -rf $RPM_BUILD_DIR/qterm
rm -rf ../file.list.qterm


%files -f ../file.list.qterm
