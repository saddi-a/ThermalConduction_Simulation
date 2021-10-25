#include "listeSC.h"

	/// Partie specifique a implementer (par les etudiants)	
	/** Suppression d'une donnee dans la liste
	  * @param d l'adresse de la donnee a supprimer
	  */
	void freeData( void *d ) {
	}
	
	/** Affichage d'une donnee
	  * @param d la donnee a afficher
	  */
	void afficherData( void *d ) {
	}
	
	
	
	
	/// Partie generique deja implementee (par les enseignants)	
	// Constructeur et Destructeur
	/** Creation d'une liste
	  * @return une liste prete a l'emploi
	  */
	ListeSC* creerListe( int dataSize ) {
		// On cree la tete de liste
		ListeSC* liste = malloc( sizeof(ListeSC) );
		// On l'initialise
		liste->length = 0;   // longueur nulle
		liste->dataSize = dataSize;   // taille d'un element
		liste->root = NULL;  // aucun element
		liste->last = NULL;  // aucun element
		liste->current = NULL;  // aucun element
		// On renvoie la tete de liste
		return liste;
	}

	/** Suppression d'une liste
	  * @param liste la liste a supprimer
	  */
	void freeListe( ListeSC* liste ) {
		// Tant que la liste n'est pas vide
		while( liste->root != NULL )
			suppr( liste, 0 );  // On supprime le premier element
		
		// On libere l'espace memoire occupe par la tete de liste
		free( liste );
		liste = NULL;
	}


	// Methodes de parcours de liste
	/** Methode indiquant si l'element courant a un successeur
	  * @param liste la liste parcourue
	  * @return 1 si le point courant a un successeur, O sinon
	  */
	int hasNext( ListeSC* liste ) {
		// Si l'element n'est pas le dernier de la liste
		return( liste->current != NULL );
	}

	/** Acces au successeur d'un element d'une liste
	  * @param liste la liste parcourue
	  */
	void getNext( ListeSC* liste ) {
		liste->current = liste->current->next;
	}


	// Methodes d'ajout / de suppression d'element de la liste
	/** Ajout d'un element dans une liste
	  * @param liste la liste ou l'on veut ajouter un element
	  * @param d l'element a ajouter
	  * @param typeAjout type d'ajout (0: au debut, 1: au milieu, 2: en fin de liste)
	  */
	void ajout( ListeSC* liste, void *d, int typeAjout ) {
		if( (typeAjout < 0) || (typeAjout > 2) ) {	// Mauvaise valeur
			printf( "Erreur: typeAjout doit etre compris entre 0 et 2\n" );
		} else {
			// On declare et alloue en memoire une variable de type Maille
			pMaille tmpLP = malloc( sizeof( Maille ) );
			tmpLP->data = malloc( liste->dataSize );	// On alloue de la memoire pour les donnees
			memcpy( tmpLP->data, d, liste->dataSize );	// On les copie
			// Ajout dans la liste et chainage
			if( liste->length == 0 ) {	// Liste vide ?
				tmpLP->next = NULL;		// Pas de successeur
				liste->root = tmpLP;	// On definit la tete de liste,
				liste->current = tmpLP;	// l'element courant
				liste->last = tmpLP;	// et la fin de liste
			} else {
				switch( typeAjout ) {
					case 0:				// Ajout en tete de liste
						tmpLP->next = liste->root;	// Chainage
						liste->root = tmpLP;		// Ajout
						break;
					case 1:				// Ajout en milieu de liste
						tmpLP->next = liste->current->next;		// Chainage
						liste->current->next = tmpLP;			// Ajout
						break;
					case 2:				// Ajout en fin de liste
						tmpLP->next = NULL;				// Chainage: Pas de successeur
						liste->last->next = tmpLP;		// Ajout
						liste->last = tmpLP;			// Nouvelle fin de liste
						break;
				}
			}
			// On incremente le nombre de points dans la liste
			liste->length++;
		}
	}

	/** Suppression d'un element dans une liste
	  * @param liste la liste ou l'on veut supprimer un element
	  * @param typeSuppr type de suppression (0: au debut, 1: au milieu, 2: en fin de liste)
	  */
	void suppr( ListeSC* liste, int typeSuppr ) {
		pMaille pRem;		// Element a supprimer
		if( (typeSuppr < 0) || (typeSuppr > 2) ) {	// Mauvaise valeur
			pRem = NULL;
			printf( "Erreur: typeSuppr doit etre compris entre 0 et 2\n" );
		} else if( liste->length == 0 ) {			// Liste vide => rien a faire
			printf( "Erreur: liste vide => Rien a supprimer\n" );
			pRem = NULL;
		} else if( liste->length == 1 ) {	// Liste vide apres suppression
			pRem = liste->root;
			liste->root = NULL;
			liste->current = NULL;
			liste->last = NULL;
		} else {							// Cas general
			switch( typeSuppr ) {
				case 0:						// Suppression en tete de liste
					pRem = liste->root;
					// Le successeur de pRem est la nouvelle tete de liste
					liste->root = pRem->next;
					// On verifie que l'on ne supprime pas l'element courant
					if( pRem == liste->current )
						liste->current = liste->root;
					break;
				case 1:						// Suppression en milieu de liste
					// Remarque: par simplicite, on supprime le successeur du point courant
					if( liste->current == liste->last ) {		// Element courant n'a pas de successeur?
						pRem = NULL;	// => rien a faire
					} else {
						pRem = liste->current->next;	// Successeur du point courant
						if( pRem == liste->last ) {		// Suppression de la fin de liste?
							liste->last = liste->current;	// Nouvelle fin de liste
							liste->last->next = NULL;
						} else		// On change l'adresse du successeur de current
							liste->current->next = pRem->next;
					}
					break;
				case 2:						// Suppression en fin de liste
					pRem = liste->last;
					// On cherche l'avant-dernier element
					pMaille avantDernier = liste->root;		// On se place au debut de la liste
					while( avantDernier->next != liste->last )	// On la parcourt jusqu'a l'avant dernier element
						avantDernier = avantDernier->next;		// On avance d'un pas
					liste->last = avantDernier;					// Avant dernier devient le dernier
					liste->last->next = NULL;					// et n'a plus de successeur
					// On verifie que l'on ne supprime pas l'element courant
					if( pRem == liste->current )
						liste->current = liste->last;
					break;
			}
		}
		if( pRem != NULL ) {	// Un element a supprimer
			// On fait pointer l'adresse du successeur de pRem vers NULL
			pRem->next = NULL;
			// On libere l'espace memoire occupe par pRem
			freeData( pRem->data );	// les donnees
			free( pRem->data );		// le pointeur vers les donnees
			free( pRem );			// le pointeur vers la maille
			// On decremente le nombre de points dans la liste
			liste->length--;
		}
	}
		
	
	// Methode d'affichage
	/** Affichage d'une liste
	  * @param liste la liste a afficher
	  */
	void afficherListe( ListeSC* liste ) {
		int i;
		for( liste->current = liste->root, i = 0; hasNext( liste ); getNext( liste ), i++ ) {
			// On affiche les infos
			printf( "%d: ", i );
			afficherData( liste->current->data );
		}
	}
