#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "bitmap.h"
#include "machine.h"
#include "string.h"

/**
 * \class FrameProvider
 * \brief Le FrameProvider permet d'attribuer des cadres de mémoire physique aux processus
 * en encapsulant l'allocation des pages physiques aux pages virtuelles
 */
class FrameProvider
{
	public:
		/**
		 * \fn FrameProvider::FrameProvider()
		 * \brief Constructeur de la classe FrameProvider
		 */
		FrameProvider();

		/**
		 * \fn FrameProvider::~FrameProvider()
		 * \brief Destructeur de la classe FrameProvider
		 */
		~FrameProvider();


		/**
		 * \fn FrameProvider::GetEmptyFrame(TranslationEntry *frame)
		 * \brief Récupére un cadre libre et l'initialise à 0
		 * \param frame Table de page du processus
		 */
		int GetEmptyFrame();

		/**
		 * \fn FrameProvider::ReleaseFrame(TranslationEntry *frame)
		 * \brief Libére un cadre occupé 
		 * \param frame Table de page du processus
		 */
		void ReleaseFrame(int frame);

		/**
		 * \fn FrameProvider::NumAvailFrame(TranslationEntry *frame)
		 * \brief Retourne le nombre de cadres libres
		 */
		int NumAvailFrame();

	private:
		Bitmap frameMap;


}


#endif // ADDRSPACE_H