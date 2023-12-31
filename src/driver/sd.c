#define SDC_CMD(n) (0x40 | (n))
#define SDC_GO_IDLE_STATE        SDC_CMD(0)
#define SDC_SEND_OP_COND         SDC_CMD(1)
#define SDC_SEND_IF_COND         SDC_CMD(8)
#define SDC_SEND_CSD             SDC_CMD(9)
#define SDC_SEND_CID             SDC_CMD(10)
#define SDC_STOP_TRANSMISSION    SDC_CMD(12)
#define SDC_SET_BLOCKLEN         SDC_CMD(16)
#define SDC_READ_SINGLE_BLOCK    SDC_CMD(17)
#define SDC_READ_MULTIPLE_BLOCK  SDC_CMD(18)
#define SDC_SET_BLOCK_COUNT      SDC_CMD(23)
#define SDC_WRITE_BLOCK          SDC_CMD(24)
#define SDC_WRITE_MULTIPLE_BLOCK SDC_CMD(25)
#define SDC_APP_SEND_OP_COND     SDC_CMD(41)
#define SDC_APP_PREFIX           SDC_CMD(55)
#define SDC_READ_OCR             SDC_CMD(58)
#define SDC_R1_IDLE        0x01
#define SDC_R1_ERASE_RESET 0x02
#define SDC_R1_ILLEGAL_CMD 0x04
#define SDC_R1_CRC_ERROR   0x08
#define SDC_R1_ERASE_ERROR 0x10
#define SDC_R1_ADDR_ERROR  0x20
#define SDC_R1_PARAM_ERROR 0x40
