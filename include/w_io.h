
int redir_in_to(int fd); // used by pipes
int redir_out_to(int fd); // 1 >& fd or >& fd
int out_to_file(const char * filepath); // cmd > filename or > filename or 1 > filename
int append_to_file(const char * filepath); // cmd >> filename or >> filename or 1 >> filename
int err_to_file(const char *filepath); // 2>filename
int append_err_to_file(const char *filepath); // 2>> filename
int truncate_file(const char* filepath); // : > filename
int redir_to_file(int fd,const char* filepath); // fd > filename
int redir(int M,int N); // M >& N example 2 >&1 
int rdir_err_out_to_file(const char* filename); // &> filename //new to BASH
