## Mejoras

### Capaz remover udev rule para permitir modificar el brightness para los usuarios en el grupo video
La utilidad light ya agrega udev rules iguales.

### Cambiar lugar donde installar udev rule para backlight
En la archwiki dice que es mejor en /etc/udev/rules.d/

### Usar xdg-autostart o algún coso de user services como tunstile.
Tunstile hace otras cosas capaz xdg es mejor.

### Configurar FZF y capaz snipets para neovim
Puede ser util aunque pueda ser bloat.

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
- Una forma de ver todos los paquetes instalados indicando si son huerfanos, si pertenecen a un grupo como xorg, etc.
- Alguna forma de verificar que la instalación de python del sistema esta "prestine", que no existen modulos instalados a travez de pip install. Puede ser que si los haya de paquetes de la forma python-.

### No utilizar gnome-keyring para dbus secrets, muy bloteado.
Buscar alternativas.

### Solucionar el tema de que los caracteres unicode y nerd fonts (si se quiere usar) se salen del tamaño del cursor
En a configuracion actual para nerd font sourcecode, por ejemplo:  

### Arreglar lentitud en levantar una terminal y considerar utilizar una terminal menos bloat
Puede ser por zsh (en particular zshrc) o la terminal en si mismo. Alacritty esta escrita en rust y ademas usa la gpu o algo así, es medio bloat.

### Agregar config para picom o removerlo por ser medio bloteado
Funciona medio raro.

### Buscar una forma de mostrar que procesos crearon deamons sin pid padre.
Quiero relacionar los deamons sin padre con servicios de runit.

### Utilizar tree-sitter en vez de el highlighting de vim 
nota: neovim depende de el paquete tree-sitter el cual se relaciona con otros paquetes que instalan gramaticas para cada lenjuage, por ejemplo: tree-sitter-c. Alugnas de estas gramaticas ya vienen con neovim pero otras no, capaz agregar en el script de setup una sección de donde se instalan gramaticas extra.

### Areglar el clipboard manager
Podria ser que esta vez corre por debajo de xdg-autostart o lo que sea.

### Crear una forma de administrar mejor imagenes y texto
1. Capaz exista algo como mongo () pero mas chiquito.
2. En el shell poder "copiar" una imagen a una "galeria" de la computadora.
3. La "galeria" se encarga de organizar las imagenes segun su fecha, etc.
4. Buscar otras integraciones, capaz con firefox?: si estas descargando una imagen de firefox, entonces
   firefox llama a nuestro programa para organizar a donde va a ir esa imagen.
