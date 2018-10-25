

#include "lwipExt.h"

/* Initialize a ExtHttpConn */
static void _extHttpStateInit(ExtHttpConn* mhc)
{
	/* Initialize the structure. */
	memset(mhc, 0, sizeof(ExtHttpConn));

	mhc->httpStatusCode = WEB_RES_CONTINUE;
	mhc->state = H_STATE_REQ;
#if LWIP_EXT_NMOS
	mhc->nodeInfo = &nmosNode;
#endif
}

/** Free a ExtHttpConn.
 * Also frees the file data if dynamic.
 */
static void _mhttpStateEof(ExtHttpConn *mhc)
{
	if(mhc->handle)
	{
#if	MHTTPD_TIMING
		u32_t ms_needed = sys_now() - mhc->time_started;
		u32_t needed = LWIP_MAX(1, (ms_needed/100));
		LWIP_DEBUGF(HTTPD_DEBUG_TIMING, ("httpd: needed %"U32_F" ms to send file of %d bytes -> %"U32_F" bytes/sec\n", ms_needed, mhc->handle->len, ((((u32_t)mhc->handle->len) * 10) / needed)));
#endif
		mfsClose(mhc->handle);
		mhc->handle = NULL;
	}
	
#if	MHTTPD_DYNAMIC_FILE_READ
	if (mhc->buf != NULL)
	{
		mem_free(mhc->buf);
		mhc->buf = NULL;
	}
#endif

#if	MHTTPD_SSI
	if (mhc->ssi)
	{
		mhttp_ssi_state_free(mhc->ssi);
		mhc->ssi = NULL;
	}
#endif

	if (mhc->req)
	{
		pbuf_free(mhc->req);
		mhc->req = NULL;
	}
}


/**
 * The connection shall be actively closed (using RST to close from fault states).
 * Reset the sent- and recv-callbacks.
 *
 * @param pcb the tcp pcb to reset callbacks
 * @param mhc connection state to free
 */
static err_t _mhttpConnCloseOrAbort(ExtHttpConn *mhc, struct tcp_pcb *pcb, u8_t abort_conn)
{
	err_t err = ERR_OK;

#if EXT_HTTPD_DEBUG
	EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Closing connection %s on %p\n", (mhc)?mhc->name:"NULL CONN", (void*)pcb));
#endif
	EXT_RUNTIME_CFG *runCfg = NULL;

	if (mhc != NULL)
	{
		if ((mhc->postDataLeft != 0)
#if	MHTTPD_POST_MANUAL_WND
			|| ((mhc->no_auto_wnd != 0) && (mhc->unrecved_bytes != 0))
#endif
			)
		{

			/* make sure the post code knows that the connection is closed */
			extHttpPostDataFinished(mhc);
		}

		if( HTTPREQ_IS_WEBSOCKET(mhc) )
		{
			extHttpWebSocketSendClose(mhc);
		}

	}
	EXT_ASSERT(("HTTP Connection is null"), mhc!= NULL);
	runCfg = mhc->runCfg;  ///????


	tcp_arg(pcb, NULL);
	tcp_recv(pcb, NULL);
	tcp_err(pcb, NULL);
	tcp_poll(pcb, NULL, 0);
	tcp_sent(pcb, NULL);

	
	if (mhc != NULL)
	{
		mhttpConnFree(mhc);
	}

	if (abort_conn)
	{
#if EXT_HTTPD_DEBUG
		EXT_INFOF( ("TCP connection %s %p is abort"EXT_NEW_LINE, mhc->name,  (void*)pcb));
#else
		EXT_INFOF( ("TCP connection %p is abort"EXT_NEW_LINE, (void*)pcb));
#endif
		tcp_abort(pcb);
		return ERR_OK;
	}
	
	runCfg->currentHttpConns --;
	err = tcp_close(pcb);
	if (err != ERR_OK)
	{
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Error %d closing %p"EXT_NEW_LINE, err, (void*)pcb));
		EXT_ERRORF( ("Error %d closing %p"EXT_NEW_LINE, err, (void*)pcb));
		/* error closing, try again later in poll */
		tcp_poll(pcb, extHttpPoll,  MHTTPD_POLL_INTERVAL);
	}
	return err;
}

#if	MHTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED
/** global list of active HTTP connections, use to kill the oldest when running out of memory */
static ExtHttpConn *_mhttpConns;

static void _addConnection(ExtHttpConn *mhc)
{
	/* add the connection to the list */
	mhc->next = _mhttpConns;
	_mhttpConns = mhc;
}

static void _removeConnection(ExtHttpConn *mhc)
{
	/* take the connection off the list */
	if (_mhttpConns)
	{
		if (_mhttpConns == mhc)
		{
			_mhttpConns = mhc->next;
		}
		else
		{
			ExtHttpConn *last;
			for(last = _mhttpConns; last->next != NULL; last = last->next)
			{
				if (last->next == mhc)
				{
					last->next = mhc->next;
					break;
				}
			}
		}
	}
}

static void _killOldestConnection(u8_t ssi_required)
{
	ExtHttpConn *mhc = _mhttpConns;
	ExtHttpConn *hs_free_next = NULL;

	while(mhc && mhc->next)
	{
#if	MHTTPD_SSI
		if (ssi_required)
		{
			if (mhc->next->ssi != NULL)
			{
				hs_free_next = mhc;
			}
		}
		else
#else
		LWIP_UNUSED_ARG(ssi_required);
#endif
		{
			hs_free_next = mhc;
		}
		EXT_ASSERT(("broken list"), mhc != mhc->next);
		mhc = mhc->next;
	}
	
	if (hs_free_next != NULL)
	{
		EXT_ASSERT(("hs_free_next->next != NULL"), hs_free_next->next != NULL);
		EXT_ASSERT(("hs_free_next->next->pcb != NULL"), hs_free_next->next->pcb != NULL);
		/* send RST when killing a connection because of memory shortage */
		_mhttpConnCloseOrAbort(hs_free_next->next, hs_free_next->next->pcb, 1); /* this also unlinks the mhttp_state from the list */
	}
}
#else /* MHTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED */

#define _addConnection(mhc)
#define _removeConnection(mhc)

#endif


/** Allocate a ExtHttpConn. */
ExtHttpConn *mhttpConnAlloc(EXT_RUNTIME_CFG *runCfg)
{
	ExtHttpConn *mhc = HTTP_ALLOC_HTTP_STATE();
#if	MHTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED
	if (mhc == NULL)
	{
		_killOldestConnection(0);
		mhc = HTTP_ALLOC_HTTP_STATE();
	}
#endif

	if (mhc != NULL)
	{
		_extHttpStateInit(mhc);

#if EXT_HTTPD_DEBUG
		httpStats.connCount++;
		httpStats.currentConns++;
		snprintf(mhc->name, sizeof(mhc->name), "CONN#%d", httpStats.connCount);
#endif
		
		mhc->runCfg = runCfg;
		_addConnection(mhc);
	}
	
	return mhc;
}

/** Free a ExtHttpConn.
 * Also frees the file data if dynamic.
 */
void mhttpConnFree(ExtHttpConn *mhc)
{
	if (mhc != NULL)
	{
		_mhttpStateEof(mhc);
		_removeConnection(mhc);
#if EXT_HTTPD_DEBUG
//		httpStats.connCount--;
		httpStats.currentConns--;
		EXT_DEBUGF(EXT_HTTPD_DEBUG,("Connection %s is freed, total %d CONNs now ", mhc->name, httpStats.currentConns) );
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("URL:'%s'; Headers %d bytes: '%.*s'", mhc->uri, mhc->headerLength, mhc->headerLength, mhc->headers));
#endif

#if 0
		HTTP_FREE_HTTP_STATE(mhc);
#else
		mhc->state = H_STATE_FREE;
#endif
TRACE();
	}
}


/**
 * The connection shall be actively closed.
 * Reset the sent- and recv-callbacks.
 *
 * @param pcb the tcp pcb to reset callbacks
 * @param mhc connection state to free
 */
err_t mhttpConnClose(ExtHttpConn *mhc, struct tcp_pcb *pcb)
{
	return _mhttpConnCloseOrAbort(mhc, pcb, 0);
}

/** End of file: either close the connection (Connection: close) or
 * close the file (Connection: keep-alive)
 */
void mhttpConnEof(ExtHttpConn *mhc)
{
  /* HTTP/1.1 persistent connection? (Not supported for SSI) */
#if	MHTTPD_SUPPORT_11_KEEPALIVE
	if (mhc->keepalive)
	{
		_removeConnection(mhc);

		_mhttpStateEof(mhc);
		_mhttpStateInit(mhc);
		/* restore state: */
//		mhc->pcb = pcb;
		mhc->keepalive = 1;
		_addConnection(mhc);
		/* ensure nagle doesn't interfere with sending all data as fast as possible: */
		tcp_nagle_disable(mhc->pcb);
	}
	else
#endif
	{
		mhttpConnClose(mhc, mhc->pcb);
	}
}



/** Call tcp_write() in a loop trying smaller and smaller length
 *
 * @param pcb tcp_pcb to send
 * @param ptr Data to send
 * @param length Length of data to send (in/out: on return, contains the
 *        amount of data sent)
 * @param apiflags directly passed to tcp_write
 * @return the return value of tcp_write
 */
err_t extHttpWrite(ExtHttpConn *mhc, const void* ptr, u16_t *length, u8_t apiflags)
{
	u16_t len, max_len;
	err_t err;
	LWIP_ASSERT(("length != NULL"), length != NULL);
	len = *length;
	if (len == 0)
	{
		return ERR_OK;
	}
	
	/* We cannot send more data than space available in the send buffer. */
	max_len = tcp_sndbuf(mhc->pcb);
	EXT_DEBUGF(EXT_HTTPD_DEBUG, ("max length %d , length:%d", max_len, len));
	if (max_len < len)
	{
		len = max_len;
	}

	/* Oct.24, 2018,  SND_BUF_SIZE has limits its size, more limitation is not needed. JL*/
#if 0//def MHTTPD_MAX_WRITE_LEN
	/* Additional limitation: e.g. don't enqueue more than 2*mss at once */
	max_len = MHTTPD_MAX_WRITE_LEN(mhc->pcb);
	EXT_DEBUGF(EXT_HTTPD_DEBUG, ("max length2 %d , length2:%d", max_len, len));
	if(len > max_len)
	{
		len = max_len;
	}
#endif

	do
	{
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Trying go send %d bytes, flags:0x%x", len, apiflags));
		err = tcp_write(mhc->pcb, ptr, len, apiflags);
		if (err == ERR_MEM)
		{
			if ((tcp_sndbuf(mhc->pcb) == 0) ||(tcp_sndqueuelen(mhc->pcb) >= TCP_SND_QUEUELEN))
			{/* no need to try smaller sizes */
				len = 1;
			}
			else
			{
				len /= 2;
			}

			EXT_DEBUGF(EXT_HTTPD_DEBUG,  ("Send failed, trying less (%d bytes)", len));
		}
	} while ((err == ERR_MEM) && (len > 1));

	if (err == ERR_OK)
	{
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Sent %d bytes", len));
		*length = len;
	}
	else
	{
		EXT_DEBUGF(EXT_HTTPD_DEBUG, ("Send failed with err %d (\"%s\")", err, lwip_strerr(err)));
		*length = 0;
	}

#if	MHTTPD_SUPPORT_11_KEEPALIVE
	/* ensure nagle is normally enabled (only disabled for persistent connections
	when all data has been enqueued but the connection stays open for the next
	request */
	tcp_nagle_enable(mhc->pcb);
#endif

	return err;
}


#if	MHTTPD_SSI
/** Allocate as struct mhttp_ssi_state. */
static struct mhttp_ssi_state *http_ssi_state_alloc(void)
{
	struct mhttp_ssi_state *ret = HTTP_ALLOC_SSI_STATE();
	
#if	MHTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED
	if (ret == NULL)
	{
		_killOldestConnection(1);
		ret = HTTP_ALLOC_SSI_STATE();
	}
#endif

	if (ret != NULL)
	{
		memset(ret, 0, sizeof(struct mhttp_ssi_state));
	}
	return ret;
}

/** Free a struct mhttp_ssi_state. */
static void http_ssi_state_free(struct mhttp_ssi_state *ssi)
{
	if (ssi != NULL)
	{
		HTTP_FREE_SSI_STATE(ssi);
	}
}

#endif

