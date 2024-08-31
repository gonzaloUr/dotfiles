## Notas

### Usar gnome-keyring con VSCode
VSCode determina que keyring utilizar fijandose en que DE esta corriendo. En dwm no funciona porque VSCode solo conoce DE populares. Para forzar a que use gnome-keyring:
- Abrir command pallete Ctrl+Shift+P
- Buscar `> Preferences: Configure Runtime Arguments command.`
- Agregar a argv.json file la propiedad `"password-store": "gnome-libsecret"`

### Chequear que gnome-keyring expone en dbus la API de Secrets
Usar secret-tool, que viene con la libreria de libsecret:
```sh
secret-tool store --label="MySecret" my password
secret-tool lookup my MySecret
secret-tool clear my MySecret
```

## Mejoras

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
