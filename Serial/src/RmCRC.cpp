//
// Created by ziyu on 11/17/23.
//

#include "RmCRC.h"


unsigned char Get_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength,unsigned char ucCRC8)
{
    unsigned char ucIndex;
    while (dwLength--)
    {
        ucIndex = ucCRC8^(*pchMessage++); ucCRC8 = CRC8_TAB[ucIndex];
    }
    return(ucCRC8);
}

unsigned int Verify_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength)
{
    unsigned char ucExpected = 0; if ((pchMessage == 0) || (dwLength <= 2)) return 0;
    ucExpected = Get_CRC8_Check_Sum(pchMessage, dwLength-1, CRC8_INIT);
    return ( ucExpected == pchMessage[dwLength-1] );
}

void Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength)
{
    unsigned char ucCRC = 0;
    if ((pchMessage == 0) || (dwLength <= 2)) return;
    ucCRC = Get_CRC8_Check_Sum ( (unsigned char *)pchMessage, dwLength-1, CRC8_INIT);
    pchMessage[dwLength-1] = ucCRC;
}

NE_16U Get_CRC16_Check_Sum(NE_8U *pchMessage,NE_32U dwLength,NE_16U wCRC)
{
    NE_8U chData;
    if (pchMessage == nullptr)
    {
        return 0xFFFF;
    }

    while(dwLength--)
    {
        chData = *pchMessage++;
        (wCRC) = ((NE_16U)(wCRC) >> 8) ^ wCRC_Table[((NE_16U)(wCRC) ^ (NE_16U)(chData)) & 0x00ff];
    }
    return wCRC;
}

NE_32U Verify_CRC16_Check_Sum(NE_8U *pchMessage, NE_32U dwLength)
{
    NE_16U wExpected = 0;
    if ((pchMessage == nullptr) || (dwLength <= 2))
    {
        return false;
    }
    wExpected = Get_CRC16_Check_Sum ( pchMessage, dwLength - 2, CRC_INIT);
    return ((wExpected & 0xff) == pchMessage[dwLength - 2] && ((wExpected >> 8) & 0xff) == pchMessage[dwLength - 1]);
}

void Append_CRC16_Check_Sum(NE_8U * pchMessage,NE_32U dwLength)
{
    NE_16U wCRC = 0;
    if ((pchMessage == nullptr) || (dwLength <= 2)) {
        return;
    }
    wCRC = Get_CRC16_Check_Sum((NE_8U *) pchMessage, dwLength - 2, CRC_INIT);
    pchMessage[dwLength - 2] = (NE_8U)(wCRC & 0x00ff);
    pchMessage[dwLength - 1] = (NE_8U)((wCRC >> 8) & 0x00ff);
}