`timescale 1ns/10ps

interface axi_if #(int ADDR_WIDTH=32,int STRB_WIDTH=8, int LEN_WIDTH=4, int SIZE_WIDTH=3, int ID_WIDTH=1) ( input clk, input rst);

    bit has_checks        = 1;
    bit has_coverage  = 1;

    string name                              ;
    logic [ID_WIDTH-1:0    ]        AWID     ;
    logic [ADDR_WIDTH-1:0  ]        AWADDR   ;
    logic [4-1:0           ]        AWREGION ;
    logic [LEN_WIDTH-1:0   ]        AWLEN    ;
    logic [SIZE_WIDTH-1:0  ]        AWSIZE   ;
    logic [2-1:0           ]        AWBURST  ;
    logic                           AWLOCK   ;
    logic [4-1:0           ]        AWCACHE  ;
    logic [3-1:0           ]        AWPROT   ;
    logic [4-1:0           ]        AWQOS    ;
    logic                           AWVALID  ;
    logic                           AWREADY  ;
    logic [0:0             ]        AWUSER   ;

    logic [ID_WIDTH-1:0    ]        WID      ;
    logic [STRB_WIDTH*8-1:0]        WDATA    ;
    logic [STRB_WIDTH-1:0  ]        WSTRB    ;
    logic                           WLAST    ;
    logic                           WVALID   ;
    logic                           WREADY   ;
    logic [0:0             ]         WUSER   ;

    logic [ID_WIDTH-1:0    ]        BID      ;
    logic [2-1:0           ]        BRESP    ;
    logic                           BVALID   ;
    logic                           BREADY   ;
    logic [0:0             ]         BUSER   ;

    logic [ID_WIDTH-1:0    ]        ARID     ;
    logic [ADDR_WIDTH-1:0  ]        ARADDR   ;
    logic [4-1:0           ]        ARREGION ;
    logic [LEN_WIDTH-1:0   ]        ARLEN    ;
    logic [SIZE_WIDTH-1:0  ]        ARSIZE   ;
    logic [2-1:0           ]        ARBURST  ;
    logic                           ARLOCK   ;
    logic [4-1:0           ]        ARCACHE  ;
    logic [3-1:0           ]        ARPROT   ;
    logic [4-1:0           ]        ARQOS    ;
    logic                           ARVALID  ;
    logic                           ARREADY  ;
    logic [0:0             ]        ARUSER   ;

    logic [ID_WIDTH-1:0    ]        RID      ;
    logic [STRB_WIDTH*8-1:0]        RDATA    ;
    logic [2-1:0           ]        RRESP    ;
    logic                           RLAST    ;
    logic                           RVALID   ;
    logic                           RREADY   ;
    logic [0:0             ]         RUSER   ;

endinterface

