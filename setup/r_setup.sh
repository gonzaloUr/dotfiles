#!/bin/sh

# Create folder for R_LIBS_USER, see Renviron config file.
mkdir -p ~/R

# Install packages.
R --interactive <<EOT
install.packages("rmarkdown", repo="https://cran.r-project.org/")
install.packages("languageserver", repo="https://cran.r-project.org/")
install.packages("reticulate", repo="https://cran.r-project.org/")
EOT
