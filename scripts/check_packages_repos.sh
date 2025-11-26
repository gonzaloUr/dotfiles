#!/bin/sh
#
# Lista que paquetes son instalados de los repos de arch linux.
#
# Si un paquete tiene el mismo nombre en varios repos este aparecera installado en todos los repos, este script busca 
# identificar de que repo viene el paquete instalado comparando la info del paquete local y los paquetes remotos de 
# igual nombre, identificando paquetes de arch linux que podrian contener dependencias a systemd.
#
# Observar que en pacman no hay hashes por paquete, por lo que lo unico posible es matchear la info de cada uno.
#
# Aparentemente pacman -Qi en archilinux retorna un campo repository pero artix no.

tmpdir=$(mktemp -d)

# Obtener paquetes instalados de repos y no de AUR.
explicit="$(pacman -Qqen)"

echo "Se comparan campos de los paqutes locales con los de los repos remotos"
echo "Si el repo tiene una versión mas actualizada puede tener campos distintos"
echo

# Para cada paquete instalado explicitamente
for x in $explicit
do
    # Obtener fields de la base de datos de paqutes instalados locales y remover lineas en blanco.
    pkg_fields="$(expac -Q '%p|%b' "$x" | sed '/^$/d')"

    # Obtener fields de la base de datos de paqutes remotos con su repo y remover lineas en blanco.
    expac -S '%r|%p|%b' "$x" | sed '/^$/d' > "$tmpdir/pkg_fields_remote"

    found=0

    # Para cada paqute remoto que matchea el nombre del paqute local chequear si los campos son iguales.
    while IFS= read -r pkg_fields_remote
    do
        repo=$(printf '%s\n' "$pkg_fields_remote" | cut -d '|' -f1)
        pkg_fields_remote_no_repo=$(printf '%s\n' "$pkg_fields_remote" | cut -d '|' -f2-)

        if [ "$pkg_fields_remote_no_repo" = "$pkg_fields" ]; then
            echo "package $x matchea con repo: $repo"
            found=1
        fi
    done < "$tmpdir/pkg_fields_remote"

    # Si no se encuentra ningun remoto que matchee, mostrar una advertencia.
    if [ "$found" -eq 0 ]
    then
        echo "package $x no matchea con ningun repo, actualizar este paquete para que matchee con los repos remotos"
    fi
done

rm -rf "$tmpdir"
