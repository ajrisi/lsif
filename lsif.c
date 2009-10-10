/*
 * Updated code to obtain IP and MAC address for all "up" network
 * interfaces on a linux system. Now IPv6 friendly and updated to use
 * inet_ntop instead of the deprecated inet_ntoa function. This version
 * should not seg fault on newer linux systems
 * Authors: Adam Pierce, Adam Risi
 * Date: 10/09/2009
 * http://www.adamrisi.com
 * http://www.doctort.org/adam/
 *
 * Previous file header follows:
 *
 * Example code to obtain IP and MAC for all available interfaces on
 * Linux.
 * by Adam Pierce <adam@doctort.org>
 * http://www.doctort.org/adam/
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h> 
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>


/* 
 * this is straight from beej's network tutorial. It is a nice wrapper
 * for inet_ntop and helpes to make the program IPv6 ready
 */
char *get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen)
{
  switch(sa->sa_family) {
  case AF_INET:
    inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr),
	      s, maxlen);
    break;
 
  case AF_INET6:
    inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr),
	      s, maxlen);
    break;
 
  default:
    strncpy(s, "Unknown AF", maxlen);
    return NULL;
  }
 
  return s;
}

int main(int argc, char **argv)
{
  char buf[1024] = {0};
  struct ifconf ifc = {0};
  struct ifreq *ifr = NULL;
  int sck = 0;
  int nInterfaces = 0;
  int i = 0;
  char hostname[NI_MAXHOST] = {0};
  struct ifreq *item = NULL;
  struct sockaddr *addr;
  socklen_t salen;
 
  /* Get a socket handle. */
  sck = socket(AF_INET, SOCK_DGRAM, 0);
  if(sck < 0) {
    perror("socket");
    return 1;
  }
 
  /* Query available interfaces. */
  ifc.ifc_len = sizeof(buf);
  ifc.ifc_buf = buf;
  if(ioctl(sck, SIOCGIFCONF, &ifc) < 0) {
    perror("ioctl(SIOCGIFCONF)");
    return 1;
  }
 
  /* Iterate through the list of interfaces. */
  ifr = ifc.ifc_req;
  nInterfaces = ifc.ifc_len / sizeof(struct ifreq);
  for(i = 0; i < nInterfaces; i++) {
    /* clear the hostname array */
    memset(hostname, 0, sizeof(hostname));
    
    item = &ifr[i];
    
    /* Show the device name and IP address */
    addr = &(item->ifr_addr);
    char ip[INET6_ADDRSTRLEN];
    printf("%s: IP %s",
	   item->ifr_name,
	   get_ip_str(addr, ip, INET6_ADDRSTRLEN));
    
    switch(addr->sa_family) {
    case AF_INET:
      salen = sizeof(struct sockaddr_in);
      break;
    case AF_INET6:
      salen = sizeof(struct sockaddr_in6);
      break;
    default:
      salen = 0;
    }

    
    /* the call to get the mac address changes what is stored in the
       item, meaning that we need to determine the hostname now */
    getnameinfo(addr, salen, hostname, sizeof(hostname), NULL, 0, NI_NAMEREQD);
    
    
    /* Get the MAC address */
      if(ioctl(sck, SIOCGIFHWADDR, item) < 0) {
	perror("ioctl(SIOCGIFHWADDR)");
	return 1;
      }
      
      /* display result */
      printf(", MAC %.2x:%.2x:%.2x:%.2x:%.2x:%.2x, ",
	     (unsigned char)item->ifr_hwaddr.sa_data[0],
	     (unsigned char)item->ifr_hwaddr.sa_data[1],
	     (unsigned char)item->ifr_hwaddr.sa_data[2],
	     (unsigned char)item->ifr_hwaddr.sa_data[3],
	     (unsigned char)item->ifr_hwaddr.sa_data[4],
	     (unsigned char)item->ifr_hwaddr.sa_data[5]);
      
      
      if(hostname[0] != 0) {
	printf("%s", hostname);
      } else {
	printf("NO_HOSTNAME");
      }
      
      printf("\n");
      
  }
  
  return 0;
}
