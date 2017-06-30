cd ../../Release/x64
printf "Собираем плагин...\n\n"
CFLAGS="-I/e/Dev/GXEngine-Windows-OS-x64/Include" gimptool-2.0 --build ../../Sources/GXGimpBarycentricFillFilter/main.cpp
mv "'main.exe'" gx-barycentric-fill-filter.exe
printf "\nГотово\n\nУстанавливаем плагин в GIMP...\n"
cp -f gx-barycentric-fill-filter.exe "/e/Tools/GIMP-2.8.22/lib/gimp/2.0/plug-ins/gx-barycentric-fill-filter.exe"
printf "Готово"
