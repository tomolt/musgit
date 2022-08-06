# Customize below to fit your system

# compiler and linker
CC = gcc
LD = gcc

# installation paths
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

# compiler flags for musgit
CPPFLAGS =
CFLAGS   = -g -Wall
LDFLAGS  = -g
LIBS     = -lc-stache -lgit2

