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
### Nerd fonts
Las versiones de fuentes con nerdfont icons que son mono en general los iconos de nerd se ven muy chicos.
Fijarse si fc-list dice si la fuente es mono, si lo es lo mas probable es que los iconos sean pequeños.

### DBus
dbus ofrece buses IPC para sistema (iniciado por runit) y usuario/sesion (iniciado por xinit script), ademas de funcionar como IPC tiene otras features. 
Es configurables globalmente atravez de los archivos `/usr/share/dbus-1`.

En particular tiene activation. En los configs de dbus `/usr/share/dbus-1/` existen archivos `.service`, bajo carpetas como `services`, `system-services`
y `accessibility-services`, que describen un provedor de alguna interfaz (nombre de dbus). Si no existe un probedor de dicha interfaz actualmente en el bus, dbus 
automaticamente corre el deamon descrito en el `.service`. Este es el caso de los siguientes deamons que si uno se fija en htop, estan aparentados por PID 1 (runit)
debido a ser iniciados por dbus, es decir son iniciados por el mecanismo de activation:

- polkitd (system bus): permite que programas no privilegiados hagan cosas privilegiadas.
- rtkit-deamon (system bus): permite que programas modifiquen prioridades de scheduling sobre procesos.
- at-spi/at-spi2 (session bus): servicio de acesibilidad, en general se inicia porque librewolf lo necesita
- dconf-service (session bus): permite configurar propiedades de GNOME/GTK atravez de dbus.

Dbus también tiene politicas de seguridad para cada bus configurables en `/usr/share/dbus-1/`, estas permiten denegar o permitir acceso a ciertos servicios según
distintas propiedades como grupo o usuario. También existen archivos que describen interfaces posiblemente expuestas por distintos deamons, estan en la carpeta
`interfaces`. Se utilizan como documentación y no es necesario que cada deamon con apis en dbus installe archivos en esta carpeta. Por ultimo también existe la posiblidad
de que deamons se comuniquen por dbus entre si para remplazar uno al otro, esto se llama replacment.

Los archivos de configuración de dbus son los siguientes:

```sh
% tree --noreport -L 1 -F /usr/share/dbus-1
/usr/share/dbus-1/
├── accessibility-services/
├── interfaces/
├── services/
├── session.conf
├── session.d/
├── system.conf
├── system.d/
└── system-services/
```

Las carpetas `accessibility-services`, `services` y `system-services` contiene archivos `.service` para el system bus (`system-services`) y para los buses
de sesion por usuario (el resto). Las los archivos `system.conf` y `session.conf`  contienen las configuraciones de los buses de sistema y sesion. Por ultimo
las carpetas `system.d` y `session.d` contiene configuraciones de politicas para el bus de sistema y el bus de session.

No solo los deamons mencionados anteriormente existen, por ejemplo elogid, gnome-keyring, pulseaudio, etc exponen en dbus. 
Para ver los deamons que exponen en dbus ejecutar lo siguiente:

```sh
busctl --user
busctl --system
```

### PAM

PAM es una libreria que permite que programas llamen a codigo externo que maneje la autenticación. Estos modulos son configurables en `/etc/pam.d`.
Una vez dado el okay por el modulo en cuestion, el proceso cambia de UID o se le permite abrir ciertos archivos, etc.

Algunos modulos de PAM se comunican atravez de dbus con deamons, inician buses, o incluso logean logins.
