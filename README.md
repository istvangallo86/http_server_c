# http_server_c
A HTTP server for web application implemented in C programming language with UNIX sockets. Every request handled in a separate thread to increase the performance!
This project is experimental. This is only for hobby purpose, there is absolute no warranty granted for this code to work.

Tested on Linux with gcc 9.4 and on Windows with Cygwing.

Compile with "gcc src/* -o server.bin" command, run with "./server.bin" and check with your browser the address http://127.0.0.1:8000

Good luck.
