include $(MAKE)files/PROC.mk

fullRelease:

	$(MAKE) clear
	rm -rf __OUT

	$(MAKE) debug
	$(MAKE) lib BUILD_MODE=DEBUG

	$(MAKE) clearBuild
	
	$(MAKE) release
	$(MAKE) lib BUILD_MODE=RELEASE

	$(MAKE) dllCopy

copyTmp:
		
	@echo "CWD: $(CURDIR)"
	rm -rf tmp
	mkdir tmp

	mkdir -p tmp/build
	find build -type f \( -name '*.exe' -o -name '*.dll' -o -name '*.a' \) -exec cp --parents {} tmp/ \;

	cp -r Recc tmp/
	cp -r Import tmp/
	cp -r Documentation tmp/
	cp -r Batch tmp/
	cp -f .VERSION tmp/
	cp -f LICENSE tmp/
	cp -f README.md tmp/
	cp -f README.de.md tmp/
	cp -f MAINTAINING.md tmp/
	cp -f CONTRIBUTING.md tmp/

movRelease:

#	rm -rf __OUT

	mkdir -p __OUT

	rm -rf __OUT/prebuild
	rm -rf __OUT/prebuildLib

	mkdir __OUT/prebuild
	mkdir __OUT/prebuildLib

	@make copyTmp
	rm -f tmp/build/*.a
	cp -r tmp/ __OUT/prebuild/

	@make copyTmp
	rm -f tmp/build/*.exe
	cp -r tmp/ __OUT/prebuildLib/

	rm -rf tmp

exportRelease:# fullRelease

	mkdir -p __OUT

	rm -rf __OUT/BALDUR.zip
	rm -rf __OUT/BALDURLib.zip

	@make copyTmp
	rm -f tmp/build/*.a
	zip -r __OUT/BALDUR.zip tmp/.VERSION tmp/*

	@make copyTmp
	rm -f tmp/build/*.exe
	zip -r __OUT/BALDURLib.zip tmp/.VERSION tmp/*

	rm -rf tmp

extension:

	rm -rf extensions/vscode/build
	mkdir -p extensions/vscode/build

	cp -r __OUT/prebuild/tmp/. extensions/vscode/build
	cp -r Recc/textures/* extensions/vscode/icons
	
	find extensions/vscode/build/build -type f -name "*.exe" ! -name "Baldur.exe" ! -name "BaldurLSP.exe" -delete
	find extensions/vscode/build/build -type f -name "*LLVM*.dll" -delete

	rm -rf extensions/vscode/build/Recc

	find extensions/vscode/icons -type f ! -name "Baldur_File_Logo.png" ! -name "Baldur_File_Logo_raw_slim.svg" -delete

rawExtension: fullRelease movRelease extension

fexport: fullRelease movRelease exportRelease extension