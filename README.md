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
