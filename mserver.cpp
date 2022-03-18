/*
    C socket server example, handles multiple clients using threads
    Compile
    gcc mserver.c -lpthread -o mserver

    if port is been used,you can do something like below:
    ps -ef | grep mserver
    sudo kill -9 xxxxx

    bind failed. Error: Address already in use
    
*/
 
#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with -lpthread
 
//the thread function
void *connection_handler(void *);
 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
     
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
	pthread_t thread_id;
	
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
        printf("new client：IP = %s \n", inet_ntoa(client.sin_addr));
         
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
        //Now detach the thread, so that we can terminate it convient
        pthread_detach(thread_id);

        puts("Handler assigned");
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}
 
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char message[100] , client_message[2000];
     
    //Send some messages to the client
    sprintf(message ,"%s" , "Greetings! I am your connection handler\n");
    write(sock , message , strlen(message));
     
    sprintf(message ,"%s" , "Now type something and i shall repeat what you type \n");
    write(sock , message , strlen(message));
     
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //end of string marker
		client_message[read_size] = '\0';

        // 处理请求
		// strcmp 函数比较两个字符串是否相等，相等返回 0
		if (0 == strcmp(client_message, "getName"))
		{
			char msgBuf[] = "computer chen";
			send(sock, msgBuf, strlen(msgBuf) + 1, 0);
		}
		else if (0 == strcmp(client_message, "getAge"))
		{
			char msgBuf[] = "50";
			send(sock, msgBuf, strlen(msgBuf) + 1, 0);
		}
		else
		{
			// send 向客户端发送一条数据

			// 连接的 TCP socket 存储远程主机的地址信息。因此，进程不需要为传输数据的函数传入地址参数，可以直接用 send 函数通过 socket 发送数据。
			// 第二个参数 buf 是写入缓冲区。将数据放到输出缓冲区中，socket 库来决定在将来某一时间发送出去。
			// len 是传输的字节数量。只要 socket 的输出缓冲区有空间，网络库就可以将数据放到缓冲区中，然后等到缓冲区数据块大小合适时再发送出去。+ 1 包括结尾符。
			// flags 是对控制数据发送标志进行按位或运算的结果。大多数游戏代码中，该参数取值为 0。
			// 如果没有判断返回值，数据有可能没有完全发送。
			//Send the message back to client
            write(sock , client_message , strlen(client_message));
		}
		
		//clear the message buffer
		memset(client_message, 0, 2000);
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    return 0;
} 