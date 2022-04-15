PROJ_NAME = PrHa

SRC_FOLDER = src
DOCS_FOLDER = docs
DOXYGEN = /usr/local/bin/doxygen

.PHONY: all
all: PrHaFolder BuildPIgateSystem transferPIgateSystem clean 
	
PrHaFolder:
	mkdir -p $(PROJ_NAME)

BuildPIgateSystem: 
	$(MAKE) -C $(SRC_FOLDER)

transferPIgateSystem:
	cp -t $(PROJ_NAME) $(SRC_FOLDER)/PrHaProgram.elf
	cp -t $(PROJ_NAME) $(DOCS_FOLDER)/config

sendPIgateToRasp:
	scp -r $(PROJ_NAME) root@$(RASPIP):/etc
	scp $(DOCS_FOLDER)/eng.mypatterns root@$(RASPIP):/usr/share/tessdata
	scp $(DOCS_FOLDER)/cascade.xml root@$(RASPIP):/usr/share/OpenCV/haarcascades

documentation:
	$(DOXYGEN) Doxyfile

.PHONY: clean 
clean: 
	$(MAKE) -C $(SRC_FOLDER) clean

	
