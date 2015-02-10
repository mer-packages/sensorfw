Name:       hybris-libsensorfw-qt5
Summary:    Sensor Framework Qt5
Version:    0.8.4
Release:    0
Group:      System/Sensor Framework
License:    LGPLv2+
URL:        http://gitorious.org/sensorfw
Source0:    %{name}-%{version}.tar.bz2
# Eventhough these are really needed we have these here
# as those are added to packaging dir by tar_git.
Source1:    sensord-daemon-conf-setup
Source2:    sensorfw-rpmlintrc
Source3:    sensorfwd.service
Requires:   %{name}-configs
Requires:   sensord-qt5
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Network)
BuildRequires:  pkgconfig(Qt5Test)
BuildRequires:  pkgconfig(mlite5)
BuildRequires:  pkgconfig(sensord-qt5)
BuildRequires:  pkgconfig(libhardware)
BuildRequires:  pkgconfig(android-headers)

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


%package configs
Summary:    Sensorfw configuration files
Group:      System/Libraries
BuildArch:  noarch
Requires:   %{name} = %{version}
Provides:   sensord-configs
Obsoletes:  sensorfw-qt5-configs <= 0.7.3.31

%description configs
Sensorfw configuration files.

%prep
%setup -q -n %{name}-%{version}

%build
unset LD_AS_NEEDED
export LD_RUN_PATH=/usr/lib/sensord-qt5/
export QT_SELECT=5

%qmake5 CONFIG+=hybris

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
export QT_SELECT=5
%qmake5_install

%post
/sbin/ldconfig
%{_bindir}/sensord-daemon-conf-setup

%postun
/sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/libhybrissensorfw*.so.*
%{_libdir}/libhybrissensorfw*.so
%{_libdir}/sensord-qt5/*.so

%files devel
%defattr(-,root,root,-)
%{_includedir}/sensord-qt5/*

%files configs
%defattr(-,root,root,-)
%config %{_sysconfdir}/sensorfw/sensord-hybris.conf
