#!/bin/sh

# Create folder for R_LIBS_USER, see Renviron config file.
mkdir -p ~/R

# Choose mirror and install packages.
R --interactive <<EOT
chooseCRANmirror()

install.packages("rmarkdown")
install.packages("languageserver")
EOT
