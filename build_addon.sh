#!/usr/bin/bash
exec > "tmp.log" 2>&1

rm -f addons.zip

scons
../Godot.exe --doctool ./ --gdextension-docs
scons target=template_debug
scons target=template_release
wsl -e scons target=template_debug
wsl -e scons target=template_release

rm -f addons/*/bin/~*.TMP
cp LICENSE README.md addons/
wsl zip -9 -r addons.zip addons/
rm -f addons/README.md addons/LICENSE