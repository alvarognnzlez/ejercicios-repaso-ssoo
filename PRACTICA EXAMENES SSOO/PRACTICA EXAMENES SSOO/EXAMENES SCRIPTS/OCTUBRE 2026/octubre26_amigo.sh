#!/bin/bash

# Solución octubre 2026 (versión for con backticks)
# Uso: $0 <ext1> <ext2> <ext3> <fichSal> [<ruta a buscar>]

if test $# -lt 4 || test $# -gt 5
then
	echo "Uso: $0 <ext1> <ext2> <ext3> <fichSal> [<ruta a buscar>]"
	exit 1
fi

EXT1="$1"
EXT2="$2"
EXT3="$3"
SALIDA="$4"

if test $# -eq 5
then
	if test -d "$5"
	then
		DIRECTORIO="$5"
	else
		echo "$5 no es un directorio válido o no existe"
		exit 1
	fi
else
	DIRECTORIO="./"
fi

NUM_FICH_EXT1=0
TAM_EXT1=0
NUM_FICH_EXT2=0
TAM_EXT2=0
NUM_FICH_EXT3=0
TAM_EXT3=0

for fichero in `find "$DIRECTORIO" -type f -iname "*.$EXT1"`
do
	NUM_FICH_EXT1=`expr $NUM_FICH_EXT1 + 1`
	T=`du -b "$fichero" | cut -f1`
	TAM_EXT1=`expr $TAM_EXT1 + $T`   # ← era "+$T" sin espacio (error de sintaxis en expr)
done

for fich in `find "$DIRECTORIO" -type f -iname "*.$EXT2"`
do
	NUM_FICH_EXT2=`expr $NUM_FICH_EXT2 + 1`
	TEMP=`du -b "$fich" | cut -f1`   # ← era "$fichero" (variable del primer bucle, no del segundo)
	TAM_EXT2=`expr $TAM_EXT2 + $TEMP`
done

for fi in `find "$DIRECTORIO" -type f -iname "*.$EXT3"`
do
	NUM_FICH_EXT3=`expr $NUM_FICH_EXT3 + 1`
	TEMPORAL=`du -b "$fi" | cut -f1`   # ← era "$fichero" (variable del primer bucle, no del tercero)
	TAM_EXT3=`expr $TAM_EXT3 + $TEMPORAL`
done

FECHA=`date`
TOTAL_ARCHIVOS=`expr $NUM_FICH_EXT1 + $NUM_FICH_EXT2 + $NUM_FICH_EXT3`
TOTAL_TAMANO=`expr $TAM_EXT1 + $TAM_EXT2 + $TAM_EXT3`

echo "Directorio analizado: $DIRECTORIO"           >> "$SALIDA"
echo "Fecha de ejecución: $FECHA"                  >> "$SALIDA"
echo "-----------------------------------------------" >> "$SALIDA"
echo "Extensión | Nº de archivos | Tamaño total"   >> "$SALIDA"
echo "-----------------------------------------------" >> "$SALIDA"

if test $NUM_FICH_EXT1 -ne 0
then
	echo "$EXT1 | $NUM_FICH_EXT1 | $TAM_EXT1 bytes" >> "$SALIDA"
fi

if test $NUM_FICH_EXT2 -ne 0
then
	echo "$EXT2 | $NUM_FICH_EXT2 | $TAM_EXT2 bytes" >> "$SALIDA"
fi

if test $NUM_FICH_EXT3 -ne 0
then
	echo "$EXT3 | $NUM_FICH_EXT3 | $TAM_EXT3 bytes" >> "$SALIDA"
fi

echo "-----------------------------------------------" >> "$SALIDA"
echo "Total de archivos: $TOTAL_ARCHIVOS"           >> "$SALIDA"
echo "Total tamaño: $TOTAL_TAMANO"                  >> "$SALIDA"
echo "-----------------------------------------------" >> "$SALIDA"

exit 0
