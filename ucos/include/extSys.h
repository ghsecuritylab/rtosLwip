#ifndef	__EXT_SYS_H__
#define	__EXT_SYS_H__

/* defined in stdlib.h */
#ifndef	EXIT_FAILURE
#define	EXIT_FAILURE 			1
#endif

#ifndef	EXIT_SUCCESS
#define	EXIT_SUCCESS 			0
#endif

#ifndef EXT_FALSE
#define EXT_FALSE				0
#endif

#ifndef EXT_TRUE
#define EXT_TRUE				(!EXT_FALSE)
#endif

#include <stdint.h>



#define	EXT_NEW_LINE						"\r\n"
//#define	EXT_NEW_LINE							"\r"

#define	EXT_PROMPT							">"


#define	ASCII_KEY_TAB					( 0x09 )	/* horizontal TAB */
#define	ASCII_KEY_LF					( 0x0A )	/* Line Feed, \n */
#define	ASCII_KEY_CR					( 0x0D )	/* Carier Return, \r */
/* DEL acts as a backspace. */
#define	ASCII_KEY_DEL					( 0x7F )

/* macros for version */
#define	EXT_VERSION_DOT(a, b, c)				a ##.## b ##.## c

#define	EXT_VERSION(a, b, c)					EXT_VERSION_DOT(a, b, c)

#define	EXT_STRINGIFY(s)         					EXT_TOSTRING(s)
#define	EXT_TOSTRING(s)						#s


#define	EXT_GLUE(a, b)							a ## b
#define	EXT_JOIN(a, b)							EXT_GLUE(a, b)

#define	EXT_VERSION_MAJOR					0
#define	EXT_VERSION_MINOR					1
#define	EXT_VERSION_REVISION					1

#define	BL_VERSION_MAJOR						0
#define	BL_VERSION_MINOR						1
#define	BL_VERSION_REVISION					1


#define	EXT_VERSION_INFO()					((EXT_VERSION_MAJOR<<16)|(EXT_VERSION_MINOR<<8)|(EXT_VERSION_REVISION))


#define	BL_VERSION_INFO()						((BL_VERSION_MAJOR<<16)|(BL_VERSION_MINOR<<8)|(BL_VERSION_REVISION))


#define	EXT_VERSION_INTEGER()					((EXT_VERSION_MAJOR)<<16 | (EXT_VERSION_MINOR)<<8 | (EXT_VERSION_REVISION))


#define	BL_VERSION_TOKEN						EXT_VERSION(BL_VERSION_MAJOR, BL_VERSION_MINOR, BL_VERSION_REVISION)
#define	EXT_VERSION_TOKEN					EXT_VERSION(EXT_VERSION_MAJOR, EXT_VERSION_MINOR, EXT_VERSION_REVISION)


/* only call EXT_STRINGFY, can't call EXT_TOSTRING; otherwise return string of 'EXT_VERSION_TOKEN' */
#define	EXT_VERSION_STRING		\
			EXT_STRINGIFY(EXT_VERSION_TOKEN)	
#define	BL_VERSION_STRING		\
			EXT_STRINGIFY(BL_VERSION_TOKEN)	


#define	BL_SYSTEM_NAME			"USB Sys Bootloader"
#define	EXT_SYSTEM_NAME			"uCoS USB"

//EXT_NEW_LINE EXT_NEW_LINE 

#define	BUILD_DATE_TIME			__DATE__" "__TIME__

#define	EXT_SYSTEM_STRING(sysName, verInfo) 		\
		EXT_NEW_LINE"" sysName" (" \
		"Version: " verInfo "; " \
		"Built: " BUILD_DATE_TIME ")"EXT_NEW_LINE


#define	EXT_OS_NAME		EXT_SYSTEM_STRING(EXT_SYSTEM_NAME, EXT_VERSION_STRING)

#define	EXT_BL_NAME		EXT_SYSTEM_STRING(BL_SYSTEM_NAME, BL_VERSION_STRING)



#define	KB(x)						((x) / 1024)
#define	PERCENTAGE(x,total)			(((x) * 100) / (total))

#define	UNIT_OF_KILO					1024
#define	UNIT_OF_MEGA					(UNIT_OF_KILO*UNIT_OF_KILO)
#define	UNIT_OF_GIGA					(UNIT_OF_KILO*UNIT_OF_MEGA)

#define EXT_ARRAYSIZE(x)		(sizeof(x)/sizeof((x)[0]))

#define EXT_MAKEU32(a,b,c,d) (((int)((a) & 0xff) << 24) | \
                               ((int)((b) & 0xff) << 16) | \
                               ((int)((c) & 0xff) << 8)  | \
                                (int)((d) & 0xff))




/** The MAX value of shifting. */
#define MAX_SHIFTING_NUMBER    (32)



#define	GET_BIT(value, bit)				(((value)>>(bit))&0x01)
#define	SET_BIT(value, bit)				((value) << (bit))



#define	CMN_SET_BIT(flags, bitPosition)	\
		flags |= SET_BIT(0x01, (bitPosition) ) 

#define	CMN_CLEAR_BIT(flags, bitPosition)	\
		flags &= ~(SET_BIT(0x01, (bitPosition) ) )	

#define	CMN_CHECK_BIT(flags, bitPosition)	\
		( (flags&SET_BIT(0x01,(bitPosition) ) )!=0 )

#define	CMN_SET_FLAGS(flags, value)	\
		flags |= (value) 

#define	CMN_CLEAR_FLAGS(flags, value)	\
		flags &= ~((value) ) )	

#define	CMN_CHECK_FLAGS(flags, value)	\
		( (flags&(value) )!=0 )


/** Debug level: ALL messages*/
#define	EXT_DBG_LEVEL_ALL				0x00
#define	EXT_DBG_LEVEL_INFO			0x01
#define	EXT_DBG_LEVEL_WARN			0x02
#define	EXT_DBG_LEVEL_ERROR			0x03
#define	EXT_DBG_LEVEL_FATAL			0x04


#define	EXT_DBG_MASK_LEVEL			0x04
#define	EXT_DBG_LEVEL_OFF			LWIP_DBG_LEVEL_ALL


#define	EXT_DBG_ON					0x80U
#define	EXT_DBG_OFF					0x00U


#define	EXT_DBG_TRACE					0x40U
#define	EXT_DBG_STATE					0x20U
#define	EXT_DBG_FRESH					0x10U
#define	EXT_DBG_HALT					0x08U


/* level used */
#define	EXT_DBG_TYPES_ON				EXT_DBG_ON
#define	EXT_DBG_MIN_LEVEL			EXT_DBG_LEVEL_ALL



/*
* ESC (27, 0x1b) charactor is '\e' or '\x1b'
*/

#define ANSI_COLOR_RED				"\x1b[31m"	/* ESC[31m : red */
#define ANSI_COLOR_GREEN			"\x1b[32m"
#define ANSI_COLOR_YELLOW			"\x1b[33m"
#define ANSI_COLOR_BLUE			"\x1b[34m"
#define ANSI_COLOR_MAGENTA		"\x1b[35m"
#define ANSI_COLOR_CYAN			"\x1b[36m"
#define ANSI_COLOR_RESET			"\x1b[0m"	/* for all colors, other than red, this must be used. April,15,2018. JL*/


#define	ERROR_TEXT_BEGIN			"\t\e[31m ERR:"
#define	ERROR_TEXT_END			"\e[0m"


#define	WARN_TEXT_BEGIN			""ANSI_COLOR_MAGENTA"WARN:"

#define	INFO_TEXT_BEGIN			""ANSI_COLOR_BLUE"INFO:"


#ifndef __EXT_RELEASE__
	#define	EXT_PRINTF(x)						{printf x ;}
	
//	#define	EXT_DEBUGF(fmt, args...)	{printf("[%s-%u] DEBUG: " fmt EXT_NEW_LINE, __FILE__, __LINE__, ## args);}
	#define	EXT_DEBUGF(debug, message)		do { \
                               if ( \
                                   ((debug) & EXT_DBG_ON) && \
                                   ((debug) & EXT_DBG_TYPES_ON) && \
                                   ((s16_t)((debug) & EXT_DBG_MASK_LEVEL) >= EXT_DBG_MIN_LEVEL)) { \
                                 _TRACE_OUT(message);printf(EXT_NEW_LINE); \
                                 if ((debug) & LWIP_DBG_HALT) { \
                                   while(1); \
                                 } \
                               } \
                             } while(0)

                             
	#define	EXT_INFOF(message)		{printf(ANSI_COLOR_CYAN "%s:[%s-%u]:", sysTaskName(), __FILE__, __LINE__);EXT_PRINTF(message);printf((ANSI_COLOR_RESET EXT_NEW_LINE));}
	
	#define	EXT_ERRORF(message)		{printf(ERROR_TEXT_BEGIN "%s: ERROR:[%s-%u]:", sysTaskName(), __FILE__, __LINE__);_TRACE_OUT(message); printf((ERROR_TEXT_END  EXT_NEW_LINE));}

//	#define	EXT_ASSERT(x)				{printf("Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__); while(1);}
	#define	EXT_ASSERT(msg, x)			{if((x)==0) {printf(ERROR_TEXT_BEGIN"%s: ASSERT: [%s-%u]:",  sysTaskName(), __FILE__, __LINE__ );printf msg ;printf((ERROR_TEXT_END EXT_NEW_LINE)); while(0){};}}
	#define	EXT_ABORT(fmt, args... )		printf("%s: ABORT in [" __FILE__ "-%u]:" fmt EXT_NEW_LINE, sysTaskName(), __LINE__, ##args );while(1){}
#else
	#define	EXT_PRINTF(x)						{;}

	#define	EXT_DEBUGF(debug, message)		{}

	#define	EXT_INFOF(message)				{printf message ;printf(EXT_NEW_LINE);}

	#define	EXT_ERRORF(message)				{printf message ; printf(EXT_NEW_LINE);}
	
//	#define	EXT_ASSERT(x)				{while (1);}
	#define	EXT_ASSERT(msg, x)				{}
	#define	EXT_ABORT(fmt, args... )		{}
#endif

#define	_TRACE_OUT(message)	\
			{EXT_PRINTF(("%s: [%s-%u.%s()]: ",  sysTaskName(), __FILE__, __LINE__, __FUNCTION__) );EXT_PRINTF(message); }

#define	TRACE()						_TRACE_OUT((EXT_NEW_LINE) )




/* when RELEASE build, only omit the output  */
#define	EXT_DBG_ERRORF(message, expression, handler) do { if (!(expression)) { \
		_TRACE_OUT(message); handler;}} while(0)


#define EXT_ARRAYSIZE(x)		(sizeof(x)/sizeof((x)[0]))


#define CFG_MAKEU32(a,b,c,d) (((int)((a) & 0xff) << 24) | \
                               ((int)((b) & 0xff) << 16) | \
                               ((int)((c) & 0xff) << 8)  | \
                                (int)((d) & 0xff))


#define	CFG_SET_FLAGS(flags, value)	\
		flags |= (value) 

#define	CFG_CLEAR_FLAGS(flags, value)	\
		flags &= ~((value) ) 


/* add at the header of list */
#define ADD_ELEMENT(header, element)	\
	if (header == NULL){					\
		header  = element;				\
		element->next   = NULL;			\
	}									\
	else	{								\
		element->next   = header;		\
		header = element;				\
	}


/* add at the tail of list */
#define APPEND_ELEMENT(header, element, type)	\
	if ((header) == NULL){					\
		(header)  = (element);				\
		(element)->next   = NULL;			\
	}									\
	else	{ type *cur = (header), *_next = (header)->next; \
		while(_next){cur = _next; _next = cur->next;};		\
		cur->next = (element);				\
	}



#define REMOVE_ELEMENT(header, element, type)	\
{type **cp, *c1; cp = &header; \
	while ((*cp) != NULL){  c1 = *cp; \
		if (c1 == element ) \
		{ *cp = element->next;} \
		else	{ cp = &c1->next;} \
	}; }


#define		STR_BOOL_VALUE(bValue)	\
			(((bValue)==0)?"NO":"YES")



#define	IS_STRING_NULL( str)	\
	(((str)==NULL)||(!strlen((str))) )


#define	IS_STRING_EQUAL( str1, str2)	\
	( !strcasecmp((str1), (str2)))


#define	INVALIDATE_VALUE_U8						0xFF
#define	INVALIDATE_VALUE_U16						0xFFFF
#define	INVALIDATE_VALUE_U32						0xFFFFFFFF


#define	FIELD_INVALIDATE_U8( field) 					((field) = INVALIDATE_VALUE_U8)
#define	FIELD_INVALIDATE_U16(field) 					((field) = INVALIDATE_VALUE_U16)
#define	FIELD_INVALIDATE_U32(field)					((field) = INVALIDATE_VALUE_U32)

#define	FIELD_IS_CHANGED_U8(field)					((field) != INVALIDATE_VALUE_U8)
#define	FIELD_IS_CHANGED_U16(field)				((field) != INVALIDATE_VALUE_U16)
#define	FIELD_IS_CHANGED_U32(field)				((field) != INVALIDATE_VALUE_U32)


//EXT_PACK_RESET();

// #pragma		pack()

/* update size by setParams command */
#define		EXT_RUNTIME_CFG_UPDATE_SIZE		(offsetof(struct _EXT_RUNTIME_CFG, name))

#define		EXT_RUNTIME_CFG_WRITE_SIZE		(offsetof(struct _EXT_RUNTIME_CFG, debugOptions))


//#if ( sizeof(EXT_FM_VERSION) != 3 )
//	#error Data structure is packed wrong
//#endif

//typedef char assertion_on_mystruct[( sizeof(EXT_FM_VERSION)==3  )*2-1 ];

#define DUMB_STATIC_ASSERT(test)	typedef char assertion_on_mystruct[( !!(test) )*2-1 ]


//DUMB_STATIC_ASSERT( sizeof(mystruct)==1024 );
//DUMB_STATIC_ASSERT( sizeof(my_other_struct)==23 );
//DUMB_STATIC_ASSERT( sizeof(minimum_size_struct) >= 23 );

//static_assert(sizeof(EXT_FM_VERSION) == 3,   "Check your assumptions");


typedef	struct _EXT_RUNTIME_CFG		EXT_RUNTIME_CFG;


typedef	char (*MuxDelayJob)(void *data);


#define	CMN_SN_PRINTF(data, size, index, ...)   \
	do{ if((size)-(index) > 0 ){\
		(index) += snprintf((data)+(index), ((size)-(index)), __VA_ARGS__);}else{EXT_DEBUGF(EXT_DBG_ON, ("index %d is out of range of size %d", (index), (size)) );}			\
		}while(0)



#endif

