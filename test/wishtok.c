#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define CMDPERINPUT 10


/*Pure FSM based tokenizer that would replace the original strtok_r() based wishtokenizer function

	these are more specialized and specific than the original
	and can be made to follow any rule for token generation 




*/


char** wish_tok(char* inp_str)
{
    char** tokarr =(char**) malloc(sizeof(char*)*CMDPERINPUT);
	char *p;
	
	char* start_of_word=NULL;
	int index_of_token= -1;
	int c;
	enum states { DULL, IN_WORD, IN_STRING } state = DULL;

		for (p = inp_str;*p!=0; p++) {
			c = (unsigned char) *p; /* convert to unsigned char for is* functions */
			switch (state) {
			case DULL: /* not in a word, not in a double quoted string */
				if (c==' ') {
					/* still not in a word, so ignore this char */
					continue;
				}
				/* not a space -- if it's a double quote we go to IN_STRING, else to IN_WORD */
				if (c == '"') {
					state = IN_STRING;
					start_of_word = p + 1; /* word starts at *next* char, not this one */
					index_of_token++;
					continue;
				}
				state = IN_WORD;
				start_of_word = p; /* word starts here */
				index_of_token++;
				continue;

			case IN_STRING:
				/* we're in a double quoted string, so keep going until we hit a close " */
				if (c == '"') {
					/* word goes from start_of_word to p-1 */
					*p= 0;
					tokarr[index_of_token]=start_of_word;
					state = DULL; /* back to "not in word, not in string" state */
				}
				continue; /* either still IN_STRING or we handled the end above */

			case IN_WORD:
				/* we're in a word, so keep going until we get to a space */
				if (c==' ') {
					/* word goes from start_of_word to p-1 */
					
					state = DULL; /* back to "not in word, not in string" state */
					*p= '\0';
					
					tokarr[index_of_token]=start_of_word;
				}
				continue; /* either still IN_WORD or we handled the end above */

			}
		}
		if(state==IN_STRING||state==IN_WORD)tokarr[index_of_token]=start_of_word;
		tokarr[++index_of_token]=NULL;    

 	 int i=0;
 	 printf("\n");
 	 for(;tokarr[i]!=NULL;i++)puts(tokarr[i]); //for debugging purpose
      if(!tokarr[i])printf("NULL ");
 	 printf("\n");			
    
	return NULL;
}


//tokenizer below does not tokenize the delimitor if its inside a balanced double quoted substring
//more delimitors can be added by adding more if conditions 
char** wish_tok_quote_muted(char* inp_str) 
{
    char** tokarr =(char**) malloc(sizeof(char*)*CMDPERINPUT);
	char *p;
	
	
	char* start_of_word=NULL;
	int index_of_token= -1;
	int c;
	enum states { DULL, IN_WORD, IN_STRING } state = DULL;

		for (p = inp_str;*p!=0;p++) {
			c = (unsigned char) *p; /* convert to unsigned char for is* functions */
			switch (state) {
			case DULL: /* not in a word, not in a double quoted string */
				if (c==';') {
					/* still not in a word, so ignore this char */
					continue;
				}
				/* not a space -- if it's a double quote we go to IN_STRING, else to IN_WORD */
				if (c == '"') {
					state = IN_STRING;
					//start_of_word = p + 1; /* word starts at *next* char, not this one */
					//index_of_token++;
					continue;
				}
				state = IN_WORD;
				start_of_word = p; /* word starts here */
				index_of_token++;
				continue;

			case IN_STRING:
				/* we're in a double quoted string, so keep going until we hit a close " */
				if (c == '"') {
					/* word goes from start_of_word to p-1 */
					// unlike the prev version this IN_STRING state does nothing just skips tokenizing process
					//until it finds that its finally going to next non STRING state
					state = IN_WORD; /* back to "not in word, not in string" state */
				}
				continue; /* either still IN_STRING or we handled the end above */

			case IN_WORD:
				/* we're in a word, so keep going until we get to a space or our delimiter char */
				if (c==';') {
					
					
					state = DULL; 
					*p= '\0';
					
					tokarr[index_of_token]=start_of_word;
				}
				if(c=='"')
				{
					state=IN_STRING;

				}
				continue; /* either still IN_WORD or we handled the end above */

			}
		}
		//exit state check here sort of pythonic while-else construct
		if(state==IN_STRING||state==IN_WORD)tokarr[index_of_token]=start_of_word;
		tokarr[++index_of_token]=NULL;    

 	 int i=0;
 	 printf("\n");
 	 for(;tokarr[i]!=NULL;i++)puts(tokarr[i]); //for debugging purpose
      if(!tokarr[i])printf("NULL ");
 	 printf("\n");			
    
	return NULL;
}







int main()
{
	char teststr[]="ayush is \"nihilitstic creep\"asd;aadsd ";
	wish_tok(teststr);
	
	char teststrmuted[]= "ayush is; \"nihilitstic; creep\"asd;aadsd ";
	wish_tok_quote_muted(teststrmuted);
		
	return 0;
}