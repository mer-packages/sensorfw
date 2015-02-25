Name:       sensorfw
Summary:    Sensor Framework
Version:    0.8.6
Release:    0
Group:      System/Sensor Framework
License:    LGPLv2+
URL:        http://gitorious.org/sensorfw
Source0:    %{name}-%{version}.tar.bz2
Source1:    sensorfw-rpmlintrc
Requires:   qt
Requires:   sensorfw-qt5
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  pkgconfig(QtCore)
BuildRequires:  pkgconfig(mlite5)
BuildRequires:  pkgconfig(QtDeclarative)
BuildRequires:  pkgconfig(contextprovider-1.0)

%description
Sensor Framework provides an interface to hardware sensor drivers through logical sensors. This package contains sensor framework daemon and required libraries.


%package devel
Summary:    Sensor framework daemon libraries development headers
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}
Requires:   qt-devel

%description devel
%{summary}.

%prep
%setup -q -n %{name}-%{version}

%build
unset LD_AS_NEEDED

%qmake

make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%qmake_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/libsensorclient.so.*
%{_libdir}/libsensordatatypes.so.*

%files devel
%defattr(-,root,root,-)
%{_libdir}/libsensordatatypes.so
%{_libdir}/libsensorclient*.so
%{_libdir}/pkgconfig/*
%{_includedir}/sensord/*
%{_datadir}/qt4/mkspecs/features/sensord.prf

