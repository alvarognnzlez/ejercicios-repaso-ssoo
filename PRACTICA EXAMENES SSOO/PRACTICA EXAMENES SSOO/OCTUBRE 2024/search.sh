#!/bin/bash

# Comprobación de argumentos
if test $# -gt 2 || test $# -lt 1
then
    echo "Uso incorrecto"
    echo "Uso: $0 <fichero_salida> [<ruta_a_buscar>]"
    exit 1 
fi

# Determinar el directorio de búsqueda
if test $# -eq 2
then 
    if ! test -d "$2"
    then
        echo "La ruta $2 no es una ruta valida"
        exit 1 
    else
        directorio="$2"
    fi
else
    directorio="./"
fi

# Guardamos el fichero de salida
fichero_salida="$1"

# Gestión del fichero de salida: si ya existe, se elimina al inicio del script
if test -e "$fichero_salida"
then
    rm -f "$fichero_salida"
fi

total_archivos=0

# Buscamos archivos que empiecen por a, b o c y terminen en .txt
# Usamos un bucle while para evitar errores de sintaxis y manejar archivos con espacios
while read -r i #i: Es el nombre de la variable donde se guardará la línea leída (en este caso, la ruta de cada archivo encontrado). En cada vuelta del bucle, la variable $i contendrá un archivo diferente.
do 
    # Contar las palabras de forma segura (redireccionando la entrada de wc)
    palabras=$(wc -w < "$i")
    
    # Mostrar el mensaje en el terminal con el formato solicitado
    echo "El fichero $i tiene $palabras palabras"

    if test "$palabras" -gt 1000 
    then
        # Escribir en el fichero de salida (añadiendo con >> para no sobrescribir en cada iteración)
        echo "El fichero $i tiene $palabras palabras" >> "$fichero_salida"
        total_archivos=$((total_archivos + 1))
    fi  

done < <(find "$directorio" -type f -name "[abc]*.txt")

# Contar los ficheros y mostrar el total por pantalla
echo "Hay un total de $total_archivos con más de 1000 palabras"
exit 0






#A) El primer < (Redirección de entrada)
#Este menor que redirige la entrada estándar del bucle while. En lugar de esperar a que escribas algo por teclado, le dice a while read que lea el contenido de lo que venga a continuación.

#B) El segundo <(...) (Sustitución de proceso o Process Substitution)
#Esta es una característica avanzada de Bash. Lo que hace es:

#Ejecutar el comando que está dentro de los paréntesis: find "$directorio" -type f -name "[abc]*.txt".
#Bash toma la salida de ese comando (una lista de rutas de archivos, una por línea) y la expone como si fuera un archivo virtual temporal.
#El primer < lee de ese archivo virtual.