ADD_LIBRARY(chaussettes SHARED chaussettes.cpp)
INSTALL_TARGETS(/lib/gpp/ RUNTIME_DIRECTORY /lib/gpp chaussettes chaussettes)