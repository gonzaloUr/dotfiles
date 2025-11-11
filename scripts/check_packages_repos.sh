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

# Obtener paquetes instalados de repos y no de AUR.
explicit="$(pacman -Qqen | tr '\n' ' ')"

# Para cada paquete instalado explicitamente
for x in $explicit
do
    echo "package: $x"

    # Obtener fields de la base de datos de paqutes instalados locales y remover lineas en blanco.
    pkg_fields="$(expac -Q '%p|%b' "$x" | sed '/^$/d')"

    # Obtener fields de la base de datos de paqutes remotos con su repo y remover lineas en blanco.
    pkgs_fields_remote="$(expac -S '%r|%p|%b' "$x" | sed '/^$/d')"

    echo "$pkgs_fields_remote" | while IFS= read -r pkg_fields_remote
    do
        # Obtener repo.
        repo="$(echo "$pkg_fields_remote" | cut -d '|' -f1)"

        # Remover repo.
        pkg_fields_remote_no_repo="$(echo "$pkg_fields_remote" | cut -d '|' -f2-)"

        # Comparar fields locales con los del repo, si son iguales informar.
        if [ "$pkg_fields_remote_no_repo" = "$pkg_fields" ]
        then
            echo "matchea con repo: $repo"
        fi
    done
done
