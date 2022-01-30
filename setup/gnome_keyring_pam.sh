#!/bin/sh

echo 'auth       optional     pam_gnome_keyring.so' >> /etc/pam.d/login
echo 'session    optional     pam_gnome_keyring.so auto_start' >> /etc/pam.d/login
