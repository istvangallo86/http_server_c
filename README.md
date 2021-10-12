# http_server_c
A HTTP server template for simple web apps implemented in C programming language with UNIX sockets. Every request handled in a separate thread to increase the performance and turn the server into an absolutely non-blocking async system!
This project is experimental and only for hobby purpose, there is absolute no warranty granted for this code to work.

Tested on Linux with gcc 9.4 and on Windows with Cygwin.

Compile with "gcc src/* -o server.bin -lpthread" command, run with "./server.bin" and check with your browser the address http://127.0.0.1:8000.

Place your assets within "web/public" folder into the relevant subfolders. 

Good luck.
