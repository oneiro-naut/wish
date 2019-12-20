# wish_beta

Minimal Unix shell written in C to study systemcalls and other POSIX concepts.\
Command expressions have the syntax: \
[./]cmd1 arg1 arg2 arg3 ... argN(sep)cmd2 arg1 arg2 ...(sep)\
where (sep) is a command separator or an operator. Example - pipeline operator'|',etc.            

Some features are:\
  Supports sequential command execution 'cmd1 ;cmd2;cmd3;cmd4 && cmd5'.\
  Supports pipelines constructs 'A|B|C....' where A,B,C,... are command constructs.\
  Supports Output redirection to a file '>' , '>>'.\
  Supports File descriptor redirection.\
  Supports background process execution 'cmd &'.\
  Has it's own Finite State Machine based token generator and syntax checking functions.
  

