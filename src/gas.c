/*
 ============================================================================
 Name        : caca_x.c
 Author      : ernesto
 Version     :
 Copyright   : a veces siento que
 Description : Hello World in C, Ansi-style
 ============================================================================
 tamaño 41333
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <stddef.h>
#include <unistd.h>
#include <math.h>

#define MAX_NUMEROS 100000
#define MAX_VALOR INT_MAX
#define MAX_QUERIES 100000
// 12 es 10 de el num, 1 del espacio 1 de signo
#define TAM_MAX_LINEA (MAX_NUMEROS*12) 
#define CACA_X_MAX_PROFUNDIDAD 19
// Menos 1 por que es para redondear y menos otro por que el num de nodos es el doble del redondeado
#define MAX_NUMEROS_REDONDEADO (1<<(CACA_X_MAX_PROFUNDIDAD-2))
#define CACA_X_MAX_NODOS_AFECTADOS CACA_X_MAX_PROFUNDIDAD*2
#define MAX_NODOS (1 << CACA_X_MAX_PROFUNDIDAD)
#define CACA_X_VALOR_INVALIDO -1

//#define GAS_VALIDAR_SEGMENTOS

#ifdef GAS_VALIDAR_SEGMENTOS
#define caca_log_debug printf
#else
#define caca_log_debug(formato, args...) 0
#endif

#define assert_timeout(condition) assert(condition);
/*
 #define assert_timeout(condition) 0
 #define assert_timeout(condition) if(!(condition)){printf("fuck\n");sleep(10);}
 */

typedef long long tipo_dato;
typedef unsigned int natural;

typedef long long bitch_vector;

typedef enum BOOLEANOS {
	falso = 0, verdadero
} bool;

#define max(x,y) ((x) < (y) ? (y) : (x))
#define min(x,y) ((x) < (y) ? (x) : (y))

typedef struct caca_x_numeros_unicos_en_rango {
	bool necesita_actualizacion;
	tipo_dato suma;
	natural max_numeros;
	natural max_num_esperados;
	natural num_operacion;
	int altura;
	int idx;
	int limite_izq;
	int limite_der;
} caca_x_numeros_unicos_en_rango;

typedef struct caca_x_estado_recursion {
	int profundidad;
	int idx_ini;
	int idx_fin;
	int idx_nodo;
	int num_popeado;
	caca_x_numeros_unicos_en_rango *nodo;
} caca_x_estado_recursion;

caca_x_estado_recursion *estado = NULL;
tipo_dato res_seg;
caca_x_numeros_unicos_en_rango *arbol_numeros_unicos;
tipo_dato *numeros;

static inline void gas_valida_segmentos(
		caca_x_numeros_unicos_en_rango *arbolini, tipo_dato *numeros,
		natural num_nodos, natural num_numeros, natural *indices,
		natural num_indices) {

	natural limite_iteracion = 0;

	assert_timeout(!!num_numeros != !!(indices && num_indices));

	limite_iteracion = num_numeros ? num_numeros : num_indices;

	for (int i = 0; i < limite_iteracion; i++) {
		natural num_idx_ini = 0;
		natural num_idx_fin = 0;
		natural indice_nodo = 0;
		tipo_dato suma = 0;
		caca_x_numeros_unicos_en_rango *nodo = NULL;

		indice_nodo = num_numeros ? i : indices[i];

		nodo = arbolini + indice_nodo;

		num_idx_ini = nodo->limite_izq;
		num_idx_fin = nodo->limite_der;
		assert_timeout(
				!num_numeros || nodo->max_num_esperados
						|| num_idx_ini >= num_numeros);
		if (nodo->max_num_esperados && !nodo->necesita_actualizacion) {
			for (int j = num_idx_ini; j <= num_idx_fin; j++) {
				suma += numeros[j];
			}
			assert_timeout(suma == nodo->suma);
		}
	}
}

static inline char *caca_arreglo_a_cadena(tipo_dato *arreglo,
		natural tam_arreglo, char *buffer) {
	int i;
	char *ap_buffer = NULL;
	int characteres_escritos = 0;

#ifdef ONLINE_JUDGE
	return NULL;
#endif

#ifndef CACA_X_LOG
	return NULL ;
#endif

	memset(buffer, 0, 100);
	ap_buffer = buffer;

	for (i = 0; i < tam_arreglo; i++) {
		characteres_escritos +=
				sprintf(ap_buffer + characteres_escritos, "%lld",
						*(arreglo + i));
		if (i < tam_arreglo - 1) {
			*(ap_buffer + characteres_escritos++) = ',';
		}
	}
	*(ap_buffer + characteres_escritos) = '\0';
	return ap_buffer;
}

static inline char *caca_arreglo_a_cadena_natural(natural *arreglo,
		natural tam_arreglo, char *buffer) {
	int i;
	char *ap_buffer = NULL;
	int characteres_escritos = 0;

#ifdef ONLINE_JUDGE
	return NULL;
#endif

#ifndef CACA_X_LOG
	return NULL ;
#endif

	memset(buffer, 0, 100);
	ap_buffer = buffer;

	for (i = 0; i < tam_arreglo; i++) {
		characteres_escritos += sprintf(ap_buffer + characteres_escritos, "%u",
				*(arreglo + i));
		if (i < tam_arreglo - 1) {
			*(ap_buffer + characteres_escritos++) = ',';
		}
	}
	*(ap_buffer + characteres_escritos) = '\0';
	return ap_buffer;
}

static inline void caca_x_inicializar_nodo(caca_x_numeros_unicos_en_rango *nodo,
		int altura, int idx_nodo, int limite_izq, int limite_der) {
	nodo->altura = altura;
	nodo->max_numeros = 1 << nodo->altura;
	nodo->idx = idx_nodo;
	nodo->limite_izq = limite_izq;
	nodo->limite_der = limite_der;
}

static inline void caca_x_construye_arbol_binario_segmentado(
		natural num_numeros, natural max_profundidad,
		natural ultimo_indice_numero_valido) {
	bool zero_ya_asignado = falso;
	int profundidad = -1;

	estado->idx_ini = 0;
	estado->idx_fin = num_numeros;
	estado->idx_nodo = 0;
	estado->nodo = NULL;
	estado->profundidad = 0;
	estado->num_popeado = 0;

	profundidad++;

	while (profundidad != -1) {
		int idx_ini = 0;
		int idx_fin = 0;
		int idx_nodo = 0;
		int altura = 0;
		caca_x_numeros_unicos_en_rango *nodo = NULL;
		caca_x_estado_recursion *estado_actual = NULL;

		estado_actual = estado + profundidad;

		assert_timeout(estado_actual->profundidad == profundidad);
		caca_log_debug("perfume %d\n", profundidad);

		idx_ini = estado_actual->idx_ini;
		idx_fin = estado_actual->idx_fin;
		idx_nodo = estado_actual->idx_nodo;

		altura = max_profundidad - profundidad;

		caca_log_debug("en altura %d de pseudo rec idx %d\n", altura, idx_nodo);

		if (!estado_actual->num_popeado) {
			nodo = arbol_numeros_unicos + idx_nodo;

			caca_x_inicializar_nodo(nodo, altura, idx_nodo, idx_ini, idx_fin);
		}

		estado_actual->num_popeado++;

		profundidad--;

		if (altura) {
			int distancia_media = 0;
			int idx_medio = 0;
			int ultimo_num_popeado = 0;
			caca_x_estado_recursion *estado_futuro = NULL;

			if (estado_actual->num_popeado == 1
					|| estado_actual->num_popeado == 2) {

				caca_log_debug("creando nueva profundidad \n");
				estado_futuro = estado + estado_actual->profundidad + 1;

				caca_log_debug("num de popeos d estado actual es %d\n",
						estado_actual->num_popeado);

				if (estado_actual->num_popeado == 1) {
					estado_actual->nodo = nodo;
				}

				memset(estado_futuro, 0, sizeof(caca_x_estado_recursion));

				estado_futuro->profundidad = estado_actual->profundidad + 1;

				distancia_media = (idx_fin - idx_ini) / 2;
				idx_medio = idx_ini + distancia_media;

			}

			ultimo_num_popeado = estado_actual->num_popeado;

			switch (estado_actual->num_popeado) {
			case 1:
				estado_futuro->idx_ini = idx_ini;
				estado_futuro->idx_fin = idx_medio;
				estado_futuro->idx_nodo = 2 * idx_nodo + 1;
				caca_log_debug("segmentando izq de %d a %d en alt %d\n",
						idx_ini, idx_medio, altura);
				break;
			case 2:
				estado_futuro->idx_ini = idx_medio + 1;
				estado_futuro->idx_fin = idx_fin;
				estado_futuro->idx_nodo = 2 * idx_nodo + 2;
				caca_log_debug("segmentando der de %d a %d en alt %d\n",
						idx_medio + 1, idx_fin, altura);
				break;
			case 3: {
				caca_x_numeros_unicos_en_rango *hijo_izq = NULL;
				caca_x_numeros_unicos_en_rango *hijo_der = NULL;

				hijo_izq = arbol_numeros_unicos + (idx_nodo * 2 + 1);
				hijo_der = arbol_numeros_unicos + (idx_nodo * 2 + 2);

				estado_actual->nodo->suma = hijo_izq->suma + hijo_der->suma;

				estado_actual->nodo->max_num_esperados =
						(ultimo_indice_numero_valido
								>= estado_actual->nodo->limite_der) ?
								estado_actual->nodo->max_numeros :
						(ultimo_indice_numero_valido
								>= estado_actual->nodo->limite_izq) ?
								(ultimo_indice_numero_valido
										- estado_actual->nodo->limite_izq + 1) :
								0;

				memset(estado_actual, 0, sizeof(caca_x_estado_recursion));
				caca_log_debug("ultimo 'recursion' de nodo %d, nada q acer\n",
						idx_nodo);
			}
				break;
			default:
				assert_timeout(0)
				;
				break;
			}

			if (ultimo_num_popeado < 3) {
				profundidad += 2;
				caca_log_debug("aumentando profundidad a %d\n", profundidad);
			}

		} else {
			int numero_actual = 0;

			assert_timeout(idx_ini == idx_fin);
			assert_timeout(nodo);

			numero_actual = numeros[idx_ini];

			assert_timeout(nodo->max_numeros == 1);
			assert_timeout(nodo->limite_izq == nodo->limite_der);

			if (idx_ini || !zero_ya_asignado) {
				nodo->suma = numero_actual;
				nodo->max_num_esperados = 1;

				caca_log_debug(
						"asignado unico numero %d a indice de arbol %d\n",
						numero_actual, idx_nodo);
			} else {
				caca_log_debug(
						"no asignado unico numero %d a indice de arbol %d\n",
						numero_actual, idx_nodo);
			}

			memset(estado_actual, 0, sizeof(caca_x_estado_recursion));
			if (!idx_ini) {
				zero_ya_asignado = verdadero;
			}
		}
	}
}

static inline void caca_x_suma_segmentos(natural idx_nodo, natural limite_izq,
		natural limite_der) {
	caca_x_numeros_unicos_en_rango *nodo = arbol_numeros_unicos + idx_nodo;
	natural nodo_lim_izq = nodo->limite_izq;
	natural nodo_lim_der = nodo->limite_der;

	if (nodo_lim_der < limite_izq || limite_der < nodo_lim_izq) {
		return;
	}

	if (limite_izq <= nodo_lim_izq && nodo_lim_der <= limite_der) {
		res_seg += nodo->suma;
		assert_timeout(res_seg > 0);
		return;
	}
	natural idx_nodo_hijo_izq = (idx_nodo << 1) | 1;
	natural idx_nodo_hijo_der = idx_nodo_hijo_izq + 1;

	caca_x_suma_segmentos(idx_nodo_hijo_izq, limite_izq, limite_der);
	caca_x_suma_segmentos(idx_nodo_hijo_der, limite_izq, limite_der);

}

static inline tipo_dato caca_x_actualiza_segmentos(natural idx_nodo,
#ifdef GAS_VALIDAR_SEGMENTOS
		tipo_dato *numeros,
#endif
		natural limite_izq, natural limite_der) {
	caca_x_numeros_unicos_en_rango *nodo = arbol_numeros_unicos + idx_nodo;
	natural nodo_lim_izq = nodo->limite_izq;
	natural nodo_lim_der = nodo->limite_der;

	caca_log_debug(
			"stand by my side actualizando? segmento %u->%u en nodo %u %u->%u\n",
			limite_izq, limite_der, idx_nodo, nodo_lim_izq, nodo_lim_der);

	if (nodo_lim_der < limite_izq || limite_der < nodo_lim_izq
			|| nodo->max_num_esperados == nodo->suma) {
		return nodo->suma;
	}

	caca_log_debug("pues nadie sera %d,%d\n", nodo->limite_izq,
			nodo->limite_der);

	if (nodo_lim_izq != nodo_lim_der) {
		natural idx_nodo_hijo_izq = (idx_nodo << 1) | 1;
		natural idx_nodo_hijo_der = idx_nodo_hijo_izq + 1;
		caca_log_debug(
				"antes de actualizar recursivo nodo %u %u->%u suma %lld\n",
				idx_nodo, nodo_lim_izq, nodo_lim_der, nodo->suma);

		nodo->suma = caca_x_actualiza_segmentos(idx_nodo_hijo_izq,
#ifdef GAS_VALIDAR_SEGMENTOS
				numeros,
#endif
				limite_izq, limite_der)
				+ caca_x_actualiza_segmentos(idx_nodo_hijo_der,
#ifdef GAS_VALIDAR_SEGMENTOS
						numeros,
#endif
						limite_izq, limite_der);
		caca_log_debug(
				"despues de actualizar recursivo nodo %u %u->%u suma %lld\n",
				idx_nodo, nodo_lim_izq, nodo_lim_der, nodo->suma);

	} else {
		caca_log_debug("actualizando de %llu a %llu nodo %u %u->%u\n",
				nodo->suma, numeros[nodo_lim_izq], idx_nodo, limite_izq,
				limite_der);
		nodo->suma = floor(sqrt(nodo->suma));
#ifdef GAS_VALIDAR_SEGMENTOS
		numeros[nodo_lim_izq]=nodo->suma;
#endif
	}
	return nodo->suma;
}

static inline tipo_dato caca_x_suma_segmento(natural limite_izq,
#ifdef GAS_VALIDAR_SEGMENTOS
		tipo_dato *numeros, natural num_nodos,
#endif
		natural limite_der) {

	res_seg = 0;
	caca_x_suma_segmentos(0, limite_izq, limite_der);

	assert_timeout(res_seg > 0);

	caca_log_debug("La suma es %lld\n", res_seg);

	return res_seg;
}

static inline void caca_x_actualiza_estado(natural idx_actualizado_ini,
#ifdef GAS_VALIDAR_SEGMENTOS
		tipo_dato *numeros, natural num_nodos,
#endif
		natural idx_actualizado_fin) {
#ifdef CACA_X_LOG
	char *buf;
#endif

#ifdef CACA_X_LOG
	buf = calloc(100000, sizeof(char));
#endif

	caca_x_actualiza_segmentos(0,
#ifdef GAS_VALIDAR_SEGMENTOS
			numeros,
#endif
			idx_actualizado_ini, idx_actualizado_fin);

#ifdef GAS_VALIDAR_SEGMENTOS
	gas_valida_segmentos(arbol_numeros_unicos, numeros, num_nodos,
			arbol_numeros_unicos->max_num_esperados, NULL, 0);
#endif

#ifdef CACA_X_LOG
	free(buf);
#endif
}

static inline void caca_x_main() {
	tipo_dato *matriz_nums = NULL;
	natural num_numeros = 0;
	natural cont_casos = 0;

	char buf[10000] = { '\0' };

	matriz_nums = calloc(MAX_NUMEROS_REDONDEADO, sizeof(tipo_dato));
	assert_timeout(matriz_nums);

	estado = calloc(CACA_X_MAX_PROFUNDIDAD, sizeof(caca_x_estado_recursion));
	assert_timeout(estado);

	while (scanf("%u\n", &num_numeros) > 0) {
		natural num_filas = 0;
		natural num_queries = 0;
		natural cont_queries = 0;
		natural max_profundidad = 0;
		natural num_numeros_redondeado = 0;
		natural num_nodos = 0;

		memset(matriz_nums, 0, MAX_NUMEROS_REDONDEADO*sizeof(tipo_dato));

		caca_log_debug("a vece siento q %d\n", num_numeros);

		/*
		 lee_matrix_long_stdin(matriz_nums, &num_filas, NULL, 1,
		 MAX_NUMEROS_REDONDEADO);
		 */

		tipo_dato caca = 0;
		natural contacaca = 0;

		while (contacaca < num_numeros) {
			scanf("%llu", &caca);
			matriz_nums[contacaca] = caca;
			contacaca++;
		}

		numeros = matriz_nums;
		scanf("%u\n", &num_queries);

		assert_timeout(num_queries > 0);

		caca_log_debug("as corrido con algo de s %s\n",
				caca_arreglo_a_cadena((tipo_dato*) numeros, num_numeros, buf));
		caca_log_debug("en estas paginas %d\n", num_queries);

		while ((num_numeros >> max_profundidad)) {
			max_profundidad++;
		}
		num_numeros_redondeado = (1 << max_profundidad);

		assert_timeout(num_numeros_redondeado<=MAX_NUMEROS_REDONDEADO);

		caca_log_debug("en estas paginas %s\n",
				caca_arreglo_a_cadena(numeros, num_numeros_redondeado, buf));

		num_nodos = (2 << (max_profundidad + 0)) - 1;

		caca_log_debug("el numero de nodos %d\n", num_nodos);

		arbol_numeros_unicos = calloc(num_nodos,
				sizeof(caca_x_numeros_unicos_en_rango));
		assert_timeout(arbol_numeros_unicos);

		caca_log_debug("llamando a func rec con max prof %d\n",
				max_profundidad + 2);

		caca_x_construye_arbol_binario_segmentado(num_numeros_redondeado - 1,
				max_profundidad, num_numeros - 1);

#ifdef GAS_VALIDAR_SEGMENTOS
		gas_valida_segmentos(arbol_numeros_unicos, numeros, num_nodos,
				num_numeros, NULL, 0);
#endif

		printf("Case #%u:\n", cont_casos + 1);

		while (cont_queries < num_queries) {
			unsigned short tipo_query;
			tipo_dato sum;
			natural idx_query_ini;
			natural idx_query_fin;

			scanf("%hu %d %d\n", &tipo_query, &idx_query_ini, &idx_query_fin);
			if (idx_query_ini > idx_query_fin) {
				tipo_dato tmp = 0;
				tmp = idx_query_fin;
				idx_query_fin = idx_query_ini;
				idx_query_ini = tmp;
			}
			caca_log_debug("q: %hu, ini %d, fin %d\n", tipo_query,
					idx_query_ini, idx_query_fin);

			assert_timeout(idx_query_ini - 1 < num_numeros_redondeado);
			assert_timeout(idx_query_fin - 1 < num_numeros_redondeado);
			assert_timeout(idx_query_ini <= num_numeros);
			assert_timeout(idx_query_fin <= num_numeros);

			switch (tipo_query) {
			case 1:
				sum = caca_x_suma_segmento(idx_query_ini - 1,
#ifdef GAS_VALIDAR_SEGMENTOS
						numeros, num_nodos,
#endif
						idx_query_fin - 1);
				printf("%lld\n", sum);
				break;
			case 0:
				caca_log_debug("intervalo a actualizar %u a %u\n",
						idx_query_ini - 1, idx_query_fin - 1);

				caca_x_actualiza_estado(idx_query_ini - 1,
#ifdef GAS_VALIDAR_SEGMENTOS
						numeros, num_nodos,
#endif
						idx_query_fin - 1);
				break;
			default:
				abort();
				break;
			}
			cont_queries++;
		}

		printf("\n");
		free(arbol_numeros_unicos);

		cont_casos++;
	}
	free(matriz_nums);
	free(estado);
}

int main(void) {
//	puts("he corrido con algo de suerte");
//	sleep(10);
	caca_x_main();
	return EXIT_SUCCESS;
}
