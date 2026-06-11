#!/bin/bash

if test $# -ne 2
then
    echo "Uso incorrecto del script"
    echo "Uso correcto: $0 Accion(Borrar/PermisoW) nombre_fichero"
    exit 1
fi

accion="$1"   # ← añadidas comillas (buena práctica)
fichero="$2"

# miramos que accion se ha solicitado
if test "$accion" = "Borrar"
then
    if ! test -f "$fichero"
    then
        echo "Fichero $fichero no valido"
        exit 1
    else
        rm -f "$fichero" 2>/dev/null
        if test $? -eq 0
        then
            echo "Borrado el fichero $fichero"
            exit 0
        else
            echo "Error al borrar el fichero $fichero"
            exit 1
        fi
    fi

elif test "$accion" = "PermisoW"
then
    if test -f "$fichero"
    then
        chmod ug+w "$fichero"
        echo "Permiso W otorgado al fichero $fichero"
        exit 0
    else
        echo "Fichero $fichero no valido"
        exit 1
    fi

else
    echo "Uso incorrecto"
    echo "Uso correcto: $0 Accion(PermisoW/Borrar) Fichero"
    exit 1
fi