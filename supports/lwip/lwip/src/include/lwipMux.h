
#ifndef	__LWIP_MUX_H__
#define	__LWIP_MUX_H__


#include "lwip/opt.h"  /* lwip/arch.h --> arch/cc.h --> #include "muxSysParams.h" */

#include <string.h>

#include "lwip/def.h"
#include "lwip/ip_addr.h"
#include "lwip/ip6_addr.h"
#include "lwip/netif.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/udp.h"
#include "lwip/raw.h"
#include "lwip/snmp.h"
#include "lwip/igmp.h"
#include "lwip/etharp.h"
#include "lwip/stats.h"
#include "lwip/sys.h"
#include "lwip/ip.h"
#if ENABLE_LOOPBACK
#if LWIP_NETIF_LOOPBACK_MULTITHREADING
#include "lwip/tcpip.h"
#endif /* LWIP_NETIF_LOOPBACK_MULTITHREADING */
#endif /* ENABLE_LOOPBACK */

#include "netif/ethernet.h"

#if LWIP_AUTOIP
#include "lwip/autoip.h"
#endif /* LWIP_AUTOIP */
#if LWIP_DHCP
#include "lwip/dhcp.h"
#endif /* LWIP_DHCP */
#if LWIP_IPV6_DHCP6
#include "lwip/dhcp6.h"
#endif /* LWIP_IPV6_DHCP6 */
#if LWIP_IPV6_MLD
#include "lwip/mld6.h"
#endif /* LWIP_IPV6_MLD */
#if LWIP_IPV6
#include "lwip/nd6.h"
#endif

#include "lwip/ip_addr.h"
#include "lwip/inet.h"

#include "compact.h"

#include "muxNmos.h"
#include "muxHttp.h"

#ifndef	TRACE
	#define	TRACE()						printf(__FILE__", line %u\r\n", __LINE__)
#endif


void muxLwipHttpSvrInit(void *data);

void mHttpSvrMain(void *data);

char muxLwipStartup(struct netif *netif, MUX_RUNTIME_CFG *runCfg);
void muxNetRawTelnetInit(MUX_RUNTIME_CFG *runCfg);



char	 muxLwipGroupMgr(struct netif *netif, unsigned int gAddress, unsigned char isAdd);


void muxLwipIp4DebugPrint(struct pbuf *p, const char *prompt);
void muxLwipIgmpDebugPrint(const ip4_addr_t *groupaddr, const char isJoin);

char muxNetMulticastIP4Mac(unsigned int	*ipAddress, MUX_MAC_ADDRESS *macAddress);


void muxVideoConfigCopy(MUX_VIDEO_CONFIG *dest, MUX_VIDEO_CONFIG *src);


char	muxNetIsGroupAddress(unsigned int	*ipAddress);




#define	NETIF_HWADDR_OFFSET()		\
			(offsetof(struct netif, hwaddr_len))


#define	MUX_LWIP_INT_TO_IP(ipAddr,  intAddr)	\
				((ipAddr)->addr = intAddr)

#define	MUX_LWIP_IPADD_TO_STR(ipAddr)		\
			inet_ntoa((*(struct in_addr *)(ipAddr)) )


#define	MUX_LWIP_DEBUG	0


#define	MUX_LWIP_DEBUG_NETIF(_netif)	\
			MUX_DEBUGF(MUX_DBG_ON, ("netif :%p; hwaddr_len:%d, offset:%d:%d:%d"MUX_NEW_LINE,  \
				(_netif), (_netif)->hwaddr_len, (offsetof(struct netif, rs_count)), (offsetof(struct netif, mtu)), NETIF_HWADDR_OFFSET() ) )


#define	MUX_LWIP_DEBUG_PBUF(_pbuf)	\
			MUX_DEBUGF(MUX_DBG_ON, ("pbuf :%p, size:%d, next:%p; payload:%p, total:%d, len:%d, type:%d, ref:%d"MUX_NEW_LINE,  \
				(_pbuf), sizeof(struct pbuf), _pbuf->next, _pbuf->payload, _pbuf->tot_len, _pbuf->len, _pbuf->type, _pbuf->ref ) )



#define	MDNS_SERVICE_NAME_SIZE		128

typedef	enum
{
	MDNS_CLIENT_S_INIT = 0,
	MDNS_CLIENT_S_PARSE_PTR,
	MDNS_CLIENT_S_PARSE_SRV,
	MDNS_CLIENT_S_PARSE_TXT,

	MDNS_CLIENT_S_REQUEST_API,
		
	MDNS_CLIENT_S_UNKNOWN,

}MDNS_CLIENT_STATE;

#include "lwip/apps/mdns.h"
#include "lwip/apps/mdns_priv.h"


#define	MUX_MDNS_CLIENT_DEBUG				MUX_DBG_OFF
#define	MUX_HTTPD_DEBUG						MUX_DBG_OFF

#define	MUX_HTTPD_DATA_DEBUG				MUX_DBG_OFF


typedef	struct 
{
	u16_t				txId;

	char					domainName[MDNS_DOMAIN_MAXLEN];

	char					service[MDNS_SERVICE_NAME_SIZE];
	u16_t				qType;

	unsigned	char			rdata[MDNS_DOMAIN_MAXLEN];

	unsigned char			state;
	
	struct udp_pcb		*udpPcb;
	struct mdns_packet	*pkt;

	MuxNmosNode			node;
	
	MUX_RUNTIME_CFG	*runCfg;
}mdns_client_t;


char mdnsClientInit(mdns_client_t *mdnsClient, MUX_RUNTIME_CFG *runCfg);
char mdnsClientParseAnswer(mdns_client_t *mdnsClient, struct mdns_packet *pkt);

struct ptptime_t
{
	s32_t	tv_sec;
	s32_t	tv_nsec;
};


char muxCmdConnect(MUX_RUNTIME_CFG  *runCfg);


char *muxLwipIpAddress(void);

//void bspConsoleDumpMemory(uint8_t *buffer, uint32_t size, uint32_t address);


#endif
