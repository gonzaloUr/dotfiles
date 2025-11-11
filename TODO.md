## Mejoras

### Config file para librewolf.
Ver si existe y crear una config incluso con mis bookmarks y eso.

### Password manager y llaves ssh.
Tener un password manager y capaz un script para imprimirlas a papel???.

### Capaz remover udev rule para permitir modificar el brightness para los usuarios en el grupo video
La utilidad light ya agrega udev rules iguales.

### Cambiar lugar donde installar udev rule para backlight
En la archwiki dice que es mejor en /etc/udev/rules.d/

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

### No utilizar gnome-keyring para dbus secrets, muy bloteado.
Buscar alternativas o utilizar solo la funcionalidad de dbus secrets porque actualmente tambien uso el ssh-agent de gnome-keyring.

### Buscar una forma de mostrar que procesos crearon deamons sin pid padre.
Quiero relacionar los deamons sin padre con servicios de runit.

### Utilizar tree-sitter en vez de el highlighting de vim 
nota: neovim depende de el paquete tree-sitter el cual se relaciona con otros paquetes que instalan gramaticas para cada lenjuage, por ejemplo: tree-sitter-c. Alugnas de estas gramaticas ya vienen con neovim pero otras no, capaz agregar en el script de setup una sección de donde se instalan gramaticas extra.

### Areglar el clipboard manager
Podria ser que esta vez corre por debajo de xdg-autostart o lo que sea.

### Crear una forma de administrar mejor documentos y backups.
1. Organizar el home mejor, intentar que las dot folders sean XDG compatible.
2. Tener carpetas especificas y configurar todo alrededor de ellas.
3. Mejorar la manera de hacer backup de todas las carpetas del home, actualmente solo Dropbox es backapeada.
