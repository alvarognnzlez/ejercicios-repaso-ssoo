# 🐚 CHULETA COMPLETA — SCRIPTS BASH (SSOO)

---

## 1. ESTRUCTURA MÍNIMA DE UN SCRIPT

```bash
#!/bin/bash          # shebang: obligatorio en la primera línea

# ... código ...

exit 0               # salida correcta (exit 1 = error)
```

**Dar permisos y ejecutar:**
```bash
chmod +x script.sh
./script.sh arg1 arg2
```

---

## 2. ARGUMENTOS ($1, $2, $#, $@)

```bash
$0        # nombre del script
$1, $2    # primer y segundo argumento
$#        # número total de argumentos
$@        # todos los argumentos como lista
```

**Comprobación de argumentos (patrón de examen):**
```bash
if test $# -lt 1 || test $# -gt 2
then
    echo "Uso: $0 <argumento1> [argumento2]" >&2   # >&2 = a stderr
    exit 1
fi
```

---

## 3. CONDICIONALES — if / elif / else

```bash
if condicion
then
    # código
elif otra_condicion
then
    # código
else
    # código
fi
```

### Tests de ficheros y directorios

```bash
test -f "$ruta"    # ¿es un fichero regular?
test -d "$ruta"    # ¿es un directorio?
test -e "$ruta"    # ¿existe?
test -s "$ruta"    # ¿existe y no está vacío?
test -r "$ruta"    # ¿tiene permiso de lectura?
```

### Tests de cadenas

```bash
test -z "$var"         # ¿cadena vacía?
test -n "$var"         # ¿cadena NO vacía?
test "$a" = "$b"       # ¿cadenas iguales?
test "$a" != "$b"      # ¿cadenas distintas?
```

### Tests numéricos

```bash
test $a -eq $b    # igual
test $a -ne $b    # distinto
test $a -lt $b    # menor que
test $a -le $b    # menor o igual
test $a -gt $b    # mayor que
test $a -ge $b    # mayor o igual
```

### Operadores lógicos

```bash
test ... -a ...    # AND (dentro de test)
test ... -o ...    # OR  (dentro de test)
test ... || test ...    # OR  (entre dos test)
test ... && test ...    # AND (entre dos test)
! test ...              # negación
```

> ⚠️ En exámenes, `-o` y `-a` dentro de `test` están desaconsejados en POSIX.
> Usa `||` y `&&` entre dos `test` por separado para mayor fiabilidad.

---

## 4. VARIABLES

```bash
variable="valor"          # asignar (sin espacios alrededor del =)
echo "$variable"          # usar (siempre entre comillas dobles)
resultado=$(comando)      # guardar salida de un comando
```

**Aritmética — dos formas equivalentes:**
```bash
# Forma moderna (recomendada)
n=$((n + 1))
total=$((total + bytes))
resto=$((n % 2))

# Forma clásica con expr (válida en exámenes, más verbosa)
n=`expr $n + 1`           # ← los espacios alrededor del operador son OBLIGATORIOS
total=`expr $total + $bytes`
```
> ⚠️ Con `expr`, los espacios son obligatorios: `expr $n +1` falla, `expr $n + 1` funciona.

---

## 5. BUCLES

### for — iterar sobre lista

```bash
for i in "val1" "val2" "val3"
do
    echo "$i"
done
```

### while — con condición

```bash
while test $n -gt 0
do
    n=$((n - 1))
done
```

### while read — leer líneas (patrón más usado en exámenes)

```bash
while read -r linea
do
    echo "$linea"
done < fichero.txt
```

### while read + process substitution (patrón estrella ⭐)

```bash
while read -r fichero
do
    # procesar $fichero
done < <(find "$directorio" -type f -name "*.txt")
#       ↑                                         ↑
#  redirección                    sustitución de proceso
```

> 💡 Este patrón evita problemas con espacios en nombres de fichero y
> permite modificar variables del bucle externo (a diferencia de pipe |).

### for + backticks — alternativa clásica (válida en exámenes) ⭐

```bash
for fichero in `find "$DIRECTORIO" -type f -iname "*.$EXT"`
do
    NUM=`expr $NUM + 1`
    TAM=`du -b "$fichero" | cut -f1`
    TOTAL=`expr $TOTAL + $TAM`
done
```

**Cuándo usar cada uno:**

| | `for` + backticks | `while read` + `< <(find)` |
|---|---|---|
| Simplicidad | ✅ Más sencillo de escribir | Más verboso |
| Espacios en nombres | ❌ Falla si hay espacios | ✅ Seguro |
| Variables modificables | ✅ Sí | ✅ Sí (process substitution) |
| Pipe `\|` | — | ❌ Subshell, pierde variables |
| Estilo examen | ✅ Muy común | ✅ Más correcto |

> 💡 En el examen ambos son aceptados. El `for` con backticks es más rápido de escribir
> si el enunciado no menciona ficheros con espacios en el nombre.

---

## 6. FIND — buscar ficheros

```bash
find ruta -type f              # ficheros regulares
find ruta -type d              # directorios
find ruta -type l              # enlaces simbólicos
find ruta -name "*.txt"        # por nombre (con comodines)
find ruta -name "[abc]*.txt"   # que empiecen por a, b o c
find ruta -size +1k            # mayores de 1 KB
find ruta -size -1M            # menores de 1 MB
find ruta -mindepth 1          # excluye el propio directorio raíz
find ruta -samefile otro       # mismo inodo (hard links)
find ruta -type f -exec ls -l {} \;   # ejecutar comando en cada resultado
```

---

## 7. GREP — buscar texto

```bash
grep "patron" fichero          # líneas que contienen el patrón
grep -c "patron" fichero       # contar cuántas líneas lo contienen
grep -q "patron" fichero       # modo silencioso (solo código de retorno)
grep -r "patron" directorio    # búsqueda recursiva
grep -l "patron" *.txt         # solo mostrar nombres de fichero
```

**En if (patrón de examen):**
```bash
if grep -q "cadena" "$fichero"
then
    echo "encontrado"
fi
```

---

## 8. WC — contar

```bash
wc -l fichero     # número de líneas
wc -w fichero     # número de palabras
wc -c fichero     # número de bytes

# Contar solo el número (sin nombre de fichero):
wc -l < fichero
wc -w < "$fichero"   # ← mejor forma: redirige stdin
```

---

## 9. STAT — información de fichero

```bash
stat -c %s "$fichero"    # tamaño en bytes
stat -c %n "$fichero"    # nombre
stat -c %i "$fichero"    # número de inodo
```

---

## 10. OTRAS UTILIDADES FRECUENTES

```bash
realpath "$fichero"        # ruta absoluta
cmp -s "$f1" "$f2"        # compara binariamente (silencioso); 0=iguales
date                       # fecha y hora actual
echo "texto" >> "$fichero" # añadir al fichero (>> no sobreescribe)
> "$fichero"               # vaciar/crear fichero
echo "texto" >&2           # escribir en stderr
```

**Redirigir un bloque al fichero de salida (patrón de examen):**
```bash
{
    echo "Línea 1"
    echo "Línea 2"
} >> "$fichero_salida"
```

---

## 11. ESTRUCTURA TIPO DE UN EJERCICIO DE EXAMEN

```bash
#!/bin/bash

# ── 1. Comprobar argumentos ──
if test $# -lt 2 || test $# -gt 3
then
    echo "Uso: $0 <dir> <fichero> [extension]" >&2
    exit 1
fi

# ── 2. Validar argumentos ──
if ! test -d "$1"
then
    echo "Error: '$1' no es un directorio" >&2
    exit 1
fi

# ── 3. Asignar variables ──
directorio="$1"
fichero_salida="$2"
extension="${3:-log}"    # valor por defecto si no se da $3

# ── 4. Preparar fichero de salida ──
> "$fichero_salida"      # vaciarlo al inicio

# ── 5. Lógica principal ──
total=0

while read -r f
do
    total=$((total + 1))
    # ... procesar $f ...
done < <(find "$directorio" -type f -name "*.$extension")

# ── 6. Mostrar resultado ──
echo "Total: $total"

exit 0
```

---

## 12. ERRORES MÁS COMUNES EN EXÁMENES

```bash
# ❌ MAL — espacios alrededor del = en asignación
variable = "valor"         # error de sintaxis
variable="valor"           # ✅

# ❌ MAL — usar $var sin comillas (falla con espacios)
if test -f $fichero        # falla si $fichero tiene espacios
if test -f "$fichero"      # ✅

# ❌ MAL — pipe con while (variables no se guardan fuera)
find . -type f | while read -r f; do total=$((total+1)); done
echo $total   # ← vale 0 porque el while corrió en subshell

# ✅ BIEN — process substitution (mantiene variables)
while read -r f; do total=$((total+1)); done < <(find . -type f)
echo $total   # ← vale el valor correcto

# ❌ MAL — stat con $(( ))
bytes=$((stat -c %s "$f"))   # $((…)) es aritmética, no ejecuta comandos
bytes=$(stat -c %s "$f")     # ✅ $( ) es sustitución de comando

# ❌ MAL — while read sin -r
while read linea             # las barras \ se interpretan
while read -r linea          # ✅ -r = raw, no interpreta \

# ❌ MAL — olvidar do en while read en una línea
while read -r fichero do     # error: "do" no va pegado a read
while read -r fichero        # ✅
do
```

---

## 13. CHECKLIST PARA EL EXAMEN

- [ ] ¿Empieza con `#!/bin/bash`?
- [ ] ¿Compruebas `$#` con el rango correcto?
- [ ] ¿Validas que directorios/ficheros existen con `-d`/`-f`?
- [ ] ¿Todas las variables están entre comillas dobles `"$var"`?
- [ ] Si usas `for` + backticks: ¿la variable del bucle es la correcta en cada `for`?
- [ ] Si usas `while read`: ¿usas `< <(find ...)` y no pipe `|`?
- [ ] Con `expr`: ¿hay espacios alrededor del operador? (`expr $n + 1`, no `expr $n +1`)
- [ ] ¿Los bytes los obtienes con `$(stat -c %s ...)` o `` `du -b "$f" | cut -f1` ``?
- [ ] ¿Escribes errores en stderr con `>&2`?
- [ ] ¿Terminas con `exit 0`?
