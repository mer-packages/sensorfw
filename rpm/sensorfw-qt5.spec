Name:       sensorfw-qt5
Summary:    Sensor Framework Qt5
Version:    0.7.3.38
Release:    0
Group:      System/Sensor Framework
License:    LGPLv2+
URL:        http://gitorious.org/sensorfw
Source0:    %{name}-%{version}.tar.bz2
Source1:    sensorfw-rpmlintrc
Source2:    sensorfwd.service
Source3:    sensord-daemon-conf-setup
Requires:   qt5-qtcore
Requires:   sensord-configs
Requires:   systemd
Requires(preun): systemd
Requires(post): /sbin/ldconfig
Requires(post): systemd
Requires(postun): /sbin/ldconfig
Requires(postun): systemd
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Network)
BuildRequires:  pkgconfig(Qt5Test)
BuildRequires:  pkgconfig(mlite5)
BuildRequires:  systemd
Provides:   sensord-qt5
Obsoletes:   sensorframework

%description
Sensor Framework provides an interface to hardware sensor drivers through logical sensors. This package contains sensor framework daemon and required libraries.


%package devel
Summary:    Sensor framework daemon libraries development headers
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}
Requires:   qt5-qtcore-devel
Requires:   qt5-qtdbus-devel
Requires:   qt5-qtnetwork-devel

%description devel
Development headers for sensor framework daemon and libraries.


%package tests
Summary:    Unit test cases for sensord
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}
Requires:   qt5-qttest-devel
Requires:   testrunner-lite
Requires:   python
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description tests
Contains unit test cases for CI environment.


%package configs
Summary:    Sensorfw configuration files
Group:      System/Libraries
BuildArch:  noarch
Requires:   %{name} = %{version}
Provides:   sensord-configs
Provides:   config-n900
Provides:   config-aava
Provides:   config-icdk
Provides:   config-ncdk
Provides:   config-oemtablet
Provides:   config-oaktraili
Provides:   config-u8500

%description configs
Sensorfw configuration files.


%prep
%setup -q -n %{name}-%{version}

%build
unset LD_AS_NEEDED
export LD_RUN_PATH=/usr/lib/sensord-qt5/
export QT_SELECT=5

%qmake5  \
    CONFIG+=mce

make %{?_smp_mflags}

%install
rm -rf %{buildroot}
export QT_SELECT=5
%qmake5_install

install -D -m644 %{SOURCE2} $RPM_BUILD_ROOT/%{_unitdir}/sensorfwd.service
install -D -m750 %{SOURCE3} $RPM_BUILD_ROOT/%{_bindir}/sensord-daemon-conf-setup

mkdir -p %{buildroot}/%{_unitdir}/graphical.target.wants
ln -s ../sensorfwd.service %{buildroot}/%{_unitdir}/graphical.target.wants/sensorfwd.service

%preun
if [ "$1" -eq 0 ]; then
systemctl stop sensorfwd.service || :
fi

%post
/sbin/ldconfig
systemctl daemon-reload || :
systemctl reload-or-try-restart sensorfwd.service || :

%postun
/sbin/ldconfig
systemctl daemon-reload || :

%post tests -p /sbin/ldconfig

%postun tests -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/libsensorclient-qt5.so.*
%{_libdir}/libsensordatatypes-qt5.so.*
%attr(755,root,root)%{_sbindir}/sensorfwd
%dir %{_libdir}/sensord-qt5
%{_libdir}/sensord-qt5/*.so
%{_libdir}/libsensorfw*.so.*

%config %{_sysconfdir}/dbus-1/system.d/sensorfw.conf
%dir %{_sysconfdir}/sensorfw
%config %{_sysconfdir}/sensorfw/sensord.conf
%dir %{_sysconfdir}/sensorfw/sensord.conf.d/
%{_unitdir}/sensorfwd.service
%{_unitdir}/graphical.target.wants/sensorfwd.service
%{_bindir}/sensord-daemon-conf-setup

%files devel
%defattr(-,root,root,-)
%{_libdir}/libsensorfw*.so
%{_libdir}/libsensordatatypes*.so
%{_libdir}/libsensorclient*.so
%{_libdir}/pkgconfig/*
%{_includedir}/sensord-qt5/*
%{_datadir}/qt5/mkspecs/features/sensord.prf

%files tests
%defattr(-,root,root,-)
%{_libdir}/libsensorfakeopen*.so
%{_libdir}/libsensorfakeopen*.so.*
%dir %{_libdir}/sensord-qt5/testing
%{_libdir}/sensord-qt5/testing/*
%dir %{_datadir}/sensorfw-tests
%attr(755,root,root)%{_datadir}/sensorfw-tests/*.p*
%attr(644,root,root)%{_datadir}/sensorfw-tests/*.xml
%attr(644,root,root)%{_datadir}/sensorfw-tests/*.conf
%attr(755,root,root)%{_bindir}/datafaker-qt5
%attr(755,root,root)%{_bindir}/sensoradaptors-test
%attr(755,root,root)%{_bindir}/sensorapi-test
%attr(755,root,root)%{_bindir}/sensorbenchmark-test
%attr(755,root,root)%{_bindir}/sensorchains-test
%attr(755,root,root)%{_bindir}/sensordataflow-test
%attr(755,root,root)%{_bindir}/sensord-deadclient
%attr(755,root,root)%{_bindir}/sensordiverter.sh
%attr(755,root,root)%{_bindir}/sensordriverpoll-test
%attr(755,root,root)%{_bindir}/sensordummyclient-qt5
#%attr(755,root,root)%{_bindir}/sensorexternal-test
%attr(755,root,root)%{_bindir}/sensorfilters-test
%attr(755,root,root)%{_bindir}/sensormetadata-test
%attr(755,root,root)%{_bindir}/sensorpowermanagement-test
%attr(755,root,root)%{_bindir}/sensorstandbyoverride-test
%attr(755,root,root)%{_bindir}/sensortestapp

%files configs
%defattr(-,root,root,-)
%config %{_sysconfdir}/sensorfw/sensord.conf.d/*conf
%config %{_sysconfdir}/sensorfw/*conf
%exclude %{_sysconfdir}/sensorfw/sensord.conf

