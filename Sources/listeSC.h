#ifndef _listeSC_h_
#define _listeSC_h_

	// Bibliotheques "standards"
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	// Bibliotheques personnelles (a completer si besoin)
	
	/// Partie specifique a implementer (par les etudiants)	
	/** Suppression d'une donnee dans la liste
	  * @param d l'adresse de la donnee a supprimer
	  */
	void freeData( void *d );
	
	/** Affichage d'une donnee
	  * @param d la donnee a afficher
	  */
	void afficherData( void *d );



	/// Partie generique deja implementee (par les enseignants)	
	// Definition de la liste
		// Definition des types
	typedef struct Maille Maille;	// Element de la liste
	typedef Maille* pMaille;		// Pointeur vers une maille
		// Declaration de la structure Maille
	struct Maille {
		void *data;			// La donnee
		pMaille next;		// Le pointeur vers la maille suivante
	};

	typedef struct {
		int length;
		int dataSize;
		pMaille root;		// Tete de liste
		pMaille last;		// Fin de liste
		pMaille current;	// Element courant
		/* last permet de gerer plus facilement l'ajout en fin de liste
		 * Ajout en fin de liste uniquement afin de garder l'ordre des points
		 * dans le fichier lu
		 */
	} ListeSC;

	// Constructeur et Destructeur
	/** Creation d'une liste
	  * @return une liste prete a l'emploi
	  */
	ListeSC* creerListe( int dataSize );
	
	/** Suppression d'une liste
	  * @param liste la liste a supprimer
	  */
	void freeListe( ListeSC* liste );


	// Methodes de parcours de liste
	/** Methode indiquant si l'element courant a un successeur
	  * @param liste la liste parcourue
	  * @return 1 si le point courant a un successeur, O sinon
	  */
	int hasNext( ListeSC* liste );
	
	/** Acces au successeur d'un element d'une liste
	  * @param liste la liste parcourue
	  */
	void getNext( ListeSC* liste );

	
	// Methodes d'ajout / de suppression d'element de la liste
	/** Ajout d'un element dans une liste
	  * @param liste la liste ou l'on veut ajouter un element
	  * @param d l'element a ajouter
	  * @param typeAjout type d'ajout (0: au debut, 1: au milieu, 2: en fin de liste)
	  */
	void ajout( ListeSC* liste, void *d, int typeAjout );
	
	/** Suppression d'un element dans une liste
	  * @param liste la liste ou l'on veut supprimer un element
	  * @param typeSuppr type de suppression (0: au debut, 1: au milieu, 2: en fin de liste)
	  */
	void suppr( ListeSC* liste, int typeSuppr );
	

	// Methode d'affichage
	/** Affichage d'une liste
	  * @param liste la liste a afficher
	  */
	void afficherListe( ListeSC* liste );

#endif
