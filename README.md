## Assignment 9

Due Wed Dec. 8 at 5pm.  
The assignment is worth 100 points.

**Purpose**

Exercise TCP server socket system calls.  

**Assignment**

Write a C++ program that implements a simple file server.

**Program**

Implement a C++ program that in a loop listens on a port for incoming TCP requests from clients. For each accepted incoming request it forks a child to read and process the request. The parent process continues to listen and accept incoming TCP requests in an endless loop.  

The program accepts 2 command line parameters:  

1. the port number to listen on,
2. the pathname to a directory that serves as root to all requested files or directories.

For example:  
    % ./z123456 9001 www  

The requests received by the program are of the form:  
	
  GET pathname  

where the pathname refers to a file or directory to be sent back to the client. The file/directory will be found in the directory specified on the command line. The following rules apply to the pathname:  
- it must start with a "/"
- it may contain additional "/" path separators to access subdirectories
- a single "/" character refers to the directory specified on the command line
- a trailing "/" in the pathname can be ignored if the pathname refers to a directory
- any data in the request beyond the pathname should be ignored
- it may not contain the substring ".." 


If the pathname refers to a file, then the content of the file is returned to the client.  

If the pathname refers to a directory, then:
- if a file "index.html" exists in that directory, it will be returned;
- else, a list of files in that directory will be returned (not including any file that starts with ".").  

**Error Checking**

If the command line arguments are incomplete, or if the path to the root directory is invalid, print an error message and exit. If any of the system calls fail, the program should use "perror" to report and exit. If the pathname in the GET request is invalid or a file/directory cannot be accessed, then an appropriate error message should be constructed and sent back to the client.

**Other Points**

- you can test your server program with the TCPClient command we used in class, as in:  

% ./TCPClient localhost 9001 "GET /"  
fileOne.html fileTwo.html  
% ./TCPClient localhost 9001 "GET /fileOne"  
Error: fileOne not found  
% ./TCPClient localhost 9001 "GET /fileOne.html"  
  ... content of fileOne.html  
  ...  
%  

- you can find the source code for the TCPClient program  >> here << Click for more options   ;
- make sure that your assignment is contained in a single file called "z123456.cxx" based on your Z-id;
- make sure that your program compiles, links and runs fine on your Linux system, turing or hopper.

**Submission**

Submit your C++ source code file via Blackboard below.

**Extra Credit**

If you implement this part of the assignment, please indicate it in the file header comment.  
For an additional 20 extra credit points, implement a second request command:  
	INFO  
It will return the current time and date in text format to the client.
