/*
 * @Project      : RM_Infantry_Neptune_frame
 * @FilePath     : \BTlog\debug_BTlog_forInfantry.c
 * @Descripttion :
 * @Author       : GDDG08
 * @Date         : 2021-10-31 09:16:32
 * @LastEditors  : GDDG08
 * @LastEditTime : 2022-04-30 10:38:12
 */

#include "debug_BTlog.h"
#if __FN_IF_ENABLE(__FN_INFANTRY_CHASSIS)
#include "cha_chassis_ctrl.h"
#include "cha_gimbal_ctrl.h"
#include "cha_referee_ctrl.h"
#include "cha_power_ctrl.h"
#endif

#if __FN_IF_ENABLE(__FN_INFANTRY_GIMBAL)
#include "gim_gimbal_ctrl.h"
#include "gim_ins_ctrl.h"
#include "key_periph.h"
#include "minipc_periph.h"
#endif

#if __FN_IF_ENABLE(__FN_SUPER_CAP)
#include "supercap_ctrl.h"
#endif

#define ADD_SEND_DATA(x, y, z) AddSendData(&(x), sizeof(x), y, z)
#define ADD_RECV_DATA(x, y) AddRecvData(&(x), sizeof(x), y)

#if __FN_IF_ENABLE(__FN_DEBUG_BTLOG)

#if __FN_IF_ENABLE(__FN_INFANTRY_GIMBAL)
UART_HandleTypeDef* Const_BTlog_UART_HANDLER = &huart6;
const uint8_t Const_BTlog_ID = 0x01;
#elif __FN_IF_ENABLE(__FN_INFANTRY_CHASSIS)
UART_HandleTypeDef* Const_BTlog_UART_HANDLER = &huart5;
const uint8_t Const_BTlog_ID = 0x02;
#elif __FN_IF_ENABLE(__FN_SUPER_CAP)
#endif

/*              Debug BTlog constant            */
const uint32_t Const_BTlog_HEART_SENT_PERIOD = 10;  // (ms)
const uint16_t Const_BTlog_RX_BUFF_LEN_MAX = 5000;
const uint16_t Const_BTlog_TX_BUFF_LEN_MAX = 5000;
const uint16_t Const_BTlog_RX_DATA_LEN_MAX = 50;
const uint16_t Const_BTlog_TX_DATA_LEN_MAX = 50;

uint8_t BTlog_RxData[Const_BTlog_RX_BUFF_LEN_MAX];
uint8_t BTlog_TxData[Const_BTlog_TX_BUFF_LEN_MAX];
BTlog_TableEntry BTlog_Send_Data[Const_BTlog_TX_DATA_LEN_MAX];
BTlog_TableEntry BTlog_Recv_Data[Const_BTlog_RX_DATA_LEN_MAX];

uint8_t BTlog_state_pending = 0;
uint8_t BTlog_state_sending = 0;

uint8_t BTlog_startFlag = 0xfa;
char BTlog_endFlag[] = "@\r\n";
uint8_t BTlog_Recv_endFlag = 0x5a;

// StartFlag, Head and EndFlag
uint16_t BTlog_TX_BUFF_LEN = 3 + 1 + 3;
uint16_t BTlog_TX_DATA_LEN = 0;
// Head Checksum and EndFlag
uint16_t BTlog_RX_BUFF_LEN = 1 + 1 + 1;
uint16_t BTlog_RX_DATA_LEN = 0;

uint32_t BTlog_time = 0;

/**
 * @name: anonymous
 * @msg:
 * @param {void*} ptr
 * @param {uint8_t} size
 * @param {BTlog_TypeEnum} type
 * @param {char*} tag
 * @return {*}
 */
void AddSendData(void* ptr, uint8_t size, BTlog_TypeEnum type, char* tag) {
    BTlog_Send_Data[BTlog_TX_DATA_LEN].ptr = ptr;
    BTlog_Send_Data[BTlog_TX_DATA_LEN].size = size;
    BTlog_Send_Data[BTlog_TX_DATA_LEN].type = type;
    memcpy(BTlog_Send_Data[BTlog_TX_DATA_LEN].tag, tag, BTlog_tagSize);
    BTlog_TX_BUFF_LEN += size;
    BTlog_TX_DATA_LEN++;
}

/**
 * @name: anonymous
 * @msg:
 * @param {void*} ptr
 * @param {uint8_t} size
 * @param {BTlog_TypeEnum} type
 * @param {char*} tag
 * @return {*}
 */
void AddRecvData(void* ptr, uint8_t size, BTlog_TypeEnum type) {
    BTlog_Recv_Data[BTlog_RX_DATA_LEN].ptr = ptr;
    BTlog_Recv_Data[BTlog_RX_DATA_LEN].size = size;
    BTlog_Recv_Data[BTlog_RX_DATA_LEN].type = type;
    BTlog_RX_BUFF_LEN += size;
    BTlog_RX_DATA_LEN++;
}

/**
 * @name: INIT
 * @test: TODO: Add to init
 * @msg:
 * @param {*}
 * @return {*}
 */
void BTlog_Init() {
#if __FN_IF_ENABLE(__FN_INFANTRY_GIMBAL)
    GPIO_Set(LASER);
#endif

#if __FN_IF_ENABLE(__FN_INFANTRY_GIMBAL)
    INS_IMUDataTypeDef* imu = Ins_GetIMUDataPtr();
    Gimbal_GimbalTypeDef* gimbal = Gimbal_GetGimbalControlPtr();
    MiniPC_MiniPCDataTypeDef* minipc_data = MiniPC_GetMiniPCDataPtr();
#elif __FN_IF_ENABLE(__FN_INFANTRY_CHASSIS)
    GimbalYaw_GimbalYawTypeDef* gimbal = GimbalYaw_GetGimbalYawPtr();
    Referee_RefereeDataTypeDef* referee = Referee_GetRefereeDataPtr();
#elif __FN_IF_ENABLE(__FN_SUPER_CAP)
    Sen_PowerValueTypeDef* powerValue = Sen_GetPowerDataPtr();
#endif
    BusComm_BusCommDataTypeDef* buscomm = BusComm_GetBusDataPtr();
    // Motor_MotorTypeDef Motor_chassisMotor1, Motor_chassisMotor2, Motor_chassisMotor3, Motor_chassisMotor4, Motor_gimbalMotorYaw, Motor_gimbalMotorPitch, Motor_feederMotor, Motor_shooterMotorLeft, Motor_shooterMotorRight;

    // Log Data Send
    ADD_SEND_DATA(BTlog_time, uInt32, "current_time");
#if __FN_IF_ENABLE(__FN_INFANTRY_GIMBAL)
    ADD_SEND_DATA(imu->angle.pitch, Float, "imu->angle.pitch");
    ADD_SEND_DATA(imu->angle.yaw, Float, "imu->angle.yaw");
    ADD_SEND_DATA(minipc_data->state, uInt8, "minipcD->state");
    ADD_SEND_DATA(minipc_data->pitch_angle, Float, "minipcD->pitch_angle");
    ADD_SEND_DATA(minipc_data->yaw_angle, Float, "minipcD->yaw_angle");

#elif __FN_IF_ENABLE(__FN_INFANTRY_CHASSIS)
    ADD_SEND_DATA(buscomm->yaw_relative_angle, Float, "yaw_relative_angle");
    ADD_SEND_DATA(Motor_chassisMotor1.encoder.speed, Int16, "Chassis_Motor1_spd");
    ADD_SEND_DATA(Motor_chassisMotor2.encoder.speed, Int16, "Chassis_Motor2_spd");
    ADD_SEND_DATA(Motor_chassisMotor3.encoder.speed, Int16, "Chassis_Motor3_spd");
    ADD_SEND_DATA(Motor_chassisMotor4.encoder.speed, Int16, "Chassis_Motor4_spd");
#elif __FN_IF_ENABLE(__FN_SUPER_CAP)

#endif

// Customize Remote Control Receive
#if __FN_IF_ENABLE(__FN_INFANTRY_GIMBAL)
    ADD_RECV_DATA(remoteData->remote.s[0], uInt8);
    ADD_RECV_DATA(remoteData->remote.s[1], uInt8);
    ADD_RECV_DATA(remoteData->key.w, uInt8);
    ADD_RECV_DATA(remoteData->key.a, uInt8);
    ADD_RECV_DATA(remoteData->key.s, uInt8);
    ADD_RECV_DATA(remoteData->key.d, uInt8);
    ADD_RECV_DATA(remoteData->mouse.l, uInt8);
    ADD_RECV_DATA(remoteData->mouse.r, uInt8);

    ADD_RECV_DATA(remoteData->remote.ch[0], Int16);
    ADD_RECV_DATA(remoteData->remote.ch[1], Int16);
    ADD_RECV_DATA(remoteData->remote.ch[2], Int16);
    ADD_RECV_DATA(remoteData->remote.ch[3], Int16);
    ADD_RECV_DATA(remoteData->remote.ch[4], Int16);

    ADD_RECV_DATA(remoteData->mouse.x, Int16);
    ADD_RECV_DATA(remoteData->mouse.y, Int16);
    ADD_RECV_DATA(remoteData->mouse.z, Int16);

#elif __FN_IF_ENABLE(__FN_INFANTRY_CHASSIS)
    ADD_RECV_DATA(Chassis_Gyro_compensate[0], Float);
    ADD_RECV_DATA(Chassis_Gyro_compensate[1], Float);
    ADD_RECV_DATA(Chassis_Gyro_compensate[2], Float);
    ADD_RECV_DATA(Chassis_Gyro_compensate[3], Float);
#elif __FN_IF_ENABLE(__FN_SUPER_CAP)

#endif

    Uart_InitUartDMA(Const_BTlog_UART_HANDLER);
    Uart_ReceiveDMA(Const_BTlog_UART_HANDLER, BTlog_RxData, Const_BTlog_RX_BUFF_LEN_MAX);
}

/**
 * @name: SEND
 * @msg: TODO:  call this when needed
 * @param {*}
 * @return {*}
 */
void BTlog_Send() {
    if (BTlog_state_pending || !BTlog_state_sending)
        return;

    static uint32_t heart_count = 0;
    if ((HAL_GetTick() - heart_count) <= Const_BTlog_HEART_SENT_PERIOD)
        return;

    BTlog_time = HAL_GetTick();
    uint8_t* buff = BTlog_TxData;

    buff[0] = BTlog_startFlag;
    buff[1] = BTlog_startFlag;
    buff[2] = BTlog_startFlag;
    buff[3] = Const_BTlog_ID;
    int cur_pos = 4;
    for (uint16_t i = 0; i < BTlog_TX_DATA_LEN; i++) {
        uint8_t size = BTlog_Send_Data[i].size;
        memcpy(buff + cur_pos, BTlog_Send_Data[i].ptr, size);
        cur_pos += size;
    }
    memcpy(buff + cur_pos, BTlog_endFlag, sizeof(BTlog_endFlag) - 1);

    heart_count = HAL_GetTick();

    if (HAL_UART_GetState(Const_BTlog_UART_HANDLER) & 0x01)
        return;  // tx busy
    HAL_UART_Transmit_IT(Const_BTlog_UART_HANDLER, buff, BTlog_TX_BUFF_LEN);
}

const uint8_t CMD_GET_STRUCT = 0xFF;
const uint8_t CMD_START_SENDING = 0xF1;
const uint8_t CMD_STOP_SENDING = 0xF2;

const uint8_t CMD_SET_GYRO_COMPENSATE = 0xA0;
const uint8_t CMD_SET_CUSTOMIZE = 0xA5;
/**
 * @name: DECODE
 * @msg:
 * @param {uint8_t*} BTlog_RxData
 * @param {uint16_t} rxdatalen
 * @return {*}
 */
void BTlog_DecodeData(uint8_t* BTlog_RxData, uint16_t rxdatalen) {
    // HAL_UART_Transmit_IT(Const_BTlog_UART_HANDLER, BTlog_RxData, rxdatalen);
    if (rxdatalen == 1) {
        BTlog_state_pending = 1;
        if (BTlog_RxData[0] == CMD_GET_STRUCT) {
            int size = (BTlog_tagSize + 1) * BTlog_TX_DATA_LEN + 3 + 1 + sizeof(BTlog_endFlag);

            uint8_t* buff = BTlog_TxData;
            buff[0] = BTlog_startFlag;
            buff[1] = BTlog_startFlag;
            buff[2] = BTlog_startFlag;
            buff[3] = CMD_GET_STRUCT;
            for (uint16_t i = 0; i < BTlog_TX_DATA_LEN; i++) {
                buff[(BTlog_tagSize + 1) * i + 4] = BTlog_Send_Data[i].type & 0xff;
                memcpy(buff + (BTlog_tagSize + 1) * i + 1 + 4, BTlog_Send_Data[i].tag, BTlog_tagSize);
            }

            memcpy(buff + size - sizeof(BTlog_endFlag), BTlog_endFlag, sizeof(BTlog_endFlag) - 1);
            Uart_SendMessage_IT(Const_BTlog_UART_HANDLER, buff, size);
        } else if (BTlog_RxData[0] == CMD_START_SENDING) {
            BTlog_state_sending = 1;
        } else if (BTlog_RxData[0] == CMD_STOP_SENDING) {
            BTlog_state_sending = 0;
        }
        BTlog_state_pending = 0;

    } else {
#if __FN_IF_ENABLE(__FN_INFANTRY_CHASSIS)
        if (BTlog_RxData[0] == CMD_SET_GYRO_COMPENSATE) {
            // float * 4
            Chassis_Gyro_compensate[0] = buff2float(BTlog_RxData + 1);
            Chassis_Gyro_compensate[1] = buff2float(BTlog_RxData + 5);
            Chassis_Gyro_compensate[2] = buff2float(BTlog_RxData + 9);
            Chassis_Gyro_compensate[3] = buff2float(BTlog_RxData + 13);
        }
#endif

        if (BTlog_RxData[0] == CMD_SET_CUSTOMIZE) {
            // check
            if (BTLog_VerifyData(BTlog_RxData, rxdatalen)) {
                uint8_t* buff = BTlog_RxData;
                int cur_pos = 1;
                for (uint16_t i = 0; i < BTlog_RX_DATA_LEN; i++) {
                    uint8_t size = BTlog_Recv_Data[i].size;
                    memcpy(BTlog_Recv_Data[i].ptr, buff + cur_pos, size);
                    cur_pos += size;
                }
            }
        }
    }
}
/**
 * @brief      Data Checksum Verify
 * @param      buff: Data buffer
 * @param      rxdatalen: recevie data length
 * @retval     Match is 1  not match is 0
 */

uint8_t BTLog_VerifyData(uint8_t* buff, uint16_t rxdatalen) {
    if (rxdatalen != BTlog_RX_BUFF_LEN)
        return 0;
    if (buff[0] != CMD_SET_CUSTOMIZE || buff[rxdatalen - 1] != BTlog_Recv_endFlag)
        return 0;

    uint8_t sum = buff[rxdatalen - 2];
    uint32_t checksum = 0;
    for (int i = 1; i < rxdatalen - 2; i++)
        checksum += buff[i];
    checksum = checksum & 0xff;
    return checksum == sum;
}

/**
 * @name: RX
 * @msg: TODO: add this to uart cbk
 * @param {UART_HandleTypeDef*} huart
 * @return {*}
 */
void BTlog_RXCallback(UART_HandleTypeDef* huart) {
    __HAL_DMA_DISABLE(huart->hdmarx);
    uint16_t rxdatalen = Const_BTlog_RX_BUFF_LEN_MAX - Uart_DMACurrentDataCounter(huart->hdmarx->Instance);

    BTlog_DecodeData(BTlog_RxData, rxdatalen);

    __HAL_DMA_SET_COUNTER(huart->hdmarx, Const_BTlog_RX_BUFF_LEN_MAX);
    __HAL_DMA_ENABLE(huart->hdmarx);
}

#endif
