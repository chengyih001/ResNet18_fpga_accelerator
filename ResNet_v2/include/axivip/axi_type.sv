typedef enum {MASTER=0, SLAVE=1, MON_ALONE=2}     drv_type_enum;
typedef enum {AW_TR=0,W_TR=1,B_TR=2,AR_TR=3,R_TR=4,AW2B_TR=5,AR2R_TR=6,W_ALL=7,R_ALL=8} tr_type_enum;
typedef enum {RD=0, WR=1}                         direction_enum;
typedef enum {RESET_LEVEL_LOW=0, RESET_LEVEL_HIGH=1}    reset_level_enum;
typedef enum {FIXED=0, INCR=1, WRAP=2, RESV=3}       burst_enum;
typedef enum {OKAY=0, EXOKAY=1, SLVERR=2, DECERR=3}  resp_enum;
typedef enum {NORMAL=0, EXCLUSIVE=1, LOCKED=2, RESERVED=3} lock_enum;
typedef enum {NO_DELAY=0,LEFT_ROUND_DELAY=1,LEFT_RND_DELAY=2,PRAGMA_DELAY=3} axi_delay_enum;
typedef enum {  WA0_RA0_C0_B0                        = 0,
                WA0_RA0_C0_B1                        = 1,
                WA0_RA0_C1_B0                        = 2,
                WA0_RA0_C1_B1                        = 3,
                WA0_RA1_C0_B0                        = 4,
                WA0_RA1_C0_B1                        = 5,
                WA0_RA1_C1_B0                        = 6,
                WA0_RA1_C1_B1                        = 7,
                WA1_RA0_C0_B0                        = 8,
                WA1_RA0_C0_B1                        = 9,
                WA1_RA0_C1_B0                        = 10,
                WA1_RA0_C1_B1                        = 11,
                WA1_RA1_C0_B0                        = 12,
                WA1_RA1_C0_B1                        = 13,
                WA1_RA1_C1_B0                        = 14,
                WA1_RA1_C1_B1                        = 15 } cache_enc_enum;

typedef enum
    {
    AP_START = 0,
    AP_READY = 1,
    AP_DONE  = 2,
    FINAL_AP_DONE = 3,
    FINISH   = 4,
    EVENT_NUM = 5
} hs_event_enum;
typedef enum {CHANNEL_FIRST=0, TRANSACTION_FIRST=1} latency_mode_enum;
class axi_latency extends uvm_object;
    rand int    wr_latency;
    rand int    rd_latency;
    rand int    wcrtl_latency;
    rand int    wdata_latency;
    rand int    wbrsp_latency;
    rand int    rcrtl_latency;
    rand int    rdata_latency;

    `uvm_object_utils_begin(axi_latency)
        `uvm_field_int(wr_latency,  UVM_DEFAULT)
        `uvm_field_int(rd_latency,  UVM_DEFAULT)
    `uvm_object_utils_end

    function new ( string name = "axi_latency" );
        super.new(name);
        init();
    endfunction

    function init();
        wr_latency      = 0;
        rd_latency      = 0;
        wcrtl_latency   = 0;
        wdata_latency   = 0;
        wbrsp_latency   = 0;
        rcrtl_latency   = 0;
        rdata_latency   = 0;
    endfunction
    virtual function int get_wr_lat();
        return wr_latency;
    endfunction

    virtual function int get_rd_lat();
        return rd_latency;
    endfunction

    virtual function int get_wctrl_lat();
        return wcrtl_latency;
    endfunction

    virtual function int get_wdata_lat();
        return wdata_latency;
    endfunction

    virtual function int get_wbrsp_lat();
        return wbrsp_latency;
    endfunction

    virtual function int get_rctrl_lat();
        return rcrtl_latency;
    endfunction

    virtual function int get_rdata_lat();
        return rcrtl_latency;
    endfunction
endclass 
//`define MYMSG(A) `"A: %0h`", A
//`define UNKNOWN_CHK(A) if ($isunknown(A)) `uvm_error("unknown value", $psprintf(`MYMSG(A)))
