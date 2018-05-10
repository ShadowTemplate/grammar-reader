#include "Header.h"

int main (int argc, char *argv[])
{
  char* filename = argv[1]; //Primo file di testo trascinato nell'eseguibile
  char* filename2 = argv[2]; //Secondo file di testo trascinato nell'eseguibile
  FILE* gram_file; //File contenente la grammatica
  FILE* gram_file2; //File contenente la seconda grammatica
  Grammar grammar; //Grammatica letta dal file
  Grammar grammar2; //Grammatica letta dal file
  Grammar CSgrammar; //Grammatica CS ottenuta convertendo argv[1], se monotono
  Grammar uniongrammar; //Grammatica unione di argv[1] e argv[2]
  Grammar concatenationgrammar; //Grammatica concatenazione di argv[1] e argv[2]
  Grammar iterationgrammar; //Grammatica iterazione di argv[1]
  

  // controlla se è stato inserito il nome del file
  if (filename == 0)
  {
     printf("nome file non specificato \n");
     system("PAUSE");	
	 return -1;
  }
  
  // apertura del file contenente la grammatica
  gram_file = fopen(filename,"r+");
  if (gram_file == NULL)
  {
     printf("nome di file errato\n");
	 system("PAUSE");	
     return -1;
  }
  
  print_grammar(load_grammar(gram_file,&grammar));

  if (grammar.numprod > 0)
  {
  
	  if(is_CF(&grammar))
		  printf("Grammatica Context-Free\n\n");
	  else
		  printf("Grammatica non Context-Free\n\n");
  
	  if(is_MN(&grammar))
		  printf("Grammatica Monotona\n\n");
	  else
		  printf("Grammatica non Monotona\n\n");

	  if(is_RL(&grammar))
		  printf("Grammatica Lineare Destra\n\n");
	  else
		  printf("Grammatica non Lineare Destra\n\n");

	  if(is_CS(&grammar))
		  printf("Grammatica Context-Sensitive\n\n");
	  else
		  printf("Grammatica non Context-Sensitive\n\n");

  }
  else printf("Impossibile stabilire la classe di appartenza. \n\n");

  delete_productions( &grammar );
  printf("\n");
  write_productions(&grammar, gram_file);
  printf("\n");
  
  convert_MN_to_CS(&grammar, &CSgrammar);

  fclose(gram_file);


  // controlla se è stato inserito il nome del secondo file
  if (filename2 == 0)
  {
     printf("nome secondo file non specificato \n");
     system("PAUSE");	
	 return -1;
  }
  
  // apertura del file contenente la grammatica
  gram_file2 = fopen(filename2,"r+");
  if (gram_file2 == NULL)
  {
     printf("nome del secondo file errato\n");
	 system("PAUSE");	
     return -1;
  }
  
  print_grammar(load_grammar(gram_file2,&grammar2));
  
  if (grammar2.numprod > 0)
  {
  
	  if(is_CF(&grammar2))
		  printf("Grammatica Context-Free\n\n");
	  else
		  printf("Grammatica non Context-Free\n\n");
  
	  if(is_MN(&grammar2))
		  printf("Grammatica Monotona\n\n");
	  else
		  printf("Grammatica non Monotona\n\n");

	  if(is_RL(&grammar2))
		  printf("Grammatica Lineare Destra\n\n");
	  else
		  printf("Grammatica non Lineare Destra\n\n");

	  if(is_CS(&grammar2))
		  printf("Grammatica Context-Sensitive\n\n");
	  else
		  printf("Grammatica non Context-Sensitive\n\n");

  }
  else printf("Impossibile stabilire la classe di appartenza. \n\n");


  union_grammar (&grammar, &grammar2, &uniongrammar);
  printf("\n");
  concatenation_grammar(&grammar, &grammar2, &concatenationgrammar);
  printf("\n");
  iteration_grammar(&grammar, &iterationgrammar);
 
  fclose(gram_file2);

  system("PAUSE");	
  return 0;
}