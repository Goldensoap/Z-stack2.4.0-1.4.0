/***************************************************************************************************
  Filename:       MT_APP.c
  Revised:        $Date: 2010-01-06 15:16:46 -0800 (Wed, 06 Jan 2010) $
  Revision:       $Revision: 21443 $

  Description:    MonitorTest processing for APP commands

  Copyright 2007-2009 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED �AS IS� WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.

 ***************************************************************************************************/

/***************************************************************************************************
 * INCLUDES
 ***************************************************************************************************/
#include "ZComDef.h"
#include "MT.h"        /* This is here because all the SPI_CMD_XXX are defined in this header */
#include "MT_APP.h"
#include "MT_AF.h"     /* This is here because this MT_APP makes some routine call to AF */

#if defined( APP_TP )
 #include "TestProfile.h"
#endif
#if defined( APP_TP2 )
 #include "TestProfile2.h"
 #include "nwk_util.h"
#endif

/***************************************************************************************************
 * LOCAL FUNCTIONS
 ***************************************************************************************************/
#if defined (MT_APP_FUNC)
void MT_AppMsg(uint8 *pBuf);
void MT_AppUserCmd(byte *pData);
#endif

#if defined (MT_APP_FUNC)
/***************************************************************************************************
 * @fn      MT_AppCommandProcessing
 *
 * @brief  Process all the APP commands that are issued by test tool
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  status
 ***************************************************************************************************/
uint8 MT_AppCommandProcessing(uint8 *pBuf)
{
  uint8 status = MT_RPC_SUCCESS;

  switch (pBuf[MT_RPC_POS_CMD1])
  {
    case MT_APP_MSG:
      MT_AppMsg(pBuf);
      break;

    case MT_APP_USER_TEST:
      MT_AppUserCmd(pBuf);
      break;

    default:
      status = MT_RPC_ERR_COMMAND_ID;
      break;
  }

  return status;
}

/***************************************************************************************************
 * @fn      MT_AppMsg
 *
 * @brief   Process APP_MSG command
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  void
 ***************************************************************************************************/
void MT_AppMsg(uint8 *pBuf)
{
  uint8 retValue = ZFailure;
  uint8 endpoint;
  endPointDesc_t *epDesc;
  mtSysAppMsg_t *msg;
  uint8 cmdId, dataLen;

  /* parse header */
  dataLen = pBuf[MT_RPC_POS_LEN];
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  /* Get the endpoint and skip past it.*/
  endpoint = *pBuf++;
  dataLen--;

  /* Look up the endpoint */
  epDesc = afFindEndPointDesc( endpoint );

  if (epDesc)
  {
    /* Build and send the message to the APP */
    msg = (mtSysAppMsg_t *)osal_msg_allocate(sizeof(mtSysAppMsg_t) + (dataLen));
    if ( msg )
    {
      /* Build and send message up the app */
      msg->hdr.event = MT_SYS_APP_MSG;
      msg->endpoint = endpoint;
      msg->appDataLen = dataLen;
      msg->appData = (uint8*)(msg+1);
      osal_memcpy( msg->appData, pBuf, dataLen);
      osal_msg_send( *(epDesc->task_id), (uint8 *)msg );

      /* Info for response */
      retValue = ZSuccess;
    }
  }

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_APP), cmdId, 1, &retValue);
}

/***************************************************************************************************
 * @fn      MT_AppMsg
 *
 * @brief   Process APP_MSG command
 *
 * @param   pBuf - pointer to the received buffer
 *
 * @return  void
 ***************************************************************************************************/
void MT_AppUserCmd(uint8 *pBuf) 						
{

  uint8 retValue, cmdId;

#if defined (APP_TGEN) || defined (NWK_TEST) || defined (APP_TP) || defined (APP_TP2) || defined (OSAL_TOTAL_MEM) || defined (APP_DEBUG)
  uint16 app_cmd;
  uint8 srcEp;
  uint16 param1;
  uint16 param2;
#endif
#if defined (OSAL_TOTAL_MEM)
  uint8 pData[2];
#endif

  /* parse header */
  cmdId = pBuf[MT_RPC_POS_CMD1];
  pBuf += MT_RPC_FRAME_HDR_SZ;

  retValue = INVALID_TASK;     //should be changed later

#if defined (APP_TGEN) || defined (NWK_TEST) || defined (APP_TP) || defined (APP_TP2) || defined (OSAL_TOTAL_MEM) || defined (APP_DEBUG)

  srcEp = *pBuf++;

  app_cmd = BUILD_UINT16( pBuf[0] , pBuf[1] );
  pBuf = pBuf + sizeof( uint16 );

  param1 = BUILD_UINT16( pBuf[0] , pBuf[1] );
  pBuf = pBuf + sizeof( uint16 );

  param2 = BUILD_UINT16( pBuf[0] , pBuf[1] );

  switch ( app_cmd )
  {

#if defined (APP_TGEN)
    case TGEN_START:
      TrafficGenApp_SendCmdMSG( param1, param2, TRAFFICGENAPP_CMD_START );
      retValue = ZSUCCESS;
      break;

    case TGEN_STOP:
      TrafficGenApp_SendCmdMSG( param1, param2, TRAFFICGENAPP_CMD_STOP );
      retValue = ZSUCCESS;
      break;

    case TGEN_COUNT:
      retValue = TrafficGenApp_CountPkt( param1, param2 );
      return;	
      break;				
#endif

#if defined (NWK_TEST)
    case HW_TEST:
      HwApp_Start( HI_UINT16(param1), LO_UINT16(param1), HI_UINT16(param2),
                    1000, LO_UINT16(param2), 3, 0 );
      break;

    case HW_DISPLAY_RESULT:
      HwApp_TestInfo();
      break;

    case HW_SEND_STATUS:
      HwApp_SendStats();
      break;
#endif

#if defined( APP_TP ) || defined ( APP_TP2 )
  #if defined( APP_TP )
    case TP_SEND_NODATA:
      retValue = TestProfileApp_SendNoData( srcEp, (byte)param1 );
      break;
  #endif // APP_TP
			
    case TP_SEND_BUFFERTEST:
      retValue = TestProfileApp_SendBufferReq( srcEp, (byte)param1 );
      break;
			
  #if defined( APP_TP )
    case TP_SEND_UINT8:
      retValue = TestProfileApp_SendUint8( srcEp, (byte)param1 );
      break;

    case TP_SEND_INT8:
      retValue = TestProfileApp_SendInt8( srcEp, (byte)param1 );
      break;

    case TP_SEND_UINT16:
      retValue = TestProfileApp_SendUint16( srcEp, (byte)param1 );
      break;

    case TP_SEND_INT16:
      retValue = TestProfileApp_SendInt16( srcEp, (byte)param1 );
      break;

    case TP_SEND_SEMIPREC:
      retValue = TestProfileApp_SendSemiPrec( srcEp, (byte)param1 );
      break;

    case TP_SEND_FREEFORM:
      retValue = TestProfileApp_SendFreeFormReq( srcEp, (byte)param1 );
      break;
			
  #else // APP_TP
    case TP_SEND_FREEFORM:
      retValue = TestProfileApp_SendFreeFormReq(srcEp, (byte)param1, (byte)param2);
      break;
  #endif
			
  #if defined( APP_TP )
    case TP_SEND_ABS_TIME:
      retValue = TestProfileApp_SendAbsTime( srcEp, (byte)param1 );
      break;

    case TP_SEND_REL_TIME:
      retValue = TestProfileApp_SendRelativeTime( srcEp, (byte)param1 );
      break;

    case TP_SEND_CHAR_STRING:
      retValue = TestProfileApp_SendCharString( srcEp, (byte)param1 );
      break;

    case TP_SEND_OCTET_STRING:
      retValue = TestProfileApp_SendOctetString( srcEp, (byte)param1 );
      break;		
  #endif // APP_TP
				
    case TP_SET_DSTADDRESS:			
      retValue = TestProfileApp_SetDestAddress(HI_UINT16(param1), LO_UINT16(param1), param2);
      break;	

  #if defined( APP_TP2 )
    case TP_SEND_BUFFER_GROUP:
      retValue = TestProfileApp_SendBufferGroup( srcEp, (byte)param1 );
      break;
  #endif // APP_TP

    case TP_SEND_BUFFER:
      retValue = TestProfileApp_SendBuffer( srcEp, (byte)param1 );
      break;
				
  #if defined( APP_TP )
    case TP_SEND_MULT_KVP_8BIT:
      TestProfileApp_SendMultiKVP_8bit( srcEp, (byte)param1 );
      retValue = ZSuccess;
      break;

    case TP_SEND_MULT_KVP_16BIT:
      TestProfileApp_SendMultiKVP_16bit( srcEp, (byte)param1 );
      retValue = ZSuccess;
      break;

    case TP_SEND_MULT_KVP_TIME:
      TestProfileApp_SendMultiKVP_Time( srcEp, (byte)param1 );
      retValue = ZSuccess;
      break;

    case TP_SEND_MULT_KVP_STRING:
      TestProfileApp_SendMultiKVP_String( srcEp, (byte)param1 );
      retValue = ZSuccess;
      break;

    case TP_SEND_MULTI_KVP_STR_TIME:
      retValue = ZSuccess;
      TestProfileApp_SendMultiKVP_String_Time( srcEp, (byte)param1 );
      break;
  #endif // APP_TP
				
    case TP_SEND_COUNTED_PKTS:
      TestProfileApp_SendCountedPktsReq(HI_UINT16(param1), LO_UINT16(param1), param2);
      retValue = ZSuccess;
      break;

    case TP_SEND_RESET_COUNTER:
      TestProfileApp_CountedPakts_ResetCounterReq( (byte)param1 );
      retValue = ZSuccess;
      break;

    case TP_SEND_GET_COUNTER:
      TestProfileApp_CountedPakts_GetCounterReq( srcEp, (byte)param1 );
      retValue = ZSuccess;
      break;
				
    case TP_SET_PERMIT_JOIN:
      if ( ZG_BUILD_RTR_TYPE && ZG_DEVICE_RTR_TYPE )
      {
        NLME_PermitJoiningRequest( (byte)param1 );
        retValue = ZSuccess;
      }
      else
      {
        retValue = ZFailure;
      }
      break;

  #if defined ( APP_TP2 )
    case TP_ADD_GROUP:
      retValue = TestProfileApp_SetGroup( srcEp, param1 );
      break;

    case TP_REMOVE_GROUP:
      retValue = TestProfileApp_RemoveGroup( srcEp, param1 );
      break;

    case TP_SEND_UPDATE_KEY:
      retValue = TestProfileApp_UpdateKey( srcEp, (uint8)param1, param2 );
      break;

    case TP_SEND_SWITCH_KEY:
      retValue = TestProfileApp_SwitchKey(  srcEp, (uint8)param1, param2 );
      break;
			
    case TP_SEND_BUFFERTEST_GROUP:
      retValue = TestProfileApp_SendBufferGroupReq( srcEp, (byte)param1, (byte)param2 );
      break;

    case TP_SEND_ROUTE_DISC_REQ:
      retValue = TestProfileApp_SendRouteDiscReq( srcEp, param1,
                                  HI_UINT16( param2 ), LO_UINT16( param2 ) );
      break;

    case TP_SEND_ROUTE_DISCOVERY:
      if ( ZG_BUILD_RTR_TYPE && ZG_DEVICE_RTR_TYPE )
      {
        retValue = TestProfileApp_SendRouteDiscovery( param1,
                                    HI_UINT16( param2 ), LO_UINT16( param2 ) );
      }
      break;

    case TP_SEND_NEW_ADDR:
      retValue = TestProfileApp_ChangeShortAddr( param1, LO_UINT16(param2) );
      break;

    case TP_SEND_NWK_UPDATE:
      /* Send out a Network Update command. */
      retValue = NLME_SendNetworkUpdate( NWK_BROADCAST_SHORTADDR, NWKUPDATE_PANID_UPDATE,
                                        _NIB.extendedPANID, _NIB.nwkUpdateId+1, param1 );
      break;
      
    case TP_NWK_ADDR_CONFLICT:
      retValue = NLME_SendNetworkStatus( NWK_BROADCAST_SHORTADDR_DEVZCZR,
                         param1, NWKSTAT_ADDRESS_CONFLICT );
      break;  

 #if (ZG_BUILD_JOINING_TYPE)
    case TP_AK_SETUP_PARTNER:
      retValue = TestProfileApp_AppKeySetupPartner( srcEp, param1, param2 );
      break;

    case TP_AK_REQ_KEY:
      retValue = TestProfileApp_AppKeyRequest( srcEp, param1, param2 );
      break;

    case TP_AK_PARTNER_NWKADDR:
      retValue = TestProfileApp_SetPartnerNwkAddr( srcEp, param1, param2 );
      break;

    case TP_AK_PARTNER_EXTADDR7654:
       retValue = TestProfileApp_SetPartnerExtAddr7654( srcEp, param1, param2 );
      break;

    case TP_AK_PARTNER_EXTADDR3210:
      retValue = TestProfileApp_SetPartnerExtAddr3210( srcEp, param1, param2 );
      break;

    case TP_AK_PARTNER_SET:
      retValue = TestProfileApp_SetPartner( srcEp, param1, param2 );
      break;
#endif /* ZG_BUILD_JOINING_TYPE */

#if (ZG_BUILD_COORDINATOR_TYPE)
    case TP_AK_TYPE_SET:
      retValue = TestProfileApp_AppKeyTypeSet( srcEp, param1, param2 );
      break;
#endif /* ZG_BUILD_COORDINATOR_TYPE */

#if defined ( ZIGBEE_FRAGMENTATION )
    case TP_FRAG_SKIP_BLOCK:
      retValue = TestProfileApp_FragSkipBlock( (uint8)param1 );
      break;
#endif

    case TP_APS_REMOVE:
      retValue = TestProfileApp_APSRemove( param1, param2 );
      break;
#endif // APP_TP2

#endif  // APP_TP || APP_TP2

#if defined ( OSAL_TOTAL_MEM )
    case OSAL_MEM_STACK_HIGH_WATER:
    case OSAL_MEM_HEAP_HIGH_WATER:
      if ( app_cmd == OSAL_MEM_STACK_HIGH_WATER)
      {
        param1 = osal_stack_used();
      }
      else
      {
        param1 = osal_heap_high_water();
      }

      pData[0] = LO_UINT16( param1 );
      pData[1] = HI_UINT16( param1 );

      MT_BuildAndSendZToolResponse((MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_APP), cmdId, 2, pData);
      return;
#endif

#if defined ( APP_DEBUG )
    case DEBUG_GET:
      DebugApp_SendQuery( param1 );
      retValue = ZSUCCESS;
      break;
#endif

#if defined ( APP_TP2 )
    case TP_SEND_BCAST_RSP:
      retValue = TestProfileApp_SendBcastRsp( srcEp, (byte)param1 );
      break;
#endif
			
    default:
      break;
  }
#endif // (APP_TGEN) || (NWK_TEST) || (APP_TP) || (APP_TP2) || (OSAL_TOTAL_MEM) || (APP_DEBUG)

  /* Build and send back the response */
  MT_BuildAndSendZToolResponse(((uint8)MT_RPC_CMD_SRSP | (uint8)MT_RPC_SYS_APP), cmdId, 1, &retValue);
}

#endif /* MT_APP_FUNC */

/***************************************************************************************************
 ***************************************************************************************************/
