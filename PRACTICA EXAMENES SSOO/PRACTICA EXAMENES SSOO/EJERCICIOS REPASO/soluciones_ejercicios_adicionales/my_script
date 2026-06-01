#!/bin/bash

# Comprobamos que el número de argumentos sea exactamente 2
if test $# -ne 2
then
	echo "Uso incorrecto del script."
	echo "Uso: $0 [Borrar|PermisoW] nombre_fichero"
	exit 1
fi

accion=$1
fichero=$2

# Evaluamos la acción solicitada
if test "$accion" = "Borrar"
then
	# Se borra el fichero y se comprueba el valor devuelto por el comando rm
	rm "$fichero" 2>/dev/null
	if test $? -eq 0
	then
		echo "Borrado el fichero $fichero"
	else
		echo "Error al borrar el fichero"
	fi
elif test "$accion" = "PermisoW"
then
	# Se comprueba primero si el fichero existe
	if test -e "$fichero"
	then
		# Proporcionamos permiso de escritura al propietario (u) y al grupo (g)
		chmod ug+w "$fichero"
		echo "Permiso de W otorgado al fichero $fichero"
	else
		echo "Error: El fichero $fichero no existe."
	fi
else
	# Si la acción no es válida, mostramos el mensaje de ayuda
	echo "Uso incorrecto del script."
	echo "Uso: $0 [Borrar|PermisoW] nombre_fichero"
	exit 1
fi
