# 🧵 CHULETA COMPLETA — THREADS EN C (pthreads)

---

## 1. INCLUDES SIEMPRE NECESARIOS

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>       // sleep, rand
#include <pthread.h>      // threads, mutex, cond
#include <semaphore.h>    // semáforos POSIX
```

**Compilar siempre con:**
```bash
gcc programa.c -o programa -lpthread
```

---

## 2. LAS 3 HERRAMIENTAS DE SINCRONIZACIÓN

### 🔒 MUTEX — Exclusión mutua (sección crítica)
> Usa cuando: varios hilos acceden a una **variable compartida**.

```c
pthread_mutex_t mutex;                    // declarar (global)

pthread_mutex_init(&mutex, NULL);         // inicializar (en main)
pthread_mutex_lock(&mutex);               // entrar sección crítica
  /* ... código que toca variable compartida ... */
pthread_mutex_unlock(&mutex);             // salir sección crítica
pthread_mutex_destroy(&mutex);            // destruir (al final de main)
```

**Regla de oro:** Todo lo que modifique una variable compartida va entre `lock` y `unlock`.

---

### 📢 VARIABLE DE CONDICIÓN — Esperar que pase algo
> Usa cuando: un hilo necesita **esperar a que una condición sea verdadera** (ej: "espera a que la caja esté llena").

```c
pthread_cond_t cond;                      // declarar (global)

pthread_cond_init(&cond, NULL);           // inicializar (en main)
pthread_cond_destroy(&cond);              // destruir (al final)

// ── HILO QUE ESPERA ──
pthread_mutex_lock(&mutex);
while (!condicion_que_espero) {           // SIEMPRE while, nunca if
    pthread_cond_wait(&cond, &mutex);     // libera mutex y duerme
}
/* condición ya es verdadera, trabajar */
pthread_mutex_unlock(&mutex);

// ── HILO QUE SEÑALIZA ──
pthread_mutex_lock(&mutex);
condicion = true;                         // cambiar el estado
pthread_cond_signal(&cond);               // despertar UN hilo
// pthread_cond_broadcast(&cond);         // despertar TODOS los hilos
pthread_mutex_unlock(&mutex);
```

> ⚠️ `pthread_cond_wait` siempre va **dentro de un `while`**, nunca `if`.
> Porque pueden ocurrir "spurious wakeups" (despertar falso).

---

### 🚦 SEMÁFORO — Contador de recursos disponibles
> Usa cuando: hay un **número limitado de recursos** (ej: 3 puestos, capacidad de la cinta, etc.).

```c
sem_t sem;                                // declarar (global)

sem_init(&sem, 0, VALOR_INICIAL);         // inicializar (en main)
                  // 0 = compartido entre hilos del mismo proceso
sem_wait(&sem);                           // restar 1 (bloquea si = 0)
sem_post(&sem);                           // sumar 1 (desbloquea quien espera)
sem_destroy(&sem);                        // destruir (al final)
```

**Valores iniciales típicos:**
| Situación | Valor inicial |
|---|---|
| Recurso libre al inicio | `1` (como mutex) |
| N recursos disponibles | `N` |
| Esperar a que otro produzca | `0` |
| Capacidad de buffer/cinta | `capacidad` |

---

## 3. THREADS — CREAR, PASAR ARGUMENTOS, ESPERAR

```c
pthread_t hilo;                           // un hilo
pthread_t *hilos = malloc(sizeof(pthread_t) * N);  // N hilos dinámicos

// Crear hilo
pthread_create(&hilo, NULL, funcion_hilo, argumento);
//                            ↑                ↑
//                     función que ejecuta   void* (puede ser NULL)

// Esperar a que termine
pthread_join(hilo, NULL);

// El hilo termina con:
pthread_exit(NULL);   // dentro del manejador
return NULL;          // equivalente, al final del manejador
```

### Pasar el ID a cada hilo (patrón estándar)

```c
int *ids = malloc(sizeof(int) * N);

for (int i = 0; i < N; i++) {
    ids[i] = i;                                      // ← IMPORTANTE: no pasar &i
    pthread_create(&hilos[i], NULL, funcion, &ids[i]);
}

// En el manejador:
void *funcion(void *arg) {
    int id = *(int *)arg;   // castear void* a int*
    ...
}
```

> ⚠️ **NUNCA** pases `&i` directamente. Para cuando el hilo lo lee, `i` ya cambió.
> Siempre copia el valor a `ids[i]` primero.

---

## 4. ESTRUCTURA TIPO DE UN EJERCICIO

```c
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* ── Variables globales compartidas ── */
int contador = 0;
pthread_mutex_t mutex;
pthread_cond_t  cond;
sem_t           sem;

/* ── Manejador del hilo ── */
void *manejador(void *arg) {
    int id = *(int *)arg;

    /* Lógica del hilo */
    pthread_mutex_lock(&mutex);
    /* sección crítica */
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    int N = atoi(argv[1]);

    /* 1. Inicializar herramientas */
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    sem_init(&sem, 0, 1);

    /* 2. Reservar memoria dinámica */
    pthread_t *hilos = malloc(sizeof(pthread_t) * N);
    int       *ids   = malloc(sizeof(int) * N);

    /* 3. Crear hilos */
    for (int i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&hilos[i], NULL, manejador, &ids[i]);
    }

    /* 4. Esperar a que terminen */
    for (int i = 0; i < N; i++) {
        pthread_join(hilos[i], NULL);
    }

    /* 5. Destruir herramientas */
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    sem_destroy(&sem);

    /* 6. Liberar memoria */
    free(hilos);
    free(ids);

    return 0;
}
```

---

## 5. PATRONES COMUNES EN EXÁMENES

### 🔄 Patrón Productor-Consumidor (con semáforos)
> Visto en: `fabrica.c`, `Mudanza_sem.c`

```c
sem_t sem_libres;   // espacios libres en el buffer   → init a CAPACIDAD
sem_t sem_listos;   // elementos disponibles           → init a 0

// PRODUCTOR
sem_wait(&sem_libres);   // espera a que haya espacio
/* producir */
sem_post(&sem_listos);   // avisa que hay producto

// CONSUMIDOR
sem_wait(&sem_listos);   // espera a que haya producto
/* consumir */
sem_post(&sem_libres);   // avisa que hay espacio libre
```

---

### 📊 Patrón Variable compartida + condición
> Visto en: `impresora.c`, `fabrica.c`

```c
// Hilo que espera una condición compuesta
pthread_mutex_lock(&mutex);
while (condicion_bloquea_A || condicion_bloquea_B) {
    pthread_cond_wait(&cond, &mutex);
}
/* trabajar */
pthread_mutex_unlock(&mutex);

// Hilo que cambia el estado y señaliza
pthread_mutex_lock(&mutex);
estado = nuevo_valor;
pthread_cond_signal(&cond);   // o broadcast si despiertas a todos
pthread_mutex_unlock(&mutex);
```

---

### 🍽️ Patrón Filósofos (evitar deadlock)
> Visto en: `filosofos_mutex_cond.c`

```c
// Romper la simetría: el último filósofo coge los palillos al revés
if (id == N - 1) {
    sem_wait(&palillos[der]);   // primero derecho
    sem_wait(&palillos[izq]);
} else {
    sem_wait(&palillos[izq]);   // primero izquierdo
    sem_wait(&palillos[der]);
}
/* comer */
sem_post(&palillos[izq]);
sem_post(&palillos[der]);
```

---

### 🏁 Patrón Condición de parada con variable global
> Visto en: `fabrica.c`, `impresora.c`

```c
// Hilo trabajador con condición de salida
while (1) {
    // Comprobar condición de parada ANTES de bloquearse
    pthread_mutex_lock(&mutex);
    if (terminado >= total) {
        pthread_mutex_unlock(&mutex);
        pthread_exit(NULL);
    }
    pthread_mutex_unlock(&mutex);

    sem_wait(&sem);   // se puede bloquear aquí

    // Comprobar de NUEVO tras despertar (puede que hayan terminado mientras)
    pthread_mutex_lock(&mutex);
    if (terminado >= total) {
        pthread_mutex_unlock(&mutex);
        sem_post(&sem);   // devolver el token para que salgan otros
        pthread_exit(NULL);
    }
    pthread_mutex_unlock(&mutex);

    /* trabajar */
}
```

---

### 🔓 Despertar hilos bloqueados al terminar
> Patrón de limpieza al final del `main`.

```c
pthread_join(gestor, NULL);   // esperar al hilo principal

// Liberar hilos productores bloqueados en sem_libres:
for (int i = 0; i < t1; i++) sem_post(&sem_libres);
// Liberar hilos consumidores bloqueados en sem_listos:
for (int i = 0; i < t2; i++) sem_post(&sem_listos);

// Ahora sí hacer join de todos
for (int i = 0; i < t1; i++) pthread_join(hilos1[i], NULL);
for (int i = 0; i < t2; i++) pthread_join(hilos2[i], NULL);
```

---

## 6. TABLA RESUMEN: ¿QUÉ HERRAMIENTA USAR?

| Situación | Herramienta |
|---|---|
| Proteger variable compartida | `mutex` |
| Esperar a que `contador == N` | `mutex` + `cond` |
| Limitar acceso a N recursos | `sem` iniciado a N |
| Comunicar productor→consumidor | dos `sem` (libres/listos) |
| Que hilo A espere a hilo B | `sem` iniciado a 0 |
| Ejecutar en orden A → B → C | semáforos en cadena |
| Varios hilos esperan lo mismo | `cond_broadcast` |
| Un hilo espera a ser avisado | `cond_signal` |

---

## 7. ERRORES MÁS COMUNES (de tus ejercicios)

```c
// ❌ MAL — olvidar el segundo unlock
pthread_mutex_lock(&mutex);
// ...
pthread_mutex_lock(&mutex);   // ← debería ser UNLOCK

// ❌ MAL — usar if en vez de while con cond_wait
if (condicion) pthread_cond_wait(&cond, &mutex);   // ← SIEMPRE while

// ❌ MAL — pasar &i directamente al crear hilos
pthread_create(&h[i], NULL, f, &i);   // ← i cambia antes de que f lo lea

// ❌ MAL — olvidar fflush después de printf en hilos
printf("Hilo %d\n", id);   // puede no salir si el proceso termina antes
fflush(stdout);             // ← añadir esto

// ❌ MAL — olvidar el segundo argumento de pthread_cond_init
pthread_cond_init(&cond);             // ← falta NULL
pthread_cond_init(&cond, NULL);       // ✅

// ❌ MAL — sem_init con 2 argumentos
sem_init(&sem, 0);          // ← falta el valor inicial
sem_init(&sem, 0, 1);       // ✅

// ❌ MAL — pthread_cond_signal con el mutex bloqueado fuera
pthread_cond_signal(&cond, &mutex);   // ← signal NO tiene mutex como argumento
pthread_cond_signal(&cond);           // ✅ (el mutex va solo en cond_wait)
```

---

## 8. CHECKLIST PARA EL EXAMEN

- [ ] ¿Todas las variables compartidas están declaradas **globales**?
- [ ] ¿Inicializas mutex/cond/sem **antes** de crear hilos?
- [ ] ¿El `while(1)` tiene **condición de salida** con check del estado?
- [ ] ¿El `cond_wait` está dentro de un **`while`**, no `if`?
- [ ] ¿Pasas `&ids[i]` (no `&i`) al crear hilos?
- [ ] ¿Destruyes y liberas todo al final del `main`?
- [ ] ¿Despertas los hilos bloqueados en semáforos antes de los `join` finales?
- [ ] ¿Compilas con `-lpthread`?
