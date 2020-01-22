#ifndef W_TOKENIZER_H
#define W_TOKENIZER_H

// this tokenizer handles and tokenizes double quotes
char** wish_tok(char* inp_str);

//tokenizer below does not tokenize the delimitor if its inside a balanced double quoted substring
//more delimitors can be added by adding more if conditions 
char** wish_tok_quote_muted(char* inp_str);

//tokenizer returns argv now**
//dependency on this tokenizer is to be removed as it just sucks and depends on strtok which sucks because it takes away flexibility 
char** w_tokenizer(char* inp_str,char* deli,int dquote); 

#endif