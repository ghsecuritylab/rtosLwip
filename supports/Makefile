# $Id$


# configuration options for build this project
NAME=supports

ROOT_DIR:=$(shell pwd)
RULE_DIR:=$(ROOT_DIR)/..

ARCH=arm
EXTENSION=


export	ROOT_DIR
export	RULE_DIR
export	EXTENSION
export	ARCH


include $(RULE_DIR)/Rules.mak



ifeq ($(CPU_E70Q20),YES)
	SUBDIRS += rtk	
endif	


SUBDIRS += rtos
SUBDIRS += lwip/lwip/src


#OBJECTS = $(wildcard $(OBJ_DIR)/*.o)
#OBJECTS = $(patsubst $(SOURCEDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))
OBJECTS := $(shell find $(OBJ_DIR) -name '*.o')

all: BUILD_OBJS


#	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) $@ ) ; done
BUILD_OBJS:
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) all ) ; done

clean: 
	rm -rf $(NAME)_*
	rm -rf pkg
	rm -rf BIN
	rm -rf $(OBJ_DIR) 
	- find . -name $(OBJ_DIR) -prune -exec rm -r -f {} \;
	rm -rf *.log

	
# all	
install:
	@$(SHELL) $(ROOT_DIR)/buildver.sh $(ROOT_DIR)
	@$(SHELL) $(ROOT_DIR)/install.sh $(ROOT_DIR)/Linux.bin.$(ARCH) $(ROOT_DIR)/releases  

#	@$(SHELL) $(ROOT_DIR)/buildver.sh $(ROOT_DIR)

package:clean
	cd ..; tar -cvjf $(NAME).$(BUILDTIME).tar.bz2 $(NAME) Rules.mak python

	