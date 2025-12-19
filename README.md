This is about the assignment given in Computer networks lab CS520

Q1)Write a program to run TCP client and server socket programs where client first says “Hi” and in response server says “Hello”.
Client1
![Client](Assignment_1/Ass1client.png) 
Server1
![Server](Assignment_1/Ass1server.png)

Q2)Write a program using TCP socket to implement the following:

i. Server maintains records of fruits in the format: fruit-name, quantity Last-sold,(server timestamp),

ii. Multiple client purchase the fruits one at a time,

iii. The fruit quantity is updated each time any fruit is sold,

iv. Send regret message to a client if therequested quantityof the fruit is not available.

v. Display the customer ids <IP, port> who has done transactions already. This list should be updated in the server every time a transaction occurs.

vi. The total number of unique customers who did some transaction will be displayed to the customer every time.

Client2
![Client](Assignment_2/Ass2client.png) 
Server2
![Server](Assignment_2/Ass2server.png)


Q3)Do question 2 using UDP socket

Client3
![Client](Assignment_3/Ass3client.png) 
Server3
![Server](Assignment_3/Ass3server.png)

Q4)Install wireshark in a VM (Virtual Machine) environment. Draw a time diagram to show the steps in the protocols recorded in the captured file 
(saved in the .pcap file of wireshark) during a PING operation. List the L2, L3, L4 header fields that can be extracted from the .pcap file.

Captured pcap
![Client](Assignment_4/Ass4capture.png) 
Bar Graph
![Client](Assignment_4/Ass4bargraph.png) 
Time Diagram
![Client](Assignment_4/Ass4time.png) 
L2 Header
![Client](Assignment_4/l2-header.png) 
L3 Header
![Client](Assignment_4/l3-header.png) 
L4 Header
![Client](Assignment_4/l4-header.png) 

Q5)Learn and use maximum number of packet generation tools.

DITG
![Client](Assignment_5/ditg.png) 
Iperf TCP
![Client](Assignment_5/iperf_tcp.png) 
Iperf UDP
![Client](Assignment_5/iperf_udp.png) 

Q6)Develop a simple C based network simulator to analyze TCP traffic.

![Client](Assignment_6/Assign_6.png)

Q7)Write UDP client server socket program where client sends one/two number(s) (integer or floating point) to server and a scientific calculator operation (like sin,cos,*,/, inv etc.) and server responds with the result after evaluating the value of operation as sent by the client. Server will maintain a scientific calculator. Detect in the mininet hosts with wireshark if there is any packet loss?

Client7
![Client](Assignment_7/Ass7client.png) 
Server7
![Client](Assignment_7/Ass7server.png)

Q8)Write a program in C using thread library and TCP sockets to build a chat server which enable clients communicating to each other through the chat server. Message logs must be maintained in the server in a text file. Each client will see the conversations in real time. Clients must handled by a server thread. (Keep it like a group chatbox)

Client8
![Client](Assignment_8/Ass8client.png) 
Server8
![Client](Assignment_8/Ass8server.png) 

Q9)Write a client server socket program in TCP for uploading and downloading files between two different hosts. Also calculate the transfer time in both the cases.

Client9
![Client](Assignment_9/Ass9client.png) 
Server9
![Client](Assignment_9/Ass9server.png) 

Q10)Write two C programs using raw socket to send i. TCP packet where TCP payload will contain your roll number. ii. ICMP time stamp messages towards a target IP.

Capture
![Client](Assignment_10/cap.png) 
TCP packet roll
![Client](Assignment_10/roll_tcp.png) 
ICMP time
![Client](Assignment_10/time_icmp.png)

Q11)Write a RAW socket program to generate TCP SYN flood based DDoS attack towards an IP address. Take four mininet hosts as agent devices.

Capture
![Client](Assignment_11/Ass11.png) 
Pcap Capture
![Client](Assignment_11/Ass11capture.png) 
Packet Capture
![Client](Assignment_11/Ass11packet.png)

Q12)Write a RAW socket program to generate ICMP flood based DDoS attack towards an IP address. Take four mininet hosts as agent devices.

Capture
![Client](Assignment_12/Ass12capture.png)  
Packet Capture
![Client](Assignment_12/Ass12packet.png) 

Q13)Create a binary tree topology with 7 switches in mininet. Capture packets at the root switch. Write a C program to extract the headers and draw a time diagram to show the protocols displayed in the captured file (save the .pcap/.pcapng file of wireshark/tshark) during a PING operation.
List the L2, L3, L4 protocols that can beextracted from the .pcap/.pcapng file.

Capture
![Client](Assignment_13/Ass13_cap.png) 
Ping capture
![Client](Assignment_13/Ass13cap.png) 
Pcap file
![Pcap file](Assignment_13/ping_cap.pcap) 

Q14)Create a custom leaf-spine topology in mininet using python which can be scaled with increasing switch radix.

Capture1
![Client](Assignment_14/Ass14.png)  
Capture2
![Client](Assignment_14/Ass141.png)  
