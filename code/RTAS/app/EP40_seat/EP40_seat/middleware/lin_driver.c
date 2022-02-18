#include "lin_driver.h"
#include "lin1.h"
#include "ep40_function_specification.h"

/*  用来保存LIN 驱动状态的表 */
lin_state_t *g_linStatePtr[LIN_COM_NUM] = {NULL}; // LIN_COM_NUM 取决于芯片自身支持的外设数目 一般LIN用两个就够了，再多会影响其他功能
/*  用来保存LIN（串口）状态的表 */
lin_user_config_t *g_linUserconfigPtr[LIN_COM_NUM] = {NULL};

lin_func_callback_t *g_linFuncCallbackPtr[LIN_COM_NUM] = {NULL};

static void LIN_DRV_ProcessFrameHeader(uint32_t instance, uint8_t tmpbyte);

static void LIN_DRV_ProcessSendFrameData(uint32_t instance, uint8_t tmpByte);

static void LIN_DRV_ProcessReceiveFrameData(uint32_t instance, uint8_t tmpByte);

status_t LIN_DRV_Init(uint32_t instance, lin_user_config_t *linUserConfig, lin_state_t *linCurrentState)
{

    DEV_ASSERT(instance < LIN_COM_NUM);

    /* Save runtime structure pointer. */
    g_linStatePtr[instance] = linCurrentState;

    /* Save LIN user config structure pointer. */
    g_linUserconfigPtr[instance] = linUserConfig;

    g_linFuncCallbackPtr[instance] = &lin1_Callback;

    /* Change node's current state to IDLE */
    linCurrentState->currentNodeState = LIN_NODE_STATE_IDLE;

    /* Clear flags in current LIN state structure */
    linCurrentState->isTxBusy = false;
    linCurrentState->isRxBusy = false;
    linCurrentState->isBusBusy = false;
    linCurrentState->isRxBlocking = false;
    linCurrentState->isTxBlocking = false;
    linCurrentState->timeoutCounterFlag = false;
    linCurrentState->timeoutCounter = 0U;

    /****************这部分需要自己实现匹配不同的端口*****************/

    if (instance == 0U)
    {
        // 必备
        // SERCOM3_USART_ReadNotificationEnable(true, true);
        // SERCOM3_USART_ReadThresholdSet(1);
        // NVIC_EnableIRQ(SERCOM3_IRQn);
        // SERCOM3_USART_ReadCallbackRegister(SERCOM3_RX_Handler, 0);
    }

    return STATUS_SUCCESS;
}

/*  重新实现复位 */
void LIN_DRV_Deinit(uint32_t instance)
{
    //TODO:
}

/*FUNCTION**********************************************************************
 *
 * Function Name : LIN_LPUART_DRV_GetCurrentNodeState
 * Description   : This function gets the current LIN node state.
 *
 * Implements    : LIN_LPUART_DRV_GetCurrentNodeState_Activity
 *END**************************************************************************/
lin_node_state_t LIN_DRV_GetCurrentNodeState(uint32_t instance)
{
    /* Assert parameters. */
    DEV_ASSERT(instance < LIN_COM_NUM);

    lin_node_state_t retVal = LIN_NODE_STATE_UNINIT;
    /* Get the current LIN state of this LPUART instance. */
    const lin_state_t *linCurrentState = g_linStatePtr[instance];

    if (linCurrentState != NULL)
    {
        retVal = linCurrentState->currentNodeState;
    }

    /* Return LIN node's current state */
    return retVal;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : LIN_LPUART_DRV_SendWakeupSignal
 * Description   : This function sends a wakeup signal through the LPUART interface.
 *
 * Implements    : LIN_LPUART_DRV_SendWakeupSignal_Activity
 *END**************************************************************************/
status_t LIN_DRV_SendWakeupSignal(uint32_t instance)
{
    /* DEV_ASSERT parameters. */
    DEV_ASSERT(instance < LIN_COM_NUM);

    /* Get the current LIN state of this LPUART instance. */
    const lin_state_t *linCurrentState = g_linStatePtr[instance];
    status_t retVal = STATUS_SUCCESS;

    /* Check if bus is not busy */
    if (linCurrentState->isBusBusy == false)
    {
        /* Send a wakeup signal */
        //LPUART_Putchar(base, s_wakeupSignal[instance]);
        //TODO: 发送唤醒信号
    }
    else
    {
        retVal = STATUS_BUSY;
    }

    return retVal;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : LIN_LPUART_DRV_MasterSendHeader
 * Description   : This function sends frame header out through the LPUART module
 * using a non-blocking method. Non-blocking  means that the function returns
 * immediately. This function sends LIN Break field, sync field then the ID with
 * correct parity. This function checks if the interface is Master, if not, it will
 * return STATUS_ERROR.This function checks if id is in range from 0 to 0x3F, if not
 * it will return STATUS_ERROR. This function also check node's current state is in
 * SLEEP mode then the function will return STATUS_ERROR. And check if isBusBusy is
 * currently true then the function will return STATUS_BUSY.
 *
 * Implements    : LIN_LPUART_DRV_MasterSendHeader_Activity
 *END**************************************************************************/
status_t LIN_DRV_MasterSendHeader(uint32_t instance, uint8_t id)
{
    /* Assert parameters. */
    DEV_ASSERT(instance < LIN_COM_NUM);

    status_t retVal = STATUS_SUCCESS;

    /* Get the current LIN user config structure of this LPUART instance. */
    const lin_user_config_t *linUserConfig = g_linUserconfigPtr[instance];

    /* Get base address of the LPUART instance. */
    //LPUART_Type * base = g_linLpuartBase[instance];

    /* Get the current LIN state of this LPUART instance. */
    lin_state_t *linCurrentState = g_linStatePtr[instance];

    /* Check whether current mode is sleep mode */
    bool checkSleepMode = (LIN_NODE_STATE_SLEEP_MODE == linCurrentState->currentNodeState);

    /* Check if the current node is slave or id is invalid or node's current
     * state is in SLEEP state */
    if ((linUserConfig->nodeFunction == (bool)SLAVE) || (0x3FU < id) || checkSleepMode)
    {
        retVal = STATUS_ERROR;
    }
    else
    {
        /* Check if the LIN bus is busy */
        if (linCurrentState->isBusBusy)
        {
            retVal = STATUS_BUSY;
        }
        else
        {
            linCurrentState->currentId = id;

            /* Make parity for the current ID */
            linCurrentState->currentPid = LIN_DRV_ProcessParity(id, MAKE_PARITY);

            /* Set LIN current state to sending Break field */
            linCurrentState->currentNodeState = LIN_NODE_STATE_SEND_BREAK_FIELD;
            linCurrentState->currentEventId = LIN_NO_EVENT;
            linCurrentState->isBusBusy = true;

            //TODO: 这里需要实现主机发送报头的功能
        }
    }

    return retVal;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : LIN_LPUART_DRV_EnableIRQ
 * Description   : This function enables LPUART hardware interrupts.
 *
 * Implements    : LIN_DRV_EnableIRQ_Activity
 *END**************************************************************************/
status_t LIN_DRV_EnableIRQ(uint32_t instance)
{
    /* Assert parameters. */
    DEV_ASSERT(instance < LPUART_INSTANCE_COUNT);

    status_t retVal = STATUS_SUCCESS;

    //TODO: 实现中断使能功能

    return retVal;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : LIN_LPUART_DRV_GotoIdleState
 * Description   : This function puts current node to Idle state.
 *
 * Implements    : LIN_DRV_GotoIdleState_Activity
 *END**************************************************************************/
status_t LIN_DRV_GotoIdleState(uint32_t instance)
{
    /* Get the current LIN state of this LPUART instance. */
    lin_state_t *linCurrentState = g_linStatePtr[instance];

    linCurrentState->currentEventId = LIN_NO_EVENT;

    //TODO:

    /* Change node's current state to IDLE */
    linCurrentState->currentNodeState = LIN_NODE_STATE_IDLE;

    /* Clear Bus busy Flag */
    linCurrentState->isBusBusy = false;

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : LIN_LPUART_DRV_DisableIRQ
 * Description   : This function disables LPUART hardware interrupts.
 *
 * Implements    : LIN_DRV_DisableIRQ_Activity
 *END**************************************************************************/
status_t LIN_DRV_DisableIRQ(uint32_t instance)
{
    /* Assert parameters. */
    DEV_ASSERT(instance < LPUART_INSTANCE_COUNT);

    status_t retVal = STATUS_SUCCESS;

    /* Get the current LIN state of this LPUART instance. */
    const lin_state_t *linCurrentState = g_linStatePtr[instance];

    if (linCurrentState->currentNodeState == LIN_NODE_STATE_SLEEP_MODE)
    {
    }
    else
    {
    }

    /* Disable LPUART interrupts. */
    //TODO:

    return retVal;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : LIN_LPUART_DRV_GoToSleepMode
 * Description   : This function puts current LIN node to sleep mode.
 * This function changes current node state to LIN_NODE_STATE_SLEEP_MODE.
 *
 * Implements    : LIN_LPUART_DRV_GoToSleepMode_Activity
 *END**************************************************************************/
status_t LIN_DRV_GoToSleepMode(uint32_t instance)
{
    /* Assert parameters. */
    DEV_ASSERT(instance < LPUART_INSTANCE_COUNT);

    /* Get the current LIN state of this LPUART instance. */
    lin_state_t *linCurrentState = g_linStatePtr[instance];

    /* Update node's current state to SLEEP_MODE. */
    linCurrentState->currentNodeState = LIN_NODE_STATE_SLEEP_MODE;
	
    lin_go_to_sleep();

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : LPUART_DRV_AbortTransferData
 * Description   : Aborts an on-going non-blocking transmission/reception.
 * While performing a non-blocking transferring data, users can call this
 * function to terminate immediately the transferring.
 *
 * Implements    : LIN_DRV_AbortTransferData_Activity
 *END**************************************************************************/
status_t LIN_DRV_AbortTransferData(uint32_t instance)
{
    /* Assert parameters. */
    DEV_ASSERT(instance < LPUART_INSTANCE_COUNT);

    status_t retVal = STATUS_SUCCESS;

    /* Get the current LIN state of this LPUART instance. */
    lin_state_t *linCurrentState = g_linStatePtr[instance];

    /* Change node's current state to IDLE */
    (void)LIN_DRV_GotoIdleState(instance);

    /* Clear LIN Tx and Rx Busy flag */
    linCurrentState->isTxBusy = false;
    linCurrentState->isRxBusy = false;

    return retVal;
}

lin_callback_t LIN_DRV_InstallCallback(uint32_t instance,
                                       lin_callback_t function)
{
    /* Assert parameters. */
    DEV_ASSERT(instance < LPUART_INSTANCE_COUNT);

    /* Get the current LIN state of this LPUART instance. */
    lin_state_t *linCurrentState = g_linStatePtr[instance];

    /* Get the current callback function. */
    lin_callback_t currentCallback = linCurrentState->Callback;

    /* Install new callback function. */
    linCurrentState->Callback = function;

    return currentCallback;
}

/*  参数instance为串口标号，取决于配置的顺序
*/

void LIN_DRV_IRQHandler(uint32_t instance, int8_t event)
{
    /* 一下功能实现只针对AVRxxDA系列8位单片机 */
    uint8_t temp_buf;
    /************/
    const lin_user_config_t *linUserConfig = g_linUserconfigPtr[instance];
    lin_state_t *linCurrentState = g_linStatePtr[instance];
    const lin_func_callback_t *linFuncCallback = g_linFuncCallbackPtr[instance];

    if (event == 1) // 检测到PID
    {
        //AVR系列单片机特殊的地方 BREAK SYNC PID 一起识别触发
        if (linUserConfig->nodeFunction == (bool)MASTER)
        {
            //TODO:主机模式后续更新，AVR 8位机无法实现主机模式
        }
        else
        {
            /* Set flag LIN bus busy */
            linCurrentState->isBusBusy = true;
            /* Change the node's current state to RECEIVING PID */
            linCurrentState->currentEventId = LIN_SYNC_OK;
            /* Change the node's current state to RECEIVING PID */
            linCurrentState->currentNodeState = LIN_NODE_STATE_RECV_PID;
            /* 直接读取data缓冲区数据，存放得当前PID */
            linFuncCallback->ReadByte(&temp_buf);
            /* 判断PID是否合理，有错误 */
            LIN_DRV_ProcessFrameHeader(instance, temp_buf);
            /* Callback function */ //不需要回调
            // if(linCurrentState->Callback != NULL)
            // {
            //     linCurrentState->Callback(instance, linCurrentState);
            // }
        }
    }
    else if (event == 0) //处理帧数据
    {
        /* Check node's current state */
        switch (linCurrentState->currentNodeState)
        {
        /* if current state is RECEIVE SYNC FIELD */
        case LIN_NODE_STATE_RECV_SYNC:

        /* if current state is MASTER SENDING PID */
        case LIN_NODE_STATE_SEND_PID:

        /* if current state is RECEIVE PID */
        case LIN_NODE_STATE_RECV_PID:
            break;
        /* if current state is RECEIVE DATA */
        case LIN_NODE_STATE_RECV_DATA:
            linFuncCallback->ReadByte(&temp_buf);
            LIN_DRV_ProcessReceiveFrameData(instance, temp_buf);
            break;
        /* if current state is SENDING DATA */
        case LIN_NODE_STATE_SEND_DATA:
            /* 发送时会回显数据 读上次发送的回显数据 */
            linFuncCallback->ReadByte(&temp_buf);
            LIN_DRV_ProcessSendFrameData(instance, temp_buf);
            break;

        default:
            /* Other node state */
            linFuncCallback->ReadByte(&temp_buf);
            break;
        }
    }
    else if (event == -1) //报错
    {
        // TODO: break 或者 sync 报错
        // USART_ERROR err = USART_ERROR_NONE;
        // err = linFuncCallback->GetError();
        // if(err == SERCOM_USART_INT_STATUS_ISF_Msk)  // SYNC错误，0x55有问题
        // {
        //     linCurrentState->currentEventId = LIN_SYNC_ERROR;
        //     if(linCurrentState->Callback != NULL)
        //     {
        //         linCurrentState->Callback(instance, linCurrentState);
        //     }
        //     (void)LIN_DRV_GotoIdleState(instance);
        // }
        // else if(err == USART_ERROR_FRAMING)
        // {

        // }
        // AVRXXDA系列的型号
        linCurrentState->currentEventId = LIN_SYNC_ERROR;
        if (linCurrentState->Callback != NULL)
        {
            linCurrentState->Callback(instance, linCurrentState);
        }
        (void)LIN_DRV_GotoIdleState(instance);
    }
    else if (event == -2)
    {
        linCurrentState->currentEventId = LIN_READBACK_ERROR;
        if (linCurrentState->Callback != NULL)
        {
            linCurrentState->Callback(instance, linCurrentState);
        }

                /* Check if the transmission is non-blocking */
        if (linCurrentState->isTxBlocking == false)
        {
            /* Clear Tx busy flag */
            linCurrentState->isTxBusy = false;

            /* Change node's current state to IDLE */
            (void)LIN_DRV_GotoIdleState(instance);
        }
    }
}

static void LIN_DRV_ProcessFrameHeader(uint32_t instance, uint8_t tmpbyte)
{
    /* Get the current LIN user config structure of this LPUART instance. */
    const lin_user_config_t *linUserConfig = g_linUserconfigPtr[instance];

    /* Get the current LIN state of this LPUART instance. */
    lin_state_t *linCurrentState = g_linStatePtr[instance];

    switch (linCurrentState->currentNodeState)
    {
    case LIN_NODE_STATE_RECV_SYNC:
        break;
    case LIN_NODE_STATE_SEND_PID:
        break;
    case LIN_NODE_STATE_RECV_PID:
        /* If the node is MASTER */
        if (linUserConfig->nodeFunction == (bool)MASTER)
        {
            //TODO: 实现主机的方法
        }
        /* If the node is SLAVE */
        else
        {
            linCurrentState->currentId = LIN_DRV_ProcessParity(tmpbyte, CHECK_PARITY);
            linCurrentState->currentPid = tmpbyte;
            if (linCurrentState->currentId != 0xFFU)
            {
                /* Set current event ID to PID correct */
                linCurrentState->currentEventId = LIN_PID_OK;
                if (linCurrentState->isRxBlocking == true)
                {
                    /* Starting receive data blocking */
                    linCurrentState->currentNodeState = LIN_NODE_STATE_RECV_DATA;
                    linCurrentState->isBusBusy = true;
                    linCurrentState->isRxBusy = true;
                }
                else
                {
                    linCurrentState->isBusBusy = false;

                    if (linCurrentState->Callback != NULL)
                    {
                        linCurrentState->Callback(instance, linCurrentState);
                    }
                }
            }
            else
            {
                linCurrentState->currentEventId = LIN_PID_ERROR;
                linCurrentState->Callback(instance, linCurrentState);
            }
        }
        break;
    default:
        break;
    }
}

static uint8_t LIN_COM_DRV_MakeChecksumByte(uint32_t instance,
                                            const uint8_t *buffer,
                                            uint8_t sizeBuffer,
                                            uint8_t PID)
{
    /* Get list of PIDs use classic checksum. */
    const uint8_t *classicPID = g_linUserconfigPtr[instance]->classicPID;
    const uint8_t numOfClassicPID = g_linUserconfigPtr[instance]->numOfClassicPID;
    uint8_t checkSum = PID;
    uint8_t retVal = 0U;

    if (numOfClassicPID == 255U)
    {
        /*all frame use enhanced checksum */
        checkSum = 0U;
    }
    else
    {
        if (classicPID != NULL)
        {
            for (retVal = 0U; retVal < numOfClassicPID; retVal++)
            {
                if (checkSum == classicPID[retVal])
                {
                    checkSum = 0U;
                    break;
                }
            }
        }
    }
    retVal = LIN_DRV_MakeChecksumByte(buffer, sizeBuffer, checkSum);
    return retVal;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : LIN_LPUART_DRV_SendFrameData
 * Description   : This function sends data out through the LPUART module using
 * non-blocking method. This function will calculate the checksum byte and send
 * it with the frame data. The function will return immediately after calling
 * this function. If txSize is equal to 0 or greater than 8  or node's current
 * state is in SLEEP mode then the function will return STATUS_ERROR. If
 * isBusBusy is currently true then the function will return STATUS_BUSY.
 * 非阻塞模式下发送数据,该函数会计算校验字节，并发送帧数据，立即返回
 * Implements    : LIN_LPUART_DRV_SendFrameData_Activity
 * */
status_t LIN_DRV_SendFrameData(uint32_t instance, const uint8_t *txBuff, uint8_t txSize)
{
    /* Assert parameters. */
    DEV_ASSERT(txBuff != NULL);
    DEV_ASSERT(instance < LIN_COM_NUM);

    status_t retVal = STATUS_SUCCESS;

    /* Get the current LIN state of instance. */
    lin_state_t *linCurrentState = g_linStatePtr[instance];

    const lin_func_callback_t *linFuncCallback = g_linFuncCallbackPtr[instance];

    /* 判断节点是否处于睡眠状态 */
    bool checkSleepMode = (LIN_NODE_STATE_SLEEP_MODE == linCurrentState->currentNodeState);

    /* Check if txSize > 8 or equal to 0 or node's current state
     * is in SLEEP mode then return STATUS_ERROR */
    if ((8U < txSize) || (0U == txSize) || checkSleepMode)
    {
        retVal = STATUS_ERROR;
    }
    else
    {
        /* Check if the LIN Bus is busy */
        if (linCurrentState->isBusBusy)
        {
            retVal = STATUS_BUSY;
        }
        else
        {
            /* Make the checksum byte. */
            linCurrentState->checkSum = LIN_COM_DRV_MakeChecksumByte(instance, txBuff, txSize, linCurrentState->currentPid);

            /* Update the LIN state structure. */
            linCurrentState->txBuff = txBuff;
            /* Add a place for checksum byte */
            linCurrentState->txSize = (uint8_t)(txSize + 1U);
            linCurrentState->cntByte = 0U;
            linCurrentState->currentNodeState = LIN_NODE_STATE_SEND_DATA;
            linCurrentState->currentEventId = LIN_NO_EVENT;
            linCurrentState->isBusBusy = true;
            linCurrentState->isTxBusy = true;

            /* Set Break char detect length as 10 bits minimum */
            //LPUART_SetBreakCharDetectLength(base, LPUART_BREAK_CHAR_10_BIT_MINIMUM);

            /* Start sending data */
            linFuncCallback->SendByte((uint8_t *)linCurrentState->txBuff);
        }
    }

    return retVal;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : LIN_LPUART_DRV_ProcessSendFrameData
 * Description   : Part of Interrupt handler for sending data.
 *
 * Implements    : LIN_LPUART_DRV_ProcessSendFrameData_Activity
 *END**************************************************************************/
static void LIN_DRV_ProcessSendFrameData(uint32_t instance,
                                         uint8_t tmpByte)
{
    bool sendFlag = true;
    uint8_t tmpSize;
    bool tmpCheckSumAndSize;
    bool tmpBuffAndSize;

    /* Get the current LIN state of this LPUART instance. */
    lin_state_t *linCurrentState = g_linStatePtr[instance];

    const lin_func_callback_t *linFuncCallback = g_linFuncCallbackPtr[instance];

    // /* Check if Tx data register empty flag is false */
    // if (LPUART_GetStatusFlag(base, LPUART_TX_DATA_REG_EMPTY) == false)
    // {
    //     linCurrentState->currentEventId = LIN_READBACK_ERROR;
    //     /* callback function to handle Readback error */
    //     if (linCurrentState->Callback != NULL)
    //     {
    //         linCurrentState->Callback(instance, linCurrentState);
    //     }

    //     /* Check if the transmission is non-blocking */
    //     if (linCurrentState->isTxBlocking == false)
    //     {
    //         /* Clear Tx busy flag */
    //         linCurrentState->isTxBusy = false;

    //         /* Change node's current state to IDLE */
    //         (void)LIN_LPUART_DRV_GotoIdleState(instance);
    //     }

    //     sendFlag = false;
    // }
    // else
    // {
    tmpSize = (uint8_t)(linCurrentState->txSize - linCurrentState->cntByte);
    tmpCheckSumAndSize = (tmpSize == 1U) && (linCurrentState->checkSum != tmpByte);
    tmpBuffAndSize = (*linCurrentState->txBuff != tmpByte) && (tmpSize != 1U);
    if (tmpBuffAndSize || tmpCheckSumAndSize)
    {
        linCurrentState->currentEventId = LIN_READBACK_ERROR;

        /* callback function to handle Readback error */
        if (linCurrentState->Callback != NULL)
        {
            linCurrentState->Callback(instance, linCurrentState);
        }

        /* Check if the transmission is non-blocking */
        if (linCurrentState->isTxBlocking == false)
        {
            /* Clear Tx busy flag */
            linCurrentState->isTxBusy = false;

            /* Change node's current state to IDLE */
            (void)LIN_DRV_GotoIdleState(instance);
        }

        sendFlag = false;
    }
    else
    {
        linCurrentState->txBuff++;
        linCurrentState->cntByte++;
    }
    // }

    if (sendFlag)
    {
        if (linCurrentState->cntByte < linCurrentState->txSize)
        {
            /* Send checksum byte */
            if ((linCurrentState->txSize - linCurrentState->cntByte) == 1U)
            {
                linFuncCallback->SendByte(&linCurrentState->checkSum);
                //LPUART_Putchar(base, linCurrentState->checkSum);
            }
            /* Send data bytes */
            else
            {
                linFuncCallback->SendByte((uint8_t *)linCurrentState->txBuff);
                //LPUART_Putchar(base, *linCurrentState->txBuff);
            }
        }
        else //发送完成
        {
            linCurrentState->currentEventId = LIN_TX_COMPLETED;
            linCurrentState->currentNodeState = LIN_NODE_STATE_SEND_DATA_COMPLETED;

            //LPUART_SetIntMode(base, LPUART_INT_RX_DATA_REG_FULL, false);
            /* callback function to handle event TX COMPLETED */
            if (linCurrentState->Callback != NULL)
            {
                linCurrentState->Callback(instance, linCurrentState);
            }

            /* Check if the transmission is non-blocking */
            if (linCurrentState->isTxBlocking == false)
            {
                /* Clear Tx busy flag */
                linCurrentState->isTxBusy = false;

                /* Change node's current state to IDLE 更改节点状态为空闲状态*/
                linCurrentState->currentEventId = LIN_NO_EVENT;
                linCurrentState->currentNodeState = LIN_NODE_STATE_IDLE;
                linCurrentState->isBusBusy = false;
            }
            else
            {
                //阻塞模式下处理
                /* Post Semaphore to signal Tx Completed*/
                //(void)OSIF_SemaPost(&linCurrentState->txCompleted);
            }
			//TODO:
            ep40_lin_signal_update_while_task();
        }
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : LIN_LPUART_DRV_RecvFrmData
 * Description   : This function receives data from LPUART module using
 * non-blocking method. This function returns immediately after initiating the
 * receive function. The application has to get the receive status to see when
 * the receive is complete. In other words, after calling non-blocking get
 * function, the application must get the receive status to check if receive
 * is completed or not. The interrupt handler LIN_LPUART_DRV_IRQHandler will check
 * the checksum byte. If the checksum is correct, it will receive the frame data.
 * If the checksum is incorrect, this function will return STATUS_TIMEOUT and data in
 * rxBuff might be wrong. This function also check if rxSize is in range from 1 to 8.
 * If not, it will return STATUS_ERROR. This function also returns STATUS_ERROR if
 * node's current state is in SLEEP mode. This function checks if the
 * isBusBusy is false, if not it will return STATUS_BUSY.
 *
 * Implements    : LIN_LPUART_DRV_RecvFrmData_Activity
 *END**************************************************************************/
status_t LIN_DRV_ReceiveFrameData(uint32_t instance, uint8_t *rxBuff, uint8_t rxSize)
{
    /* Assert parameters. */
    DEV_ASSERT(rxBuff != NULL);
    DEV_ASSERT(instance < LPUART_INSTANCE_COUNT);

    status_t retVal = STATUS_SUCCESS;

    /* Get the current LIN state of this LPUART instance. */
    lin_state_t *linCurrentState = g_linStatePtr[instance];

    /* Check whether current mode is sleep mode */
    bool checkSleepMode = (LIN_NODE_STATE_SLEEP_MODE == linCurrentState->currentNodeState);

    /* Check if rxSize > 8 or equal to 0 or node's current state
     * is in SLEEP mode then return STATUS_ERROR */
    if ((8U < rxSize) || (0U == rxSize) || checkSleepMode)
    {
        retVal = STATUS_ERROR;
    }
    else
    {
        /* Check if the LIN Bus is busy */
        if (linCurrentState->isBusBusy)
        {
            retVal = STATUS_BUSY;
        }
        else
        {
            /* Update the LIN state structure. */
            linCurrentState->rxBuff = rxBuff;
            /* Add a place for checksum byte */
            linCurrentState->rxSize = (uint8_t)(rxSize + 1U);
            linCurrentState->cntByte = 0U;

            /* Start receiving data */
            linCurrentState->currentNodeState = LIN_NODE_STATE_RECV_DATA;
            linCurrentState->currentEventId = LIN_NO_EVENT;
            linCurrentState->isBusBusy = true;
            linCurrentState->isRxBusy = true;
            linCurrentState->isRxBlocking = false;
        }
    }

    return retVal;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : LIN_LPUART_DRV_ProcessReceiveFrameData
 * Description   : Part of Interrupt handler for receiving.
 *
 * Implements    : LIN_DRV_ProcessReceiveFrameData_Activity
 *END**************************************************************************/
static void LIN_DRV_ProcessReceiveFrameData(uint32_t instance, uint8_t tmpByte)
{
    /* Get the current LIN state of this LPUART instance. */
    lin_state_t *linCurrentState = g_linStatePtr[instance];

    if (linCurrentState->rxSize > (linCurrentState->cntByte + 1U))
    {
        *(linCurrentState->rxBuff) = tmpByte;
        linCurrentState->rxBuff++;
    }
    else
    {
        if ((linCurrentState->rxSize - linCurrentState->cntByte) == 1U)
        {
            linCurrentState->checkSum = tmpByte;
        }
    }

    linCurrentState->cntByte++;
    if (linCurrentState->cntByte == linCurrentState->rxSize)
    {
        /* Restore rxBuffer pointer */
        linCurrentState->rxBuff -= linCurrentState->rxSize - 1U;
        if (LIN_COM_DRV_MakeChecksumByte(instance, linCurrentState->rxBuff, linCurrentState->rxSize - 1U, linCurrentState->currentPid) == linCurrentState->checkSum)
        {
            linCurrentState->currentEventId = LIN_RX_COMPLETED;
            linCurrentState->currentNodeState = LIN_NODE_STATE_RECV_DATA_COMPLETED;

            /* callback function to handle RX COMPLETED */
            if (linCurrentState->Callback != NULL)
            {
                linCurrentState->Callback(instance, linCurrentState);
            }

            /* Check if the reception is non-blocking */
            if (linCurrentState->isRxBlocking == false)
            {
                /* Clear Bus busy flag */
                linCurrentState->isBusBusy = false;

                /* Clear Rx busy flag */
                linCurrentState->isRxBusy = false;

                /* In case of receiving a go to sleep request, after callback, node is in SLEEP MODE */
                /* In this case, node is in SLEEP MODE state */
                if (linCurrentState->currentNodeState != LIN_NODE_STATE_SLEEP_MODE)
                {
                    (void)LIN_DRV_GotoIdleState(instance);
                }
            }
            else
            {
                /* Post Semaphore to signal Rx Completed*/
                //(void)OSIF_SemaPost(&linCurrentState->rxCompleted);
            }
        }
        else
        {
            linCurrentState->currentEventId = LIN_CHECKSUM_ERROR;
            /* callback function to handle checksum error */
            if (linCurrentState->Callback != NULL)
            {
                linCurrentState->Callback(instance, linCurrentState);
            }

            /* Clear Rx busy flag */
            linCurrentState->isRxBusy = false;

            /* Change node's current state to IDLE */
            (void)LIN_DRV_GotoIdleState(instance);
        }
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : LIN_LPUART_DRV_TimeoutService
 * Description   : This is callback function for Timer Interrupt Handler.
 * Users shall initialize a timer (for example FTM) in Output compare mode
 * with period of about 500 micro seconds. In timer IRQ handler, call this function.
 *
 * Implements    : LIN_DRV_TimeoutService_Activity
 * 
 * 需要初始化一个定时器,每0.5ms需要回调一次该函数,计数没有实现,AVR单片机容易导致卡死
 *END**************************************************************************/
void LIN_DRV_TimeoutService(uint32_t instance)
{
    /* Assert parameters. */
    DEV_ASSERT(instance < LPUART_INSTANCE_COUNT);

    /* Get the current LIN state of this LPUART instance. */
    lin_state_t *linCurrentState = g_linStatePtr[instance];

    /* Get LIN node's current state */
    lin_node_state_t state = linCurrentState->currentNodeState;

    switch (state)
    {
    /* If the node is SENDING DATA */
    case LIN_NODE_STATE_SEND_DATA:
        /* Check if timeout Counter is 0 */
        if (linCurrentState->timeoutCounter == 0U)
        {
            /* Set timeout Counter flag */
            linCurrentState->timeoutCounterFlag = true;

            if (linCurrentState->isTxBlocking == false)
            {
                /* Callback to handle timeout Counter flag */
                if (linCurrentState->Callback != NULL)
                {
                    linCurrentState->Callback(instance, linCurrentState);
                }

                /* Clear Tx busy flag */
                linCurrentState->isTxBusy = false;

                /* Change the node's current state to IDLE */
                (void)LIN_DRV_GotoIdleState(instance);
            }
        }
        else /* If timeout Counter is not 0, then decrease timeout Counter by one */
        {
            linCurrentState->timeoutCounter--;
        }

        break;
    /* If the node is RECEIVING DATA */
    case LIN_NODE_STATE_RECV_DATA:
        /* Check if timeout Counter is 0 */
        if (linCurrentState->timeoutCounter == 0U)
        {
            /* Set timeout Counter flag */
            linCurrentState->timeoutCounterFlag = true;

            /* Check if the reception is non-blocking */
            if (linCurrentState->isRxBlocking == false)
            {
                /* Callback to handle timeout Counter flag */
                if (linCurrentState->Callback != NULL)
                {
                    linCurrentState->Callback(instance, linCurrentState);
                }

                /* Clear Rx busy flag */
                linCurrentState->isRxBusy = false;

                /* Change the node's current state to IDLE */
                (void)LIN_DRV_GotoIdleState(instance);
            }
        }
        /* If timeout Counter is not 0, then decrease timeout Counter by one */
        else
        {
            linCurrentState->timeoutCounter--;
        }
        break;
    default:
        /* The node state is not SENDING nor RECEIVING data */
        break;
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : LIN_DRV_SetTimeoutCounter
 * Description   : This function sets value for timeout counter that is used in
 * LIN_DRV_TimeoutService
 *
 * Implements    : LIN_DRV_SetTimeoutCounter_Activity
 *END**************************************************************************/
void LIN_DRV_SetTimeoutCounter(uint32_t instance, uint32_t timeoutValue)
{
    /* Assert parameters. */
    //DEV_ASSERT(instance < LPUART_INSTANCE_COUNT);

    /* Get the current LIN state of this LPUART instance. */
    lin_state_t *linCurrentState = g_linStatePtr[instance];

    /* Clear Timeout Counter Flag */
    linCurrentState->timeoutCounterFlag = false;

    /* Set new value for Timeout Counter */
    linCurrentState->timeoutCounter = timeoutValue;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : LIN_LPUART_DRV_GetReceiveStatus
 * Description   : This function returns whether the previous LPUART reception is
 * complete. When performing a non-blocking receive, the user can call this
 * function to ascertain the state of the current receive progress: in progress
 * or complete. In addition, if the reception is still in progress, the user can
 * obtain the number of words that is still needed to receive.
 *
 * Implements    : LIN_DRV_GetReceiveStatus_Activity
 *END**************************************************************************/
status_t LIN_DRV_GetReceiveStatus(uint32_t instance, uint8_t *bytesRemaining)
{
    /* Assert parameters. */
    DEV_ASSERT(instance < LPUART_INSTANCE_COUNT);

    status_t retVal = STATUS_SUCCESS;

    /* Get the current LIN state of this LPUART instance. */
    const lin_state_t *linCurrentState = g_linStatePtr[instance];

    /* Get the number of bytes that is still needed to receive */
    *bytesRemaining = (uint8_t)(linCurrentState->rxSize - linCurrentState->cntByte);

    /* Return status of the on-going reception */
    if ((linCurrentState->currentEventId == LIN_NO_EVENT) && (*bytesRemaining != 0U))
    {
        if (linCurrentState->timeoutCounterFlag == false)
        {
            retVal = STATUS_BUSY;
        }
        else
        {
            retVal = STATUS_TIMEOUT;
        }
    }

    return retVal;
}