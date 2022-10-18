PROGRAM_SUBDIRS ?= $(shell ls -d ./programs/*/)

ISO_NAME = cs535final.iso

all: iso
	
# Builds the ISO for the project. Takes all the elf files from the ./programs
# directory and startup and then puts them into one iso
.PHONY: iso
iso: programs startup
	@echo "\033[0;96mCreating ISO\033[0;0m"
	mkdir -p out

	for dir in $(PROGRAM_SUBDIRS); do								\
		cp $$dir/out/*.elf out/;									\
	done

	cp SYSTEM.CNF out/

	mkisofs -o ./out/$(ISO_NAME) ./out

# Build all the programs in the ./programs subdirectory. 
# This should build all of the .elf files that we need
.PHONY: programs
programs:
	for dir in $(PROGRAM_SUBDIRS); do 								\
		echo "\033[0;96mBuilding program $$dir \033[0;0m";			\
		$(MAKE) -C $$dir;											\
	done

# Build the startup program. This will just build the .elf file for the startup,
# not create the cd
.PHONY: startup
startup:
	@echo "\033[0;96mBuilding startup program \033[0;0m"
	$(MAKE) -C ./Startup/

clean:
	rm -rf out