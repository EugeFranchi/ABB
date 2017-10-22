#include "abb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


typedef struct abb_nodo{
	struct abb_nodo* izq;
	struct abb_nodo* der;
	char* clave;
	void* dato;
	} abb_nodo_t;
	
typedef struct abb{
	abb_nodo_t* raiz;
	abb_comparar_clave_t cmp;
	abb_destruir_dato_t destruir_dato;
	size_t cant;
	} abb_t;


/* ************************************************************
 *                        ABB_NODO
 * ***********************************************************/

abb_nodo_t* abb_nodo_crear(const char* clave, void* dato){
	abb_nodo_t* abb_nodo = malloc(sizeof(abb_nodo_t));
	if (!abb_nodo){
		return NULL;
	}
	char* datos = malloc(sizeof(char) * strlen(clave)+1);
	if (!datos){
		return NULL;
	}
	abb_nodo->clave = datos;
	abb_nodo->dato = dato;
	abb_nodo->der = NULL;
	abb_nodo->izq = NULL;
	
	return abb_nodo;
}

bool abb_nodo_insertar(abb_nodo_t* nodo, const char* clave, void* dato, abb_comparar_clave_t cmp){
	if (cmp(nodo->clave, clave) == 0){
		char* datos = malloc(sizeof(char) * strlen(clave)+1);
		if (!datos){
			return false;
		}
		nodo->clave = datos;
		nodo->dato = dato;
	}
	else if (cmp(nodo->clave, clave) > 0){
		if (nodo->izq){
			return abb_nodo_insertar(nodo->izq, clave, dato, cmp);
		}
		nodo->izq = abb_nodo_crear(clave, dato);
	}
	else{
		if (nodo->der){
			return abb_nodo_insertar(nodo->der, clave, dato, cmp);
		}
		nodo->der = abb_nodo_crear(clave, dato);
	}
	return true;
}

abb_nodo_t* abb_nodo_buscar(abb_nodo_t* nodo, const char* clave, abb_comparar_clave_t cmp){
	if (cmp(nodo->clave, clave) == 0){
		return nodo;
	}
	else if (cmp(nodo->clave, clave) > 0){
		if (nodo->izq){
			return abb_nodo_buscar(nodo->izq, clave, cmp);
		}
		return NULL;
	}
	else{
		if (nodo->der){
			return abb_nodo_buscar(nodo->der, clave, cmp);
		}
		return NULL;
	}
}

//Busca el padre de un nodo y lo devuelve. Si el nodo es la raiz, devuelve NULL.
//Pre: el nodo debe pertenecer al arbol.
abb_nodo_t* abb_nodo_buscar_padre(abb_nodo_t* nodo, abb_nodo_t* padre, const char* clave, abb_comparar_clave_t cmp){
	if (cmp(nodo->clave, clave) == 0){
		if (!padre){
			return NULL;
		}
		return padre;
	}
	else if (cmp(nodo->clave, clave) > 0){
		if (nodo->izq){
			padre = nodo;
			return abb_nodo_buscar_padre(nodo->izq, padre, clave, cmp);
		}
	}
	else{
		if (nodo->der){
			padre = nodo;
			return abb_nodo_buscar_padre(nodo->der, padre, clave, cmp);
		}
	}
	return nodo;
}

abb_nodo_t* abb_nodo_minimo(abb_nodo_t* nodo) {
	abb_nodo_t* act = nodo;
	while (act->izq != NULL){
		act = act->izq;
	}
	return act;
}

bool abb_nodo_swap(abb_nodo_t* nodo1, abb_nodo_t* nodo2){
	if (!nodo1 || !nodo2){
		return false;
	}
	char* clave = nodo1->clave;
	void* dato = nodo1->dato;
	nodo1->clave = nodo2->clave;
	nodo1->dato = nodo2->dato;
	nodo2->clave = clave;
	nodo2->dato = dato;
	return true;
}


/* ************************************************************
 *                            ABB
 * ***********************************************************/
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato){
	abb_t* abb = malloc(sizeof(abb_t));
	if (!abb){
		return NULL;
	}
	
	abb->cant = 0;
	abb->cmp = cmp;
	abb->destruir_dato = destruir_dato;
	abb->raiz = NULL;
	
	return abb;
}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato){
	if (!arbol){
		return false;
	}
	if (!arbol->raiz){
		arbol->raiz = abb_nodo_crear(clave, dato);
	}
	else{
		return abb_nodo_insertar(arbol->raiz, clave, dato, arbol->cmp);
	}
	return true;
}

void *abb_borrar(abb_t *arbol, const char *clave){
	if (!arbol || !arbol->raiz){
		return NULL;
	}
	
	abb_nodo_t* nodo = abb_nodo_buscar(arbol->raiz, clave, arbol->cmp);
	if (!nodo){
		return NULL;
	}
	
	abb_nodo_t* padre = abb_nodo_buscar_padre(nodo, NULL, clave, arbol->cmp);
	
	if (!nodo->izq && !nodo->der){
		if(arbol->cmp(padre->clave, nodo->clave) > 0){
			padre->izq = NULL;
		}
		else{
			padre->der = NULL;
		}
	}
	else if (!nodo->izq){
		if(arbol->cmp(padre->clave, nodo->clave) > 0){
			padre->izq = nodo->der;
		}
		else{
			padre->der = nodo->der;
		}
	}
	else if (!nodo->der){
		if(arbol->cmp(padre->clave, nodo->clave) > 0){
			padre->izq = nodo->izq;
		}
		else{
			padre->der = nodo->izq;
		}
	}
	else{
		abb_nodo_t* minimo = abb_nodo_minimo(nodo->der);
		abb_nodo_swap(nodo, minimo);
		void* dato = minimo->dato;
		free(minimo->clave);
		free(minimo);
		return dato;
	}
	
	void* dato = nodo->dato;
	free(nodo->clave);
	free(nodo);
	return dato;
}

void *abb_obtener(const abb_t *arbol, const char *clave){
	abb_nodo_t* nodo = abb_nodo_buscar(arbol->raiz, clave, arbol->cmp);
	return nodo->dato;
}

bool abb_pertenece(const abb_t *arbol, const char *clave){
	return abb_nodo_buscar(arbol->raiz, clave, arbol->cmp);
}

size_t abb_cantidad(abb_t *arbol){
	return arbol->cant;
}

void abb_destruir(abb_t *arbol);

/* ************************************************************
 *                          ITERADOR
 * ***********************************************************/



