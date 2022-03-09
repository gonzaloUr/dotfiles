#!/bin/sh

mkdir -p ~/.local/share/applications
cd ../applications
for x in *
do
    ln -sf "$PWD/$x" ~/.local/share/applications
done

mkdir -p ~/.local/bin
cd ../scripts
for x in *
do
    ln -sf "$PWD/$x" ~/.local/bin
done

cd ../configs
mkdir -p ~/.config/alacritty
mkdir -p ~/.config/dunst
mkdir -p ~/.config/fontconfig
mkdir -p ~/.config/nvim
mkdir -p ~/.config/kak-lsp
mkdir -p ~/.config/kak
mkdir -p ~/.config/lf
mkdir -p ~/.config/newsboat
mkdir -p ~/.config/zathura

ln -sf $PWD/alacritty.yml ~/.config/alacritty
ln -sf $PWD/dunstrc ~/.config/dunst
ln -sf $PWD/fonts.conf ~/.config/fontconfig
ln -sf $PWD/init.vim ~/.config/nvim
ln -sf $PWD/kak-lsp.toml ~/.config/kak-lsp
ln -sf $PWD/kakrc ~/.config/kak
ln -sf $PWD/lfrc ~/.config/lf
ln -sf $PWD/newsboat ~/.config/newsboat/config
ln -sf $PWD/zathurarc ~/.config/zathura

ln -sf $PWD/bashrc ~/.bashrc
ln -sf $PWD/inputrc ~/.inputrc
ln -sf $PWD/profile ~/.bash_profile
ln -sf $PWD/profile ~/.zprofile
ln -sf $PWD/xinitrc ~/.xinitrc
ln -sf $PWD/zshrc ~/.zshrc

sudo ln -sf $PWD/xorg.conf /etc/X11/

cd ../suckless
for x in *
do
    sudo ln -sf "$PWD/$x/config.def.h" "/usr/local/src/$x/"
done
