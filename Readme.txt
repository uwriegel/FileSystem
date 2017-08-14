install python 2.7.
set PATH=%PATH%;"C:\Program Files (x86)\Python"
npm install nw-gyp –g
install vcbuildtools 2015 (visualcppbuildtools_full.exe) mit Windows SDKs 8 und 10
npm install nan --save
in commandprompt visual studio 2015 x64, im Verzeichis filesystem:
//nw-gyp rebuild --target=0.24.2 --msvs_version=2015

nw-gyp configure --target=0.24.2 --msvs_version=2015

legt ein weiters Visual Studio Projekt unter build an, zum kompilieren des .node
