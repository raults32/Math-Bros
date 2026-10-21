#!/bin/sh
# Ejecuta Math Bros en macOS desde la carpeta del proyecto para que encuentre src/.
cd "$(dirname "$0")"
if [ ! -x ./main ]; then
  ./compile.sh || exit 1
fi
exec ./main
