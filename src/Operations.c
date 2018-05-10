#include "Header.h"

/* Scrivere una grammatica su file ------------------------------------------------*/

void write_grammar (FILE * fp, Grammar * g)
{
	int i;
	char tempright [MAX_WORD_LENGTH];
	char templeft [MAX_WORD_LENGTH];
	int j;
	
	rewind(fp);
	for(i=0; i < g->numprod; i++)
	{
		
		for(j=0; j< g->productions[i].left.length; j++) //Copio tutti i simboli
			templeft[j]= g->productions[i].left.word[j];
	
		templeft[j]='\0'; //Aggiungo il carattere di fine stringa
		fprintf(fp, "%s > ", templeft);

		for(j=0; j< g->productions[i].right.length; j++) //Copio tutti i simboli
			tempright[j]= g->productions[i].right.word[j];
	
		tempright[j]='\0'; //Aggiungo il carattere di fine stringa	
		fprintf(fp, "%s\n", tempright);

	}

}

/* Aggiungere manualmente produzioni alla grammatica ------------------------------------------------*/

void write_productions(Grammar *g, FILE *fp)
{
	Production* nuova_produzione;
	char buffer;
	int num;
	int i;
	Word newword;
	int j;

	if (g->numprod == 0)
        printf ("Impossibile aggiungere produzioni alla grammatica.\n\n");
     else
     {
		rewind(fp);
		while (!feof(fp))
			buffer = fgetc(fp);
		buffer = '0';
		

		printf("Quante produzioni vuoi aggiungere? ");
		scanf("%d", &num);
		getchar();
		for(i=0; i<num; i++)
		{

			nuova_produzione = add_new_production(g);
			
			printf("\nATTENZIONE: la parte sinistra della produzione deve avere almeno un nonterminale");
			printf("\n");
						
			do{
				printf("Inserire parte sinistra della %d' produzione: ", i+1);
				strcpy(newword.word, "");
			
				j=0;
				buffer='0';
				do{
					buffer = getchar();
					if ( ( buffer >= 'A' &&  buffer <= 'Z' ) || ( buffer >= 'a' &&  buffer <= 'z') )
						{
							newword.word[j]= buffer;
							j++;
						}
				} while( buffer != '\n' && j < MAX_WORD_LENGTH);
				newword.word[j]='\0';

				newword.length = strlen( newword.word );
			} while( !there_is_nonterminal( &newword ) );
			nuova_produzione->left = newword;			
			fprintf(fp, "\n%s > ", nuova_produzione->left.word);

			printf("Inserire parte destra della %d' produzione: ", i+1);

			j=0;
			buffer='0';
			do{
				buffer = getchar();
				if ( ( buffer >= 'A' &&  buffer <= 'Z' ) || ( buffer >= 'a' &&  buffer <= 'z') )
					{
						nuova_produzione->right.word[j] = buffer;
						j++;
					}
			} while( buffer != '\n' && j < MAX_WORD_LENGTH);
			nuova_produzione->right.word[j]='\0';
			nuova_produzione->right.length = strlen(nuova_produzione->right.word);
			fprintf(fp, "%s", nuova_produzione->right.word);
		}
		
		if( num>0 )
		{
			printf("\nOperazione di aggiunta completata correttamente.\n\n");
			printf("Il file di testo della grammatica e' stato aggiornato correttamente\n\n");
			printf("Ecco la nuova grammatica:\n\n");
			print_grammar(g);
		}

	}

}

/* Rimuovere produzioni dalla grammatica ------------------------------------------------*/

void delete_productions( Grammar* g )
{
	int num;
	int i;
	FILE* fp;
	int cursor=0;
	int j;
	int scelta;

	if (g->numprod == 0)
        printf ("Impossibile cancellare produzioni dalla grammatica.\n\n");
     else
     {
		 printf("Quante produzioni vuoi cancellare? ");
		 scanf("%d", &num);

		 for(i=0; i<num; i++)
		 {
			 printf("Inserisci il numero della %d' produzione che vuoi eliminare: ", i+1);
			 scanf("%d", &cursor);
			 if( cursor > g->numprod)
			 {
				do{
				printf("ATTENZIONE: numero non valido!\nReinserisci il numero della produzione: ");
				scanf("%d", &cursor);
				} while (cursor > g->numprod);
			 }
			 cursor--; //Il C conta gli elementi a partire da 0
			 
			 //Correzione della grammatica: "riaccorpo le produzioni"
			 for(j=0; j<(g->numprod-cursor-1); j++)
				 g->productions[cursor+j] = g->productions[cursor+j+1];
			 
			 g->productions[g->numprod-1].left.length=0;
			 strcpy(g->productions[g->numprod-1].left.word, "");
			 g->productions[g->numprod-1].right.length=0;
			 strcpy(g->productions[g->numprod-1].right.word, "");
			 g->numprod--;

			 printf("\nOperazione di eliminazione completata correttamente.\n\n");
			 printf("Ecco la nuova grammatica:\n\n");
			 print_grammar(g);

		 }

		 getchar(); //Svuota il buffer
		 
		 //Correzione del file
		 if( num > 0)
		 {
			 printf("Premi 1 per salvare su un nuovo file la grammatica soprastante\no un altro tasto per non salvarla: ");
			 scelta = getchar();
			 scelta=scelta-48; //Trasformo il numero ascii in decimale
			 if(scelta == 1)
			 {
				fp = fopen("SmallerGrammar.txt", "w");
				write_grammar(fp, g);
				fclose(fp);
				printf("\nLa grammatica e' stata salvata nel file 'SmallerGrammar.txt'\n");
			 }

		 }
	 }
}

/* Conversione Monotona->Context-Sensitive ------------------------------------------------*/

void convert_MN_to_CS (Grammar* MNg, Grammar* CSg)
{

	FILE* fp;
	int i; //indice
	int k; //indice
	int j; //indice
	int freeNT=0; //Posizione del primo NT disponibile ad essere utilizzato, nel vettore dei NTavailable
	Grammar tempg; //Grammatica temporanea
	int freeslot=0; //Posizione della prima produzione vuota, pronta ad essere riempita, di CSg
	char NTavailable[27]; //Vettore di NT disponibili (tutte le lettere dell'alfabeto eccetto quelle usate in qualche produzione)
	Word tempword; //Parte sinistra della produzione
	Word tempword2; //Parte destra della produzione
	Word lastword; //Parola temporanea usata per continuare l'algoritmo
	int NTrequested=0; //Numero di NT richiesti dalla grammatica monotona per essere convertita in CS
	int goodNT; //Flag: indica se una lettera è usata oppure no nella grammatica monotona
	int cursor=0; //Numero di NT disponibili
	int flag = 1; //Indica


	if( ! is_MN( MNg ) )
		printf("ATTENZIONE: grammatica non Monotona: non convertibile in Context-Sensitive!\n\n");

	if ( is_CS( MNg ) ) //Se è CS non necessita di conversione
	{
		*CSg = *MNg;
		fp = fopen("Context-Sensitive_Grammar.txt","w");
		write_grammar(fp, CSg);
		fclose(fp);

	}
	
	//NT sufficienti per la grammatica?
	
	//Determino i NT usabili nella conversione
	for( i='A'; i<='Z'; i++) //Per ogni lettera scorro tutte le produzioni: se non è usata allora la metto nel vettore dei NT usabili
	{
		goodNT = 1;
		for(k=0; ( k < MNg->numprod && goodNT == 1 ); k++)
		{
			for(j=0; j < MNg->productions[k].left.length; j++)
				if( i == MNg->productions[k].left.word[j] )
					goodNT = 0;
			for(j=0; j < MNg->productions[k].right.length; j++)
				if( i == MNg->productions[k].right.word[j] )
					goodNT = 0;
		}
		if (goodNT == 1) //Se la lettera non è stata usata
		{
			NTavailable[cursor] = i; //La metto nel vettore dei NT usabili
			cursor++;
		}
	}

	//Calcolo quanti NT ho bisogno per convertire tutte le produzioni MN in CS
	for(i=0; i < MNg->numprod; i++)
	{

		tempg.productions[0] = MNg->productions[i];
		tempg.numprod = 1;
		
		//Conto quanti simboli dovrei sostituire: terminali delle parti sinistre + lunghezza della parte sinistra -1
		if ( !is_CS( &tempg ) ) //Solo se la produzione non è CS (altrimenti non "subirebbe" l'algoritmo)
		{
			for(k=0; k < MNg->productions[i].left.length ; k++)
			{
				if( is_terminal( MNg->productions[i].left.word[k] ) )
					NTrequested++;
			}
			NTrequested = NTrequested + MNg->productions[i].left.length -1;
		}

	}

	if( NTrequested > cursor && is_MN( MNg ) )
	{
		printf("ATTENZIONE: simboli non sufficienti a convertire in CS tutte le produzioni\n\n");
		flag=0;
	}


	if ( is_MN( MNg ) && !is_CS( MNg ) && flag == 1 )
	{
		fp = fopen("Context-Sensitive_Grammar.txt","w");
		CSg->numprod=0; //Inizializzo la CSg
		for( i=0; i< MNg->numprod ; i++ ) //Per ogni produzione della grammatica MN stabilisco se è gia CS o se deve diventarlo
		{
			tempg.productions[0] = MNg->productions[i];
			tempg.numprod = 1;
			if ( is_CS( &tempg ) ) //Se è CS la aggiungo direttamente a CSg
			{
				CSg->productions[freeslot] = MNg->productions[i];
				freeslot ++;
				CSg->numprod ++;

			}
			if ( !is_CS( &tempg ) ) //Se non è CS, allora deve cominciare l'algoritmo
			{
				tempword.length = MNg->productions[i].left.length;
				strcpy(tempword.word, MNg->productions[i].left.word);
				for(k=0; k < MNg->productions[i].left.length; k++ ) //Creo le produzioni NT (primo disponibile) -> T/NT
				{
					if( is_terminal(MNg->productions[i].left.word[k]) )
					{
						CSg->numprod ++;
						CSg->productions[freeslot].left.word[0] = NTavailable[freeNT];
						CSg->productions[freeslot].left.length = 1;
						CSg->productions[freeslot].right.word[0] = MNg->productions[i].left.word[k];
						CSg->productions[freeslot].right.length = 1;
						tempword.word[k] = NTavailable[freeNT];
						freeNT++;
						freeslot++;
					}						
				}
				
				tempword2.length = MNg->productions[i].right.length; //Creo le produzioni NT (primo disponibile) -> T/NT
				strcpy(tempword2.word, MNg->productions[i].right.word);

				//Creo la prima metà delle produzione dell'algoritmo
				strcpy(lastword.word, tempword.word);
				for(k=0; k < tempword.length -1 ; k++)
				{
					CSg->numprod ++;
					
					CSg->productions[freeslot].left.length = tempword.length;
					strncpy( CSg->productions[freeslot].left.word, lastword.word, tempword.length);
					
					lastword.word[k] = NTavailable[freeNT]; //Cambio la lettera nella parte destra (che diventa la parte sinistra la passo successivo)

					CSg->productions[freeslot].right.length = tempword.length;
					strncpy( CSg->productions[freeslot].right.word, lastword.word, tempword.length);					
					
					freeNT ++;
					freeslot ++;

				}

				//Gestisco la produzione centrale dell'algoritmo
				CSg->numprod++;
				strcpy(CSg->productions[freeslot].left.word, lastword.word);
				CSg->productions[freeslot].left.length = tempword.length;
				
				lastword.length = tempword.length; 
				for(k=0; k < (tempword2.length - tempword.length + 1); k++ ) //Accodo alla parola tutti i nonterminali di avanzo
					lastword.word[ lastword.length + k -1] = tempword2.word[  lastword.length + k -1 ];

				//Completo la seconda parte della produzione
				lastword.length = tempword2.length;
				strcpy(CSg->productions[freeslot].right.word, lastword.word);
				CSg->productions[freeslot].right.length = tempword2.length;
				freeslot++;

				//Completo la seconda metà dell'algoritmo
				for(k = ( tempword.length - 1 ); k>0; k-- )
				{
					CSg->numprod ++;
					
					CSg->productions[freeslot].left.length = lastword.length;
					strncpy( CSg->productions[freeslot].left.word, lastword.word, lastword.length);
					
					lastword.word[k-1] = tempword2.word[k-1]; //Sostituisco i NT con quelli che erano originariamente nella parola

					CSg->productions[freeslot].right.length = lastword.length;
					strncpy( CSg->productions[freeslot].right.word, lastword.word, lastword.length);					
					
					freeslot ++;
				}	
			}	
		}
		//Dopo che ho convertito tutte le produzioni
		write_grammar(fp, CSg);
		fclose(fp);
		printf("Operazione di conversione da Monotona a Context-Sensitive completata.\n\n");
		printf("La grammatica e' stata salvata nel file 'Context-Sensitive_Grammar.txt'\n\n");
		printf("Ecco la nuova grammatica:\n\n");
		print_grammar(CSg);
	}
}

/* Teorema di chiusura ------------------------------------------------*/

void union_grammar (Grammar* g1, Grammar* g2, Grammar* g3)
{
	int uniontype; //Classe di g3
	int typeg1; //Classe di g1
	int typeg2; //Classe di g2
	char vetNTg1[27]; //NT di g1
	int cursvetNTg1=0; //Indice del vettore dei NT di g1
	char vetNTg2[27]; //NT di g2
	int cursvetNTg2=0; //Indice del vettore dei NT di g2
	char vetNTnotg1[27]; //NT assenti in g1
	int cursvetNTnotg1=0; //Indice del vettore dei NT assenti in g1
	char vetNTnotg2[27]; //NT assenti in g1 e g2
	int cursvetNTnotg2=0; //Indice del vettore dei NT assenti in g1 e g2
	int i, j, k, p, q;
	int found; //Flag
	int freeslot = 0; //Prima produzione libera di g3
	char S2 = '0';
	Grammar tempg1; //g1 con produzioni corrette
	Grammar tempg2; //g2 con produzioni corrette
	char temp;
	int substcursor=0; 
	FILE* fp;
	int error=0;

	tempg1 = *g1;
	tempg2 = *g2;
	g3->numprod = 0;

	if( is_CS(g1) )
		typeg1 = 1;
	if( is_CF(g1) )
		typeg1 = 2;
	if( is_RL(g1) )
		typeg1 = 3;
	if( !is_CS(g1) && !is_CF(g1) && !is_RL(g1) )
		typeg1 = 0;

	if( is_CS(g2) )
		typeg2 = 1;
	if( is_CF(g2) )
		typeg2 = 2;
	if( is_RL(g2) )
		typeg2 = 3;
	if( !is_CS(g2) && !is_CF(g2) && !is_RL(g2) )
		typeg2 = 0;

	if( typeg1 < typeg2 )
		uniontype = typeg1;
	else uniontype = typeg2;


	//Prima di rendere disgiunti gli alfabeti dei NT di g1 e g2 scrivo nella grammatica unione tutte le produzione { S -> w | S' -> w v S'' -> w } (solo se è una unione tra L3)
	if( uniontype == 3 )
	{
		for(i=0; i< g1->numprod; i++)
			if( ( g1->productions[i].left.length == 1 ) && ( g1->productions[i].left.word[0] == 'S' ) && ( there_is_nonterminal( &(g1->productions[i].right) ) == 0 )  ) //Se è una produzione S -> X*
			{
				g3->numprod++;
				g3->productions[freeslot] = g1->productions[i];
				freeslot++;
			}
		for(i=0; i< g2->numprod; i++)
			if( ( g2->productions[i].left.length == 1 ) && ( g2->productions[i].left.word[0] == 'S' )  && ( there_is_nonterminal( &(g2->productions[i].right) ) == 0 )   ) //Se è una produzione S -> X*
			{
				g3->numprod++;
				g3->productions[freeslot] = g2->productions[i];
				freeslot++;
			}
		}


	//Controllo che i due linguaggi soddisfino le assunzioni di fondo del teorma di chiusura
	//Alfabeti dei nonterminali disgiunti

	//Inserisco in vetNTg1[] tutti i NT presenti in g1
	for(i=0; i< g1->numprod; i++)
	{
		//Controllo i NT nella parte sinistra della produzione
		for(j=0; j< g1->productions[i].left.length; j++)
		{
			if ( is_nonterminal(g1->productions[i].left.word[j] ) )
			{
				//Scandisco vetNTg1; se non è presente il NT lo aggiungo

				//Se vetNTg1 è vuoto sicuramente dovrò aggiungere il NT
				if( cursvetNTg1 == 0)
				{
					vetNTg1[cursvetNTg1] = g1->productions[i].left.word[j];
					cursvetNTg1++;
				}
				//Se vetNTg1 non è vuoto dovrò aggiungere il NT solo se non è già presente
				if( cursvetNTg1 > 0)
				{
					found=0;
					for(k=0; k < cursvetNTg1; k++) //Controllo se è già presente
					{
						if( vetNTg1[k] == g1->productions[i].left.word[j] )
							found=1;
					}
					
					if(found == 0) //Se non l'ha trovato lo aggiungo
					{
						vetNTg1[cursvetNTg1] = g1->productions[i].left.word[j];
						cursvetNTg1++;

					}
				}
			}
		}

		//Controllo i NT nella parte destra della produzione
		for(j=0; j< g1->productions[i].right.length; j++)
		{
			if ( is_nonterminal(g1->productions[i].right.word[j] ) )
			{
				//Scandisco vetNTg1; se non è presente il NT lo aggiungo
				found=0;
				for(k=0; k < cursvetNTg1; k++) //Controllo se è già presente
				{
					if( vetNTg1[k] == g1->productions[i].right.word[j] )
						found=1;
				}
				
				if(found == 0) //Se non l'ha trovato lo aggiungo
				{
					vetNTg1[cursvetNTg1] = g1->productions[i].right.word[j];
					cursvetNTg1++;
				}
			}
		}
	}

	if( cursvetNTg1 > 24 )	//Se la prima grammatica utilizza più di 24 NT sicuramente non 
		error = 1;			//si avranno NT sufficienti a rinominare i due simboli iniziali 
							//delle grammatiche (S non è mai utilizzabile)

	if( error != 1)
	{

	//Inserisco in vetNTnotg1[] tutti i NT non presenti in g1
	for(i='A'; i<='Z'; i++)
	{
		found = 0;
		for(j=0; j < cursvetNTg1; j++)
		{
			if( vetNTg1[j] == i )
				found = 1;
		}
		if ( found == 0 )
		{
			vetNTnotg1[cursvetNTnotg1] = i;
			cursvetNTnotg1++;
		}

	}

	//Sostituisco il simbolo S di g1 con l'ultimo elemento di vetNTnotg1
	for(i=0; i< tempg1.numprod; i++)
	{
		for(j=0; j< tempg1.productions[i].left.length; j++)
			if ( tempg1.productions[i].left.word[j] == 'S' )
				tempg1.productions[i].left.word[j] = vetNTnotg1[cursvetNTnotg1-1];
		for(j=0; j< tempg1.productions[i].right.length; j++)
			if ( tempg1.productions[i].right.word[j] == 'S' )
				tempg1.productions[i].right.word[j] = vetNTnotg1[cursvetNTnotg1-1];
	
	}

	//Aggiungo l'ultimo elemento di vetNTnotg1 a vetNTg1
	vetNTg1[cursvetNTg1] = vetNTnotg1[cursvetNTnotg1-1];
	cursvetNTg1++;
	//N.B.: L'ultimo elemento di vetNTg1 sarà il nuovo simbolo iniziale di g1

	//Elimino l'ultimo elemento di vetNTnotg1 da vetNTnotg1
	cursvetNTnotg1--;
	vetNTnotg1[cursvetNTnotg1] = '0';

	//Inserisco in vetNTg2[] tutti i NT presenti in g2
	for(i=0; i< g2->numprod; i++)
	{
		//Controllo i NT nella parte sinistra della produzione
		for(j=0; j< g2->productions[i].left.length; j++)
		{
			if ( is_nonterminal(g2->productions[i].left.word[j] ) )
			{
				//Scandisco vetNTg2; se non è presente il NT lo aggiungo

				//Se vetNTg2 è vuoto sicuramente dovrò aggiungere il NT
				if( cursvetNTg2 == 0)
				{
					vetNTg2[cursvetNTg2] = g2->productions[i].left.word[j];
					cursvetNTg2++;
				}
				//Se vetNTg2 non è vuoto dovrò aggiungere il NT solo se non è già presente
				if( cursvetNTg2 > 0)
				{
					found=0;
					for(k=0; k < cursvetNTg2; k++) //Controllo se è già presente
					{
						if( vetNTg2[k] == g2->productions[i].left.word[j] )
							found=1;
					}
					
					if(found == 0) //Se non l'ha trovato lo aggiungo
					{
						vetNTg2[cursvetNTg2] = g2->productions[i].left.word[j];
						cursvetNTg2++;

					}
				}
			}
		}

		//Controllo i NT nella parte destra della produzione
		for(j=0; j< g2->productions[i].right.length; j++)
		{
			if ( is_nonterminal(g2->productions[i].right.word[j] ) )
			{
				//Scandisco vetNTg1; se non è presente il NT lo aggiungo
				found=0;
				for(k=0; k < cursvetNTg2; k++) //Controllo se è già presente
				{
					if( vetNTg2[k] == g2->productions[i].right.word[j] )
						found=1;
				}
				
				if(found == 0) //Se non l'ha trovato lo aggiungo
				{
					vetNTg2[cursvetNTg2] = g2->productions[i].right.word[j];
					cursvetNTg2++;
				}
			}
		}
	}

	//Inserisco in vetNTnotg2 i NT che non sono presenti nè in g1 nè in g2: saranno quelli utilizzabili per la sostituzione
	for(i=0; i< cursvetNTnotg1; i++)
	{
		found = 0;
		for(j=0; j< cursvetNTg2; j++)
		{
			if(vetNTnotg1[i] == vetNTg2[j] )
				found = 1;
		}
		if( found == 0 )
		{
			vetNTnotg2[cursvetNTnotg2] = vetNTnotg1[i];
			cursvetNTnotg2++;
		}
	}

	//Sostituisco tutti i NT di g2 presenti anche in g1 con NT non utilizzati (conservati in vetNTnotg2[] )
	for(i=0; i< tempg2.numprod && ( substcursor <= cursvetNTnotg2 ); i++)
	{
		//Sostituisco i NT delle parti sinistre delle produzioni
		for(j=0; j< tempg2.productions[i].left.length; j++)
		{
			if( is_nonterminal( tempg2.productions[i].left.word[j] ) )
			{
				found = 0;
				//Verifico se appartiene a vetNTg1
				for(k=0; k < cursvetNTg1 && found == 0; k++)
					if( tempg2.productions[i].left.word[j] == vetNTg1[k] )
						found = 1;
				//Se appartiene lo sostituisco col primo NT disponibile di vetNTnotg2
				if ( found == 1 )
				{
					temp = tempg2.productions[i].left.word[j];
					
					if( tempg2.productions[i].left.word[j] == 'S' && S2 == '0') //Mi salvo quale NT sarà il simbolo iniziale di g2
						S2 = vetNTnotg2[substcursor];
					
					tempg2.productions[i].left.word[j] = vetNTnotg2[substcursor];

					for(q=0; q< tempg2.numprod;q++)
					{
						for(p=0; p< tempg2.productions[q].left.length; p++)
						{
							if( tempg2.productions[q].left.word[p] == temp )
								tempg2.productions[q].left.word[p] = vetNTnotg2[substcursor]; 
						}

						for(p=0; p< tempg2.productions[q].right.length; p++)
						{
							if( tempg2.productions[q].right.word[p] == temp )
								tempg2.productions[q].right.word[p] = vetNTnotg2[substcursor]; 
						}

					}


					substcursor++;

				}

			}
		}

		//Sostituisco i NT delle parti destre delle produzioni
		for(j=0; j< tempg2.productions[i].right.length; j++)
		{
			if( is_nonterminal( tempg2.productions[i].right.word[j] ) )
			{
				found = 0;
				//Verifico se appartiene a vetNTg1
				for(k=0; k < cursvetNTg1 && found == 0; k++)
					if( tempg2.productions[i].right.word[j] == vetNTg1[k] )
						found = 1;
				//Se appartiene lo sostituisco col primo NT disponibile di vetNTnotg2
				if ( found == 1 )
				{
					temp = tempg2.productions[i].right.word[j];
					tempg2.productions[i].right.word[j] = vetNTnotg2[substcursor];

					for(q=0; q< tempg2.numprod;q++)
					{
						for(p=0; p< tempg2.productions[q].left.length; p++)
						{
							if( tempg2.productions[q].left.word[p] == temp )
								tempg2.productions[q].left.word[p] = vetNTnotg2[substcursor]; 
						}

						for(p=0; p< tempg2.productions[q].right.length; p++)
						{
							if( tempg2.productions[q].right.word[p] == temp )
								tempg2.productions[q].right.word[p] = vetNTnotg2[substcursor]; 
						}

					}


					substcursor++;

				}

			}
		}

	}
	
	if( uniontype != 3) //Aggiungo la produzione S -> A|B   (A e B sono i due nuovi simboli iniziali)
	{
		g3->productions[freeslot].left.length = 1;
		g3->productions[freeslot].left.word[0] = 'S';
		g3->productions[freeslot].right.length = 1;
		g3->productions[freeslot].right.word[0] = vetNTg1[cursvetNTg1 -1];
		g3->numprod++;
		freeslot++;
		g3->productions[freeslot].left.length = 1;
		g3->productions[freeslot].left.word[0] = 'S';
		g3->productions[freeslot].right.length = 1;
		g3->productions[freeslot].right.word[0] = S2;
		g3->numprod++;
		freeslot++;
	}

	//Aggiungo a g3 le produzioni S -> (X u V)* di g1 e g2, ma con i NT rinominati correttamente
	for(i=0; i< g1->numprod; i++)
	{
		if( ( g1->productions[i].left.length == 1 ) && ( g1->productions[i].left.word[0] == 'S' ) && ( there_is_nonterminal( &(g1->productions[i].right) ) )  )
		{
			g3->productions[freeslot].left.length = 1;
			g3->productions[freeslot].left.word[0] = 'S';
			g3->productions[freeslot].right = tempg1.productions[i].right;
			freeslot++;
			g3->numprod++;
		}
	}

	for(i=0; i< g2->numprod; i++)
	{
		if( ( g2->productions[i].left.length == 1 ) && ( g2->productions[i].left.word[0] == 'S' ) && ( there_is_nonterminal( &(g2->productions[i].right) ) )  )
		{
			g3->productions[freeslot].left.length = 1;
			g3->productions[freeslot].left.word[0] = 'S';
			g3->productions[freeslot].right = tempg2.productions[i].right;
			freeslot++;
			g3->numprod++;
		}
	}

	//Aggiungo a g3 le produzioni di tempg1 e tempg2
	for(i=0; i< tempg1.numprod; i++)
	{
		g3->productions[freeslot] = tempg1.productions[i];
		freeslot++;
		g3->numprod++;
	}

	for(i=0; i< tempg2.numprod; i++)
	{
		g3->productions[freeslot] = tempg2.productions[i];
		freeslot++;
		g3->numprod++;
	}

	if( substcursor > cursvetNTnotg2 )
		error = 1;

	if(error == 1)
		printf("ATTENZIONE: Simboli dell'alfabeto insufficienti per l'unione delle grammatiche\n\n");
	else {

	fp = fopen("Union_Grammar.txt","w");
	write_grammar(fp, g3);
	fclose(fp);
	printf("Operazione di unione delle grammatiche completata.\n\n");
	printf("La grammatica e' stata salvata nel file 'Union_Grammar.txt'\n\n");
	printf("Ecco la nuova grammatica:\n\n");
	print_grammar(g3);	
	
	}

	}
	else printf("ATTENZIONE: Simboli dell'alfabeto insufficienti per l'unione delle grammatiche\n\n");

}

void concatenation_grammar (Grammar* g1, Grammar* g2, Grammar* g3)
{
	int concatenationtype;
	int typeg1;
	int typeg2;
	char vetNTg1[27];
	int cursvetNTg1=0;
	char vetNTg2[27];
	int cursvetNTg2=0;
	char vetNTnotg1[27];
	int cursvetNTnotg1=0;
	char vetNTnotg2[27];
	int cursvetNTnotg2=0;
	int i, j, k, p, q;
	int found; //Flag
	int freeslot = 0; //Prima produzione libera di g3
	char S2 = '0';
	Grammar tempg1;
	Grammar tempg2;
	char temp;
	int substcursor=0;
	FILE* fp;
	int error=0;

	tempg1 = *g1;
	tempg2 = *g2;
	g3->numprod = 0;

	if( is_CS(g1) )
		typeg1 = 1;
	if( is_CF(g1) )
		typeg1 = 2;
	if( is_RL(g1) )
		typeg1 = 3;
	if( !is_CS(g1) && !is_CF(g1) && !is_RL(g1) )
		typeg1 = 0;

	if( is_CS(g2) )
		typeg2 = 1;
	if( is_CF(g2) )
		typeg2 = 2;
	if( is_RL(g2) )
		typeg2 = 3;
	if( !is_CS(g2) && !is_CF(g2) && !is_RL(g2) )
		typeg2 = 0;

	if( typeg1 < typeg2 )
		concatenationtype = typeg1;
	else concatenationtype = typeg2;


	if( concatenationtype == 2 || concatenationtype == 3)
	{

	//Controllo che i due linguaggi soddisfino le assunzioni di fondo del teorma di chiusura
	//Alfabeti dei nonterminali disgiunti

	//Inserisco in vetNTg1[] tutti i NT presenti in g1
	for(i=0; i< g1->numprod; i++)
	{
		//Controllo i NT nella parte sinistra della produzione
		for(j=0; j< g1->productions[i].left.length; j++)
		{
			if ( is_nonterminal(g1->productions[i].left.word[j] ) )
			{
				//Scandisco vetNTg1; se non è presente il NT lo aggiungo

				//Se vetNTg1 è vuoto sicuramente dovrò aggiungere il NT
				if( cursvetNTg1 == 0)
				{
					vetNTg1[cursvetNTg1] = g1->productions[i].left.word[j];
					cursvetNTg1++;
				}
				//Se vetNTg1 non è vuoto dovrò aggiungere il NT solo se non è già presente
				if( cursvetNTg1 > 0)
				{
					found=0;
					for(k=0; k < cursvetNTg1; k++) //Controllo se è già presente
					{
						if( vetNTg1[k] == g1->productions[i].left.word[j] )
							found=1;
					}
					
					if(found == 0) //Se non l'ha trovato lo aggiungo
					{
						vetNTg1[cursvetNTg1] = g1->productions[i].left.word[j];
						cursvetNTg1++;

					}
				}
			}
		}

		//Controllo i NT nella parte destra della produzione
		for(j=0; j< g1->productions[i].right.length; j++)
		{
			if ( is_nonterminal(g1->productions[i].right.word[j] ) )
			{
				//Scandisco vetNTg1; se non è presente il NT lo aggiungo
				found=0;
				for(k=0; k < cursvetNTg1; k++) //Controllo se è già presente
				{
					if( vetNTg1[k] == g1->productions[i].right.word[j] )
						found=1;
				}
				
				if(found == 0) //Se non l'ha trovato lo aggiungo
				{
					vetNTg1[cursvetNTg1] = g1->productions[i].right.word[j];
					cursvetNTg1++;
				}
			}
		}
	}

	if( cursvetNTg1 > 24 )	//Se la prima grammatica utilizza più di 24 NT sicuramente non 
		error = 1;			//si avranno NT sufficienti a rinominare i due simboli iniziali 
							//delle grammatiche (S non è mai utilizzabile)

	if( error != 1)
	{

	//Inserisco in vetNTnotg1[] tutti i NT non presenti in g1
	for(i='A'; i<='Z'; i++)
	{
		found = 0;
		for(j=0; j < cursvetNTg1; j++)
		{
			if( vetNTg1[j] == i )
				found = 1;
		}
		if ( found == 0 )
		{
			vetNTnotg1[cursvetNTnotg1] = i;
			cursvetNTnotg1++;
		}

	}

	//Sostituisco il simbolo S di g1 con l'ultimo elemento di vetNTnotg1
	for(i=0; i< tempg1.numprod; i++)
	{
		for(j=0; j< tempg1.productions[i].left.length; j++)
			if ( tempg1.productions[i].left.word[j] == 'S' )
				tempg1.productions[i].left.word[j] = vetNTnotg1[cursvetNTnotg1-1];
		for(j=0; j< tempg1.productions[i].right.length; j++)
			if ( tempg1.productions[i].right.word[j] == 'S' )
				tempg1.productions[i].right.word[j] = vetNTnotg1[cursvetNTnotg1-1];
	
	}

	//Aggiungo l'ultimo elemento di vetNTnotg1 a vetNTg1
	vetNTg1[cursvetNTg1] = vetNTnotg1[cursvetNTnotg1-1];
	cursvetNTg1++;
	//N.B.: L'ultimo elemento di vetNTg1 sarà il nuovo simbolo iniziale di g1

	//Elimino l'ultimo elemento di vetNTnotg1 da vetNTnotg1
	cursvetNTnotg1--;
	vetNTnotg1[cursvetNTnotg1] = '0';

	//Inserisco in vetNTg2[] tutti i NT presenti in g2
	for(i=0; i< g2->numprod; i++)
	{
		//Controllo i NT nella parte sinistra della produzione
		for(j=0; j< g2->productions[i].left.length; j++)
		{
			if ( is_nonterminal(g2->productions[i].left.word[j] ) )
			{
				//Scandisco vetNTg2; se non è presente il NT lo aggiungo

				//Se vetNTg2 è vuoto sicuramente dovrò aggiungere il NT
				if( cursvetNTg2 == 0)
				{
					vetNTg2[cursvetNTg2] = g2->productions[i].left.word[j];
					cursvetNTg2++;
				}
				//Se vetNTg2 non è vuoto dovrò aggiungere il NT solo se non è già presente
				if( cursvetNTg2 > 0)
				{
					found=0;
					for(k=0; k < cursvetNTg2; k++) //Controllo se è già presente
					{
						if( vetNTg2[k] == g2->productions[i].left.word[j] )
							found=1;
					}
					
					if(found == 0) //Se non l'ha trovato lo aggiungo
					{
						vetNTg2[cursvetNTg2] = g2->productions[i].left.word[j];
						cursvetNTg2++;

					}
				}
			}
		}

		//Controllo i NT nella parte destra della produzione
		for(j=0; j< g2->productions[i].right.length; j++)
		{
			if ( is_nonterminal(g2->productions[i].right.word[j] ) )
			{
				//Scandisco vetNTg1; se non è presente il NT lo aggiungo
				found=0;
				for(k=0; k < cursvetNTg2; k++) //Controllo se è già presente
				{
					if( vetNTg2[k] == g2->productions[i].right.word[j] )
						found=1;
				}
				
				if(found == 0) //Se non l'ha trovato lo aggiungo
				{
					vetNTg2[cursvetNTg2] = g2->productions[i].right.word[j];
					cursvetNTg2++;
				}
			}
		}
	}

	//Inserisco in vetNTnotg2 i NT che non sono presenti nè in g1 nè in g2: saranno quelli utilizzabili per la sostituzione
	for(i=0; i< cursvetNTnotg1; i++)
	{
		found = 0;
		for(j=0; j< cursvetNTg2; j++)
		{
			if(vetNTnotg1[i] == vetNTg2[j] )
				found = 1;
		}
		if( found == 0 )
		{
			vetNTnotg2[cursvetNTnotg2] = vetNTnotg1[i];
			cursvetNTnotg2++;
		}
	}

	//Sostituisco tutti i NT di g2 presenti anche in g1 con NT non utilizzati (conservati in vetNTnotg2[] )
	for(i=0; i< tempg2.numprod && ( substcursor <= cursvetNTnotg2 ); i++)
	{
		//Sostituisco i NT delle parti sinistre delle produzioni
		for(j=0; j< tempg2.productions[i].left.length; j++)
		{
			if( is_nonterminal( tempg2.productions[i].left.word[j] ) )
			{
				found = 0;
				//Verifico se appartiene a vetNTg1
				for(k=0; k < cursvetNTg1 && found == 0; k++)
					if( tempg2.productions[i].left.word[j] == vetNTg1[k] )
						found = 1;
				//Se appartiene lo sostituisco col primo NT disponibile di vetNTnotg2
				if ( found == 1 )
				{
					temp = tempg2.productions[i].left.word[j];
					
					if( tempg2.productions[i].left.word[j] == 'S' && S2 == '0') //Mi salvo quale NT sarà il simbolo iniziale di g2
						S2 = vetNTnotg2[substcursor];
					
					tempg2.productions[i].left.word[j] = vetNTnotg2[substcursor];

					for(q=0; q< tempg2.numprod;q++)
					{
						for(p=0; p< tempg2.productions[q].left.length; p++)
						{
							if( tempg2.productions[q].left.word[p] == temp )
								tempg2.productions[q].left.word[p] = vetNTnotg2[substcursor]; 
						}

						for(p=0; p< tempg2.productions[q].right.length; p++)
						{
							if( tempg2.productions[q].right.word[p] == temp )
								tempg2.productions[q].right.word[p] = vetNTnotg2[substcursor]; 
						}

					}


					substcursor++;

				}

			}
		}

		//Sostituisco i NT delle parti destre delle produzioni
		for(j=0; j< tempg2.productions[i].right.length; j++)
		{
			if( is_nonterminal( tempg2.productions[i].right.word[j] ) )
			{
				found = 0;
				//Verifico se appartiene a vetNTg1
				for(k=0; k < cursvetNTg1 && found == 0; k++)
					if( tempg2.productions[i].right.word[j] == vetNTg1[k] )
						found = 1;
				//Se appartiene lo sostituisco col primo NT disponibile di vetNTnotg2
				if ( found == 1 )
				{
					temp = tempg2.productions[i].right.word[j];
					tempg2.productions[i].right.word[j] = vetNTnotg2[substcursor];

					for(q=0; q< tempg2.numprod;q++)
					{
						for(p=0; p< tempg2.productions[q].left.length; p++)
						{
							if( tempg2.productions[q].left.word[p] == temp )
								tempg2.productions[q].left.word[p] = vetNTnotg2[substcursor]; 
						}

						for(p=0; p< tempg2.productions[q].right.length; p++)
						{
							if( tempg2.productions[q].right.word[p] == temp )
								tempg2.productions[q].right.word[p] = vetNTnotg2[substcursor]; 
						}

					}


					substcursor++;

				}

			}
		}

	}
	
	if( concatenationtype == 2) //Aggiungo la produzione S -> AB   (A e B sono i due nuovi simboli iniziali)
	{
		g3->productions[freeslot].left.length = 1;
		g3->productions[freeslot].left.word[0] = 'S';
		g3->productions[freeslot].right.length = 2;
		g3->productions[freeslot].right.word[0] = vetNTg1[cursvetNTg1 -1];
		g3->productions[freeslot].right.word[1] = S2;
		g3->numprod++;
		freeslot++;
	
		//Aggiungo a g3 le produzioni di tempg1 e tempg2
		for(i=0; i< tempg1.numprod; i++)
		{
			g3->productions[freeslot] = tempg1.productions[i];
			freeslot++;
			g3->numprod++;
		}

		for(i=0; i< tempg2.numprod; i++)
		{
			g3->productions[freeslot] = tempg2.productions[i];
			freeslot++;
			g3->numprod++;
		}
	}


	if( concatenationtype == 3) 
	{
		for(i=0; i< tempg1.numprod; i++) //Per ogni produzione stabilisco cosa andrà scritto in g3 (vedo se va concatenato S2, ecc...)
		{
			//Se è una produzione A -> bB allora aggiungo la produzione A -> bB
			if( tempg1.productions[i].right.length == 2 ) //La parte sx è per forza un NT. Se la parte destra è lunga 2 allora è per forza T NT
			{
				g3->numprod++;
				g3->productions[freeslot] = tempg1.productions[i];
				freeslot++;
			}

			//Se è una produzione A -> b (b non lambda) allora aggiungo la produzione A -> bS2
			if( ( tempg1.productions[i].right.length == 1 ) && ( is_terminal( tempg1.productions[i].right.word[0] ) ) ) //Se la lunghezza è 1 sicuramente non è una produzione NT -> lambda (che non rientrano in questo caso)
			{
				g3->numprod++;
				g3->productions[freeslot].left.length = 1;
				g3->productions[freeslot].left.word[0] = tempg1.productions[i].left.word[0];
				g3->productions[freeslot].right.length = 2;
				g3->productions[freeslot].right.word[0] = tempg1.productions[i].right.word[0];
				g3->productions[freeslot].right.word[1] = S2;
				freeslot++;
			}
			
			//Se è una produzione A -> bB e B -> lambda allora aggiungo la produzione A -> bS2
			if( tempg1.productions[i].right.length == 2 )	//La parte sx è per forza un NT. Se la parte destra è lunga 2 allora è per forza T NT.
			{												//Devo verificare che il NT finale della produzione generi nella grammatica lambda
				temp = tempg1.productions[i].right.word[1];
				found = 0; //Imposto il flag a falso: non è stata trovata una produzione NT -> lambda
			
				for(j=0; ( j< tempg1.numprod && found == 0 ); j++) //Vedo se ci sono produzioni NT -> lambda
					if( (tempg1.productions[j].left.word[0] == temp) && ( tempg1.productions[j].right.length == 0 ) ) //La parte sinistra deve essere il NT e la parte destra deve avere lunghezza 0
						found = 1;
				
				if ( found == 1 ) //Se esiste una produzione NT -> lambda allora posso aggiungere A -> bS2
				{
					g3->numprod++;
					g3->productions[freeslot].left.length = 1;
					g3->productions[freeslot].left.word[0] = tempg1.productions[i].left.word[0];
					g3->productions[freeslot].right.length = 2;
					g3->productions[freeslot].right.word[0] = tempg1.productions[i].right.word[0];
					g3->productions[freeslot].right.word[1] = S2;
					freeslot++;
				}

			}
		}
		
		//Verifico se S1 -> lambda: in caso affermativo aggiungo le produzioni S2 -> w 
		found = 0; //Non ci sono S1 -> lambda
		for (i=0; ( i< tempg1.numprod && found == 0 ); i++)
			if( tempg1.productions[i].left.word[0] == vetNTg1[cursvetNTg1 - 1] && tempg1.productions[i].right.length == 0  ) //vetNTg1[cursvetNTg1-1] = Nuovo simbolo iniziale della prima grammatica
				found = 1;

		if (found == 1) //Aggiungo le produzioni S1 -> w se S2 -> w
		{
			for(i=0; i< tempg2.numprod; i++ )
			{
				if( ( tempg2.productions[i].left.word[0] == S2 ) ) // && ( tempg2.productions[i].right.length < 2 )
				{
					g3->numprod++;
					g3->productions[freeslot].left.length = 1;
					g3->productions[freeslot].left.word[0] = vetNTg1[cursvetNTg1-1];
					g3->productions[freeslot].right = tempg2.productions[i].right;
					freeslot++;
				}
			}
		}

		//Aggiungo P2
		for(i=0; i< tempg2.numprod; i++)
		{
			g3->numprod++;
			g3->productions[freeslot] = tempg2.productions[i];
			freeslot++;
		}
		
	}

	if( substcursor > cursvetNTnotg2 ) //Se vanno sostituiti più simboli di quanti ne abbiamo a disposizione
		error = 1;

	if(error == 1)
		printf("ATTENZIONE: Simboli dell'alfabeto insufficienti per la concatenazione\ndelle grammatiche\n\n");
	else {

	fp = fopen("Concatenation_Grammar.txt","w");
	write_grammar(fp, g3);
	fclose(fp);
	printf("Operazione di concatenazione delle grammatiche completata.\n\n");
	printf("La grammatica e' stata salvata nel file 'Concatenation_Grammar.txt'\n\n");
	if( concatenationtype == 3 )
	{
			printf("ATTENZIONE: il simbolo iniziale della nuova grammatica %c\n", vetNTg1[cursvetNTg1-1]);
			printf("N.B.: il simbolo iniziale della seconda grammatica e' diventato %c\n\n", S2);
	}
	printf("Ecco la nuova grammatica:\n\n");
	print_grammar(g3);	
	
	}

	}
	else printf("ATTENZIONE: Simboli dell'alfabeto insufficienti per la concatenazione\ndelle grammatiche\n\n");

	}
	else printf("Operazione di concatenazione non eseguibile per le grammatiche in input\n\n");
}

void iteration_grammar (Grammar* g1, Grammar* g3)
{
	int iterationtype;
	Grammar tempg1;
	int i, j, k;
	char S1 = '0';
	int found = 0; //Flag
	int freeslot = 0;
	char temp;
	int error=0;
	int flag = 0;
	FILE* fp;

	if( is_CS(g1) )
		iterationtype = 1;
	if( is_CF(g1) )
		iterationtype = 2;
	if( is_RL(g1) )
		iterationtype = 3;
	if( !is_CS(g1) && !is_CF(g1) && !is_RL(g1) )
		iterationtype = 0;

	if( iterationtype == 3)
	{
		g3->numprod = 0;
		tempg1 = *g1;

		//Sostituisco il simbolo iniziale della grammatica in input col primo NT disponibile
		
		//Ricerco il primo NT disponibile
		for(i='A'; ( i<='Z' && S1 == '0' ); i++)
		{
			found = 0;
			
			for(j=0; j< g1->numprod; j++)
				for(k=0; k< g1->productions[j].left.length; k++)
					if( g1->productions[j].left.word[k] == i )
						found = 1;
			
			for(j=0; j< g1->numprod; j++)
				for(k=0; k< g1->productions[j].right.length; k++)
					if( g1->productions[j].right.word[k] == i )
						found = 1;

			if( found == 0 )
				S1 = i;

		}

		if( S1 == '0' ) //Se non ci sono NT disponibili
			error = 1;

		//Sostituisco il simbolo S con il NT 
		for(i=0; i<tempg1.numprod; i++)
		{
			for(j=0; j< tempg1.productions[i].left.length; j++)
				if( tempg1.productions[i].left.word[j] == 'S')
					tempg1.productions[i].left.word[j] = S1;

			for(j=0; j< tempg1.productions[i].right.length; j++)
				if( tempg1.productions[i].right.word[j] == 'S')
					tempg1.productions[i].right.word[j] = S1;
		}
	
		//Aggiungo la produzione S -> lambda
		g3->numprod++;
		g3->productions[freeslot].left.length = 1;
		g3->productions[freeslot].left.word[0] = 'S';
		g3->productions[freeslot].right.length = 0;
		freeslot++;

		/*
		//Aggiungo tutte le produzioni di P1, eccetto S1 -> lambda
		for(i=0; i< tempg1.numprod; i++)
		{
			if( ( tempg1.productions[i].left.length == 1 ) && ( tempg1.productions[i].left.word[0] == S1 ) && ( tempg1.productions[i].right.length == 0 ) )
			{
			}
			else
			{
				g3->numprod++;
				g3->productions[freeslot] = tempg1.productions[i];
				freeslot++;
			}

		}
		*/

		
		//Aggiungo tutte le produzioni di P1, INCLUSA S1 -> lambda
		for(i=0; i< tempg1.numprod; i++)
		{
			g3->numprod++;
			g3->productions[freeslot] = tempg1.productions[i];
			freeslot++;
		}
		


		//Aggiungo per ogni produzione S1 -> w la produzione S -> w
		for(i=0; i< tempg1.numprod; i++)
		{
			if( ( tempg1.productions[i].left.length == 1 ) && ( tempg1.productions[i].left.word[0] == S1 ) )
			{
				g3->numprod++;
				g3->productions[freeslot].left.length = 1;
				g3->productions[freeslot].left.word[0] = 'S';
				g3->productions[freeslot].right = tempg1.productions[i].right;
				freeslot++;
			}
		}

		//Aggiungo per ogni produzione A -> b di g3 la produzione A -> bS
		for(i=0; i< g3->numprod; i++)
		{
			if( g3->productions[i].right.length == 1 )
			{
				g3->numprod++;
				g3->productions[freeslot].left.length = 1;
				g3->productions[freeslot].left.word[0] = g3->productions[i].left.word[0];
				g3->productions[freeslot].right.length = 2;
				g3->productions[freeslot].right.word[0] = g3->productions[i].right.word[0];
				g3->productions[freeslot].right.word[1] = 'S';
				freeslot++;
			}
		}

		//Aggiungo per ogni produzione A -> bB e B-> lambda di g3 la produzione A -> bS
		for(i=0; i< g3->numprod; i++)
		{
			flag = 0;
			if( ( g3->productions[i].right.length == 2 ) && ( g3->productions[i].right.word[1] != 'S' ) ) //Se trovo una produzione del tipo A -> bB //Onde evitare loop: B diverso da S
			{
				temp = g3->productions[i].right.word[1];
				for(j=0; j< g3->numprod; j++) //Controllo se esiste una B -> lambda
					if( ( g3->productions[j].left.length == 1 ) && ( g3->productions[j].left.word[0] == temp ) && ( g3->productions[j].right.length == 0 ) )
						flag = 1;	
			}
			if( flag == 1)
				{
						g3->numprod++;
						g3->productions[freeslot].left.length = 1;
						g3->productions[freeslot].left.word[0] = g3->productions[i].left.word[0];
						g3->productions[freeslot].right.length = 2;
						g3->productions[freeslot].right.word[0] = g3->productions[i].right.word[0];
						g3->productions[freeslot].right.word[1] = 'S';
						freeslot++;
				}
		}

		if(error == 1)
			printf("ATTENZIONE: Simboli dell'alfabeto insufficienti per l'iterazione\ndella grammatica\n\n");
		else 
		{

			fp = fopen("Iteration_Grammar.txt","w");
			write_grammar(fp, g3);
			fclose(fp);
			printf("Operazione di iterazione della grammatica completata.\n\n");
			printf("La grammatica e' stata salvata nel file 'Iteration_Grammar.txt'\n\n");
			printf("ATTENZIONE: il simbolo iniziale della nuova grammatica e' 'S'\n");
			printf("N.B.: il simbolo iniziale della grammatica in input e' diventato %c\n\n", S1);
			printf("Ecco la nuova grammatica:\n\n");
			print_grammar(g3);	
		}
	}


	if( iterationtype == 2)
	{
		g3->numprod = 0;
		tempg1 = *g1;

		//Sostituisco il simbolo iniziale della grammatica in input col primo NT disponibile
		
		//Ricerco il primo NT disponibile
		for(i='A'; ( i<='Z' && S1 == '0' ); i++)
		{
			found = 0;
			
			for(j=0; j< g1->numprod; j++)
				for(k=0; k< g1->productions[j].left.length; k++)
					if( g1->productions[j].left.word[k] == i )
						found = 1;
			
			for(j=0; j< g1->numprod; j++)
				for(k=0; k< g1->productions[j].right.length; k++)
					if( g1->productions[j].right.word[k] == i )
						found = 1;

			if( found == 0 )
				S1 = i;

		}

		if( S1 == '0' ) //Se non ci sono NT disponibili
			error = 1;

		//Sostituisco il simbolo S con il NT 
		for(i=0; i<tempg1.numprod; i++)
		{
			for(j=0; j< tempg1.productions[i].left.length; j++)
				if( tempg1.productions[i].left.word[j] == 'S')
					tempg1.productions[i].left.word[j] = S1;

			for(j=0; j< tempg1.productions[i].right.length; j++)
				if( tempg1.productions[i].right.word[j] == 'S')
					tempg1.productions[i].right.word[j] = S1;
		}
	
		//Aggiungo le produzioni S-> lambda e S-> S1S
		g3->numprod++;
		g3->productions[freeslot].left.length = 1;
		g3->productions[freeslot].left.word[0] = 'S';
		g3->productions[freeslot].right.length = 0;
		freeslot++;
		g3->numprod++;
		g3->productions[freeslot].left.length = 1;
		g3->productions[freeslot].left.word[0] = 'S';
		g3->productions[freeslot].right.length = 2;
		g3->productions[freeslot].right.word[0] = S1;
		g3->productions[freeslot].right.word[1] = 'S';
		freeslot++;

		//Aggiungo tutte le produzioni di P1
		for(i=0; i< tempg1.numprod; i++)
		{
			g3->numprod++;
			g3->productions[freeslot] = tempg1.productions[i];
			freeslot++;
		}

		if(error == 1)
			printf("ATTENZIONE: Simboli dell'alfabeto insufficienti per l'iterazione\ndella grammatica\n\n");
		else 
		{

			fp = fopen("Iteration_Grammar.txt","w");
			write_grammar(fp, g3);
			fclose(fp);
			printf("Operazione di iterazione della grammatica completata.\n\n");
			printf("La grammatica e' stata salvata nel file 'Iteration_Grammar.txt'\n\n");
			printf("ATTENZIONE: il simbolo iniziale della nuova grammatica e' 'S'\n");
			printf("N.B.: il simbolo iniziale della grammatica in input e' diventato %c\n\n", S1);
			printf("Ecco la nuova grammatica:\n\n");
			print_grammar(g3);	
		}
	
	
	}

	if( iterationtype < 2)
	{
		printf("ATTENZIONE: operazione di iterazione della grammatica non supportata.\n\n");
	}
}