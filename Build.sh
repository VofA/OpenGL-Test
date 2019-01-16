#!/bin/bash

mkdir -p build
cd build || exit

rm -rf ./*
cmake ../sources/

echo -n "-- Копирование файлов: "
cp ../sources/6.lamp.fs 6.lamp.fs
cp ../sources/6.lamp.vs 6.lamp.vs
cp ../sources/6.multiple_lights.fs 6.multiple_lights.fs
cp ../sources/6.multiple_lights.vs 6.multiple_lights.vs

cp -R ../sources/resources/ resources/
echo "Готово"

while true
do
	clear
	echo -n "-- Применение стилей: "
	find ../sources/ -name '*.hpp' -or -name '*.cpp' | xargs clang-format -i -style=file $1
	echo "Готово"

	echo "-- Компиляция: Запуск"
	make
	echo "-- Компиляция: Завершение"

	read -r operation

	if [ "$operation" ] && [ "$operation" = "cmake" ]
	then
		clear
		rm -rf ./*
		cmake ../sources/
	fi
	if [ "$operation" ] && [ "$operation" = "copy" ]
	then
		echo -n "-- Копирование файлов: "
		cp ../sources/6.lamp.fs 6.lamp.fs
		cp ../sources/6.lamp.vs 6.lamp.vs
		cp ../sources/6.multiple_lights.fs 6.multiple_lights.fs
		cp ../sources/6.multiple_lights.vs 6.multiple_lights.vs

		cp -R ../sources/resources/ resources/
		echo "Готово"
	fi
done