#include "ep40_function_specification.h"
#include "atmel_start_pins.h"
#include "touch.h"
#include "lin.h"
#include "lin_cfg.h"

#define BTN_NORMAL_NUM 8U
#define BTN_L_WIND_ID 0
#define BTN_L_HEAT_ID 1
#define BTN_R_WIND_ID 3
#define BTN_R_HEAT_ID 2
#define BTN_L_SEAT_F_ID 4
#define BTN_L_SEAT_B_ID 7
#define BTN_R_SEAT_F_ID 6
#define BTN_R_SEAT_B_ID 5

uint8_t wind_config = 0;
uint8_t heat_config = 0;

typedef struct
{
    uint8_t cnt;
    uint8_t val;
    uint8_t satus;
} lin_signal_cnt_t;

lin_signal_cnt_t g_lin_signal_cnt[LIN_NUM_OF_SIGS];

static btn_normal_t btns_obj[BTN_NORMAL_NUM];

void lin_signal_send_x_times(l_signal_handle signal_handle, uint8_t cnt, uint8_t val)
{
    g_lin_signal_cnt[signal_handle].cnt = cnt;
    g_lin_signal_cnt[signal_handle].val = val;
}

void ep40_integrate_all_signal(void)
{

    /*  电容按键的状态->   ---  从节点更新信号状态
    *   主机的LIN信号状态-> ---
    * 
    */
    uint8_t signal_temp_data;
    if(wind_config)
    {
        if(btns_obj[BTN_L_WIND_ID].cap_trig_flg)
        {
            signal_temp_data = l_u8_rd_LI0_RSCU1_RL_Venting_Status_Reserved();
            if(signal_temp_data == 0x0) //off
            {
                lin_signal_send_x_times(LI0_RATS1_RLSeatVentCmd,3,0x2);
            }else if(signal_temp_data == 0x1) //step 1
            {
                lin_signal_send_x_times(LI0_RATS1_RLSeatVentCmd,3,0x3);
            }else if(signal_temp_data == 0x2) //step 2
            {
                lin_signal_send_x_times(LI0_RATS1_RLSeatVentCmd,3,0x4);
            }else if(signal_temp_data == 0x3) //step 2
            {
                lin_signal_send_x_times(LI0_RATS1_RLSeatVentCmd,3,0x1);
            }  
            btns_obj[BTN_L_WIND_ID].cap_trig_flg = 0;
        }

        if(btns_obj[BTN_R_WIND_ID].cap_trig_flg)
        {
            signal_temp_data = l_u8_rd_LI0_RSCU1_RR_Venting_Status();
            if(signal_temp_data == 0x0) //off
            {
                lin_signal_send_x_times(LI0_RATS1_RRSeatVentCmd,3,0x2);
            }else if(signal_temp_data == 0x1) //step 1
            {
                lin_signal_send_x_times(LI0_RATS1_RRSeatVentCmd,3,0x3);
            }else if(signal_temp_data == 0x2) //step 2
            {
                lin_signal_send_x_times(LI0_RATS1_RRSeatVentCmd,3,0x4);
            }else if(signal_temp_data == 0x3) //step 2
            {
                lin_signal_send_x_times(LI0_RATS1_RRSeatVentCmd,3,0x1);
            }  
            btns_obj[BTN_R_WIND_ID].cap_trig_flg = 0;
        }
    }

    if(heat_config)
    {
        if(btns_obj[BTN_L_HEAT_ID].cap_trig_flg) //触发状态
        {
            signal_temp_data = l_u8_rd_LI0_RSCU1_RL_Heating_Status();
            if(signal_temp_data == 0x0)       //off
            {
                lin_signal_send_x_times(LI0_RATS1_RLSeatHeaterCmd,3,0x2);
            }else if(signal_temp_data == 0x1) //step 1
            {
                lin_signal_send_x_times(LI0_RATS1_RLSeatHeaterCmd,3,0x3);
            }else if(signal_temp_data == 0x2) //step 2
            {
                lin_signal_send_x_times(LI0_RATS1_RLSeatHeaterCmd,3,0x4);
            }else if(signal_temp_data == 0x3) //step 2
            {
                lin_signal_send_x_times(LI0_RATS1_RLSeatHeaterCmd,3,0x1);
            }      
            btns_obj[BTN_L_HEAT_ID].cap_trig_flg = 0;
        }
        if(btns_obj[BTN_R_HEAT_ID].cap_trig_flg)
        {
            signal_temp_data = l_u8_rd_LI0_RSCU1_RR_Heating_Status();
            if(signal_temp_data == 0x0)       //off
            {
                lin_signal_send_x_times(LI0_RATS1_RRSeatHeaterCmd,3,0x2);
            }else if(signal_temp_data == 0x1) //step 1
            {
                lin_signal_send_x_times(LI0_RATS1_RRSeatHeaterCmd,3,0x3);
            }else if(signal_temp_data == 0x2) //step 2
            {
                lin_signal_send_x_times(LI0_RATS1_RRSeatHeaterCmd,3,0x4);
            }else if(signal_temp_data == 0x3) //step 2
            {
                lin_signal_send_x_times(LI0_RATS1_RRSeatHeaterCmd,3,0x1);
            }        
            btns_obj[BTN_R_HEAT_ID].cap_trig_flg = 0;
        }
    }

    if(btns_obj[BTN_L_SEAT_F_ID].cap_trig_flg) //左侧座椅靠背前调节
    {
        lin_signal_send_x_times(LI0_RATS1_RLSeatBackrestAdjCmd,3,0x1);
        btns_obj[BTN_L_SEAT_F_ID].cap_trig_flg = 0;
    }
    if(btns_obj[BTN_L_SEAT_B_ID].cap_trig_flg) //左侧座椅靠背前调节
    {
        lin_signal_send_x_times(LI0_RATS1_RLSeatBackrestAdjCmd,3,0x2);
        btns_obj[BTN_L_SEAT_B_ID].cap_trig_flg = 0;
    }
    if(btns_obj[BTN_R_SEAT_F_ID].cap_trig_flg) //左侧座椅靠背前调节
    {
        lin_signal_send_x_times(LI0_RATS1_RRSeatBackrestAdjCmd,3,0x1);
        btns_obj[BTN_R_SEAT_F_ID].cap_trig_flg = 0;
    }
    if(btns_obj[BTN_R_SEAT_B_ID].cap_trig_flg) //左侧座椅靠背前调节
    {
        lin_signal_send_x_times(LI0_RATS1_RRSeatBackrestAdjCmd,3,0x2);
        btns_obj[BTN_R_SEAT_B_ID].cap_trig_flg = 0;
    }
}

void ep40_lin_signal_update_while_task(void)
{
    for (uint8_t i = 0; i < LIN_NUM_OF_SIGS; i++)
    {
        switch (i)
        {
        case LI0_RATS1_RLSeatHeaterCmd:  //左座椅加热信号
            if (g_lin_signal_cnt[i].cnt > 0)
            {
                g_lin_signal_cnt[i].cnt--;
                l_u8_wr_LI0_RATS1_RLSeatHeaterCmd(g_lin_signal_cnt[i].val);
            }
            else
            {
                l_u8_wr_LI0_RATS1_RLSeatHeaterCmd(0);
            }
            break;
        case LI0_RATS1_RRSeatHeaterCmd:  //右座椅加热
            if (g_lin_signal_cnt[i].cnt > 0)
            {
                g_lin_signal_cnt[i].cnt--;
                l_u8_wr_LI0_RATS1_RRSeatHeaterCmd(g_lin_signal_cnt[i].val);
            }
            else
            {
                l_u8_wr_LI0_RATS1_RRSeatHeaterCmd(0);
            }
            break;
        case LI0_RATS1_RLSeatVentCmd:    //右座椅通风
            if (g_lin_signal_cnt[i].cnt > 0)
            {
                g_lin_signal_cnt[i].cnt--;
                l_u8_wr_LI0_RATS1_RLSeatVentCmd(g_lin_signal_cnt[i].val);
            }
            else
            {
                l_u8_wr_LI0_RATS1_RLSeatVentCmd(0);
            }
            break;
        case LI0_RATS1_RRSeatVentCmd:    //右座椅通风
            if (g_lin_signal_cnt[i].cnt > 0)
            {
                g_lin_signal_cnt[i].cnt--;
                l_u8_wr_LI0_RATS1_RRSeatVentCmd(g_lin_signal_cnt[i].val);
            }
            else
            {
                l_u8_wr_LI0_RATS1_RRSeatVentCmd(0);
            }
            break;
        case LI0_RATS1_RLSeatBackrestAdjCmd:
            if (g_lin_signal_cnt[i].cnt > 0)
            {
                g_lin_signal_cnt[i].cnt--;
                l_u8_wr_LI0_RATS1_RLSeatBackrestAdjCmd(g_lin_signal_cnt[i].val);
            }
            else
            {
                l_u8_wr_LI0_RATS1_RLSeatBackrestAdjCmd(0);
            }            
            break;
        case LI0_RATS1_RRSeatBackrestAdjCmd:
            if (g_lin_signal_cnt[i].cnt > 0)
            {
                g_lin_signal_cnt[i].cnt--;
                l_u8_wr_LI0_RATS1_RRSeatBackrestAdjCmd(g_lin_signal_cnt[i].val);
            }
            else
            {
                l_u8_wr_LI0_RATS1_RRSeatBackrestAdjCmd(0);
            } 
            break;
        case LI0_RATS1_DFGLS_ButtonSoundRequest:
            if (g_lin_signal_cnt[i].cnt > 0)
            {
                g_lin_signal_cnt[i].cnt--;
                l_bool_wr_LI0_RATS1_DFGLS_ButtonSoundRequest(g_lin_signal_cnt[i].val);
            }
            else
            {
                l_bool_wr_LI0_RATS1_DFGLS_ButtonSoundRequest(0);
            } 
            break;
        default:
            break;
        }
    }
}


void btn_process_task(void)
{
    uint8_t btn_status = 0;
    for(uint8_t i = 0; i < BTN_NORMAL_NUM ; i++)
    {
        btn_status = get_sensor_state(i) & 0x80;
        if(btn_status == 0x80)
        {
            if(btns_obj[i].ticks >= 2u)
            {
                if(btns_obj[i].status == 0u)
                {
                    btns_obj[i].cap_trig_flg = 1u; // 触发
                    lin_signal_send_x_times(LI0_RATS1_DFGLS_ButtonSoundRequest,1,0x1);
                }
                btns_obj[i].status = 1u;    // 状态置1
            }
            if(btns_obj[i].ticks >= 20u)    // 长按
            {
                btns_obj[i].cap_trig_flg = 1u; // 触发
            }
            btns_obj[i].ticks++;
        }else
        {
            btns_obj[i].ticks = 0;          // 清零
            btns_obj[i].status = 0u;        // 状态置0
        }
    }
}


const uint32_t g_level[11] = {
    0,
    20,
    28,
    37,
    46,
    55,
    64,
    73,
    82,
    91,
    100,
};

/* 背光亮度白天和夜晚的亮度区别 */
#define BACKLIGHT_NIGHT_GAIN 50
#define BACKLIGHT_DAYTIME_GAIN 100
#define DUTY_MAX 100u
/**
 * @brief 背光亮度控制任务
 * 
 * 输入:LIN信号更新到变量
 * 输出:背光亮度控制回调函数
 * 
 */
void backlight_task(void)
{
    uint8_t signal_temp = 0;
    uint8_t pwm_index = 0;
    uint8_t tmp = 0;
    uint8_t night_gain = 0;

    signal_temp = l_bool_rd_LI0_BDCS1_BacklightStatus();
    if(signal_temp == 1u) // 夜晚模式
    {
        night_gain = BACKLIGHT_NIGHT_GAIN;
        pwm_index = l_u8_rd_LI0_BDCS1_Backlight_brightness_fb(); 
        tmp = g_level[pwm_index] * night_gain / 100;   
        TCA0.SPLIT.LCMP2 = tmp; 
    }else
    {
        night_gain = BACKLIGHT_DAYTIME_GAIN;
        TCA0.SPLIT.LCMP2 = night_gain;
    }

    heat_config = l_bool_rd_LI0_BDCS2_RearSeatHeating_Configuration();
    wind_config = l_bool_rd_LI0_BDCS2_RearSeatVentilating_configuration();
}

void __attribute__((optimize("O0"))) lin_go_to_sleep(void)
{
	uint16_t sleep_cnt;

    /* 关闭usart模块 */
    Disable_global_interrupt();
    USART1.CTRLA &= ~(1 << USART_ABEIE_bp | 1 << USART_RXCIE_bp);
    USART1.CTRLB &= ~(1 << USART_TXEN_bp | USART_RXMODE_LINAUTO_gc | 1 << USART_RXEN_bp);
	
    /* 使能脚设置低电平 */
    LIN_EN_set_level(0);
    /* LIN TX设置低电平 */
    PC0_set_dir(PORT_DIR_OUT);
    PC0_set_level(
        // <y> Initial level
        // <id> pad_initial_level
        // <false"> Low
        // <true"> High
        false);
	for(sleep_cnt = 0; sleep_cnt < 10000; sleep_cnt++);
	for(sleep_cnt = 0; sleep_cnt < 10000; sleep_cnt++);
	
	USART_0_init();
	
	LIN_EN_set_level(1);
	Enable_global_interrupt();
}