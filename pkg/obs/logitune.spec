Name:           logitune
Version:        0.1.2
Release:        1%{?dist}
Summary:        Logitech device configurator for Linux
License:        GPL-3.0-or-later
URL:            https://github.com/mmaher88/logitune
Source0:        logitune-%{version}.tar.xz

BuildRequires:  cmake >= 3.22
BuildRequires:  gcc-c++
BuildRequires:  ninja-build

# Qt 6
%if 0%{?suse_version}
BuildRequires:  qt6-base-devel
BuildRequires:  qt6-declarative-devel
BuildRequires:  qt6-svg-devel
BuildRequires:  qt6-shadertools-devel
BuildRequires:  systemd-devel
%else
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qtdeclarative-devel
BuildRequires:  qt6-qtsvg-devel
BuildRequires:  qt6-qtshadertools
BuildRequires:  systemd-devel
%endif

Requires:       qt6-qtbase
Requires:       qt6-qtdeclarative
Requires:       qt6-qtsvg

%description
Logitune is a Logitech Options+ alternative for Linux. Configure HID++
peripherals like the MX Master 3S: per-app profiles, button remapping,
DPI, SmartShift, scroll, gesture, and thumb wheel settings.

%prep
%autosetup -n logitune-%{version}

%build
%cmake -G Ninja -DBUILD_TESTING=OFF
%cmake_build

%install
%cmake_install

%files
%{_bindir}/logitune
%{_prefix}/lib/udev/rules.d/71-logitune.rules
%{_datadir}/applications/logitune.desktop
%{_datadir}/icons/hicolor/scalable/apps/com.logitune.Logitune.svg
%dir %{_datadir}/gnome-shell
%dir %{_datadir}/gnome-shell/extensions
%dir %{_datadir}/gnome-shell/extensions/logitune-focus@logitune.com
%{_datadir}/gnome-shell/extensions/logitune-focus@logitune.com/metadata.json
%dir %{_datadir}/gnome-shell/extensions/logitune-focus@logitune.com/v42
%{_datadir}/gnome-shell/extensions/logitune-focus@logitune.com/v42/extension.js
%dir %{_datadir}/gnome-shell/extensions/logitune-focus@logitune.com/v45
%{_datadir}/gnome-shell/extensions/logitune-focus@logitune.com/v45/extension.js
