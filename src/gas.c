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

#define assert_timeout(condition) 0
/*
 #define assert_timeout(condition) assert(condition);
 #define assert_timeout(condition) if(!(condition)){printf("fuck\n");sleep(10);}
 */

typedef long long tipo_dato;
typedef unsigned int natural;
typedef int entero;

typedef long long bitch_vector;

typedef enum BOOLEANOS {
	falso = 0, verdadero
} bool;

#define max(x,y) ((x) < (y) ? (y) : (x))
#define min(x,y) ((x) < (y) ? (x) : (y))

typedef struct caca_x_numeros_unicos_en_rango {
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
				!num_numeros || nodo->max_num_esperados || num_idx_ini >= num_numeros);
		if (nodo->max_num_esperados) {
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
	nodo->max_numeros = 1 << altura;
	nodo->idx = idx_nodo;
	nodo->limite_izq = limite_izq;
	nodo->limite_der = limite_der;
}

static tipo_dato caca_x_construye_arbol_binario_segmentado(natural idx_nodo,
		natural idx_inicio, natural idx_fin, natural idx_fin_verdadero,
		entero altura) {

	caca_x_numeros_unicos_en_rango *nodo = arbol_numeros_unicos + idx_nodo;
	assert_timeout(idx_inicio<=idx_fin);
	assert_timeout(idx_fin<=idx_fin_verdadero);
	assert_timeout(altura>=0);

	caca_x_inicializar_nodo(nodo, altura, idx_nodo, idx_inicio, idx_fin);

	if (idx_inicio != idx_fin) {
		natural idx_hijo_izq = idx_nodo << 1 | 1;
		natural altura_sig = altura - 1;
		natural idx_medio = idx_inicio + ((idx_fin - idx_inicio) >> 1);

		nodo->suma = caca_x_construye_arbol_binario_segmentado(idx_hijo_izq,
				idx_inicio, idx_medio, idx_fin_verdadero, altura_sig)
				+ caca_x_construye_arbol_binario_segmentado(idx_hijo_izq + 1,
						idx_medio + 1, idx_fin, idx_fin_verdadero, altura_sig);

		/*
		nodo->max_num_esperados =
				(idx_fin_verdadero >= idx_fin) ? nodo->max_numeros :
				(idx_fin_verdadero >= idx_inicio) ?
						(idx_fin_verdadero - idx_inicio + 1) : 0;
						*/

		return nodo->suma;
	} else {
		assert_timeout(altura==0);
		if (idx_inicio <= idx_fin_verdadero) {
			nodo->suma = numeros[idx_inicio];
			nodo->max_num_esperados = 1;
			return nodo->suma;
		} else {
//			numeros[idx_inicio] = 0;
			return 0;
		}
	}
}

static tipo_dato caca_x_suma_segmentos(natural idx_nodo, natural limite_izq,
		natural limite_der) {
	caca_x_numeros_unicos_en_rango *nodo = arbol_numeros_unicos + idx_nodo;
	natural nodo_lim_izq = nodo->limite_izq;
	natural nodo_lim_der = nodo->limite_der;

	if (nodo_lim_der < limite_izq || limite_der < nodo_lim_izq) {
		return 0;
	}

	if (limite_izq <= nodo_lim_izq && nodo_lim_der <= limite_der) {
//		assert_timeout(res_seg > 0);
		return nodo->suma;
	}
	natural idx_nodo_hijo_izq = (idx_nodo << 1) | 1;
	natural idx_nodo_hijo_der = idx_nodo_hijo_izq + 1;

	return caca_x_suma_segmentos(idx_nodo_hijo_izq, limite_izq, limite_der)
			+ caca_x_suma_segmentos(idx_nodo_hijo_der, limite_izq, limite_der);

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

	while (scanf("%u\n", &num_numeros) > 0) {
		natural num_queries;
		natural cont_queries = 0;
		natural max_profundidad = 0;
		natural num_numeros_redondeado;
		natural num_nodos;

		memset(matriz_nums, 0, MAX_NUMEROS_REDONDEADO*sizeof(tipo_dato));

		caca_log_debug("a vece siento q %d\n", num_numeros);

		tipo_dato caca;
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

		num_nodos = (2 << max_profundidad) - 1;

		caca_log_debug("el numero de nodos %d\n", num_nodos);

		arbol_numeros_unicos = calloc(num_nodos,
				sizeof(caca_x_numeros_unicos_en_rango));
		assert_timeout(arbol_numeros_unicos);

		caca_log_debug("llamando a func rec con max prof %d\n",
				max_profundidad + 2);

		caca_x_construye_arbol_binario_segmentado(0, 0,
				num_numeros_redondeado - 1, num_numeros - 1, max_profundidad);

#ifdef GAS_VALIDAR_SEGMENTOS
		gas_valida_segmentos(arbol_numeros_unicos, numeros, num_nodos,
				num_numeros, NULL, 0);
#endif

		printf("Case #%u:\n", cont_casos + 1);

		while (cont_queries < num_queries) {
			unsigned short tipo_query;
			natural idx_query_ini;
			natural idx_query_fin;

			scanf("%hu %d %d\n", &tipo_query, &idx_query_ini, &idx_query_fin);
			if (idx_query_ini > idx_query_fin) {
				tipo_dato tmp;
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
				printf("%lld\n", caca_x_suma_segmentos(0, idx_query_ini - 1,
#ifdef GAS_VALIDAR_SEGMENTOS
						numeros, num_nodos,
#endif
						idx_query_fin - 1));
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
}

int main(void) {
//	puts("he corrido con algo de suerte");
//	sleep(10);
	caca_x_main();
	return EXIT_SUCCESS;
}
