" Mejor sintaxis para amsmath
"
" Copiar a ~/.vim/after/syntax/tex.vim, este archivo hace unos pequeños cambios
" para mejorar la sintaxis de amsmath en tex.
"
" Vim va a correr tex.vim despues de cargar el archivo tex.vim que utiliza para
" la sintaxis de tex por defecto localizado en
" /usr/share/vim/vim<version>/syntax/tex.vim o en ~/.vim/syntax/tex.vim.

call TexNewMathZone("E","align",1)
call TexNewMathZone("F","align*",1)
call TexNewMathZone("G","multline",1)
call TexNewMathZone("H","multline*",1)
