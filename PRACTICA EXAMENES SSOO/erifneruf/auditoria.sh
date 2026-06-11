#!/bin/bash
# Script de auditoría del sistema
# Uso: ./auditoria.sh <directorio>

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
