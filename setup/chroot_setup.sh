#!/bin/sh

ln -sf /usr/share/zoneinfo/America/Montevideo /etc/localtime
hwclock --systohc

sed -i 's/#\(en_US\.UTF-8 UTF-8\)/\1/' /etc/locale.gen
sed -i 's/#\(es_UY\.UTF-8 UTF-8\)/\1/' /etc/locale.gen
locale-gen

pacman --needed -S grub os-prober efibootmgr
grub-install --target=x86_64-efi --efi-directory=/boot --bootloader-id=grub
grub-mkconfig -o /boot/grub/grub.cfg

passwd
useradd -m -G wheel gonchi
passwd gonchi

echo 'artix' > /etc/hostname
cat <<eof >>/etc/hosts
127.0.0.1	localhost
::1			localhost
127.0.1.1	artix.localdomain artix
eof

# TODO: setup /etc/sudoers
