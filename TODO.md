## Mejoras

### Considerar remover de los configs cosas que tuve que agregar por instalar programas no descritos en setup
Actualmente my zshrc tiene configs the pyenv y en el PATH hay cosas the ghcup.
Capaz no es necesario removerlos pero cambiar el scripting para que sea opcional, como lo hice con ocaml, que chequeo si opam esta disponible como comando.

### Config file para librewolf.
Ver si existe y crear una config incluso con mis bookmarks y eso.

### Configurar backups con rclone o rsync y guardar en estos contraseñas, llaves ssh y gnugpg.
Crear alguna forma de cordinar backups con estas heramientas de forma tal que cada sitio de backup sea como un "repo de git", indepeidente de otros backups e inclusive capaz que permita excluir ciertos archivos etc.
Capaz cada backup tenga un archivo encriptado, un script o un config file, que describa a donde hacer rsycn/rclone de que archivos.
Guardar llaves y contraseñas encriptadas en cada backup con una contraseña maestra.

### Ver si se puede usar pass como password manager en firefox y otras aplicaciones
Tener un password manager e integralo con lo mas posible, usar pass obviamente y backaupearlo encryptado en mis backups con una password maestra.

### Capaz usar light y remover udev rule para permitir modificar el brightness para los usuarios en el grupo video
La utilidad light ya agrega udev rules iguales y es mas practica.
Sino En la archwiki dice que es mejor en /etc/udev/rules.d/

### Crear un programa que genere la barra de estados para dwm en vez de usar scripts
Actualmente uso un programa mio que me perimite conbinar la salida de varios comandos que generar streams de texto relacionados a ciertos daemos, por ej: pulseaudio, y redirecionarla a mas de un programa, como tee pero con multiples salidas. Lo ideal sería crear un programa que utilize las librerias de C para cada daemon y cree la barra de estados.

Información util a agregar a la barra:
- Sink y Source actual de pulseaudio.
- Consumo de memoria/cpu
- Proceso mas demandante en mem/cpu 
- Listado de procesos: bajo sv, daemons, bajo mi sesion de xorg, etc.
- Wifi y bluetooth.

### Mejorar los scripts que permiten montar archivos, conectar monitores, conectar wifi/bluetooth, etc
- smtpf
- mount
- xorg displays
- wifi/bluetooth connect

### Crear mas scripts para verificar paquetes
- Un script para listar los paquetes instalados que no estan en los scripts de setup.
- Una forma de ver todos los paquetes instalados indicando si son huerfanos, si pertenecen a un grupo como xorg, a que repo pertenecen, etc.
- Alguna forma de verificar que la instalación de python del sistema esta "prestine", que no existen modulos instalados a travez de pip install. Puede ser que si los haya de paquetes de la forma python-.
- Analogo a lo anterior pero para paquetes de pacman. Poder descubrir si despues de eliminar un paquete que genera archivos en /usr, /etc, etc estos archivos son realmente eliminados.

### No utilizar gnome-keyring para dbus secrets, muy bloteado o al menos utilizar lo minimo de este o hacerlo opciona (no prenderlo hasta que sea necesario).
Buscar alternativas o utilizar solo la funcionalidad de dbus secrets.

### Buscar una forma de mostrar que procesos crearon deamons sin pid padre.
Quiero relacionar los deamons sin padre con servicios de runit.

### Areglar el clipboard manager
Podria ser que esta vez corre por debajo de xdg-autostart o lo que sea.

### Crear una forma de administrar mejor documentos.
- Organizar el home mejor, intentar que las dot folders sean XDG compatible.
- Tener carpetas especificas y configurar todo alrededor de ellas.
