						CMSC 621
					Advanced Operating Systems
						Project-2
						README
						SAI TEJA CHALLA(JA52979)
Clock, Multicast, and COMMIT
Assignment-1:

To execute this assignment, do follow the below steps:
1.	Change the directory to the code files containing the directory using the “cd” command.
2.	Now compile the files using the command “make compile”.
3.	Before running the client and server code, make sure you open one server terminal and as many client terminals as you need, and follow the command as shown:
			./server <portno.>
			./client <portno.>
4.	Now start running the server terminal, after running you will be displayed with a question
			“How many machines need to connect?”
5.	Before entering the value make sure you open that number of client terminals in order to execute it.
6.	Once, all client terminals are connected to the server, the Berkeley algorithm is implemented.
7.	Thus, the final output will be displayed.


Assignment-2:
To execute this assignment, do follow the below steps:
1.	Change the directory to the code files containing the directory using the “cd” command.
2.	Now compile the files using the command “make causal/noncausal”(seperately).
3.	Before running the client and server code, make sure you open one server terminal and as many client terminals as you need, and follow the command as shown:
			./causal <processnumber(0,1,2)> <number 1 or 2>
			./noncausal <processnumber(0,1,2)> <number 1 or 2>
4.	Now start running the either causal/noncausal terminal, after running you will be displayed with a question
			“How many multicasts you need”
5.	Before entering the value make sure you open that number of client terminals in order to execute it.(But here we limited to 3 processes).
6. 	Once all processes done multicast, the vector clocks are displayed.


Bonus Assignment:
To execute this assignment, do follow the below steps:
1.	Change the directory to the code files containing the directory using the “cd” command.
2.	Now compile the files using the command “make compile”.
3.	Before running the client and server code, make sure you open one server terminal and as many client terminals as you need, and follow the command as shown below:
			./server <portno.>
			./client <portno.>
4.	Now start running the server terminal, after running you will be displayed with a question
		"How many number of clients to connect?"
5.	Before entering the value make sure you open that number of client terminals in order to execute it.
6.	Then open the each client terminal, it will be prompted with a question
		"How many times will you need to update the counter?"
7.	Then, enter the value.
8.	Later, every client will access the file and update the values in the file.
9.	Thus, distributed locking system is implemented.

