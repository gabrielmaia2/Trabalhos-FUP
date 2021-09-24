/*============================================================================*/
/* MÓDULO QUE DETECTA AS LUZES DO WII SENSOR BAR                              */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu - nassu@dainf.ct.utfpr.edu.br                       */
/*============================================================================*/

#ifndef __TRABALHO3_H
#define __TRABALHO3_H

/*============================================================================*/

#include "imagem.h"

/*============================================================================*/
/* Struct usada para representar uma coordenada (x,y). */

typedef struct
{
    int x;
    int y;

} Coordenada;

/*============================================================================*/
/* Função central do trabalho. */

double detectaSensorBar (Imagem1C* img, Coordenada* l, Coordenada* r);

/*============================================================================*/
#endif /* __TRABALHO3_H */
