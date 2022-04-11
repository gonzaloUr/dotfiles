#!/bin/sh
ln -sf /usr/share/zoneinfo/America/Montevideo /etc/localtime
hwclock --systohc

pacman -S nano
nano /etc/locale.gen
locale-gen
touch /etc/locale.conf
echo "LANG=en_US.UTF-8" >> /etc/locale.conf # locales de /etc/locale.gen descomentadas

pacman -S grub os-prober efibootmgr
grub-install --target=x86_64-efi --efi-directory=/boot --bootloader-id=grub
grub-mkconfig -o /boot/grub/grub.cfg
