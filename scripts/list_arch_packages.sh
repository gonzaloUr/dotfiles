#!/bin/sh
#
# Lista que paquetes son instalados de los repos de arch linux.
#
# Si un paquete tiene el mismo nombre en varios repos este aparecera installado en todos los repos, este script busca 
# identificar de que repo viene el paquete instalado comparando la info del paquete local y los paquetes remotos de 
# igual nombre, identificando paquetes de arch linux que podrian contener dependencias a systemd.
#
# Observar que en pacman no hay hashes por paquete, por lo que lo unico posible es matchear la info de cada uno.

tmp="$(mktemp --directory)"

pacman -Qqm > "$tmp/foreign"
explicit="$(pacman -Qqe | grep -v -f "$tmp/foreign" | tr '\n' ' ')"

pacman -Qi $explicit |\
    grep -e '^Name' -e '^Version' -e '^Description' -e '^Packager' -e '^Build Date' |\
    sed -e 's/^Name *: //' -e 's/^Version *: //' -e 's/^Description *: //' -e 's/^Packager *: //' \
        -e 's/^Build Date *: //' |\
    paste -d ' ' - - - - - > "$tmp/local"

pacman -Si $explicit |\
    grep -e '^Name' -e '^Repository' -e '^Version' -e '^Description' -e '^Packager' \
         -e '^Build Date' |\
    sed -e 's/^Name *: //' -e 's/^Repository *: //' -e 's/^Version *: //' -e 's/^Description *: //' \
        -e 's/^Packager *: //' -e 's/^Build Date *: //' |\
    paste -d ' ' - - - - - - > "$tmp/remote"

for x in $explicit
do
    local_info="$(grep "^$x" "$tmp/local")"
    remote_info="$(grep -F "$local_info" "$tmp/remote")"

    if [ -z "$remote_info" ]
    then
        echo "Ninguna info matchea a $x!, para comparar manualmente compare la salida de 'pacman -Qi $x' y 'pacman -Si $x'"
    elif echo "$remote_info" | grep -qv -e 'galaxy' -e 'system' -e 'world'
    then
        echo "$x matchea repo de arch linux!, para comparar manualmente compare la salida de 'pacman -Qi $x' y 'pacman -Si $x'"
    fi
done

rm -rf "$tmp"
