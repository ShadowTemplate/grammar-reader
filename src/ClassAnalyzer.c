#include "Header.h"

/* Riconoscitore di classe di appartenenza ----------------------------*/

int is_CF (Grammar* g)
{
	int cf = 1;
	int i;

	for(i=0; (i<g->numprod && cf == 1); i++)
	{
		if(g->productions[i].left.length != 1)
			cf = 0;
	}
	return cf;
}

int is_CS (Grammar* g)
{
	int cs = 1;
	int i;
	int j=0;
	int rightcontextlength=0;
	int leftcontextlength=0;
	int increase = 0;
	int Slambdaproduction = 0;
	int flag;

	for(i=0; (i<g->numprod && cs == 1); i++)
	{
		
		if( g->productions[i].left.length == 1 ) //Se la parte sinistra è lunga 1
		{
			leftcontextlength = 0;			//Ha i contesti lambda
			rightcontextlength = 0;

		}

		if( g->productions[i].left.length > 1 )
		{
	
			//Calcolo lunghezza contesto sinistro
			do{
				if ( g->productions[i].left.word[j] == g->productions[i].right.word[j] )
				{
					leftcontextlength ++;
					increase = 1;
				}
				else increase = 0;
				j++;
			} while ( increase != 0 && j< g->productions[i].left.length );

		j=0;
		
		//Calcolo lunghezza contesto destro
		do{
			if ( g->productions[i].left.word[ g->productions[i].left.length - j -1 ] == g->productions[i].right.word[ g->productions[i].right.length - j -1]  )
			{
				rightcontextlength ++;
				increase = 1;
			}
			else increase = 0;
			j++;
		} while ( increase != 0  && j< g->productions[i].left.length ); //

		j=0;

		// Controllo le dimensioni ed i vincoli sulle parti destre e sinistre
		if( g->productions[i].left.length == ( leftcontextlength + rightcontextlength + 1  ) )
		{
			if( g->productions[i].right.length > ( leftcontextlength + rightcontextlength ) )
				cs = 1;

			if ( ( g->productions[i].right.length == 0 ) && ( g->productions[i].left.word[0] != 'S' ) )
				cs = 0;
			else cs = 1;

		}
		else cs = 0;
		}
		
		if( g->productions[i].left.length > g->productions[i].right.length ) //Se non è monotona non può essere CS
			cs = 0;


		if( g->productions[i].left.length == g->productions[i].right.length ) //Se parte destra e sinistra sono uguali allora è una produzione CS
		{
				flag = strncmp( g->productions[i].left.word, g->productions[i].right.word, g->productions[i].left.length );
				if (flag == 0)
					cs = 1;
		}

		rightcontextlength = 0;
		leftcontextlength = 0;

		// Controllo se la produzione è del tipo S --> lambda
		if( (g->productions[i].left.length == 1) && ( g->productions[i].left.word[0] == 'S' ) && (g->productions[i].right.length == 0) )
		{
			cs = 1;
			Slambdaproduction = 1;
		}

	}

	//Se c'è una produzione S --> lambda, allora S non può comparire nella parte destra di nessuna produzione
	if(Slambdaproduction == 1) 
	{
		for(i=0; i<g->numprod; i++)
		{
			for(j=0; j< g->productions[i].right.length; j++)
				if( g->productions[i].right.word[j] == 'S' )
					cs = 0;
		}
	}

	return cs;
}

int is_MN (Grammar* g)
{
	int mn = 1;
	int i, j;
	int Slambdaproduction=0;

	for(i=0; (i<g->numprod && mn == 1); i++)
	{
		if( (g->productions[i].left.length == 1) && ( g->productions[i].left.word[0] == 'S' ) && (g->productions[i].right.length == 0) ) //Se è una S -> lambda
			Slambdaproduction = 1;
		else if(g->productions[i].left.length > g->productions[i].right.length)
			mn = 0;
	}

	//Se c'è una produzione S --> lambda, allora S non può comparire nella parte destra di nessuna produzione
	if(Slambdaproduction == 1) 
	{
		for(i=0; i<g->numprod; i++)
		{
			for(j=0; j< g->productions[i].right.length; j++)
				if( g->productions[i].right.word[j] == 'S' )
					mn = 0;
		}
	}
	return mn;
}

int is_RL (Grammar* g)
{
	int rl = 1;
	int i;

	for(i=0; (i<g->numprod && rl == 1); i++)
	{
		if(g->productions[i].left.length != 1)
			rl = 0;

		if( g->productions[i].right.length > 2 )
			rl = 0;

		if( g->productions[i].right.length == 1 )
			if ( is_nonterminal( g->productions[i].right.word[0] ) )
				rl = 0;

		if( g->productions[i].right.length == 2 )
			if ( is_nonterminal( g->productions[i].right.word[0] ) )
				rl = 0;

		if( ( g->productions[i].right.length == 2 ) )
			if (is_terminal( g->productions[i].right.word[1] ) )
				rl = 0;
	}
	return rl;
}
