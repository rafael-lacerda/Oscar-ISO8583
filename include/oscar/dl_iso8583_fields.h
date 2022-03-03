/******************************************************************************/
/*                                                                            */
/* Copyright (C) 2005-2007 Oscar Sanderson                                    */
/*                                                                            */
/* This software is provided 'as-is', without any express or implied          */
/* warranty.  In no event will the author(s) be held liable for any damages   */
/* arising from the use of this software.                                     */
/*                                                                            */
/* Permission is granted to anyone to use this software for any purpose,      */
/* including commercial applications, and to alter it and redistribute it     */
/* freely, subject to the following restrictions:                             */
/*                                                                            */
/* 1. The origin of this software must not be misrepresented; you must not    */
/*    claim that you wrote the original software. If you use this software    */
/*    in a product, an acknowledgment in the product documentation would be   */
/*    appreciated but is not required.                                        */
/*                                                                            */
/* 2. Altered source versions must be plainly marked as such, and must not be */
/*    misrepresented as being the original software.                          */
/*                                                                            */
/* 3. This notice may not be removed or altered from any source distribution. */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/* ISO8583 field packers / unpackers                                          */
/*                                                                            */
/******************************************************************************/

#ifndef __INC_DL_ISO8583_FIELDS
#define __INC_DL_ISO8583_FIELDS

#include "oscar/dl_base.h"
#include "oscar/dl_err.h"
#include "oscar/dl_mem.h"

#include "oscar/dl_iso8583_common.h"

extern DL_UINT16 DEBUG;

/******************************************************************************/

DL_ERR _DL_ISO8583_FIELD_Pack ( DL_UINT16                  iField,
								const DL_ISO8583_MSG      *iMsg,
								const DL_ISO8583_HANDLER  *iHandler,
								DL_UINT8                 **ioPtr,
								DL_UINT16                  isEbcdic );

DL_ERR _DL_ISO8583_FIELD_Unpack ( DL_UINT16                  iField,
								  DL_ISO8583_MSG            *ioMsg,
								  const DL_ISO8583_HANDLER  *iHandler,
								  DL_UINT8                 **ioPtr,
								  DL_UINT16                  isEbcdic );

DL_ERR _hexstr_to_bytes	(	const char *hexStr,
                     		DL_UINT8 *output,
                    		DL_UINT16 *outputLen);

DL_ERR _bytes_to_hexstr(const DL_UINT8 *bytes,
                     	DL_UINT8 *output,
                    	DL_UINT16 *outputLen);

DL_ERR _bcd_to_asc(	DL_UINT8 * BcdBuf,
					DL_UINT8 * AscBuf,
					DL_UINT16 Len);

/******************************************************************************/

#endif /* __INC_DL_ISO8583_FIELDS */
