Q1)Write a program to run TCP client and server socket programs where client firstsays “Hi” and in response server says “Hello”.
   ![](Assignment_1/Ass1client.png)
   ![](Assignment_1/Ass1server.png)

Q2)Write a program using TCP socket to implement the following:
    i. Server maintains records of fruits in the format: fruit-name, quantity Last-sold,(server timestamp),
    ii. Multiple client purchase the fruits one at a time,
    iii. The fruit quantity is updated each time any fruit is sold,
    iv. Send regret message to a client if therequested quantityof the fruit is not available.
    v. Display the customer ids <IP, port> who has done transactions already. This list should be updated in the server every time a transaction occurs.
    vi. The total number of unique customers who did some transaction will be displayed to the customer every time.
        
Q3)Same as Q2 using with UDP socket

Q4)Install wireshark in a VM (Virtual Machine) environment. Draw a time diagram to show the steps in the protocols recorded in the captured file (saved in the 
.pcap file wireshark) during a PING operation. List the L2, L3, L4 header fields that can be extracted from the .pcap file.
    
Q5)Learn and use maximum number of packet generation tools.

Q6)Develop a simple C based network simulator to analyze TCP traffic.

Q7)Write UDP client server socket program where client sends one/two number(s)(integer or floating point) to server and a scientific calculator operation (like
    sin,cos,*,/, inv etc.) and server responds with the result after evaluating the value of operation as sent by the client. Server will maintain a scientific 
    calculator. Detect in the mininet hosts with wireshark if there is any packet loss?
    
Q8)Write a program in C using thread library and TCP sockets to build a chat server which enable clients communicating to each other through the chat server. 
Messagelogs must be maintained in the server in a text file. Each client will see the conversations in real time. Clients must handled by a server thread. 
(Keep it like a group chatbox)
    
Q9)Write a client server socket program in TCP for uploading and downloading files between two different hosts. Also calculate the transfer time in both the cases.
    
Q10)Write two C programs using raw socket to send i. TCP packet where TCP payload will contain your roll number. ii. ICMP time stamp messages towards a target IP.
    
Q11)Write a RAW socket program to generate TCP SYN flood based DDoS attack towards an IP address. Take four mininet hosts as agent devices.
    
Q12)Write a RAW socket program to generate ICMP flood based DDoS attack towards an IP address. Take four mininet hosts as agent devices.

Q13)Create a binary tree topology with 7 switches in mininet. Capture packets at the root switch. Write a C program to extract the headers and draw a time diagram 
to show the protocols displayed in the captured file (save the .pcap/.pcapng file of wireshark/tshark) during a PING operation. List the L2, L3, L4 protocols that 
can be extracted from the .pcap/.pcapng file.

Q14)Create a custom leaf-spine topology in mininet using python which can be scaled with increasing switch radix.
