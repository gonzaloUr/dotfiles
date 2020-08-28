#!/bin/sh

# essentials
xbps-install -y zsh
xbps-install -y cronie
xbps-install -y NetworkManager

# setup acpid service
[ -e /var/service/acpid ] || ln -s /etc/sv/acpid /var/service

# setup NetworkManager service
[ -e /var/service/dhcpcd ] && rm /var/service/dhcpcd
[ -e /var/service/wpa_supplicant ] && rm /var/service/wpa_supplicant
[ -e /var/service/wicd ] && rm /var/service/wicd
[ -e /var/service/dbus ] || ln -s /etc/sv/dbus /var/service
[ -e /var/service/NetworkManager ] || ln -s /etc/sv/NetworkManager /var/service

# xorg
xbps-install -y xorg

# multimedia
xbps-install -y alsa-utils
xbps-install -y alsa-plugins-pulseaudio
xbps-install -y pulseaudio
xbps-install -y pavucontrol

# setup alsa service
[ -e /var/service/alsa ] || ln -s /etc/sv/alsa /var/service

# utilities
xbps-install -y curl
xbps-install -y wget
xbps-install -y unzip
xbps-install -y fzf
xbps-install -y entr
xbps-install -y simple-mtpfs

# programs
xbps-install -y vim-x11
xbps-install -y lf
xbps-install -y chromium
xbps-install -y mpv
xbps-install -y sxiv
xbps-install -y zathura
xbps-install -y zathura-pdf-mupdf
xbps-install -y git
xbps-install -y tig
xbps-install -y htop
xbps-install -y calcurse

# src dependencies
xbps-install -y base-devel
xbps-install -y libX11-devel
xbps-install -y libXinerama-devel
xbps-install -y libXft-devel
