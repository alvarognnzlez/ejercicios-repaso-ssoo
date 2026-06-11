#!/bin/bash
# Script de auditoría del sistema
# Uso: ./auditoria.sh <directorio>
Se pide implementar un script Bash llamado auditoria.sh que reciba un único argumento: la ruta a un directorio del sistema.

El script debe:

Validar los argumentos. Si no se recibe exactamente un argumento, mostrar el modo de uso por pantalla y terminar con código de error.

Comprobar que el directorio existe. Si no existe, mostrar un mensaje de error y salir.

Generar un fichero de log con nombre auditoria_YYYYMMDD_HHMMSS.log (usando la fecha y hora actuales) que contenga, en orden, las siguientes secciones separadas por líneas en blanco:

Cabecera con fecha/hora, usuario actual y nombre del host.
Usuarios conectados en este momento (who).
Espacio en disco en formato legible (df -h).
Listado detallado de los ficheros del directorio pasado como argumento (ls -la).
Los 20 primeros procesos activos (ps aux + head).
Uso de memoria (free -h).
Al terminar, mostrar por pantalla un mensaje indicando dónde se han guardado los resultados.

# Validar número de argumentos
if [ $# -ne 1 ]; then
    echo "Uso: $0 <directorio>"
    exit 1
fi

DIRECTORIO=$1
LOG="auditoria_$(date +%Y%m%d_%H%M%S).log"

# Verificar que el directorio existe
if [ ! -d "$DIRECTORIO" ]; then
    echo "Error: el directorio '$DIRECTORIO' no existe."
    exit 1
fi

echo "=== AUDITORÍA DEL SISTEMA ===" > "$LOG"
echo "Fecha y hora: $(date)" >> "$LOG"
echo "Usuario: $(whoami)" >> "$LOG"
echo "Hostname: $(hostname)" >> "$LOG"
echo "" >> "$LOG"

echo "--- Usuarios conectados ---" >> "$LOG"
who >> "$LOG"
echo "" >> "$LOG"

echo "--- Espacio en disco ---" >> "$LOG"
df -h >> "$LOG"
echo "" >> "$LOG"

echo "--- Archivos en $DIRECTORIO ---" >> "$LOG"
ls -la "$DIRECTORIO" >> "$LOG"
echo "" >> "$LOG"

echo "--- Procesos activos (top 20) ---" >> "$LOG"
ps aux | head -20 >> "$LOG"
echo "" >> "$LOG"

echo "--- Uso de memoria ---" >> "$LOG"
free -h >> "$LOG"

echo "Auditoría completada. Resultados en: $LOG"
