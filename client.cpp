// FILE:   client.cpp
// AUTHOR: Dr. S. A. Blythe & Pablo Boudet
// DATE:   04/27/2022
//
// A simple client program that utilizes Unix sockets to connect to a
// simple server. The the client can use the commands get, clear or add to change or receive the value of an accumulator

// includes to give data type and function prototypes for sockets
#include <netdb.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
using namespace std;

// various error codes that correspond to socket problems
#define SUCCESS 0
#define USAGE_ERROR 1
#define CREATE_ERROR 2
#define HOSTNAME_ERROR 3
#define SOCK_ERROR 4
#define READ_ERROR 5

// forward definition of the actual client code
int do_client(char*, char*);

int main(int argc, char *argv[])
{
  // check to make sure program has been invoked properly
  if (argc!=3)
    {
      cout << "error" << endl;
      return USAGE_ERROR;
    }
  
  // invoke client code sending the port and the ip
  return do_client(argv[1], argv[2]);

}


// FUNCTION  : do_client()
// PARAMETERS: 
//    char *server:  the name of the time of day server to connect to
int do_client(char *server, char *portNum)
{
  // holds return value (error code) from various socket functions
  int rVal=0;

  // data structure to hold internal address info for server
  struct hostent *serverEntry;
  // socket information; server side will be explcitly set
  struct sockaddr_in serverInfo;

  // convert char string of server to socket data structure representation
  serverEntry = gethostbyname(server);

  // if we couldn't find the server information ...
  if(!serverEntry)
    {
      cout << "Failed gethostbyname(), server=" << server << endl;
      return HOSTNAME_ERROR;
    }

  //create the socket we intend on using ...
  //   AF_INET ==> IPv4 
  //  SOCK_STREAM ==> TCP (a.k.a. "stream socket") 
  //  0 ==> use default flags NOTE that
  //
  // a socket is uniquely represented by an integer, which is
  // internally treated as an open file descriptor ...
  int s = socket(AF_INET, SOCK_STREAM, 0);
  // if the socket creation failed  
  if(s < 0)
    {
      cout << "Failed socket()" << endl;
      return CREATE_ERROR;
    }
  //Fill in the sockaddr_in struct, which internal represents an
  //internet address
  serverInfo.sin_family = AF_INET;  // IPv4
  serverInfo.sin_addr =  * (struct in_addr *)serverEntry->h_addr_list[0];
  serverInfo.sin_port = htons(atoi(portNum)); // Host TO Network Short (integer)

  // now, actually communicate with the server, trying to open a connection 
  // to it.
  rVal=connect(s, (struct sockaddr *) &serverInfo, sizeof(serverInfo));
 
  // if we couldn't make a connection ...
  if(rVal<0)
    {
      cout << "Failed connect()" << endl;
      return SOCK_ERROR;
    }

  // at this point, we have an active connection. By protocol, the
  // server will simply respond ... so we need to read everything
  // being sent to us

  //read the command that will be executed by the server
  
  string buffer; // will store the command
  getline(cin,buffer);//get the command from input and put it in buffer
  buffer+='\n'; //character that indicated the end of the string
  //we simply send this string to the client
  char *cbuff=(char *)buffer.c_str();
  char* value = cbuff;
  int needed=buffer.length();
  while(needed) // as long as writing is not yet completed, 
    { 
      // keep wriitng more of the buffer
      int n=write(s, cbuff, needed);
      needed-=n;
      cbuff+=n;
    }
   //make sure that the input are the one that read from server
  if((value[0] == 'a' && value[1] == 'd' && value[2] == 'd' && value[3] == ' ') || (value[0] == 'g' && value[1] == 'e' && value[2] == 't' && value[3] == '\n'))
  {
	  int n;             // number of characters read
	  char recvln[81];   // buffer to hold characters
	  bool finish = false;
	  // as long as we dont read the \n character, keep reading from th server
	  while (finish == false)
	    {
	      n=read(s,recvln,80);
	      if(recvln[n-1]=='\n')//if our final character is \n we have read everything
	      {
	      	finish = true;
	      }
	   }
	   //print the value we read from the server
	  cout << recvln;
   }
   else if(value[0] != 'c' && value[1] != 'l' && value[2] != 'e' && value[3] != 'a' && value[4] != 'r' && value[5] != '\n')//if the charcater is not clear, add or get is invalid, if it is clear we dont have to read
   {
   	cout << "Invalid value" <<endl;
   }
  // close the socket
  close(s);

  // if we get here, things worked just fine. 
  return SUCCESS;
}

