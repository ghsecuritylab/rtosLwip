
#include "lwipExt.h"
//#include "lwip/ip4_addr.h"

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "jsmn.h"
#include "extUdpCmd.h"


char extIpCmdIsLocal(EXT_JSON_PARSER  *parser)
{
	char ret;
	ret = extJsonUserValidate(parser);
	if(ret == EXIT_FAILURE)
	{
		return ret;
	}

	if(!MAC_ADDR_IS_EQUAL(&parser->target, &parser->runCfg->local.mac) )
	{
		snprintf(parser->msg, EXT_JSON_MESSAGE_SIZE, "'targ' is not my address" );
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


static unsigned char _rs232SendHexStr(char *str )
{
	int len = strlen(str);
	int i;
	unsigned char value;


#if EXT_RS232_DEBUG
	printf("Sent to RS232:"EXT_NEW_LINE);
#endif
	for(i=0; i< len/2; i++)
	{
		if(extSysAtoInt8(str+i*2, &value)== EXIT_FAILURE)
		{
			EXT_ERRORF(("'%.*s' is not an integer", 2, str+i*2));
			return EXIT_FAILURE;
		}
		
#if EXT_RS232_DEBUG
		printf("%2X ", value);
#endif

#ifdef	ARM
		extRs232Write(&value, 1);
#endif
		
	}
#if EXT_RS232_DEBUG
	printf(""EXT_NEW_LINE);
#endif


	return EXIT_SUCCESS;
}


char extIpCmdSendRsData(EXT_JSON_PARSER  *parser)
{
	char ret;
	int index = 0;
	char *data = NULL;
	int size = 0;


	ret = extIpCmdIsLocal(parser);
	if(ret == EXIT_FAILURE)
		return ret;

	if(extJsonParseDataArray(parser) == EXIT_FAILURE)
	{
		snprintf(parser->msg, sizeof(parser->msg), "No 'Data' array is found");
		goto parseFailed;
	}

	if(extJsonParseString(parser, EXT_IPCMD_RS232_DATA_HEX, parser->setupData.hexData, sizeof(parser->setupData.hexData)) == EXIT_FAILURE)
	{
		snprintf(parser->msg, sizeof(parser->msg), "No '%s' is found or it is invalidate value", EXT_IPCMD_RS232_DATA_HEX);
		goto parseFailed;
	}

	if(extJsonParseUnsignedShort(parser, EXT_IPCMD_RS232_WAIT_TIME, &parser->setupData.waitMs) == EXIT_FAILURE )
	{
		snprintf(parser->msg, sizeof(parser->msg), "No '%s' is found or it is invalidate value", EXT_IPCMD_RS232_WAIT_TIME);
		goto parseFailed;
	}

	if(extJsonParseUnsignedChar(parser, EXT_IPCMD_RS232_FEEDBACK, &parser->setupData.isFeedBack)  == EXIT_FAILURE )
	{
		snprintf(parser->msg, sizeof(parser->msg), "No '%s' is found or it is invalidate value", EXT_IPCMD_RS232_FEEDBACK);
		goto parseFailed;
	}
	
	if(EXT_DEBUG_UDP_CMD_IS_ENABLE())
	{
		printf("RS232 Data: hexdata:'%s'; feedback:%d; waitMs:%d"EXT_NEW_LINE,parser->setupData.hexData, parser->setupData.isFeedBack, parser->setupData.waitMs);
	}

//	EXT_DEBUGF(EXT_IPCMD_DEBUG, ("RS Data: hexdata:'%s'; feedback:%d; waitMs:%d",parser->setupData.hexData, parser->setupData.isFeedBack, parser->setupData.waitMs));

	rs232StartRx();
	
	if(_rs232SendHexStr(parser->setupData.hexData) == EXIT_FAILURE)
	{
		snprintf(parser->msg, sizeof(parser->msg), "'%s' is not validate when sent to RS232", parser->setupData.hexData);
		goto parseFailed;
	}

	if(!parser->setupData.isFeedBack )
	{
		parser->status = JSON_STATUS_OK;
		rs232StopRx();
		return extIpCmdResponseReply(parser);
	}

	memset(parser->setupData.hexData, 0, HEX_DATA_MAX_LENGTH);
	size = rs232StartRead(parser->setupData.waitMs, parser->setupData.hexData);
	if(size <= 0)
	{
		parser->status = JSON_STATUS_CMD_EXEC_ERROR;
		rs232StopRx();
		return extIpCmdResponseReply(parser);
	}
	
	extIpCmdResponseHeaderPrint( parser);
	data = parser->outBuffer+IPCMD_HEADER_LENGTH + parser->outIndex;
	size = parser->outSize - IPCMD_HEADER_LENGTH - parser->outIndex;
	
	if(EXT_DEBUG_UDP_CMD_IS_ENABLE())
	{
		printf(EXT_IPCMD_RS232_DATA_FEEDBACK": '%s'"EXT_NEW_LINE, parser->setupData.hexData);
	}
	
	index += snprintf(data+index, size-index, ",\""EXT_IPCMD_DATA_ARRAY"\":[{");
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_RS232_DATA_FEEDBACK"\":\"%s\"", parser->setupData.hexData);
	index += snprintf(data+index, size-index, "}]" );

	parser->outIndex += index;
	
	parser->status = JSON_STATUS_OK;
	extIpCmdResponseTailCalculate(parser, EXT_FALSE);

	rs232StopRx();
	return EXIT_SUCCESS;
	
parseFailed:
	rs232StopRx();
	parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
	return EXIT_FAILURE;
}


static char _securityCheckId(EXT_JSON_PARSER  *parser)
{
	int index = 0;
	char *data = NULL;
	int size = 0;
	int i;

	/* get ID here */
	if(0)
	{
		snprintf(parser->msg, sizeof(parser->msg), "sub-command '%s' EXEC failed", EXT_IPCMD_SC_GET_ID);
		return EXIT_FAILURE;
	}
	else
	{
		memset(parser->setupData.scID, 0, sizeof(parser->setupData.scID));
		for(i=0; i< SC_ROM_ID_SIZE; i++)
		{
			extBcd2Ascii(parser->runCfg->sc->romId[i], parser->setupData.scID+i*2);
		}	
	}

	parser->status = JSON_STATUS_OK;

	extIpCmdResponseHeaderPrint( parser);
	data = parser->outBuffer+IPCMD_HEADER_LENGTH + parser->outIndex;
	size = parser->outSize - IPCMD_HEADER_LENGTH - parser->outIndex;
	
	index += snprintf(data+index, size-index, ",\""EXT_IPCMD_DATA_ARRAY"\":[{");
	index += snprintf(data+index, size-index, "\""EXT_IPCMD_SC_GET_ID"\":\"%s\"", parser->setupData.scID );
	index += snprintf(data+index, size-index, "}]" );

	parser->outIndex += index;

	extIpCmdResponseTailCalculate(parser, EXT_FALSE);

	return EXIT_SUCCESS;
}


char extIpCmdSecurityCheck(EXT_JSON_PARSER  *parser)
{
	char ret;

#if 1
	extIpCmdSendMediaData(parser, EXT_FALSE);
#endif

	ret = extIpCmdIsLocal(parser);
	if(ret == EXIT_FAILURE)
		return ret;

	if(extJsonParseDataArray(parser) == EXIT_FAILURE)
	{
		snprintf(parser->msg, sizeof(parser->msg), "No 'Data' array is found");
		goto parseFailed;
	}

	if(! IS_SECURITY_CHIP_EXIST(parser->runCfg->sc) )
	{
		snprintf(parser->msg, sizeof(parser->msg), "No Security Chip is found in this unit");
		goto parseFailed;
	}

	if(extJsonParseString(parser, EXT_IPCMD_SC_SET_KEY, parser->setupData.scKey, sizeof(parser->setupData.scKey)) == EXIT_SUCCESS)
	{ /* for set_key */
		unsigned int i;
		memset(parser->runCfg->sc->readMac, 0xFF, SC_SECRET_SIZE);
		
		if(strlen(parser->setupData.scKey)%2 != 0)
		{
			EXT_ERRORF(("Key size is %d, is not even number of letters !", strlen(parser->setupData.scKey) ));
			snprintf(parser->msg, sizeof(parser->msg), "'%s' error: Key size '%d' is not even number", EXT_IPCMD_SC_GET_STATUS, strlen(parser->setupData.scKey) );
			ret = EXIT_FAILURE;
		}
		else
		{
//			for(i=0; i< SC_SECRET_SIZE; i++)
			for(i=0; i< strlen(parser->setupData.scKey)/2; i++)
			{
				ret = extSysAtoInt8(parser->setupData.scKey+i*2, parser->runCfg->sc->readMac+i);
				if(ret ==  EXIT_FAILURE)
				{
					EXT_ERRORF(("#%d: '%.*s' is not an integer", i*2, 2, parser->setupData.scKey+i*2) );
					snprintf(parser->msg, sizeof(parser->msg), "'%s' error: #%d letters '%.*s' is not an integer", EXT_IPCMD_SC_GET_STATUS, i*2, 2, parser->setupData.scKey+i*2);
					break;
				}

#if 0
				if(parser->runCfg->sc->readMac[i] != parser->runCfg->sc->secret[i] )
				{
					EXT_ERRORF(("Key #%d %02x != %02x (board)", i, parser->runCfg->sc->readMac[i], parser->runCfg->sc->secret[i]) );
					snprintf(parser->msg, sizeof(parser->msg), "'%s' error: key is not correct", EXT_IPCMD_SC_GET_STATUS);
				}
#endif				
			}
		}

		if( ret == EXIT_SUCCESS)
		{
			ret = bspScWriteKey(parser->runCfg->sc, parser->runCfg->sc->readMac);
		}

		if( ret == EXIT_SUCCESS)
		{
			parser->status = JSON_STATUS_OK;
		}
		else
		{
			parser->status = JSON_STATUS_CMD_EXEC_ERROR;
//			snprintf(parser->msg, sizeof(parser->msg), "'%s' error on hardware", EXT_IPCMD_SC_GET_STATUS);
		}
		
		EXT_DEBUGF(EXT_IPCMD_DEBUG, ("'%s' command OK!",EXT_IPCMD_SC_SET_KEY ));
		return extIpCmdResponseReply(parser);
	}

	if(extJsonParseString(parser, EXT_IPCMD_SC_GET_STATUS,  parser->setupData.scKey, sizeof(parser->setupData.scKey) )  == EXIT_SUCCESS )
	{/* for get_status */
		
		if( bspScCheckMAC(parser->runCfg->sc ) == EXIT_SUCCESS )
		{
			parser->status = JSON_STATUS_OK;
		}
		else
		{
			parser->status = JSON_STATUS_CMD_EXEC_ERROR;
			snprintf(parser->msg, sizeof(parser->msg), "'%s' error on hardware", EXT_IPCMD_SC_GET_STATUS);
		}
		EXT_DEBUGF(EXT_IPCMD_DEBUG, ("'%s' command OK!",EXT_IPCMD_SC_GET_STATUS ));
		return extIpCmdResponseReply(parser);
	}

TRACE();
	if(extJsonParseString(parser, EXT_IPCMD_SC_GET_ID, parser->setupData.scKey, sizeof(parser->setupData.scKey) ) == EXIT_FAILURE)
	{
		snprintf(parser->msg, sizeof(parser->msg), "No validate sub-command is found for '%s'", EXT_IPCMD_CMD_SECURITY_CHECK);
		goto parseFailed;
	}

	/* default, get_id */
	if( _securityCheckId(parser) == EXIT_FAILURE)
	{
		goto parseFailed;
	}

	EXT_DEBUGF(EXT_IPCMD_DEBUG, ("'%s' command OK!",EXT_IPCMD_SC_GET_ID ));
	return EXIT_SUCCESS;
	
parseFailed:
	parser->status = JSON_STATUS_PARSE_PARAM_ERROR;
	return EXIT_FAILURE;
}

/* send IP command of params to 811 */
char	extIpCmdSendMediaData(EXT_JSON_PARSER  *parser, char isRequest)
{
	int index = 0;
	char *data = NULL;
	int size = 0;

	/* get ID here */
	if(0)
	{
		snprintf(parser->msg, sizeof(parser->msg), "sub-command '%s' EXEC failed", EXT_IPCMD_SC_GET_ID);
		return EXIT_FAILURE;
	}
	else
	{
		snprintf(parser->setupData.scID, sizeof(parser->setupData.scID), "%s", "1234567890");
	}

	parser->status = JSON_STATUS_OK;

	extIpCmdRequestHeaderPrint( parser, EXT_IPCMD_CMD_SET_PARAMS );
	data = parser->outBuffer+IPCMD_HEADER_LENGTH + parser->outIndex;
	size = parser->outSize - IPCMD_HEADER_LENGTH - parser->outIndex;
	
	index += snprintf(data+index, size-index, ",\""EXT_IPCMD_DATA_ARRAY"\":[{");
	
	index += extIpCmdPrintMediaCfg(parser, data+index, size-index);

	index += snprintf(data+index, size-index, "}]" );

	parser->outIndex += index;
//	EXT_DEBUGF(EXT_IPCMD_DEBUG, (" %d (%d)bytes packet", parser->outIndex, index) );

	extIpCmdResponseTailCalculate(parser, isRequest);

#if 0
	return extIpCmdSendout(parser, &parser->runCfg->dest.ip, EXT_CTRL_PORT);
#else
	return extIpCmdSendout(parser, &parser->runCfg->ipSvr811, parser->runCfg->portSvr811);
#endif
}


