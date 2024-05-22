TODO:
- Crear script the bootstraping para artix y probarlo varias veces
  ```sh
  # comandos para la ultima vez que instale artix.

  # obtener cantidad de ram del equipo, se utiliza para determinar el tamaño del swap.
  free -h

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
  # Particion UEFI - cfdisk type EFI, tamaño 500MB (id ef) (Para que haya espacio para mas boot loaders, ej: multiboot con windows)
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
  # export LANG="en_US.UTF-8"
  # export LC_COLLATE="C"
  vi /etc/locale.conf
  
  # Para ver todas las variables de locale seteadas:
  locale

  # Boot loader (si al ejecutar el segundo comando muestra "EFI variables not supported on EFI system" es porque no se booteo con UEFI desde el usb)
  pacman -S grub os-prober efibootmgr
  grub-install --target=x86_64-efi --efi-directory=/boot/efi --bootloader-id=grub
  grub-mkconfig -o /boot/grub/grub.cfg
  ```
