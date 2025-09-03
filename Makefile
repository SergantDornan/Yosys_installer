OUTPUT=./installer
INCDIR=./include
SOURCEDIR=./source
deps=./depsAndObjects
CPPC=g++
C++standart=-std=c++23
OPT=-O3
DEPFLAGS=-MP -MD
GENERALFLAGS=$(C++standart) -g3 -w
CFLAGS=$(GENERALFLAGS) $(OPT) $(DEPFLAGS)
CFILES=$(foreach D, $(SOURCEDIR), $(wildcard $(D)/*.cpp))
OBJECTS=$(patsubst $(SOURCEDIR)%.cpp, $(deps)%.o, $(CFILES))
DEPFILES= $(patsubst $(SOURCEDIR)%.cpp, $(deps)%.d, $(CFILES)) $(deps)/installer.d

all:$(OUTPUT)

install:$(OUTPUT)
	@./installer

install-force:$(OUTPUT)
	@./installer force

uninstall:$(OUTPUT)
	@./installer uninstall

$(OUTPUT):$(OBJECTS)
	$(CPPC) $^ -o $@

mrproper:
	rm -rf $(OBJECTS) $(DEPFILES) $(OUTPUT)

$(deps)/%.o:$(SOURCEDIR)/%.cpp
	$(CPPC) $(CFLAGS) $(foreach D,$(INCDIR),-I$(D)) -c $< -o $@

-include $(DEPFILES) 