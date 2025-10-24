#!/usr/bin/bash

rm -f addons.zip

scons
godot --doctool ./ --gdextension-docs
scons target=template_debug
scons target=template_release
scons platform=windows target=template_debug
scons platform=windows target=template_release

rm -f addons/*/bin/~*.TMP
cp LICENSE README.md addons/PathMesh3D

prev_dir=$(basename $PWD)
cd ..
zip -9 -r "$prev_dir/addons.zip" "$prev_dir/addons/"
cd "$prev_dir"
rm -f addons/PathMesh3D/README.md addons/PathMesh3D/LICENSE