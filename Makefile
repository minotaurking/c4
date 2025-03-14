TARGET = ./code.bin

SOURCES = 

all: $(TARGET)

ifndef DIR
$(error Speicify a subfolder, like make DIR=test or make clean DIR=test)
endif

include $(DIR)/Makefile

PREPROCESS = $(SOURCES:.c=.cc)

$(TARGET): $(SOURCES)
	python preprocess.py $<
	./c4 -s $(PREPROCESS)
	./gen-code $(PREPROCESS)
	cp $(TARGET) ~/git_work/qemu/new-calc-build

clean:
	rm -f $(TARGET)
	rm -f $(PREPROCESS)
	rm -f $(SOURCES:.c=.cc.*)

.PHONY: all clean