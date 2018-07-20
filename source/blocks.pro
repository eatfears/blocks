TEMPLATE = subdirs

!include( common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

SUBDIRS += blocks
blocks.subdir = bin/blocks
blocks.depends = logger engine

SUBDIRS += logger
logger.subdir = lib/logger

SUBDIRS += engine
engine.subdir = lib/engine
engine.depends = noise logger

SUBDIRS += noise
noise.subdir = lib/noise
