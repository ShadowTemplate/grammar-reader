#include "Header.h"

/* Funzioni di riconoscimento dei simboli ------------------------------------*/

int is_terminal(Symbol s)
{
    return (islower(s));
}

int is_nonterminal(Symbol s)
{
    return (isupper(s));
}

int is_prodsym(Symbol s)
{
    return (s == '>');
}

int is_prodsep(Symbol s)
{
    return (s == '\n');
}

/* Lettura di simboli da file ------------------------------------------------*/

Symbol read_sym(FILE* file)
{
       Symbol s;
       
       //questo ciclo permette di saltare la lettura di spazi tra i simboli
       do 
          s = getc(file);
       while (s==' ');
       
       return s;
}

Production* add_new_production(Grammar *g)
{
  Production* p;
  p = &(g->productions[g->numprod++]);
  p->left.length = 0;
  p->right.length = 0;

  return p;
}
                       
void add_symbol(Word *w, Symbol s)
{
  w->word[w->length++] = s;  
}

/* Controllo presenza simbolo iniziale ------------------------------------------------*/

int there_is_S_symbol(Grammar *g)
{
	int i;
	int foundS=0;
	int length=0;
		
	for(i=0; ( i<g->numprod && foundS == 0 ); i++)
	{
		if( ( g->productions[i].left.length == 1 ) && ( g->productions[i].left.word[0] == 'S' ) )
			foundS=1;
	}

	return foundS;
}

/* Controllo presenza nonterminale  ------------------------------------------------*/

int there_is_nonterminal(Word * word)
{
	int i;
	int foundNTs=0;

	for(i=0; ( i< word->length && foundNTs == 0 ); i++ )
	{
		if(is_nonterminal( word->word[i] ) )
			foundNTs=1;
	}
		
	return foundNTs;
}

/* Funzione di acquisizione di una grammatica da un file ----------------------*/       

Grammar* load_grammar(FILE* file, Grammar* g)
{
         enum States {START,LEFT,RIGHT,ERROR};
         /*   START  = Scansione di una nuova produzione [F]
              LEFT   = Scansione della parte sinistra
              RIGHT  = Scansione della parte destra [F]
              ERROR  = Errore di scansione
         */
         enum States current_state = START;  // Stato iniziale
         Symbol s;
         Production* p;
		 int flag = 0;
         
         g->numprod = 0; // Inizializza la grammatica
                  
         while (current_state != ERROR && !feof(file)) 
         {
               s = read_sym(file);
               if (feof(file)) break;
               switch(current_state)
               {
               case START:
                    if (is_terminal(s) || is_nonterminal(s))
                    {
                       current_state = LEFT;
                       
                       p = add_new_production(g);
                       add_symbol(&p->left,s);
                    }
                    else if (is_prodsep(s))
                    {
						current_state = START;
                    }
                    else 
                         current_state = ERROR;
                    break;
               case LEFT:
                    if (is_terminal(s) || is_nonterminal(s))
                    {
                       current_state = LEFT;
                       add_symbol(&p->left,s);
                    }
                    else if (is_prodsym(s))
                    {
						if ( there_is_nonterminal( &p->left ) != 0 )
							current_state = RIGHT;
					    else //Dopo aver letto un > verifica se a sinistra ci sia almeno un NT
						{
							printf("ATTENZIONE: La produzione %d del file e' senza nonterminali a sinistra\n\n", g->numprod);
							current_state = ERROR;
						}
                    }
                    else
                        current_state = ERROR;
                    break;
               case RIGHT:
                    if (is_terminal(s) || is_nonterminal(s))
                    {
                       current_state = RIGHT;
                       add_symbol(&p->right,s);
                    }
                    else if (is_prodsep(s))
                    {
                       current_state = START;
                    }
                    else 
                         current_state = ERROR;
                    break;
               }
         }

		 if ( (current_state == START || current_state == RIGHT) && there_is_S_symbol(g) != 0 ) //Controllo che ci sia almeno una produzione S -> w  w € (XuV)*
		 {
			 printf("Il simbolo inziale 'S' e' presente, da solo, nella parte sinistra di almeno\nuna produzione.\n\n");
			 printf("Tutte le produzioni hanno almeno un nonterminale nella parte sinistra.\n\n");
			 return g;
		 }
         else
		 {
			if (there_is_S_symbol(g) == 0) 
					printf("ATTENZIONE: La grammatica non contiene il simbolo iniziale 'S' in nessuna\nparte sinistra delle produzioni.\n\n");
			g->numprod = 0;
			return g;
		 }
}
