# wish

Minimal Linux shell written in C to study systemcalls and other POSIX concepts.\
Command expressions have the syntax: 

```
$ [./]cmd1 arg1 arg2 arg3 ... argN(sep)cmd2 arg1 arg2 
```
where (sep) is a command separator or an operator. Example - pipeline operator'|',etc.            

Some features are:
-  Supports sequential command execution 'cmd1 ;cmd2;cmd3;cmd4 && cmd5'.
-  Supports pipelines constructs 'A|B|C....' where A,B,C,... are command constructs.
-  Supports Output redirection to a file '>' , '>>'.
-  Supports File descriptor redirection.
-  Supports background process execution 'cmd &'.
-  Has it's own Finite State Machine based token generator and syntax checking functions. 

#### Running on local machine
You will need a Linux machine to run this shell.

###### Dependencies:
- gcc
- cmake

After installing dependencies,

Run the following commands:
```
$ make
```
Then an executable will be generated in ./bin/ , you can run it using

```
$ bin/wish
```

To exit the shell write `exit` or press `Ctrl+D` in the shell.

#### To be added: 
-  Shell Scripting 
-  Shell customisation 
  

