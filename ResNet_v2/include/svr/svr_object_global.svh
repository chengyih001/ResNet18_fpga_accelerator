typedef enum
    {
    AP_START,
    AP_READY,
    AP_DONE,
    FINAL_AP_DONE,
    FINISH,
    EVENT_NUM
} hs_event;

typedef enum
    {
    SVR_MASTER,
    SVR_SLAVE
} svr_inst_type;

typedef enum
    {
    AXIS,
    AP_FIFO,
    AP_HS,
    AP_VLD,
    AP_ACK,
    AP_NONE
} svr_protocol_type;

typedef enum bit
    {
    SVR_ACTIVE,
    SVR_PASSIVE
} svr_active_passive_enum;

typedef enum bit
    {
    RESET_LEVEL_LOW,
    RESET_LEVEL_HIGH
} svr_reset_level_enum;

typedef enum 
    {
    NO_DELAY,
    LEFT_ROUND_DELAY, 
    LEFT_RND_DELAY 
} svr_delay_enum;

class svr_latency extends uvm_object;
    rand int    transfer_latency;

    `uvm_object_utils_begin(svr_latency)
        `uvm_field_int(transfer_latency,  UVM_DEFAULT)
    `uvm_object_utils_end

    function new ( string name = "svr_latency" );
        super.new(name);
        init();
    endfunction

    function init();
        transfer_latency      = 0;
    endfunction
    virtual function int get_transfer_lat();
        return transfer_latency;
    endfunction
endclass 