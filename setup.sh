#!/bin/sh

# essentials
xbps-install -y curl
xbps-install -y wget
xbps-install -y unzip
xbps-install -y zsh
xbps-install -y cronie
xbps-install -y NetworkManager
xbps-install -y bluez
xbps-install -y tlp

# xorg
xbps-install -y xorg
xbps-install -y dunst
xbps-install -y xcompmgr
xbps-install -y xwallpaper
xbps-install -y xbacklight
xbps-install -y sxhkd

# multimedia
xbps-install -y alsa-utils
xbps-install -y alsa-plugins-pulseaudio
xbps-install -y pulseaudio
xbps-install -y ncpamixer

# utilities
xbps-install -y fzf
xbps-install -y entr
xbps-install -y simple-mtpfs
xbps-install -y acpi

# programs
xbps-install -y vim-huge
xbps-install -y lf
xbps-install -y firefox
xbps-install -y mpv
xbps-install -y sxiv
xbps-install -y zathura
xbps-install -y zathura-pdf-mupdf
xbps-install -y git
xbps-install -y tig
xbps-install -y htop
xbps-install -y calcurse
xbps-install -y shellcheck
xbps-install -y scrot

# src dependencies
xbps-install -y base-devel
xbps-install -y libX11-devel
xbps-install -y libXinerama-devel
xbps-install -y libXft-devel
xbps-install -y libXrandr-devel

# fonts
xbps-install -y font-sourcecodepro

# setup acpid service
[ -e /var/service/acpid ] || ln -s /etc/sv/acpid /var/service

# setup tlp service
[ -e /var/service/tlp ] || ln -s /etc/sv/tlp /var/service

# setup NetworkManager service
[ -e /var/service/dhcpcd ] && rm /var/service/dhcpcd
[ -e /var/service/wpa_supplicant ] && rm /var/service/wpa_supplicant
[ -e /var/service/wicd ] && rm /var/service/wicd
[ -e /var/service/dbus ] || ln -s /etc/sv/dbus /var/service
[ -e /var/service/NetworkManager ] || ln -s /etc/sv/NetworkManager /var/service

# setup bluetoothd service
[ -e /var/service/bluetoothd ] || ln -s /etc/sv/bluetoothd /var/service

# setup alsa service
[ -e /var/service/alsa ] || ln -s /etc/sv/alsa /var/service
