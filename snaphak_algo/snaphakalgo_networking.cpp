#include "snaphakalgo.hpp"

#include<winsock2.h>
#include <cstdlib>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

struct snaphak_socket_internal_t {
	const char* m_ipaddr;
	uint16_t m_port;
	int m_socket;
};
static 
snaphak_socket_t  cs_connect_tcp(const char* ipaddr, uint16_t portnum) {
	int sockres= socket(AF_INET , SOCK_STREAM , 0 );
	if(sockres ==INVALID_SOCKET) {
		return nullptr;
	}
	else {
		snaphak_socket_internal_t* result = new snaphak_socket_internal_t();

		result->m_ipaddr = _strdup(ipaddr);
		result->m_port=portnum;
		result->m_socket =sockres;
		return (snaphak_socket_t)result;
	}
}
static 

bool cs_try_send(snaphak_socket_t sock, const void* data, size_t datasize) {
	if(!sock)
		return false;
	if(!data)
		return false;
	if(!datasize)
		return true;
	if(send(reinterpret_cast<snaphak_socket_internal_t*>(sock)->m_socket, (const char*)data, datasize, 0) < 0)
		return false;
	return true;
}
static 

size_t cs_try_recv(snaphak_socket_t sock, void* buffer, size_t buffersize) {
	if(!sock)
		return 0;
	if(!buffer)
		return 0;
	if(!buffersize)
		return 0;
	int recv_size = recv(reinterpret_cast<snaphak_socket_internal_t*>(sock)->m_socket , (char*)buffer , buffersize , 0);
	if(recv_size==SOCKET_ERROR)
		return 0;
	return static_cast<size_t>(recv_size);
}

static 	WSADATA g_wsa;

void netroutines_init(snaphak_netroutines_t* net) {
	net->m_connect_tcp = cs_connect_tcp;
	net->m_try_recv = cs_try_recv;
	net->m_try_send=cs_try_send;
	WSAStartup(MAKEWORD(2,2),&g_wsa);
}
