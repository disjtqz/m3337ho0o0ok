#pragma once
typedef struct {

}*snaphak_socket_t;

struct snaphak_netroutines_t {
	snaphak_socket_t  (*m_connect_tcp)(const char* ipaddr, uint16_t portnum);
	bool (*m_try_send)(snaphak_socket_t sock, const void* data, size_t datasize);

	size_t (*m_try_recv)(snaphak_socket_t sock, void* buffer, size_t buffersize);

};