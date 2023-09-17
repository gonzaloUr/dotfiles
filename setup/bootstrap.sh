#!/bin/sh
#
# Este script configura un sistema artix recien instalado para un usuario, se corre como root.
# Esta pensado para ser un resumen de los docs de installacion o un script.

# Setea /etc/localtime.
ln -sf /usr/share/zoneinfo/America/Montevideo /etc/localtime

# Genera /etc/adjtime.
hwclock --systohc

# Configurar los locales.
#
# Los locales son archivos que especifican cosas relacionadas al idioma, letras, etc y son
# consumidos por glibc entre otros. Estan especificados por posix.

# Descomentar los locales a generar en /etc/locale.gen (se puede usar un editor, esto es un ejemplo).
sed -i 's/^#\(en_US\.UTF-8 UTF-8\)/\1/' /etc/locale.gen
sed -i 's/^#\(es_UY\.UTF-8 UTF-8\)/\1/' /etc/locale.gen
grep -v '#.*' /etc/locale.gen

# Llamar a locale-gen
locale-gen

# Setear variables de entorno sobre locales, utilizando el /etc/locale.conf
echo 'LANG=en_US.UTF-8' >> /etc/locale.conf

# Installar boot manager para uefi.
pacman -S grub os-prober efibootmgr
grub-install --target=x86_64-efi --efi-directory=/boot --bootloader-id=grub
grub-mkconfig -o /boot/grub/grub.cfg

# Configurar usuario
useradd -m gonchi
passwd gonchi
usermod -a -G wheel gonchi
usermod -a -G video gonchi

# Configurar sudo (ver comentarios en el mismo archivo).
visudo
