/*
 * Copyright (c) 2015-2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */
/*!
 * @file lin_diagnostic_service.c
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 1.3,  Taking address of near auto variable.
 * The code is not dynamically linked. An absolute stack address is obtained
 * when taking the address of the near auto variable.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.7, Could be made static.
 * Functions are APIs, so they shall not be made static.
 */

#include "../include/lin_commontl_api.h"
#include "../include/lin_diagnostic_service.h"
#include "app_config.h"

#if (1U == SUPPORT_TRANSPORT_LAYER)
#if (1U == SUPPORT_DIAG_SERVICE)
#if (1U == SUPPORT_SLAVE_MODE)
/*******************************************************************************
 * Static function prototypes
 ******************************************************************************/
#if ((1U == SUPPORT_PROTOCOL_21) || (1U == SUPPORT_PROTOCOL_20))

#if !SUPPORT_BOOTLOADER
static void lin_assign_nad(l_ifc_handle iii);
static void lin_condittional_change_nad(l_ifc_handle iii);


#if (1U == SUPPORT_PROTOCOL_21)
static void lin_diagservice_assign_frame_id_range(l_ifc_handle iii);
#endif /* (1U == SUPPORT_PROTOCOL_21) */


#endif
#endif /* ((1U == SUPPORT_PROTOCOL_21) || (1U == SUPPORT_PROTOCOL_20)) */

#if !SUPPORT_BOOTLOADER
static void lin_diagservice_read_by_identifier(l_ifc_handle iii);
#endif

static void ld_make_slave_response_pdu(l_ifc_handle iii,
                                       l_u8 sid,
                                       l_u8 res_type,
                                       l_u8 error_code);

#if ((1U == SUPPORT_PROTOCOL_J2602) || (1U == SUPPORT_PROTOCOL_20))
static l_bool ld_change_msg_id(l_ifc_handle iii,
                               l_u8 dnn,
                               l_u8 frame_id_change);

static void lin_diagservice_assign_frame_id(l_ifc_handle iii);

#if (1U == SUPPORT_PROTOCOL_J2602)
static void lin_diagservice_target_reset(l_ifc_handle iii);
#endif /* (1U == SUPPORT_PROTOCOL_J2602) */

#endif /* ((1U == SUPPORT_PROTOCOL_J2602) || (1U == SUPPORT_PROTOCOL_20)) */

#endif /* (1U == SUPPORT_SLAVE_MODE) */
/*******************************************************************************
 * Code
 ******************************************************************************/
#if ((1U == SUPPORT_PROTOCOL_21) || (1U == SUPPORT_PROTOCOL_20))
#if (1U == SUPPORT_MASTER_MODE)
/*FUNCTION**********************************************************************
 *
 * Function Name : ld_is_ready
 * Description   : This call returns the status of the last requested configuration service
 *
 * Implements    : ld_is_ready_Activity
 *END**************************************************************************/
l_u8 ld_is_ready(l_ifc_handle iii)
{
    DEV_ASSERT((l_u8)iii < LIN_NUM_OF_IFCS);

    l_u8 ret_val;
    if (g_lin_protocol_user_cfg_array[iii].function == (bool)LIN_MASTER)
    {
        ret_val = (l_u8)(g_lin_tl_descriptor_array[iii].service_status);
    }
    else
    {
        ret_val = 0xFF;
    }

    return ret_val;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ld_check_response
 * Description   : This call returns the result of the last node configuration service, in the
 * parameters RSID and error_code. A value in RSID is always returned but not always in the
 * error_code. Default values for RSID and error_code is 0 (zero).
 *
 * Implements    : ld_check_response_Activity
 *END**************************************************************************/
void ld_check_response(l_ifc_handle iii,
                       l_u8 * const RSID,
                       l_u8 * const error_code)
{
    DEV_ASSERT((l_u8)iii < LIN_NUM_OF_IFCS);
    DEV_ASSERT(RSID != NULL);
    DEV_ASSERT(error_code != NULL);
    const lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];

    if (g_lin_protocol_user_cfg_array[iii].function == (bool)LIN_MASTER)
    {
        /* Get last response service identifier */
        *RSID = tl_desc_ptr->last_RSID;
        /* Set error_code to the default value - zero */
        *error_code = 0;
        /* Get the error code of the last config service if it is negative response */
        if (LD_NEGATIVE == (tl_desc_ptr->last_cfg_result))
        {
            *error_code = tl_desc_ptr->ld_error_code;
        }
    }
}

#if (1U == SUPPORT_PROTOCOL_21)
/*FUNCTION**********************************************************************
 *
 * Function Name : ld_assign_frame_id_range
 * Description   : This function assigns the protected identifier of up to four frames
 *
 * Implements    : ld_assign_frame_id_range_Activity
 *END**************************************************************************/
void ld_assign_frame_id_range(l_ifc_handle iii,
                              l_u8 NAD,
                              l_u8 start_index,
                              const l_u8 * const PIDs)
{
    DEV_ASSERT((l_u8)iii < LIN_NUM_OF_IFCS);
    DEV_ASSERT(PIDs != NULL);
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    l_u8 buff[6];

    /* Check if this interface is a LIN Master */
    if (g_lin_protocol_user_cfg_array[iii].function == (bool)LIN_MASTER)
    {
        if (tl_desc_ptr->service_status != LD_SERVICE_BUSY)
        {
            /* pack data into a single frame */
            /* Buffer[0] SID of Service Assign Frame ID Range: 0xB7 */
            buff[0] = SERVICE_ASSIGN_FRAME_ID_RANGE;
            buff[1] = start_index;
            buff[2] = PIDs[0];
            buff[3] = PIDs[1];
            buff[4] = PIDs[2];
            buff[5] = PIDs[3];

            /* put data into TX_QUEUE */
            ld_send_message(iii, 6U, NAD, buff);

            /* set node config status to busy */
            tl_desc_ptr->service_status = LD_SERVICE_BUSY;
        } /* End of checking service status */
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ld_save_configuration
 * Description   : This function is for Master nodes only. This call will make a
 * save configuration request to a specific slave node with the given
 * NAD, or to all slave nodes if NAD is set to broadcast.
 *
 * Implements    : ld_save_configuration_Activity
 *END**************************************************************************/
void ld_save_configuration(l_ifc_handle iii,
                           l_u8 NAD)
{
    DEV_ASSERT((l_u8)iii < LIN_NUM_OF_IFCS);

    l_u8 data[6];
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];

    /* Check if this interface is a LIN Master */
    if (g_lin_protocol_user_cfg_array[iii].function == (bool)LIN_MASTER)
    {
        /* check service is busy? */
        if (tl_desc_ptr->service_status != LD_SERVICE_BUSY)
        {
            data[0] = SERVICE_SAVE_CONFIGURATION;
            data[1] = 0xFFU;
            data[2] = 0xFFU;
            data[3] = 0xFFU;
            data[4] = 0xFFU;
            data[5] = 0xFFU;

            /* put data into TX_QUEUE */
            ld_send_message(iii, PCI_SAVE_CONFIGURATION, NAD, data);

            /* set node config status to busy */
            tl_desc_ptr->service_status = LD_SERVICE_BUSY;
        } /* End of checking service status */
    }
}
#endif /* (1U == SUPPORT_PROTOCOL_21) */

#endif /* (1U == SUPPORT_MASTER_MODE) */

#if (1U == SUPPORT_SLAVE_MODE)
#if !SUPPORT_BOOTLOADER
/*FUNCTION**********************************************************************
 *
 * Function Name : lin_condittional_change_nad
 * Description   : Process Conditional Change NAD request
 *
 * Implements    : lin_condittional_change_nad_Activity
 *END**************************************************************************/
static void lin_condittional_change_nad(l_ifc_handle iii)
{
    l_u8 id;
    l_u8 byte;
    l_u8 mask;
    l_u8 invert;
    l_bool give_positive_flg = (bool)0U;
    const lin_node_attribute_t * node_attr_ptr = &g_lin_node_attribute_array[g_lin_protocol_user_cfg_array[iii].slave_ifc_handle];
    const lin_transport_layer_queue_t * rx_queue;
    lin_product_id_t product_id = node_attr_ptr->product_id;
    lin_serial_number_t serial_number = node_attr_ptr->serial_number;

    /* Get receive queue */
    rx_queue = &(g_lin_tl_descriptor_array[iii].tl_rx_queue);
    id = rx_queue->tl_pdu_ptr[rx_queue->queue_header][3];
    byte = rx_queue->tl_pdu_ptr[rx_queue->queue_header][4];
    mask = rx_queue->tl_pdu_ptr[rx_queue->queue_header][5];
    invert = rx_queue->tl_pdu_ptr[rx_queue->queue_header][6];

    switch (id)
    {
        case LIN_PRODUCT_ID:
            if ((byte > 0U) && (byte < 6U))
            {
                /* Byte 1: Supplier ID LSB; Byte 2: Supplier ID MSB */
                if ((byte > 0U) && (byte < 3U))
                {
                    byte = (l_u8)(product_id.supplier_id >> ((byte - 1U) * 8U));
                }
                /* Byte 3: Function ID LSB; Byte 4: Function ID MSB */
                else if ((byte > 2U) && (byte < 5U))
                {
                    byte = (l_u8)(product_id.function_id >> ((byte - 3U) * 8U));
                }
                /* Byte 5: Variant */
                else
                {
                    byte = product_id.variant;
                }

                /* Do a bitwise XOR with Invert and Do a bitwise AND with Mask */
                byte = (l_u8)((byte ^ invert) & mask);
                if (byte == 0U)
                {
                    give_positive_flg = (l_bool)1U;
                }
            }

            break;
        case LIN_SERIAL_NUMBER:
            if ((byte > 0U) && (byte < 5U))
            {
                switch (byte)
                {
                    case 1U:
                        byte = serial_number.serial_0;
                        break;
                    case 2U:
                        byte = serial_number.serial_1;
                        break;
                    case 3U:
                        byte = serial_number.serial_2;
                        break;
                    case 4U:
                        byte = serial_number.serial_3;
                        break;
                    default:
                        /* Do nothing */
                        break;
                }

                /* Do a bitwise XOR with Invert and Do a bitwise AND with Mask */
                byte = (l_u8)((byte ^ invert) & mask);
                if (byte == 0U)
                {
                    give_positive_flg = (l_bool)1U;
                }
            }

            break;
        default:
            /* Do nothing */
            break;
    }

    if (give_positive_flg == (bool)1U)
    {
        /* Make response PDU before change configuration NAD */
        ld_make_slave_response_pdu(iii, SERVICE_CONDITIONAL_CHANGE_NAD, POSITIVE, 0U);
        /* If the final result is zero then change the NAD to New NAD */
        *node_attr_ptr->configured_NAD_ptr = rx_queue->tl_pdu_ptr[rx_queue->queue_header][7];
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : lin_assign_nad
 * Description   : Process lin assign nad request
 *
 * Implements    : lin_assign_nad_Activity
 *END**************************************************************************/
static void lin_assign_nad(l_ifc_handle iii)
{
    lin_tl_pdu_data_t lin_tl_pdu;
    l_u16 supid;
    l_u16 fid;
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];
    lin_product_id_t product_id = g_lin_node_attribute_array[prot_user_config_ptr->slave_ifc_handle].product_id;
    const lin_transport_layer_queue_t * rx_queue = &(tl_desc_ptr->tl_rx_queue);
    l_u8 i;
    for (i = 0; i < 8U; i++)
    {
        lin_tl_pdu[i] = rx_queue->tl_pdu_ptr[rx_queue->queue_header][i];
    }

    /* Get supplier and function identification in request */
    supid = (l_u16)(((l_u16)lin_tl_pdu[4]) << 8U);
    supid = (l_u16)(supid | ((l_u16)lin_tl_pdu[3]));

    fid = (l_u16)(((l_u16)lin_tl_pdu[6]) << 8U);
    fid = (l_u16)(fid | ((l_u16)lin_tl_pdu[5]));

    /* Check Supplier ID and Function ID */
    if (((supid != product_id.supplier_id) && (supid != LD_ANY_SUPPLIER)) ||
        ((fid != product_id.function_id) && (fid != LD_ANY_FUNCTION)))
    {
        tl_desc_ptr->slave_resp_cnt = 0U;
    }
    else
    {
        ld_make_slave_response_pdu(iii, SERVICE_ASSIGN_NAD, POSITIVE, 0U);
    }
}
#endif

#if (1U == SUPPORT_PROTOCOL_21)
/*FUNCTION**********************************************************************
 *
 * Function Name : lin_diagservice_assign_frame_id_range
 * Description   : This function to process assign frame id range request, and also prepare its response data.
 *                 This function is only for Slave Node.
 *
 * Implements    : lin_diagservice_assign_frame_id_range_Activity
 *END**************************************************************************/
#if !SUPPORT_BOOTLOADER
static void lin_diagservice_assign_frame_id_range(l_ifc_handle iii)
{
    DEV_ASSERT((l_u8)iii < LIN_NUM_OF_IFCS);

    l_u8 j;
    l_u8 i;
    l_u8 start_index;
    l_u8 cfg_frame_num = 0U;
    lin_tl_pdu_data_t lin_tl_pdu;
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];
    const lin_transport_layer_queue_t * rx_queue = &(g_lin_tl_descriptor_array[iii].tl_rx_queue);
    l_u8 storePID = 1U;
    for (i = 0; i < 8U; i++)
    {
        lin_tl_pdu[i] = rx_queue->tl_pdu_ptr[rx_queue->queue_header][i];
    }

    /* Get start index in request */
    start_index = lin_tl_pdu[3];

    /* Find the number of configurable frames plus 2, including 0x3C and 0x3D */
    i = 1U;
    while (0xFFFFU != (prot_user_config_ptr->list_identifiers_ROM_ptr)[i++])
    {
        cfg_frame_num++;
    }

    /* Calculate number of configurable frames */
    cfg_frame_num = (l_u8)(cfg_frame_num - 3U);

    i = 4U;
    /* Check request validity */
    for (j = start_index; j < (start_index + 4U); j++)
    {
        if ((0xFFU != lin_tl_pdu[i++]) && (j > cfg_frame_num))
        {
            ld_make_slave_response_pdu(iii, SERVICE_ASSIGN_FRAME_ID_RANGE, NEGATIVE, GENERAL_REJECT);
            storePID = 0U;
            break;
        }
    }

    if (storePID == 1U)
    {
        /* Store PIDs */
        for (i = 4U; i < 8U; i++)
        {
            switch (lin_tl_pdu[i])
            {
                case 0x00U:
                    /* Unassign frame */
                    start_index++;
                    (prot_user_config_ptr->list_identifiers_RAM_ptr)[start_index] = 0xFFU;
                    break;
                case 0xFFU:
                    /* keep the previous assigned value of this frame */
                    break;
                default:
                    /* Calculate frame ID and Assign ID to frame */
                    start_index++;
                    (prot_user_config_ptr->list_identifiers_RAM_ptr)[start_index] = lin_process_parity(lin_tl_pdu[i], CHECK_PARITY);
                    break;
            }
        } /* End of for statement */

        ld_make_slave_response_pdu(iii, SERVICE_ASSIGN_FRAME_ID_RANGE, POSITIVE, 0U);
    }
}
/*FUNCTION**********************************************************************
 *
 * Function Name : ld_read_configuration
 * Description   : This function copies current configuration in a reserved area.
 *
 * Implements    : OK-zhu
 *END**************************************************************************/
l_u8 ld_read_configuration(l_ifc_handle iii,
                           l_u8 * const data,
                           l_u8 * const length)
{
    DEV_ASSERT((l_u8)iii < LIN_NUM_OF_IFCS);
    DEV_ASSERT(data != NULL);
    DEV_ASSERT(length != NULL);

    l_u8 i, temp;
    const lin_node_attribute_t * node_attr_ptr;
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];
    /* Set the default returned value to LD_READ_OK */
    l_u8 retval = (l_u8)LD_READ_OK;
    /** Set the expected length value to
     * EXP = NN + NF, where :
     * NN = the number of NAD.
     * NF = the number of configurable frames;
     * Moreover:
     * Not taken PID's diagnostics frame: 3C, 3D
     */
    l_u8 expected_length;

    expected_length = (l_u8)(prot_user_config_ptr->number_of_configurable_frames - 1U);
    /* Check if slave node */
    if ((bool)LIN_SLAVE == prot_user_config_ptr->function)
    {
        temp = *length;
        /* Get node attribute */
        node_attr_ptr = &g_lin_node_attribute_array[prot_user_config_ptr->slave_ifc_handle];
        if (temp < expected_length)
        {
            /* The 'data' size is not enough to store NAD+PIDs */
            retval = (l_u8)LD_LENGTH_TOO_SHORT;
        }
        else
        {
            /* The 'data' size is enough to store NAD+PIDs, so proceed to store them */
            /* Copy actual NAD to 'data' */
            data[0] = *node_attr_ptr->configured_NAD_ptr;

            /* Copy protected IDs to 'data' */
            for (i = 1U; i < expected_length; i++)
            {
                data[i] = lin_process_parity(prot_user_config_ptr->list_identifiers_RAM_ptr[i], MAKE_PARITY);
            }

            /* Set the length parameter to the actual size of the configuration */
            *length = expected_length;
        }
    }
    else
    {
        retval = 0xFFU;
    }

    return retval;
} /* End ld_read_configuration() */
/*FUNCTION**********************************************************************
 *
 * Function Name : ld_set_configuration
 * Description   : This function configures slave node according to data.
 *
 * Implements    : ld_set_configuration_Activity
 *END**************************************************************************/
l_u8 ld_set_configuration(l_ifc_handle iii,
                          const l_u8 * const data,
                          l_u16 length)
{
    DEV_ASSERT((l_u8)iii < LIN_NUM_OF_IFCS);
    DEV_ASSERT(data != NULL);

    l_u8 i;
    /* Set the default returned value to LD_DATA_ERROR */
    l_u8 retval = LD_DATA_ERROR;
    const lin_node_attribute_t * node_attr_ptr;
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];
    /** Set the expected length value to
     * EXP = NN + NF, where :
     * NN = the number of NAD.
     * NF = the number of configurable frames;
     * Moreover:
     * Not taken PID's diagnostics frame: 3C, 3D
     */
    l_u16 expected_length;

    expected_length = (l_u16)((l_u16)prot_user_config_ptr->number_of_configurable_frames - 1U);

    /* Check if slave node */
    if ((bool)LIN_SLAVE == prot_user_config_ptr->function)
    {
        /* Get node attribute */
        node_attr_ptr = &g_lin_node_attribute_array[prot_user_config_ptr->slave_ifc_handle];
        if (length < expected_length)
        {
            /* The 'data' size is not enough to contain NAD+PIDs */
            retval = LD_LENGTH_NOT_CORRECT;
        }
        else
        {
            /* The 'data' size is enough to contain NAD+PIDs, so proceed to read from 'data' */
            /* Read actual NAD from 'data' */
            *node_attr_ptr->configured_NAD_ptr = data[0];

            /* Copy protected IDs in 'data' to RAM configuration */
            for (i = 1U; i < expected_length; ++i)
            {
                prot_user_config_ptr->list_identifiers_RAM_ptr[i] = lin_process_parity(data[i], CHECK_PARITY);
            }

            /* No error, return OK */
            retval = LD_SET_OK;
        }
    }
    else
    {
        retval = 0xFFU;
    }

    return retval;
} /* End ld_set_configuration() */
#endif
#endif /* (1U == SUPPORT_PROTOCOL_21) */

#endif /* (1U == SUPPORT_SLAVE_MODE) */

/* diagnostic services class II */
#if (1U == SUPPORT_DIAG_CLASS_II)
#if (1U == SUPPORT_MASTER_NODE)
/*FUNCTION**********************************************************************
 *
 * Function Name : diag_read_data_by_identifier
 * Description   : This function reads data by identifier, Diagnostic Class II service (0x22).
 *
 * Implements    : diag_read_data_by_identifier_Activity
 *END**************************************************************************/
void diag_read_data_by_identifier(l_ifc_handle iii,
                                  const l_u8 NAD,
                                  const l_u8 number_of_id,
                                  const l_u16 * const list_of_id)
{
    DEV_ASSERT((l_u8)iii < LIN_NUM_OF_IFCS);
    DEV_ASSERT(list_of_id != NULL);

    l_u8 buff[MAX_LENGTH_SERVICE];
    l_u8 i;
    l_u16 count = 0U;
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];
    l_u16 length = g_lin_protocol_user_cfg_array[iii].max_message_length;

    if ((bool)LIN_MASTER == prot_user_config_ptr->function)
    {
        count = (l_u16)((l_u16)number_of_id << 1U);
        if(count < length)
        {
            /* check whether service status is idle or not */
            if (tl_desc_ptr->service_status != LD_SERVICE_BUSY)
            {
                /* Buffer[0] SID of Service Read Data by Identifier: 0x22 */
                buff[0U] = SERVICE_READ_DATA_BY_IDENTIFY;
                /* Pack data */
                for(i = 0U; i < number_of_id; i++)
                {
                    count = (l_u16)(((l_u16)i + 1U) << 1U);
                    buff[count - 1U] = (l_u8)(list_of_id[i] >> 8U);
                    buff[count]      = (l_u8)(list_of_id[i] & 0xFFU);
                }

                /* send message to transport layer */
                ld_send_message(iii, count + 1U, NAD, buff);

                /* set service status to busy */
                tl_desc_ptr->service_status = LD_SERVICE_BUSY;
            }
        }
    }
}


/*FUNCTION**********************************************************************
 *
 * Function Name : diag_write_data_by_identifier
 * Description   : Write Data by Identifier for a specified node - Diagnostic Class II service (0x2E)
 *
 * Implements    : diag_write_data_by_identifier_Activity
 *END**************************************************************************/
void diag_write_data_by_identifier(l_ifc_handle iii,
                                   const l_u8 NAD,
                                   l_u16 data_length,
                                   const l_u8 * const data)
{
    DEV_ASSERT((l_u8)iii < LIN_NUM_OF_IFCS);
    DEV_ASSERT(data != NULL);

    l_u8 buff[MAX_LENGTH_SERVICE];
    l_u8 i;
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];
    l_u16 length = g_lin_protocol_user_cfg_array[iii].max_message_length;

    if ((bool)LIN_MASTER == prot_user_config_ptr->function)
    {
        /* Check if length of data is less than length maximum */
        if (data_length < length)
        {
            /* check whether service status is idle or not */
            if (tl_desc_ptr->service_status != LD_SERVICE_BUSY)
            {

                /* pack data */
                /* Buffer[0] SID of Write Data by identifier: 0x2E */
                buff[0] = SERVICE_WRITE_DATA_BY_IDENTIFY;

                for (i = 0U; i < data_length; i++)
                {
                    buff[i + 1U] = data[i];
                }

                ld_send_message(iii, data_length + 1U, NAD, buff);
                /* set service status to busy */
                tl_desc_ptr->service_status = LD_SERVICE_BUSY;
            }
        }
    }
}
#endif /* (1U == SUPPORT_MASTER_NODE) */
#endif /* (1U == SUPPORT_DIAG_CLASS_II) */

/* Diagnostic services class III */
#if (1U == SUPPORT_DIAG_CLASS_III)
#if (1U == SUPPORT_MASTER_NODE)
/*FUNCTION**********************************************************************
 *
 * Function Name : diag_session_control
 * Description   : This function is used for master node only. It will pack data
 * and send request to slave node with service ID = 0x10: Session control.
 *
 * Implements    : diag_session_control_Activity
 *END**************************************************************************/
void diag_session_control(l_ifc_handle iii,
                          const l_u8 NAD,
                          const l_u8 session_type)
{
    DEV_ASSERT((l_u8)iii < LIN_NUM_OF_IFCS);

    l_u8 buff[2];
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];

    if ((bool)LIN_MASTER == prot_user_config_ptr->function)
    {
        /* check whether service status is idle or not */
        if (tl_desc_ptr->service_status != LD_SERVICE_BUSY)
        {
            /* pack data */
            /* Buffer[0] SID of Session Control: 0x10 */
            buff[0] = SERVICE_SESSION_CONTROL;
            buff[1] = session_type;

            ld_send_message(iii, 2, NAD, buff);
            /* set service status to busy */
            tl_desc_ptr->service_status = LD_SERVICE_BUSY;
        }
    }
}


/*FUNCTION**********************************************************************
 *
 * Function Name : diag_fault_memory_read
 * Description   : This function is used for master node only. It will pack data
 * and send request to slave node with service ID = 0x19: Fault memory read.
 *
 * Implements    : diag_fault_memory_read_Activity
 *END**************************************************************************/
void diag_fault_memory_read(l_ifc_handle iii,
                            const l_u8 NAD,
                            l_u16 data_length,
                            const l_u8 * const data)
{
    DEV_ASSERT((l_u8)iii < LIN_NUM_OF_IFCS);
    DEV_ASSERT(data != NULL);

    l_u8 buff[MAX_LENGTH_SERVICE];
    l_u8 i;
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    l_u16 length = g_lin_protocol_user_cfg_array[iii].max_message_length;
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];

    if ((bool)LIN_MASTER == prot_user_config_ptr->function)
    {
        /* Check if length of data is lower than length maximum */
        if (data_length < length)
        {
            /* check whether service status is idle or not */
            if (tl_desc_ptr->service_status != LD_SERVICE_BUSY)
            {
                /* pack data */
                /* Buffer[0] SID of Service fault memory read: 0x19 */
                buff[0] = SERVICE_FAULT_MEMORY_READ;

                for (i = 0U; i < data_length; i++)
                {
                    buff[i + 1U] = data[i];
                }

                ld_send_message(iii, data_length + 1U, NAD, buff);
                /* set service status to busy */
                tl_desc_ptr->service_status = LD_SERVICE_BUSY;
            }
        }
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : diag_fault_memory_clear
 * Description   : This function is used for master node only. It will pack data
 * and send request to slave node with service ID = 0x14: Fault memory clear.
 *
 * Implements    : diag_fault_memory_clear_Activity
 *END**************************************************************************/
void diag_fault_memory_clear(l_ifc_handle iii,
                             const l_u8 NAD,
                             const l_u8 * const groupOfDTC)
{
    DEV_ASSERT((l_u8)iii < LIN_NUM_OF_IFCS);
    DEV_ASSERT(groupOfDTC != NULL);

    l_u8 buff[4];
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];
    if ((bool)LIN_MASTER == prot_user_config_ptr->function)
    {
        /* check whether service status is idle or not */
        if (tl_desc_ptr->service_status != LD_SERVICE_BUSY)
        {
            /* pack data */
            /* Buffer[0] SID of Service fault memory clear: 0x14 */
            buff[0] = SERIVCE_FAULT_MEMORY_CLEAR;
            buff[1] = groupOfDTC[0];
            buff[2] = groupOfDTC[1];
            buff[3] = groupOfDTC[2];

            ld_send_message(iii, 4U, NAD, buff);
            /* set service status to busy */
            tl_desc_ptr->service_status = LD_SERVICE_BUSY;
        }
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : diag_IO_control
 * Description   : This function is used for master node only. It will pack data
 * and send request to slave node with service ID = 0x2F: Input/Output control service.
 *
 * Implements    : diag_IO_control_Activity
 *END**************************************************************************/
void diag_IO_control(l_ifc_handle iii,
                     const l_u8 NAD,
                     l_u16 data_length,
                     const l_u8 * const data)
{
    DEV_ASSERT((l_u8)iii < LIN_NUM_OF_IFCS);
    DEV_ASSERT(data != NULL);

    l_u8 buff[MAX_LENGTH_SERVICE];
    l_u8 i;
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    l_u16 length = g_lin_protocol_user_cfg_array[iii].max_message_length;
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];
    if ((bool)LIN_MASTER == prot_user_config_ptr->function)
    {
        /* Check if length of data is lower than length maximum */
        if (data_length < length)
        {
            /* check whether service status is idle or not */
            if (tl_desc_ptr->service_status != LD_SERVICE_BUSY)
            {
                /* pack data */
                /* Buffer[0] SID of IO Control by Identify: 0x2F */
                buff[0] = SERVICE_IO_CONTROL_BY_IDENTIFY;

                for (i = 0U; i < data_length; i++)
                {
                    buff[i + 1U] = data[i];
                }

                ld_send_message(iii, data_length + 1U, NAD, buff);
                /* set service status to busy */
                tl_desc_ptr->service_status = LD_SERVICE_BUSY;
            }
        }
    }
}
#endif /* (1U == SUPPORT_MASTER_NODE) */
#endif /* (1U == SUPPORT_DIAG_CLASS_III) */

#endif /* ((1U == SUPPORT_PROTOCOL_21) || (1U == SUPPORT_PROTOCOL_20)) */

#if (1U == SUPPORT_MASTER_NODE)
/*FUNCTION**********************************************************************
 *
 * Function Name : ld_assign_NAD
 * Description   : This call assigns the NAD (node diagnostic address) of all slave nodes
 *  that matches the initial_NAD, the supplier ID and the function ID.
 *
 * Implements    : ld_assign_NAD_Activity
 *END**************************************************************************/
void ld_assign_NAD(l_ifc_handle iii,
                   l_u8 initial_NAD,
                   l_u16 supplier_id,
                   l_u16 function_id,
                   l_u8 new_NAD)
{
    DEV_ASSERT((l_u8)iii < LIN_NUM_OF_IFCS);

    l_u8 data[6];
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];

    if ((bool)LIN_MASTER == prot_user_config_ptr->function)
    {
        /* check service is busy? */
        if (tl_desc_ptr->service_status != LD_SERVICE_BUSY)
        {
            /* data[0] SID of Service assign NAD: 0xB0 */
            data[0] = SERVICE_ASSIGN_NAD;
            data[1] = (l_u8)(supplier_id & 0x00FFU);
            data[2] = (l_u8)((supplier_id >> 8U) & 0x00FFU);
            data[3] = (l_u8)(function_id & 0x00FFU);
            data[4] = (l_u8)((function_id >> 8U) & 0x00FFU);
            data[5] = new_NAD;

            /* put data into TX_QUEUE */
            ld_send_message(iii, 6U, initial_NAD, data);

            /* set node config status to busy */
            tl_desc_ptr->service_status = LD_SERVICE_BUSY;
        } /* End of checking service status */
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ld_conditional_change_NAD
 * Description   : This call changes the NAD
 *  if the node properties fulfill the test specified by id, byte, mask and invert.
 *
 * Implements    : ld_conditional_change_NAD_Activity
 *END**************************************************************************/
void ld_conditional_change_NAD(l_ifc_handle iii,
                               l_u8 NAD,
                               l_u8 id,
                               l_u8 byte_data,
                               l_u8 mask,
                               l_u8 invert,
                               l_u8 new_NAD)
{
    DEV_ASSERT((l_u8)iii < LIN_NUM_OF_IFCS);

    l_u8 data[6];
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];

    /* Check input parameters are in accepted range*/
    if ((id < 32U) && ((0U < byte_data) && (byte_data < 6U)))
    {
        if ((bool)LIN_MASTER == prot_user_config_ptr->function)
        {
            /* check service is busy? */
            if (tl_desc_ptr->service_status != LD_SERVICE_BUSY)
            {
                /* data[0] SID of Service Conditional change NAD: 0xB3 */
                data[0] = SERVICE_CONDITIONAL_CHANGE_NAD;
                data[1] = id;
                data[2] = byte_data;
                data[3] = mask;
                data[4] = invert;
                data[5] = new_NAD;

                /* put data into TX_QUEUE */
                ld_send_message(iii, 6U, NAD, data);

                /* set node config status to busy */
                tl_desc_ptr->service_status = LD_SERVICE_BUSY;
            } /* End of checking service status */
        }
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ld_read_by_id
 * Description   : The call requests the slave node selected with the NAD
 *  to return the property associated with the id parameter.
 *
 * Implements    : ld_read_by_id_Activity
 *END**************************************************************************/
void ld_read_by_id(l_ifc_handle iii,
                   l_u8 NAD,
                   l_u16 supplier_id,
                   l_u16 function_id,
                   l_u8 id,
                   lin_product_id_t * const data)
{
    DEV_ASSERT((l_u8)iii < LIN_NUM_OF_IFCS);
    DEV_ASSERT(data != NULL);

    /* Multi frame support */
    l_u8 buff[6];
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];

    if ((bool)LIN_MASTER == prot_user_config_ptr->function)
    {
        /* Check service is busy? */
        if (tl_desc_ptr->service_status != LD_SERVICE_BUSY)
        {
            /* Create data for Read by Identifier command */
            /* Buffer[0] SID of Service Read by identifier: 0xB2 */
            buff[0] = SERVICE_READ_BY_IDENTIFY;
            buff[1] = id;
            buff[2] = (l_u8)(supplier_id & 0x00FFU);
            buff[3] = (l_u8)((supplier_id >> 8U) & 0x00FFU);
            buff[4] = (l_u8)(function_id & 0x00FFU);
            buff[5] = (l_u8)((function_id >> 8U) & 0x00FFU);

            /* Store address of RAM data which contain response info */
            tl_desc_ptr->product_id_ptr = data;

            /* put data into TX_QUEUE */
            ld_send_message(iii, 6U, NAD, buff);

            /* set node config status to busy */
            tl_desc_ptr->service_status = LD_SERVICE_BUSY;
        } /* End of checking service status */
    }
}
#endif /* (1U == SUPPORT_MASTER_NODE) */

#if (1U == SUPPORT_SLAVE_MODE)
/*FUNCTION**********************************************************************
 *
 * Function Name : diag_read_data_by_identifier
 * Description   : This function reads data by identifier, Diagnostic Class II service (0x22). slave
 *
 * Implements    : diag_read_data_by_identifier_Activity
 *END**************************************************************************/
#if !SUPPORT_BOOTLOADER
void lin_read_data_by_identify(l_ifc_handle iii)
{                                
    lin_tl_pdu_data_t lin_tl_pdu;
    l_u16 did;
    l_u8 sid;
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];
    const lin_node_attribute_t * node_attr_ptr;
    node_attr_ptr = &g_lin_node_attribute_array[prot_user_config_ptr->slave_ifc_handle];
    const lin_transport_layer_queue_t * rx_queue = &(tl_desc_ptr->tl_rx_queue);
    l_u8 i;
    for (i = 0; i < 8U; i++)
    {
        lin_tl_pdu[i] = rx_queue->tl_pdu_ptr[rx_queue->queue_header][i];
    }

    /* Get did in request */
    did = (l_u16)(((l_u16)lin_tl_pdu[3]) << 8U);
    did = (l_u16)(did | ((l_u16)lin_tl_pdu[4]));
    sid = (l_u8)(lin_tl_pdu[2]);

    switch (did)
    {
        //TODO: 根据不同主机需求更改相应的功能
    case 0xF188: //读取软件版本信息
        lin_tl_pdu[0] = *node_attr_ptr->configured_NAD_ptr;
        lin_tl_pdu[1] = 0x10;
        lin_tl_pdu[2] = 0x0b;
        lin_tl_pdu[3] = (l_u8)(sid + RES_POSITIVE);
        lin_tl_pdu[4] = 0xF1;
        lin_tl_pdu[5] = 0x88;
        lin_tl_pdu[6] = 0x30;
        lin_tl_pdu[7] = 0x30;

        ld_put_raw(iii, lin_tl_pdu);

        lin_tl_pdu[0] = *node_attr_ptr->configured_NAD_ptr;
        lin_tl_pdu[1] = 0x21;
        lin_tl_pdu[2] = 0x2e;
        lin_tl_pdu[3] = 0x30;
        lin_tl_pdu[4] = 0x30;
        lin_tl_pdu[5] = 0x2e;
        lin_tl_pdu[6] = 0x30;
        lin_tl_pdu[7] = 0x30;

        ld_put_raw(iii, lin_tl_pdu);
        tl_desc_ptr->diag_state  = LD_DIAG_TX_PHY;
        break;
    case 0xF199: //编程日期
        //TODO:
        break;
    case 0xF198: 
        //TODO:
        break;
    case 0xF180: //bootloader 软件版本号
        //TODO:
        break;
    case 0xF191: //ECU硬件版本号
        //TODO:
        break;
    default:
        ld_make_slave_response_pdu(iii, SERVICE_READ_DATA_BY_IDENTIFY, NEGATIVE, 0U);
        break;
    }
}
#endif
/*FUNCTION**********************************************************************
 *
 * Function Name : lin_slave_session_ctr
 * Description   : This function change the session mode, Diagnostic service (0x10). 
 *                 会话切换,诊断会话的切换，手动切换, 
 * Implements    : OK--zhu
 *END**************************************************************************/
void lin_slave_session_ctr(l_ifc_handle iii)
{
    lin_tl_pdu_data_t lin_tl_pdu;
    l_u8 sub_func_id;
    l_u8 d_len;
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];
    const lin_node_attribute_t * node_attr_ptr;
    node_attr_ptr = &g_lin_node_attribute_array[prot_user_config_ptr->slave_ifc_handle];
    const lin_transport_layer_queue_t * rx_queue = &(tl_desc_ptr->tl_rx_queue);

    sub_func_id = rx_queue->tl_pdu_ptr[rx_queue->queue_header][3]; //获取到子功能的id
    d_len = (rx_queue->tl_pdu_ptr[rx_queue->queue_header][1]) & 0x0F;

    if(d_len == 2U)
    {
        switch (sub_func_id)
        {
#if !SUPPORT_BOOTLOADER            //bootloader下只支持编程模式
        case 1:
            /* code */
            //TODO: 切换到默认会话,在切换之前先判断是否满足切换的条件
            lin_tl_pdu[0] = *node_attr_ptr->configured_NAD_ptr;
            lin_tl_pdu[1] = 0x02; //TODO:可能附带参数信息,数据长度信息\帧类型会变动
            lin_tl_pdu[2] = 0x50; //肯定响应 0x10 + 0x40
            lin_tl_pdu[3] = sub_func_id;
            lin_tl_pdu[4] = 0xFFU; //TODO:可能附带参数信息,数据长度信息会变动
            lin_tl_pdu[5] = 0xFFU;
            lin_tl_pdu[6] = 0xFFU;
            lin_tl_pdu[7] = 0xFFU;

            ld_put_raw(iii, lin_tl_pdu); 
            tl_desc_ptr->diag_state  = LD_DIAG_TX_PHY;
            break;
        case 3:
            //TODO: 切换到扩展会话,在切换之前先判断是否满足切换的条件

            lin_tl_pdu[0] = *node_attr_ptr->configured_NAD_ptr;
            lin_tl_pdu[1] = 0x02; //TODO:可能附带参数信息,数据长度信息\帧类型会变动
            lin_tl_pdu[2] = 0x50; //肯定响应 0x10 + 0x40
            lin_tl_pdu[3] = sub_func_id;
            lin_tl_pdu[4] = 0xFFU; //TODO:可能附带参数信息,数据长度信息会变动
            lin_tl_pdu[5] = 0xFFU;
            lin_tl_pdu[6] = 0xFFU;
            lin_tl_pdu[7] = 0xFFU;

            ld_put_raw(iii, lin_tl_pdu); 
            tl_desc_ptr->diag_state  = LD_DIAG_TX_PHY;
            break;
#endif
        case 2:
            //TODO: 切换到编程会话,在切换之前先判断是否满足切换的条件
            lin_tl_pdu[0] = *node_attr_ptr->configured_NAD_ptr;
            lin_tl_pdu[1] = 0x02; //TODO:可能附带参数信息,数据长度信息\帧类型会变动
            lin_tl_pdu[2] = 0x50; //肯定响应 0x10 + 0x40
            lin_tl_pdu[3] = sub_func_id;
            lin_tl_pdu[4] = 0xFFU; //TODO:可能附带参数信息,数据长度信息会变动
            lin_tl_pdu[5] = 0xFFU;
            lin_tl_pdu[6] = 0xFFU;
            lin_tl_pdu[7] = 0xFFU;

            ld_put_raw(iii, lin_tl_pdu); 
            tl_desc_ptr->diag_state  = LD_DIAG_TX_PHY;
            session_mode = 2; //编程会话模式
            break;
        default:
            ld_make_slave_response_pdu(iii, SERVICE_SESSION_CONTROL, NEGATIVE, 0x12);       //0x12 子功能不支持
            break;
        }
    }else
    {
        ld_make_slave_response_pdu(iii, SERVICE_SESSION_CONTROL, NEGATIVE, 0x13);           //0x13 数据长度错误
    }

}
/*FUNCTION**********************************************************************
 *
 * Function Name : lin_routine_control
 * Description   : This function run the routine demo, Diagnostic service (0x31). 
 *                 例程控制,执行预置的例行程序，并返回信息 
 * Implements    : 
 *END**************************************************************************/
void lin_routine_control(l_ifc_handle iii)
{
    lin_tl_pdu_data_t lin_tl_pdu;
    l_u8 routine_type;
    l_u16 state_id;
    l_u8 pci_type;
    l_u16 index=0;
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];
    const lin_node_attribute_t * node_attr_ptr;
    node_attr_ptr = &g_lin_node_attribute_array[prot_user_config_ptr->slave_ifc_handle];
    const lin_transport_layer_queue_t * rx_queue = &(tl_desc_ptr->tl_rx_queue);   

    pci_type = (l_u8)((rx_queue->tl_pdu_ptr[rx_queue->queue_header][1] & 0xF0) >> 4); //PCI帧类型

    if(pci_type == 0U) //单帧
    {
#if !SUPPORT_BOOTLOADER
        state_id = (l_u16)(((l_u16)rx_queue->tl_pdu_ptr[rx_queue->queue_header][4]) << 8);
        state_id = (l_u16)(state_id | ((l_u16)rx_queue->tl_pdu_ptr[rx_queue->queue_header][5]));
        routine_type = rx_queue->tl_pdu_ptr[rx_queue->queue_header][3];
        if(routine_type == 0x1U && state_id == 0x203U) //检验刷新安全条件请求信息
        {
            /* 需要检查的刷新安全条件：
            *   1.ECU的电源电压不能太高或者太低（9-16V）
            *   2.车辆处于IGN ON状态，但不在Ready状态
            *   3.车辆处于静止状态，车速为0km/h
            * */
            lin_tl_pdu[0] = *node_attr_ptr->configured_NAD_ptr;
            lin_tl_pdu[1] = 0x05;  //数据长度
            lin_tl_pdu[2] = 0x71;  //肯定响应 0x10 + 0x40
            lin_tl_pdu[3] = 0x1U;
            lin_tl_pdu[4] = 0x2U;  
            lin_tl_pdu[5] = 0x3U;
            //TODO：判断当前能否检查程序状态
            lin_tl_pdu[6] = 0x2U;  //2：成功可以执行检查 5：失败无法执行检查
            lin_tl_pdu[7] = 0xFFU;       
        }else if(routine_type == 0x3U && state_id == 0x203U)
        {
            lin_tl_pdu[0] = *node_attr_ptr->configured_NAD_ptr;
            lin_tl_pdu[1] = 0x05;  //数据长度
            lin_tl_pdu[2] = 0x71;  //肯定响应 0x10 + 0x40
            lin_tl_pdu[3] = 0x3U;
            lin_tl_pdu[4] = 0x2U;  
            lin_tl_pdu[5] = 0x3U;
            //TODO：检查结果，能够执行刷写操作
            lin_tl_pdu[6] = 0x2U;  //2：可以执行 3:检查正在执行 5：不能执行
            lin_tl_pdu[7] = 0xFFU;          
        }else if(routine_type == 0x2U && state_id == 0xff01U) //停止刷新可靠性状态检测
        {
            // 
        }
        else if(routine_type == 0x3U && state_id == 0xff01U) //状态查询请求信息
        {
            //
        }
#endif
        state_id = (l_u16)(((l_u16)rx_queue->tl_pdu_ptr[rx_queue->queue_header][4]) << 8);
        state_id = (l_u16)(state_id | ((l_u16)rx_queue->tl_pdu_ptr[rx_queue->queue_header][5]));
        routine_type = rx_queue->tl_pdu_ptr[rx_queue->queue_header][3];

        if(routine_type == 0x2U && state_id == 0xff01U) //停止刷新可靠性状态检测
        {
            check_program_flag = 0;
            lin_tl_pdu[0] = *node_attr_ptr->configured_NAD_ptr;
            lin_tl_pdu[1] = 0x05;  //数据长度
            lin_tl_pdu[2] = 0x71;  //肯定响应 0x10 + 0x40
            lin_tl_pdu[3] = 0x02;
            lin_tl_pdu[4] = 0xff;  
            lin_tl_pdu[5] = 0x01;
            lin_tl_pdu[6] = 0x04;
            lin_tl_pdu[7] = 0xFFU;  
        }
        else if(routine_type == 0x3U && state_id == 0xff01U) //状态查询请求信息
        {
            lin_tl_pdu[0] = *node_attr_ptr->configured_NAD_ptr;
            lin_tl_pdu[1] = 0x05;  //数据长度
            lin_tl_pdu[2] = 0x71;  //肯定响应 0x10 + 0x40
            lin_tl_pdu[3] = 0x03;
            lin_tl_pdu[4] = 0xff;  
            lin_tl_pdu[5] = 0x01;
            lin_tl_pdu[7] = 0xFFU; 

            if(check_program_flag == 1u)
            {
                lin_tl_pdu[6] = 0x03; //正在校验
            }
            else if(check_program_flag == 2u) //检查完成
            {
                lin_tl_pdu[6] = 0x02;
            }else
            {
                lin_tl_pdu[6] = 0x05;
            }
        }
    }else
    {   
        state_id = (l_u16)(((l_u16)rx_queue->tl_pdu_ptr[rx_queue->queue_header][5]) << 8);
        state_id = (l_u16)(state_id | ((l_u16)rx_queue->tl_pdu_ptr[rx_queue->queue_header][6]));
        routine_type = rx_queue->tl_pdu_ptr[rx_queue->queue_header][4];

        if(routine_type == 0x1U && state_id == 0xff00U) //运行擦除内存的例程开始 并且是首帧
        {
            if(security_checked) // 授权通过
            {
                //这里需要启动擦除内存的操作，提取内存地址跟内存长度信息, 但是LIN通讯需要保持
                //擦除请求会附带内存地址信息以及擦除长度信息,实际应用中,暂定擦除地址固定,无需处理地址长度信息
                erase_flag = 1u;
                
                lin_tl_pdu[0] = *node_attr_ptr->configured_NAD_ptr;
                lin_tl_pdu[1] = 0x05;  //数据长度
                lin_tl_pdu[2] = 0x71;  //肯定响应 0x10 + 0x40
                lin_tl_pdu[3] = 0x01;
                lin_tl_pdu[4] = 0xff;  
                lin_tl_pdu[5] = 0x00;
                lin_tl_pdu[6] = 0x2U;  //2：OK
                lin_tl_pdu[7] = 0xFFU;  
            }else
            {
                //如果授权不通过，不给予响应
            }
        }
        else if(routine_type == 0x3U && state_id == 0xff00U) //停止擦除内存
        {
            erase_flag = 0u;

            lin_tl_pdu[0] = *node_attr_ptr->configured_NAD_ptr;
            lin_tl_pdu[1] = 0x05;  //数据长度
            lin_tl_pdu[2] = 0x71;  //肯定响应 0x10 + 0x40
            lin_tl_pdu[3] = 0x02;
            lin_tl_pdu[4] = 0xff;  
            lin_tl_pdu[5] = 0x00;
            lin_tl_pdu[6] = 0x4U;  //2：OK
            lin_tl_pdu[7] = 0xFFU;              
        }
        else if(routine_type == 0x3U && state_id == 0xff00U) //查询内存擦除状态
        {
            lin_tl_pdu[0] = *node_attr_ptr->configured_NAD_ptr;
            lin_tl_pdu[1] = 0x05;  //数据长度
            lin_tl_pdu[2] = 0x71;  //肯定响应 0x10 + 0x40
            lin_tl_pdu[3] = 0x03;
            lin_tl_pdu[4] = 0xff;  
            lin_tl_pdu[5] = 0x00;
            lin_tl_pdu[7] = 0xFFU; 

            if(erase_flag == 0)
            {
                //例程没有执行
                lin_tl_pdu[6] = 0x5U;  //5: 失败
            }else if(erase_flag == 1) 
            {
                //正在执行
                lin_tl_pdu[6] = 0x3U;  //3：正在执行
            }else if(erase_flag == 2)
            {
                //执行完成
                lin_tl_pdu[6] = 0x2U;  //2：OK
            }
        }else if(routine_type == 0x1U && state_id == 0xff01U) //检验刷新可靠性开始请求信息
        {
            //地址固定,长度动态变化 
            index = rx_queue->queue_header + 1;
            check_program_size = (l_u32)(((l_u32)rx_queue->tl_pdu_ptr[index][5]) << 24);
            check_program_size = (l_u32)(check_program_size | ((l_u32)(((l_u32)rx_queue->tl_pdu_ptr[index][6]) << 16)));
            check_program_size = (l_u32)(check_program_size | ((l_u32)(((l_u32)rx_queue->tl_pdu_ptr[index][7]) << 8)));
            check_program_size = (l_u32)(check_program_size | ((l_u32)rx_queue->tl_pdu_ptr[rx_queue->queue_tail][2]));

            //校验的code
            crc16_code = (l_u16)(((l_u16)rx_queue->tl_pdu_ptr[rx_queue->queue_tail][3]) << 8);
            crc16_code = (l_u16)(crc16_code | ((l_u16)rx_queue->tl_pdu_ptr[rx_queue->queue_tail][4]));

            check_program_flag = 1u; //标志位置1

            lin_tl_pdu[0] = *node_attr_ptr->configured_NAD_ptr;
            lin_tl_pdu[1] = 0x05;  //数据长度
            lin_tl_pdu[2] = 0x71;  //肯定响应 0x10 + 0x40
            lin_tl_pdu[3] = 0x01;
            lin_tl_pdu[4] = 0xff;  
            lin_tl_pdu[5] = 0x01;
            lin_tl_pdu[6] = 0x02;
            lin_tl_pdu[7] = 0xFFU; 

            // 这里有个问题，就是反馈肯定响应的问题，这里跟标准文档有一些区别
        }
    }
    ld_put_raw(iii, lin_tl_pdu); 
    tl_desc_ptr->diag_state = LD_DIAG_TX_PHY;
}

void control_dtc_setting(l_ifc_handle iii)
{
    lin_tl_pdu_data_t lin_tl_pdu;
    l_u8 code_type;
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];
    const lin_node_attribute_t * node_attr_ptr;
    node_attr_ptr = &g_lin_node_attribute_array[prot_user_config_ptr->slave_ifc_handle];
    const lin_transport_layer_queue_t * rx_queue = &(tl_desc_ptr->tl_rx_queue);   

    code_type = rx_queue->tl_pdu_ptr[rx_queue->queue_header][3];
    if(code_type == 0x1U) //检验刷新安全条件请求信息
    {
        lin_tl_pdu[0] = *node_attr_ptr->configured_NAD_ptr;
        lin_tl_pdu[1] = 0x02;  //数据长度
        lin_tl_pdu[2] = 0xC5;  //肯定响应 0x10 + 0x40
        lin_tl_pdu[3] = 0x1U;
        lin_tl_pdu[4] = 0xFF;  
        lin_tl_pdu[5] = 0xFF;
        lin_tl_pdu[6] = 0xFF;  
        lin_tl_pdu[7] = 0xFF;       

        ld_put_raw(iii, lin_tl_pdu); 
        tl_desc_ptr->diag_state = LD_DIAG_TX_PHY;         
    }else if(code_type == 0x2U)
    {
        lin_tl_pdu[0] = *node_attr_ptr->configured_NAD_ptr;
        lin_tl_pdu[1] = 0x02;  //数据长度
        lin_tl_pdu[2] = 0xC5;  //肯定响应 0x10 + 0x40
        lin_tl_pdu[3] = 0x2U;
        lin_tl_pdu[4] = 0xFF;  
        lin_tl_pdu[5] = 0xFF;
        lin_tl_pdu[6] = 0xFF;  
        lin_tl_pdu[7] = 0xFF;  

        ld_put_raw(iii, lin_tl_pdu); 
        tl_desc_ptr->diag_state = LD_DIAG_TX_PHY;    
    }
}
/*FUNCTION**********************************************************************
 *
 * Function Name : lin_transfer_data
 * Description   : This function transfer the mass data, Diagnostic service (0x36). 
 *                 从机接收数据，连续传输
 * Implements    : 
 *END**************************************************************************/
void lin_transfer_data(l_ifc_handle iii)
{
    lin_tl_pdu_data_t lin_tl_pdu;
    l_u8 block_count;
    l_u8 data_len;
    l_u8 pci_type;
    l_u8 byte_cnt=0;
    l_u16 buf_cnt=update_cnt;
    l_u16 index=0;
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];
    const lin_node_attribute_t * node_attr_ptr;
    node_attr_ptr = &g_lin_node_attribute_array[prot_user_config_ptr->slave_ifc_handle];
    const lin_transport_layer_queue_t * rx_queue = &(tl_desc_ptr->tl_rx_queue);   

    pci_type = (l_u8)((rx_queue->tl_pdu_ptr[rx_queue->queue_header][1] & 0xF0) >> 4); //PCI帧类型

    if(pci_type == 0U) //单帧
    {
        // state_id = (l_u16)(((l_u16)rx_queue->tl_pdu_ptr[rx_queue->queue_header][4]) << 8);
        // state_id = (l_u16)(state_id | ((l_u16)rx_queue->tl_pdu_ptr[rx_queue->queue_header][5]));
        // routine_type = rx_queue->tl_pdu_ptr[rx_queue->queue_header][3];
        // if(routine_type == 0x1U && state_id == 0x203U) //检验刷新安全条件请求信息
        // {
        //     lin_tl_pdu[0] = *node_attr_ptr->configured_NAD_ptr;
        //     lin_tl_pdu[1] = 0x05;  //数据长度
        //     lin_tl_pdu[2] = 0x71;  //肯定响应 0x10 + 0x40
        //     lin_tl_pdu[3] = 0x1U;
        //     lin_tl_pdu[4] = 0x2U;  
        //     lin_tl_pdu[5] = 0x3U;
        //     //TODO：判断当前能否检查程序状态
        //     lin_tl_pdu[6] = 0x2U;  //2：成功可以执行检查 5：失败无法执行检查
        //     lin_tl_pdu[7] = 0xFFU;       
        // }else if(routine_type == 0x3U && state_id == 0x203U)
        // {
        //     lin_tl_pdu[0] = *node_attr_ptr->configured_NAD_ptr;
        //     lin_tl_pdu[1] = 0x05;  //数据长度
        //     lin_tl_pdu[2] = 0x71;  //肯定响应 0x10 + 0x40
        //     lin_tl_pdu[3] = 0x3U;
        //     lin_tl_pdu[4] = 0x2U;  
        //     lin_tl_pdu[5] = 0x3U;
        //     //TODO：检查结果，能够执行刷写操作
        //     lin_tl_pdu[6] = 0x2U;  //2：可以执行 3:检查正在执行 5：不能执行
        //     lin_tl_pdu[7] = 0xFFU;          
        // }
    }else
    {   
        // 安全授权 + 下载请求 -> 接收数据
        if(security_checked == 1 && download_request_flag == 1)
        {
            block_count = rx_queue->tl_pdu_ptr[rx_queue->queue_header][4]; //块计数
            data_len = rx_queue->tl_pdu_ptr[rx_queue->queue_header][2]-2; //数据长度
            index = rx_queue->queue_header;
    
            if(history_blcok_count == block_count)//无错误
            {
                program_buffer[buf_cnt++] = rx_queue->tl_pdu_ptr[rx_queue->queue_header][5];
                program_buffer[buf_cnt++] = rx_queue->tl_pdu_ptr[rx_queue->queue_header][6];
                program_buffer[buf_cnt++] = rx_queue->tl_pdu_ptr[rx_queue->queue_header][7];
                
                for(index = 1u; index <= rx_queue->queue_tail; index++)
                {
                    if(byte_cnt < data_len)
                    {
                        byte_cnt++;
                        program_buffer[buf_cnt++] = rx_queue->tl_pdu_ptr[index][2];
                    }
                    if(byte_cnt < data_len)
                    {
                        byte_cnt++;
                        program_buffer[buf_cnt++] = rx_queue->tl_pdu_ptr[index][3];
                    }
                    if(byte_cnt < data_len)
                    {
                        byte_cnt++;
                        program_buffer[buf_cnt++] = rx_queue->tl_pdu_ptr[index][4];
                    }
                    if(byte_cnt < data_len)
                    {
                        byte_cnt++;
                        program_buffer[buf_cnt++] = rx_queue->tl_pdu_ptr[index][5];
                    }
                    if(byte_cnt < data_len)
                    {
                        byte_cnt++;
                        program_buffer[buf_cnt++] = rx_queue->tl_pdu_ptr[index][6];
                    }
                    if(byte_cnt < data_len)
                    {
                        byte_cnt++;
                        program_buffer[buf_cnt++] = rx_queue->tl_pdu_ptr[index][7];
                    }
                    
                }
                history_blcok_count++; 
                
                //这里执行刷写
                update_cnt = data_len + update_cnt;
                bin_size_cnt += data_len;
                if(update_cnt >= 512)
                {
                    update_cnt = 0;
                    flash_flag = 1u;
                }else
                {
                    if(bin_size_cnt >= update_mem_size)
                    {
                        
                        flash_flag = 2u; //刷最后一帧
                        bin_size_cnt = 0;
                        //update_cnt = 0;
                        history_blcok_count = 0;
                    }
                }
            }else
            {

            }
            lin_tl_pdu[0] = *node_attr_ptr->configured_NAD_ptr;
            lin_tl_pdu[1] = 0x02;  //数据长度
            lin_tl_pdu[2] = 0x76;  //肯定响应 0x10 + 0x40
            lin_tl_pdu[3] = block_count;
            lin_tl_pdu[4] = 0xff;  
            lin_tl_pdu[5] = 0xff;
            lin_tl_pdu[6] = 0xff; 
            lin_tl_pdu[7] = 0xFF; 
            
            ld_put_raw(iii, lin_tl_pdu); 
            tl_desc_ptr->diag_state = LD_DIAG_TX_PHY; 
        }   
    }
}
/*FUNCTION**********************************************************************
 *
 * Function Name : lin_security_visit
 * Description   : This function set security authorition, Diagnostic service (0x27). 
 *                 安全授权,诊断服务0x27
 * Implements    : 
 *END**************************************************************************/
void lin_security_visit(l_ifc_handle iii)
{
    lin_tl_pdu_data_t lin_tl_pdu;
    l_u8 visit_type;
    //l_u8 data_len;
    l_u8 pci_type;
    l_u8 password[SECURITY_SEED_LEN];
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];
    const lin_node_attribute_t * node_attr_ptr;
    node_attr_ptr = &g_lin_node_attribute_array[prot_user_config_ptr->slave_ifc_handle];
    const lin_transport_layer_queue_t * rx_queue = &(tl_desc_ptr->tl_rx_queue);      

    pci_type = (l_u8)((rx_queue->tl_pdu_ptr[rx_queue->queue_header][1] & 0xF0) >> 4); //PCI帧类型

    if(pci_type==0) //单帧  请求
    {
        visit_type = (l_u8)(rx_queue->tl_pdu_ptr[rx_queue->queue_header][3]);
        if(visit_type == 0x11) //安全访问类型为请求种子
        {
            lin_tl_pdu[0] = *node_attr_ptr->configured_NAD_ptr;
            if(SECURITY_SEED_LEN <= 4) // 单帧可以发送完成
            {
                lin_tl_pdu[1] = SECURITY_SEED_LEN + 2;  //数据长度
                lin_tl_pdu[2] = 0x67;  //肯定响应 0x10 + 0x40
                lin_tl_pdu[3] = 0x11;
                lin_tl_pdu[4] = security_seed[0];  
                lin_tl_pdu[5] = security_seed[1];
                //TODO：检查结果，能够执行刷写操作
                lin_tl_pdu[6] = security_seed[2];  //2：可以执行 3:检查正在执行 5：不能执行
                lin_tl_pdu[7] = security_seed[3];  

                ld_put_raw(iii, lin_tl_pdu); 
                tl_desc_ptr->diag_state = LD_DIAG_TX_PHY;  
            }
            else //需要多帧发送
            {
                // lin_tl_pdu[1] = 0x10   //首帧
                // lin_tl_pdu[2] = SECURITY_SEED_LEN + 2;  //数据长度
                // lin_tl_pdu[3] = 0x67;
                // lin_tl_pdu[4] = 0x11;  
                // lin_tl_pdu[5] = security_seed[0];
                // //TODO：检查结果，能够执行刷写操作
                // lin_tl_pdu[6] = security_seed[1];  //2：可以执行 3:检查正在执行 5：不能执行
                // lin_tl_pdu[7] = security_seed[2];  

                // ld_put_raw(iii, lin_tl_pdu); 

                // lin_tl_pdu[1] = 0x10   //首帧
                // lin_tl_pdu[2] = SECURITY_SEED_LEN + 2;  //数据长度
                // lin_tl_pdu[3] = 0x67;
                // lin_tl_pdu[4] = 0x11;  
                // lin_tl_pdu[5] = security_seed[0];
                // //TODO：检查结果，能够执行刷写操作
                // lin_tl_pdu[6] = security_seed[1];  //2：可以执行 3:检查正在执行 5：不能执行
                // lin_tl_pdu[7] = security_seed[2];  

                // ld_put_raw(iii, lin_tl_pdu);   
                // tl_desc_ptr->diag_state = LD_DIAG_TX_PHY;  
            }
        }else if(visit_type == 0x12) //安全访问类型为发送密钥
        {
            password[0] =  (l_u8)(rx_queue->tl_pdu_ptr[rx_queue->queue_header][4]);
            password[1] =  (l_u8)(rx_queue->tl_pdu_ptr[rx_queue->queue_header][5]);
            password[2] =  (l_u8)(rx_queue->tl_pdu_ptr[rx_queue->queue_header][6]);
            password[3] =  (l_u8)(rx_queue->tl_pdu_ptr[rx_queue->queue_header][7]);

            if((password[0] + password[3]) == 5U) 
            {
                security_checked = 1u; //已经授权
            
                lin_tl_pdu[1] = 0x02;  //首帧
                lin_tl_pdu[2] = 0x67;  //数据长度
                lin_tl_pdu[3] = 0x12;
                lin_tl_pdu[4] = 0xff;  
                lin_tl_pdu[5] = 0xff;
                //TODO：检查结果，能够执行刷写操作
                lin_tl_pdu[6] = 0xff;  //2：可以执行 3:检查正在执行 5：不能执行
                lin_tl_pdu[7] = 0xff; 
                ld_put_raw(iii, lin_tl_pdu); 
                tl_desc_ptr->diag_state = LD_DIAG_TX_PHY;  
            }else
            {
                //ld_make_slave_response_pdu(iii, 0x27, NEGATIVE, 0xff); //0x12 子功能不支持
            }
        }else
        {
            //ld_make_slave_response_pdu(iii, 0x27, NEGATIVE, 0xff); //0x12 子功能不支持
        }
    }
}
/*FUNCTION**********************************************************************
 *
 * Function Name : lin_write_data_by_identify
 * Description   : This function wirte software information , Diagnostic service (0x2e). 
 *                 写软件相关的数据,诊断服务0x2e
 * Implements    : 
 *END**************************************************************************/
void lin_write_data_by_identify(l_ifc_handle iii)
{
    ;
    //TODO:写产品信息
}
/*FUNCTION**********************************************************************
 *
 * Function Name : lin_download_request
 * Description   : This function get download information, Diagnostic service (0x34). 
 *                 下载请求,诊断服务0x34
 * Implements    : OK--Zhu
 *END**************************************************************************/
void lin_download_request(l_ifc_handle iii)
{
    lin_tl_pdu_data_t lin_tl_pdu;
    //l_u8 data_type; 
    //l_u8 addr_type;
    l_u8 pci_type;
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];
    const lin_node_attribute_t * node_attr_ptr;
    node_attr_ptr = &g_lin_node_attribute_array[prot_user_config_ptr->slave_ifc_handle];
    const lin_transport_layer_queue_t * rx_queue = &(tl_desc_ptr->tl_rx_queue);   

    pci_type = (l_u8)((rx_queue->tl_pdu_ptr[rx_queue->queue_header][1] & 0xF0) >> 4); //PCI帧类型

    if(pci_type == 0U) //单帧
    {
        ;//无单帧信息，万一遇到，当作错误处理
    }else
    {   
        //data_type = rx_queue->tl_pdu_ptr[rx_queue->queue_header][4];
        //addr_type = rx_queue->tl_pdu_ptr[rx_queue->queue_header][5];
        // update_mem_addr = (l_u32)(((l_u32)rx_queue->tl_pdu_ptr[rx_queue->queue_header][6]) << 24);
        // update_mem_addr = (l_u32)(update_mem_addr | ((l_u32)(((l_u32)rx_queue->tl_pdu_ptr[rx_queue->queue_header][7]) << 16)));
        // update_mem_addr = (l_u32)(update_mem_addr | ((l_u32)(((l_u32)rx_queue->tl_pdu_ptr[rx_queue->queue_tail][2]) << 8)));
        // update_mem_addr = (l_u32)(update_mem_addr | ((l_u32)rx_queue->tl_pdu_ptr[rx_queue->queue_tail][3]));

        update_mem_size = (l_u32)(((l_u32)rx_queue->tl_pdu_ptr[rx_queue->queue_tail][4]) << 24);
        update_mem_size = (l_u32)(update_mem_size | ((l_u32)(((l_u32)rx_queue->tl_pdu_ptr[rx_queue->queue_tail][5]) << 16)));
        update_mem_size = (l_u32)(update_mem_size | ((l_u32)(((l_u32)rx_queue->tl_pdu_ptr[rx_queue->queue_tail][6]) << 8)));
        update_mem_size = (l_u32)(update_mem_size | ((l_u32)rx_queue->tl_pdu_ptr[rx_queue->queue_tail][7]));

        download_request_flag = 1u; //

        lin_tl_pdu[0] = *node_attr_ptr->configured_NAD_ptr;
        lin_tl_pdu[1] = 0x04;  //数据长度
        lin_tl_pdu[2] = 0x74;  //肯定响应 0x10 + 0x40
        lin_tl_pdu[3] = 0x20;
        lin_tl_pdu[4] = 0x00;  //64字节一个块,
        lin_tl_pdu[5] = 0x40;
        lin_tl_pdu[6] = 0xff;  
        lin_tl_pdu[7] = 0xff; 

        ld_put_raw(iii, lin_tl_pdu); 
        tl_desc_ptr->diag_state = LD_DIAG_TX_PHY;  
        
    }

}
/*FUNCTION**********************************************************************
 *
 * Function Name : lin_download_exit
 * Description   : This function request quit download mode, Diagnostic service (0x37). 
 *                 下载请求退出,诊断服务0x37
 * Implements    : OK--Zhu
 *END**************************************************************************/
void lin_download_exit(l_ifc_handle iii)
{
    lin_tl_pdu_data_t lin_tl_pdu;
    const lin_node_attribute_t * node_attr_ptr;
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];
    node_attr_ptr = &g_lin_node_attribute_array[prot_user_config_ptr->slave_ifc_handle];

    lin_tl_pdu[0] = *node_attr_ptr->configured_NAD_ptr;
    lin_tl_pdu[1] = 0x02;  //数据长度
    lin_tl_pdu[2] = 0x77;  //肯定响应 0x10 + 0x40
    //TODO: CRC8计算
    lin_tl_pdu[3] = 0x0;   //校验字节
    lin_tl_pdu[4] = 0xFF;  
    lin_tl_pdu[5] = 0xFF;
    lin_tl_pdu[6] = 0xFF;  
    lin_tl_pdu[7] = 0xFF; 

    download_request_flag = 0;
    ld_put_raw(iii, lin_tl_pdu); 
    tl_desc_ptr->diag_state = LD_DIAG_TX_PHY;  
}
/*FUNCTION**********************************************************************
 *
 * Function Name : lin_ecu_reset
 * Description   : This function reset the ecu, Diagnostic service (0x11). 
 *                 复位ECU
 * Implements    : OK--Zhu
 *END**************************************************************************/
void lin_ecu_reset(l_ifc_handle iii)
{
    reset_flag = 1u; //复位置1
}
/*FUNCTION**********************************************************************
 *
 * Function Name : diag_get_flag
 * Description   : This function will return flag of diagnostic service,
 *  if LIN node receive master request or slave response.
 *
 * Implements    : diag_get_flag_Activity
 *END**************************************************************************/
l_u8 diag_get_flag(l_ifc_handle iii,
                   l_u8 flag_order)
{
    DEV_ASSERT((l_u8)iii < LIN_NUM_OF_IFCS);
    const l_u8 * service_flag;
    l_u8 ret_val = 0xFFU;
    const lin_node_attribute_t * node_attr_ptr;
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];

    if (prot_user_config_ptr->function == (bool)LIN_SLAVE)
    {
        node_attr_ptr = &g_lin_node_attribute_array[prot_user_config_ptr->slave_ifc_handle];
        service_flag = node_attr_ptr->service_flags_ptr;

        if (flag_order < node_attr_ptr->number_support_sid)
        {
            ret_val = (l_u8)service_flag[flag_order];
        }
    }
    return ret_val;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : diag_clear_flag
 * Description   : This function will clear flag of diagnostic service,
 *  if lin node receive master request or slave response.
 *
 * Implements    : diag_clear_flag_Activity
 *END**************************************************************************/
void diag_clear_flag(l_ifc_handle iii,
                     l_u8 flag_order)
{
    DEV_ASSERT((l_u8)iii < LIN_NUM_OF_IFCS);
    l_u8 * service_flag;
    const lin_node_attribute_t * node_attr_ptr;
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];

    if (prot_user_config_ptr->function == (bool)LIN_SLAVE)
    {
        node_attr_ptr = &g_lin_node_attribute_array[prot_user_config_ptr->slave_ifc_handle];
        /* Get current configuration */
        service_flag = node_attr_ptr->service_flags_ptr;

        if (flag_order < node_attr_ptr->number_support_sid)
        {
            service_flag[flag_order] = 0U;
        }
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : lin_diag_service_callback
 * Description   : Callback handler for dispatching diagnostic services
 *
 * Implements    : lin_diag_service_callback_Activity
 *END**************************************************************************/
void lin_diag_service_callback(l_ifc_handle iii,
                               l_u8 sid)
{
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    const lin_node_attribute_t * node_attr_ptr;
    node_attr_ptr = &g_lin_node_attribute_array[prot_user_config_ptr->slave_ifc_handle];
    l_bool sid_supported_flg = (bool)0U;
    l_u8 i;
    const l_u8 * service_supported_ptr;
    l_u8 * service_flag_ptr;
    /* Get support sid */
    service_supported_ptr = node_attr_ptr->service_supported_ptr;
    /* Get service flag */
    service_flag_ptr = node_attr_ptr->service_flags_ptr;

    for (i = 0U; i < node_attr_ptr->number_support_sid; i++)
    {
        if (service_supported_ptr[i] == sid)
        {
            service_flag_ptr[i] = 1U;
            sid_supported_flg = (bool)1U;
            break;
        }
    }

    if (sid_supported_flg == (bool)1U)
    {
        /* Check whether or not the Service is supported by the Slave node */
        switch (sid)
        {
#if !SUPPORT_BOOTLOADER
            case SERVICE_READ_BY_IDENTIFY:    /* Mandatory for TL LIN 2.1 & 2.0, Optional for J2602 */
                lin_diagservice_read_by_identifier(iii);
                break;

            #if ((1U == SUPPORT_PROTOCOL_21) || (1U == SUPPORT_PROTOCOL_20))
            case SERVICE_CONDITIONAL_CHANGE_NAD:
                if ((LIN_PROTOCOL_21 == prot_user_config_ptr->protocol_version) || (LIN_PROTOCOL_20 == prot_user_config_ptr->protocol_version))
                {
                    lin_condittional_change_nad(iii);
                }
                break;
            case SERVICE_ASSIGN_NAD:
                if ((LIN_PROTOCOL_21 == prot_user_config_ptr->protocol_version) || (LIN_PROTOCOL_20 == prot_user_config_ptr->protocol_version))
                {
                    lin_assign_nad(iii);
                }
                break;

            #if (1U == SUPPORT_PROTOCOL_21)
            case SERVICE_ASSIGN_FRAME_ID_RANGE:                  /* Mandatory for TL LIN 2.1 */
                if (LIN_PROTOCOL_21 == prot_user_config_ptr->protocol_version)
                {
                    lin_diagservice_assign_frame_id_range(iii);
                }
                break;
            case SERVICE_SAVE_CONFIGURATION:
                /* Set save configuration flag */
                g_lin_protocol_state_array[iii].save_config_flg = (l_bool)1U;
                /* Response to master - RSID */
                ld_make_slave_response_pdu(iii, SERVICE_SAVE_CONFIGURATION, POSITIVE, 0U);
                break;
            #endif /* (1U == SUPPORT_PROTOCOL_21) */

            #endif /* ((1U == SUPPORT_PROTOCOL_21) || (1U == SUPPORT_PROTOCOL_20)) */

            #if ((1U == SUPPORT_PROTOCOL_J2602) || (1U == SUPPORT_PROTOCOL_20))
            case SERVICE_ASSIGN_FRAME_ID:
                if (LIN_PROTOCOL_J2602 == prot_user_config_ptr->protocol_version || LIN_PROTOCOL_20 == prot_user_config_ptr->protocol_version)
                {
                    lin_diagservice_assign_frame_id(iii);
                }
                break;
            #if (1U == SUPPORT_PROTOCOL_J2602)
            case SERVICE_TARGET_RESET:
                if (LIN_PROTOCOL_J2602 == prot_user_config_ptr->protocol_version)
                {
                    lin_diagservice_target_reset(iii);
                }
                break;
            #endif /* (1U == SUPPORT_PROTOCOL_J2602) */

            #endif /* ((1U == SUPPORT_PROTOCOL_J2602) || (1U == SUPPORT_PROTOCOL_20)) */
#endif
            case SERVICE_SESSION_CONTROL: //诊断会话控制
                //诊断会话控制切换会话
                lin_slave_session_ctr(iii);
                break;
            case 0x11: //电控单元复位
                lin_ecu_reset(iii);
                break;
            case 0x27: //安全访问
                lin_security_visit(iii);
                break;
            case 0x2e: //标识符写数据
                lin_write_data_by_identify(iii);
                break;
    
#if !SUPPORT_BOOTLOADER
            case SERVICE_READ_DATA_BY_IDENTIFY:
                //根据id读取数据信息
                lin_read_data_by_identify(iii);
                break;
            case 0x85:
                control_dtc_setting(iii);
                break;
#endif
            case SERIVCE_ROUTINE_CONTROL:
                //路由控制,支持在扩展模式下
                lin_routine_control(iii);
                break;
            case 0x34: //请求下载
                //先请求下载，再传输数据
                lin_download_request(iii);
                break;
            case 0x36:
                lin_transfer_data(iii);
                break;
            case 0x37: //请求退出传输
                lin_download_exit(iii);
                break;
            case 0x3e: //待机握手
                break;
            default:
                /* do nothing */
                break;
        } /* end of switch */
    }
    else
    {
        ld_make_slave_response_pdu(iii, sid, NEGATIVE, SERVICE_NOT_SUPPORTED);
        /* clear queue */
        tl_desc_ptr->tl_rx_queue.queue_status = LD_NO_DATA;
        tl_desc_ptr->tl_rx_queue.queue_current_size = 0U;
        tl_desc_ptr->tl_rx_queue.queue_header = tl_desc_ptr->tl_rx_queue.queue_tail;
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : lin_diagservice_read_by_identifier
 * Description   : This function to process read by identifier request, and also prepare its response data.
 *                 Only for Slave Nodes.
 *
 * Implements    : lin_diagservice_read_by_identifier_Activity
 *END**************************************************************************/
#if !SUPPORT_BOOTLOADER
static void lin_diagservice_read_by_identifier(l_ifc_handle iii)
{
    lin_product_id_t product_id;
    lin_tl_pdu_data_t lin_tl_pdu;
    l_u8 id;
    l_u16 supid;
    l_u16 fid;
    const lin_node_attribute_t * node_attr_ptr;
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];
    node_attr_ptr = &g_lin_node_attribute_array[prot_user_config_ptr->slave_ifc_handle];
    const lin_transport_layer_queue_t * rx_queue = &(tl_desc_ptr->tl_rx_queue);
    l_u8 i;
    for (i = 0; i < 8U; i++)
    {
        lin_tl_pdu[i] = rx_queue->tl_pdu_ptr[rx_queue->queue_header][i];
    }

    /* Get the product identification */
    product_id = node_attr_ptr->product_id;

    /* Get supplier and function identification in request */
    supid = (l_u16)(((l_u16)(lin_tl_pdu[5])) << 8U);
    supid = (l_u16)(supid | (l_u16)(lin_tl_pdu[4]));

    fid = (l_u16)(((l_u16)(lin_tl_pdu[7])) << 8U);
    fid = (l_u16)(fid | (l_u16)(lin_tl_pdu[6]));


    /* Check Supplier ID and Function ID */
    if (((supid != product_id.supplier_id) && (supid != LD_ANY_SUPPLIER)) ||
        ((fid != product_id.function_id) && (fid != LD_ANY_FUNCTION)))
    {
        tl_desc_ptr->slave_resp_cnt = 0U;
    }
    else
    {
        /* Get the identifier of request */
        id = lin_tl_pdu[3];

        switch (id)
        {
            case LIN_PRODUCT_ID:
                ld_make_slave_response_pdu(iii, SERVICE_READ_BY_IDENTIFY, POSITIVE, id);
                break;
            case LIN_SERIAL_NUMBER:
                ld_make_slave_response_pdu(iii, SERVICE_READ_BY_IDENTIFY, POSITIVE, id);
                break;
            default:
                /* For ID from 32 to 63, call user defined ld_read_by_id_callout */
                if ((id >= LIN_READ_USR_DEF_MIN) && (id <= LIN_READ_USR_DEF_MAX))
                {
                    l_u8 data_callout[5] = {0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU};
                    l_u8 retval = ld_read_by_id_callout(iii, id, data_callout);
                    /*If the User ID is supported, make positive response*/
                    if (retval == LD_POSITIVE_RESPONSE)
                    {
                        i = 0U;
                        while ((i < 5U) && (data_callout[i] == 0xFFU))
                        {
                            i++;
                        }
                        if (i < 5U)
                        {
                            ld_make_slave_response_pdu(iii, SERVICE_READ_BY_IDENTIFY, POSITIVE, id);
                        }
                        else
                        {
                            ld_make_slave_response_pdu(iii, SERVICE_READ_BY_IDENTIFY, NEGATIVE, SUBFUNCTION_NOT_SUPPORTED);
                        }
                    }
                    /*If the User ID is not supported, make negative response*/
                    else if (retval == LD_NEGATIVE_RESPONSE)
                    {
                        /* Make a negative slave response PDU */
                        ld_make_slave_response_pdu(iii, SERVICE_READ_BY_IDENTIFY, NEGATIVE, SUBFUNCTION_NOT_SUPPORTED);
                    }
                    else
                    {
                        /*Do not answer*/
                        tl_desc_ptr->slave_resp_cnt = 0;
                        tl_desc_ptr->service_status = LD_SERVICE_IDLE;
                    }
                }
                /* For ID from 2 to 31 or 64-255, give negative response */
                else
                {
                    /* Make a negative slave response PDU */
                    ld_make_slave_response_pdu(iii, SERVICE_READ_BY_IDENTIFY, NEGATIVE, SUBFUNCTION_NOT_SUPPORTED);
                }

                break;
        } /* end of switch */
    }
}
#endif
/*FUNCTION**********************************************************************
 *
 * Function Name : ld_make_slave_response_pdu
 *
 * Description   : Creates PDUs for the transport layer for specified diagnostic service
 *                 and adds them to the transmit queue.
 *                 This function is implemented for Slave only.
 *                 这个函数实现 Service ID (B0 B1 B2 B3 B5 B6 B7) 在bootloader中都没啥用，酌情优化
 *
 * Implements    : OK-zhu
 *END**************************************************************************/
static void ld_make_slave_response_pdu(l_ifc_handle iii,
                                       l_u8 sid,
                                       l_u8 res_type,
                                       l_u8 error_code)
{
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];
    const lin_node_attribute_t * node_attr_ptr;
    node_attr_ptr = &g_lin_node_attribute_array[prot_user_config_ptr->slave_ifc_handle];
    const lin_product_id_t * ident;
    const lin_serial_number_t * serial_number;
    l_u8 i = 0U;
    l_u8 NAD;
    lin_tl_pdu_data_t lin_tl_pdu;
    const lin_transport_layer_queue_t * rx_queue;

    /* Get receive queue */
    rx_queue = &(tl_desc_ptr->tl_rx_queue);
    NAD = rx_queue->tl_pdu_ptr[rx_queue->queue_header][0];
    if (NAD != LD_FUNCTIONAL_NAD)
    {
        /* Pack data to response PDU */
        lin_tl_pdu[0] = *node_attr_ptr->configured_NAD_ptr;
        lin_tl_pdu[1] = 0x03U;        /* PCI */
        lin_tl_pdu[2] = RES_NEGATIVE; /* SID */
        lin_tl_pdu[3] = sid;          /* D0 */
        lin_tl_pdu[4] = error_code;   /* D1 */
        lin_tl_pdu[5] = 0xFFU;        /* D2 */
        lin_tl_pdu[6] = 0xFFU;        /* D3 */
        lin_tl_pdu[7] = 0xFFU;        /* D4 */

        switch (sid)
        {
            case SERVICE_READ_BY_IDENTIFY:  //B2
                if (POSITIVE == res_type)
                {
                    /* PCI type */
                    lin_tl_pdu[1] = PCI_RES_READ_BY_IDENTIFY;
                    /* SID */
                    lin_tl_pdu[2] = (l_u8)(RES_POSITIVE + sid);

                    if (error_code == LIN_PRODUCT_ID)
                    {
                        /* Get Identifier info */
                        ident = (const lin_product_id_t *)(&node_attr_ptr->product_id);
                        lin_tl_pdu[3] = (l_u8)(ident->supplier_id & 0xFFU);
                        lin_tl_pdu[4] = (l_u8)(ident->supplier_id >> 8);
                        lin_tl_pdu[5] = (l_u8)(ident->function_id & 0xFFU);
                        lin_tl_pdu[6] = (l_u8)(ident->function_id >> 8);
                        lin_tl_pdu[7] = ident->variant;
                    }
                    else if (error_code == LIN_SERIAL_NUMBER)
                    {
                        serial_number = (const lin_serial_number_t *)(&node_attr_ptr->serial_number);
                        lin_tl_pdu[3] = serial_number->serial_0;
                        lin_tl_pdu[4] = serial_number->serial_1;
                        lin_tl_pdu[5] = serial_number->serial_2;
                        lin_tl_pdu[6] = serial_number->serial_3;
                        /* PCI for Serial Number is 0x05 */
                        lin_tl_pdu[1] = 0x05U;
                    }
                    else
                    {
                        l_u8 data_callout[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
                        (void)ld_read_by_id_callout(iii, error_code, data_callout);
                        /* packing user defined pdu */
                        lin_tl_pdu[3] = data_callout[0];
                        lin_tl_pdu[4] = data_callout[1];
                        lin_tl_pdu[5] = data_callout[2];
                        lin_tl_pdu[6] = data_callout[3];
                        lin_tl_pdu[7] = data_callout[4];
                        /* Check for data values*/
                        for (i = 5U; i > 0U; i--)
                        {
                            if (data_callout[i - 1U] != 0xFFU)
                            {
                                /* PCI: Data length is 1 (RSID) + all data exclude 0xFF */
                                lin_tl_pdu[1] = (l_u8)(i + 1U);
                                break;
                            }
                        }
                    }
                }
                break;
            case SERVICE_ASSIGN_FRAME_ID:   //B1
                lin_tl_pdu[0] = *node_attr_ptr->configured_NAD_ptr;
                lin_tl_pdu[1] = 0x01U;      /* PCI */
                lin_tl_pdu[2] = 0xF1U;      /* SID */
                lin_tl_pdu[3] = 0xFFU;      /* D0 */
                lin_tl_pdu[4] = 0xFFU;      /* D1 */
                break;
            case SERVICE_ASSIGN_NAD:        //B0
                lin_tl_pdu[0] = node_attr_ptr->initial_NAD;
                *node_attr_ptr->configured_NAD_ptr = rx_queue->tl_pdu_ptr[rx_queue->queue_header][7];
                /* PCI */
                lin_tl_pdu[1] = 0x01U;
                /* RSID */
                lin_tl_pdu[2] = 0xF0U;
                lin_tl_pdu[3] = 0xFFU;
                lin_tl_pdu[4] = 0xFFU;
                break;
            case SERVICE_CONDITIONAL_CHANGE_NAD:  //B3
                /* PCI */
                lin_tl_pdu[1] = 0x01U;
                /* RSID */
                lin_tl_pdu[2] = 0xF3U;
                lin_tl_pdu[3] = 0xFFU;
                lin_tl_pdu[4] = 0xFFU;
                break;

            #if (1U == SUPPORT_PROTOCOL_21)
            case SERVICE_SAVE_CONFIGURATION:     //B6
                /* PCI type */
                lin_tl_pdu[1] = PCI_RES_SAVE_CONFIGURATION;
                /* SID */
                lin_tl_pdu[2] = (l_u8)(RES_POSITIVE + sid);
                /* Data unused */
                lin_tl_pdu[3] = 0xFFU;
                lin_tl_pdu[4] = 0xFFU;
                break;
            case SERVICE_ASSIGN_FRAME_ID_RANGE:  /* Mandatory for TL LIN 2.1 */  //B7
                if (POSITIVE == res_type)
                {
                    lin_tl_pdu[1] = PCI_RES_ASSIGN_FRAME_ID_RANGE;
                    lin_tl_pdu[2] = (l_u8)(RES_POSITIVE + sid);
                    lin_tl_pdu[3] = 0xFFU;
                    lin_tl_pdu[4] = 0xFFU;
                }
                break;
            #endif /* (1U == SUPPORT_PROTOCOL_21) */

            #if (1U == SUPPORT_PROTOCOL_J2602)
            case SERVICE_TARGET_RESET:
                lin_tl_pdu[1] = 0x06U;
                lin_tl_pdu[2] = (l_u8)(RES_POSITIVE + sid);
                ident = (lin_product_id_t *)&node_attr_ptr->product_id;
                lin_tl_pdu[3] = (l_u8)(ident->supplier_id & 0xFFU);
                lin_tl_pdu[4] = (l_u8)(ident->supplier_id >> 8U);
                lin_tl_pdu[5] = (l_u8)(ident->function_id & 0xFFU);
                lin_tl_pdu[6] = (l_u8)(ident->function_id >> 8U);
                lin_tl_pdu[7] = ident->variant;
                break;
            #endif /* (1U == SUPPORT_PROTOCOL_J2602) */

            default:
                /* do nothing */
                break;
        } /* end of switch statement */

        /* Put lin_tl_pdu data into transmit queue*/
        ld_put_raw(iii, lin_tl_pdu);
        tl_desc_ptr->diag_state  = LD_DIAG_TX_PHY;
    }
    else
    {
        tl_desc_ptr->diag_state  = LD_DIAG_IDLE;
    }
}
#endif /* (1U == SUPPORT_SLAVE_MODE) */

#if ((1U == SUPPORT_PROTOCOL_J2602) || (1U == SUPPORT_PROTOCOL_20))
/*FUNCTION**********************************************************************
 *
 * Function Name : ld_assign_frame_id
 * Description   : This function assigns the protected identifier of up to four frames
 *  in the slave node with the addressed NAD (using for J2602 and LIN 2.0).
 *
 * Implements    : ld_assign_frame_id_Activity
 *END**************************************************************************/
void ld_assign_frame_id(l_ifc_handle iii,
                        l_u8 NAD,
                        l_u16 supplier_id,
                        l_u16 message_id,
                        l_u8 PID)
{
    DEV_ASSERT((l_u8)iii < LIN_NUM_OF_IFCS);

    l_u8 data[6];
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];

    if (prot_user_config_ptr->function == (bool)LIN_MASTER)
    {
        /* check service is busy? */
        if (LD_SERVICE_BUSY != tl_desc_ptr->service_status)
        {
            /* SID of Service assign frame identifier: 0xB1 */
            data[0] = SERVICE_ASSIGN_FRAME_ID;
            data[1] = (l_u8)(supplier_id & 0x00FFU);
            data[2] = (l_u8)((supplier_id >> 8U) & 0x00FFU);
            data[3] = (l_u8)(message_id & 0x00FFU);
            data[4] = (l_u8)((message_id >> 8U) & 0x00FFU);
            data[5] = PID;

            /* put data into TX_QUEUE */
            ld_send_message(iii, 6U, NAD, data);

            /* Set state of service is BUSY */
            tl_desc_ptr->service_status = LD_SERVICE_BUSY;
        }  /* End of checking service status */
    }
}

#if (1U == SUPPORT_PROTOCOL_J2602)
/*FUNCTION**********************************************************************
 *
 * Function Name : ld_is_ready_j2602
 * Description   : Verifies a state of node setting (using for J2602 and LIN 2.0).
 *
 * Implements    : ld_is_ready_j2602_Activity
 *END**************************************************************************/
l_bool ld_is_ready_j2602(l_ifc_handle iii)
{
    DEV_ASSERT((l_u8)iii < LIN_NUM_OF_IFCS);

    l_bool retVal = false;
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];

    if (prot_user_config_ptr->function == (bool)LIN_MASTER)
    {
        /* Check the service status */
        if (LD_SERVICE_BUSY != tl_desc_ptr->service_status)
        {
            retVal = true;
        }
    }

    return retVal;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ld_check_response_j2602
 * Description   : Verifies the state of response (using for J2602 and LIN 2.0)
 *
 * Implements    : ld_check_response_j2602_Activity
 *END**************************************************************************/
l_u8 ld_check_response_j2602(l_ifc_handle iii,
                             l_u8 * const RSID,
                             l_u8 * const error_code)
{
    DEV_ASSERT((l_u8)iii < LIN_NUM_OF_IFCS);

    l_u8 retval = 0xFFU;
    lin_last_cfg_result_t temp;
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];

    if (prot_user_config_ptr->function == (bool)LIN_MASTER)
    {
        /* Get the status of the last service */
        temp = tl_desc_ptr->last_cfg_result;
        /* Check status of last configuration */
        switch (temp)
        {
            case LD_SUCCESS:
                *RSID = tl_desc_ptr->last_RSID;
                break;
            case LD_NEGATIVE:
                *RSID = tl_desc_ptr->last_RSID;
                *error_code = tl_desc_ptr->ld_error_code;
                break;
            default:
                /* do nothing */
                break;
        } /* end of switch */

        retval = (l_u8)temp;
    }

    return retval;
}
#endif /* (1U == SUPPORT_PROTOCOL_J2602) */

#if (1U == SUPPORT_SLAVE_MODE)
/*FUNCTION**********************************************************************
 *
 * Function Name : lin_diagservice_assign_frame_id
 * Description   : Process assign frame id request, and also prepare its response data
 *                 and change protected identifier of frame have correct message
 *                 identifier. This function is only for Slave Nodes
 *
 * Implements    : lin_diagservice_assign_frame_id_Activity
 *END**************************************************************************/
static void lin_diagservice_assign_frame_id(l_ifc_handle iii)
{
    l_u8 i;
    l_u8 id;
    l_u16 supplier_id;
    l_u16 message_id;
    lin_tl_pdu_data_t lin_tl_pdu;
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];
    const lin_transport_layer_queue_t * rx_queue = &(g_lin_tl_descriptor_array[iii].tl_rx_queue);
    l_u16 slave_supplier_id = g_lin_node_attribute_array[prot_user_config_ptr->slave_ifc_handle].product_id.supplier_id;

    /* Get data from queue */
    for (i = 0U; i < 8U; i++)
    {
        lin_tl_pdu[i] = rx_queue->tl_pdu_ptr[rx_queue->queue_header][i];
    }

    supplier_id = (l_u16)((l_u16)lin_tl_pdu[3] | (l_u16)(lin_tl_pdu[4] << 8U));
    message_id = (l_u16)((l_u16)lin_tl_pdu[5] | (l_u16)(lin_tl_pdu[6] << 8U));

    if (supplier_id == slave_supplier_id)
    {
        id = lin_process_parity(lin_tl_pdu[7], CHECK_PARITY);
        /* Checking id is correct */
        if (0xFFU != id)
        {
            for (i = (l_u8)(prot_user_config_ptr->number_of_configurable_frames - 2U); i > 0U; i--)
            {
                if (prot_user_config_ptr->list_identifiers_ROM_ptr[i] == message_id)
                {
                    prot_user_config_ptr->list_identifiers_RAM_ptr[i] = id;
                    /* Send positive response if assign successfully */
                    ld_make_slave_response_pdu(iii, SERVICE_ASSIGN_FRAME_ID, POSITIVE, 0U);
                    break;
                }
            }
        }
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ld_reconfig_msg_ID
 * Description   : This function reconfigures frame identifiers of a J2602 slave node
 *                 based on input dnn. This function is for slave nodes only.
 *
 * Implements    : ld_reconfig_msg_ID_Activity
 *END**************************************************************************/
l_bool ld_reconfig_msg_ID(l_ifc_handle iii,
                          l_u8 dnn)
{
    DEV_ASSERT((l_u8)iii < LIN_NUM_OF_IFCS);
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];
    l_u8 i;
    l_bool ret_val = 1U;
    /* Get number of unconditional frames not calculate id 0x3C and 0x3D */
    l_u8 number_of_unconditional_frames = (l_u8)(prot_user_config_ptr->number_of_configurable_frames - 2);
    i = number_of_unconditional_frames + prot_user_config_ptr->frame_start - 1;

    /* Calculate number of unconditional frames by removing sporadic frame */
    for(; i >= 0; i--)
    {
        if(prot_user_config_ptr->frame_tbl_ptr[i].frm_type == LIN_FRM_UNCD)
        {
            break;
        }
        else
        {
            number_of_unconditional_frames--;
        }
    }

    /* Check if this interface is a LIN Slave and the protocol is J2602 */
    if ((prot_user_config_ptr->function == LIN_SLAVE) && (prot_user_config_ptr->protocol_version == LIN_PROTOCOL_J2602))
    {
        if (dnn <= 0xDU)
        {
            /* number of unconditional frames greater than 16 */
            if (number_of_unconditional_frames > 16U)
            {
                /* Only 0x60 is valid NAD */
                /* Do nothing */
            }
            /* number of unconditional frames is from 9 - 16 */
            else if (number_of_unconditional_frames > 8U)
            {
                /* Only NAD 0x60, 0x64, 0x68 are valid, 0x6C and 0x6D not valid */
                if ((dnn == 0U) || (dnn == 4U) || (dnn == 8U))
                {
                    ret_val = ld_change_msg_id(iii, dnn, 16U);
                }
            }
            /* number of unconditional frames is from 5 - 8 */
            else if (number_of_unconditional_frames > 4U)
            {
                /* Check to verify if dnn is 0x60, 0x62, 0x64, 0x66, 0x68, 0x6A, 0x6C */
                if ((dnn % 2U) == 0U)
                {
                    ret_val = ld_change_msg_id(iii, dnn, 8U);
                }
            }
            /* number of unconditional frames is from 1 - 4 */
            else
            {
                ret_val = ld_change_msg_id(iii, dnn, 4U);
            }
        }
    }

    return ret_val;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ld_change_msg_id
 * Description   : This function reconfigures frame identifiers of a J2602 slave node
 *                 based on input dnn and frame_id_change.
 *
 * Implements    : ld_change_msg_id_Activity
 *END**************************************************************************/
static l_bool ld_change_msg_id(l_ifc_handle iii,
                               l_u8 dnn,
                               l_u8 frame_id_change)
{
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];
    l_u8 number_of_configurable_frames = prot_user_config_ptr->number_of_configurable_frames;
    l_u8 i;
    l_u8 id_origin;
    /* If new DNN is greater than current DNN */
    for (i = number_of_configurable_frames; i > 0U; i--)
    {
        /* For non-broadcast frame identifiers  less than or equal to 0x37 */
        if (prot_user_config_ptr->list_identifiers_RAM_ptr[i] <= 0x37U)
        {
            /* get id with dnn equal 0 */
            id_origin = (l_u8)(prot_user_config_ptr->list_identifiers_RAM_ptr[i] % frame_id_change);
            prot_user_config_ptr->list_identifiers_RAM_ptr[i] = (l_u8)(id_origin + (l_u8)(dnn << 2));
        }
        /* For broad cast message ID */
        else
        {
            if ((dnn  >= 8U) &&
                ((prot_user_config_ptr->list_identifiers_RAM_ptr[i] == 0x38U) || (prot_user_config_ptr->list_identifiers_RAM_ptr[i] == 0x3AU)))
            {
                prot_user_config_ptr->list_identifiers_RAM_ptr[i] += 1U;
            }
            else if ((dnn  < 8U) &&
                     ((prot_user_config_ptr->list_identifiers_RAM_ptr[i] == 0x39U) || (prot_user_config_ptr->list_identifiers_RAM_ptr[i] == 0x3BU)))
            {
                prot_user_config_ptr->list_identifiers_RAM_ptr[i] -= 1U;
            }
        }
    }

    return (l_bool)0U;
}

#if (1U == SUPPORT_PROTOCOL_J2602)
/*FUNCTION**********************************************************************
 *
 * Function Name : ld_assign_NAD_j2602
 * Description   : This function assigns NAD of a J2602 slave device based on input
 *                 dnn. NAD is (0x60 + dnn).
 *
 * Implements    : ld_assign_NAD_j2602_Activity
 *END**************************************************************************/
l_bool ld_assign_NAD_j2602(l_ifc_handle iii,
                           l_u8 dnn)
{
    DEV_ASSERT((l_u8)iii < LIN_NUM_OF_IFCS);
    const lin_protocol_user_config_t * prot_user_config_ptr = &g_lin_protocol_user_cfg_array[iii];
    const lin_node_attribute_t * node_attr_ptr;
    l_bool ret_val = 1U;

    if ((prot_user_config_ptr->function == (bool)LIN_SLAVE) && (prot_user_config_ptr->protocol_version == LIN_PROTOCOL_J2602))
    {
        node_attr_ptr = &g_lin_node_attribute_array[g_lin_protocol_user_cfg_array[iii].slave_ifc_handle];
        if (dnn <= 0xFU)
        {
            *(node_attr_ptr->configured_NAD_ptr) = (l_u8)(0x60U + dnn);
            ret_val = 0U;
        }
    }

    return ret_val;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : lin_diagservice_target_reset
 * Description   : Process read by identifier request, and also prepare its response data
 *                 and Reset the Slave Node. This function is only for Slave Node
 *
 * Implements    : lin_diagservice_target_reset_Activity
 *END**************************************************************************/
static void lin_diagservice_target_reset(l_ifc_handle iii)
{
    lin_tl_descriptor_t * tl_desc_ptr = &g_lin_tl_descriptor_array[iii];
    const lin_node_attribute_t * node_attr_ptr = &g_lin_node_attribute_array[g_lin_protocol_user_cfg_array[iii].slave_ifc_handle];
    l_u16* byte_offset_temp_ptr;
    l_u8* bit_offset_temp_ptr;
    l_u8 i;
    const lin_transport_layer_queue_t * rx_queue = &(tl_desc_ptr->tl_rx_queue);

    for (i = 0; i < node_attr_ptr->num_frame_have_esignal; i++)
    {
        /* Set the reset flag within the J2602 Status Byte */
        byte_offset_temp_ptr = node_attr_ptr->response_error_byte_offset_ptr + i;
        bit_offset_temp_ptr = node_attr_ptr->response_error_bit_offset_ptr + i;
        /* Set error signal to 0x01 means "Reset" */
        g_lin_frame_data_buffer[*byte_offset_temp_ptr] = (l_u8)((g_lin_frame_data_buffer[*byte_offset_temp_ptr] & (~(0x07U << (*bit_offset_temp_ptr)))) |
                                                    (0x01U << (*bit_offset_temp_ptr)));
    }
    /* check if pdu[0] - NAD is different from LD_BROADCAST */
    if (LD_BROADCAST != rx_queue->tl_pdu_ptr[(rx_queue->queue_header)][0])
    {
        ld_make_slave_response_pdu(iii, SERVICE_TARGET_RESET, POSITIVE, 0U);
    }
    else
    {
        tl_desc_ptr->slave_resp_cnt = 0U;
    }
}
#endif /* (1U == SUPPORT_PROTOCOL_J2602) */

#endif /* (1U == SUPPORT_SLAVE_MODE) */

#endif /* ((1U == SUPPORT_PROTOCOL_J2602) || (1U == SUPPORT_PROTOCOL_20)) */

#endif /* (1U == SUPPORT_DIAG_SERVICE) */

#endif /* (1U == SUPPORT_TRANSPORT_LAYER) */
/*******************************************************************************
 * EOF
 ******************************************************************************/
