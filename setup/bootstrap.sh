#!/bin/sh

# obtener cantidad de ram del equipo, se utiliza para determinar el tamaño del swap.
free -h

# obtener tipo de disco (MBR o GPT)
# aparece como Disklabel type, si es dos entonces es MBR
# queremos que sea GPT, porque es mas nuevo y permite mas cantidad de particiones
sudo fdisk -l /dev/sda

# si es mbr convertir a gpt, (borra todos los datos)
sudo fdisk /dev/sda

# Particionar el disco
#
# Si la cantidad de particiones primarias luego de particionar va a exceder 4 particiones principales primero crear una particion de tipo extended con todo el espacio a utilizar para linux,
# luego esta particion podra tener tantas particiones logicas como sea necesario. Para hacer esto en cfdisk selecionar el espacio vacio, presionar new en la parte inferior, ingresar
# el tamaño total deseable para linux. Luego en la parte de abajo saldran dos opciones: primary o extended. Selecionar extended y crear todas las particiones de linux como logicas bajo esta
# particion primaria. Una vez creada la particion de tipo extended, esta mostrara las particiones logicas o espacio libre de esta como un subnodo, como si se tratara de un explorador
# de archivos.
#
# La particion UEFI debe ser primaria, por lo que antes de particionar deben haber como maximo 2 particiones primarias.
# 
# Particion UEFI - cfdisk type EFI, tamaño 4Gib (Para que haya espacio para mas boot loaders, ej: multiboot con windows)
# Particion SWAP - cfdisk type Linux swap / Solaris (id 82), para el tamaño seguir recomendaciones de https://access.redhat.com/documentation/es-es/red_hat_enterprise_linux/7/html/storage_administration_guide/ch-swapspace
# Particion ROOT - cfdisk type Linux (id 83), tamaño tanto como se quiera
sudo cfdisk /dev/sda

# Formatear particiones agregando labels para que sea mas sencillo, las particiones /dev/... dependen del paso anterior.
sudo mkfs.ext4 -L ROOT /dev/sda6
sudo mkswap -L SWAP /dev/sda5
sudo mkfs.fat -F 32 /dev/sda1
sudo fatlabel /dev/sda1 ESP

# Montar particiones
sudo swapon /dev/disk/by-label/SWAP
sudo mount /dev/disk/by-label/ROOT /mnt
sudo mkdir -p /mnt/boot/efi
sudo mount /dev/disk/by-label/ESP /mnt/boot/efi

# Conectarse a internet usando connmanctl
sudo connmanctl

# En connmanctl ejecutar lo siguiente
# > enable wifi
# > scan wifi
# > services
# * se muestran todas las redes wifi con SSID y un id que empieza con wifi_f4... *
# > agent on
# > connect wifi_f4...
# * ingresar contraseña *
# > exit

# Verificar que estoy conectado
ip a

# Instalar base system
basestrap /mnt base base-devel runit elogind-runit

# Instalar kernel y firmware para varios dispositivos
basestrap /mnt linux linux-firmware

# Generar fstab utilizado para montar particiones al iniciar
fstabgen -U /mnt >> /mnt/etc/fstab

# Hacer chroot
artix-chroot /mnt

# Settear timezone
ln -sf /usr/share/zoneinfo/America/Montevideo /etc/localtime

# Generar /etc/adjtime
hwclock --systohc

# Generar locales, primero installar un editor
pacman -S vi

# Descomentar locales a generar
vi /etc/locale.gen

# Finalmente generar locales
locale-gen

# Configurar los locales del sistema
#
# ejemplo de archivo:
# 
# LANG=en_US.UTF-8
vi /etc/locale.conf

# Para ver todas las variables de locale seteadas:
locale

# Instalar boot loader (si al ejecutar el segundo comando muestra "EFI variables not supported on EFI system" es porque no se booteo con UEFI desde el usb)

# Chequear que se booteo con efi
pacman -S grub os-prober efibootmgr

# Si al ejecutar el primer comando muestra "EFI variables not supported on EFI system" es porque no se booteo con UEFI desde el usb  
grub-install --target=x86_64-efi --efi-directory=/boot/efi --bootloader-id=grub
grub-mkconfig -o /boot/grub/grub.cfg

# Cambiar constraseña de root
passwd

# Crear usuario
useradd -m gonchi
passwd gonchi

# Agregar usuario a grupos, ej de grupos:
#
# root:x:0:root
# sys:x:3:bin
# mem:x:8:
# ftp:x:11:
# mail:x:12:
# log:x:19:
# smmsp:x:25:
# proc:x:26:
# games:x:50:
# lock:x:54:
# network:x:90:
# floppy:x:94:
# scanner:x:96:
# power:x:98:
# nobody:x:65534:
# adm:x:999:daemon
# wheel:x:998:gonchi
# utmp:x:997:
# audio:x:996:
# disk:x:995:
# input:x:994:
# kmem:x:993:
# kvm:x:992:
# lp:x:991:
# optical:x:990:
# render:x:989:
# sgx:x:988:
# storage:x:987:
# tty:x:5:
# uucp:x:986:
# video:x:985:
# users:x:984:
# groups:x:983:
# rfkill:x:982:
# bin:x:1:daemon
# daemon:x:2:bin
# http:x:33:
# dbus:x:81:
# tss:x:981:
# uuidd:x:68:
# gonchi:x:1000:
# polkitd:x:102:
# dhcpcd:x:980:
#
# Ver artix linux configuration guide.

usermod -a -G wheel,video,audio,input,power,storage,optical,lp,scanner,dbus gonchi

# Agregar a suoders el grupo wheel agregando lo siguiente a /etc/suoders (en general esta comentado)
# %wheel ALL=(ALL:ALL) NOPASSWD: ALL
visudo

# en /etc/hostname escribir:
# artix
vi /etc/hostname

# en /etc/hosts escribir:
# 127.0.0.1        localhost
# ::1              localhost
# 127.0.1.1        artix.localdomain  artix
vi /etc/hosts

# instalar network manager (instala wpa_supplicant y tiene un dhcp interno)
pacman -S networkmanager networkmanager-runit

# rebootear y logearse con el nuevo usuario

# activar networkmanager
sudo ln -s /etc/runit/sv/NetworkManager /run/runit/service
