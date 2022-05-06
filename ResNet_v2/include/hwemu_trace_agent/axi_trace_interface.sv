`ifndef AXI_TRACE_INTERFACE__SV
`define AXI_TRACE_INTERFACE__SV

interface axi_trace_monitor_interface #(int ADDRW=32,int DATAW=32, int LENGTHW=4, int SIZEW=3, int IDW=1) ( input string mode, input string name, input string path, input bit clkrst_trace);

    //timeunit 1ps;
    //timeprecision  1ps;

    wire [IDW-1:0    ]             AWID     ;
    wire [ADDRW-1:0  ]             AWADDR   ;
    wire [4-1:0           ]        AWREGION ;
    wire [LENGTHW-1:0   ]          AWLEN    ;
    wire [SIZEW-1:0  ]             AWSIZE   ;
    wire [2-1:0           ]        AWBURST  ;
    wire                           AWLOCK   ;
    wire [4-1:0           ]        AWCACHE  ;
    wire [3-1:0           ]        AWPROT   ;
    wire [4-1:0           ]        AWQOS    ;
    wire                           AWVALID  ;
    wire                           AWREADY  ;
    wire [0:0             ]        AWUSER   ;

    wire [IDW     -1:0    ]        WID      ;
    wire [DATAW       -1:0]        WDATA    ;
    wire [DATAW/8   -1:0  ]        WSTRB    ;
    wire                           WLAST    ;
    wire                           WVALID   ;
    wire                           WREADY   ;
    wire [0:0             ]         WUSER   ;

    wire [IDW     -1:0    ]        BID      ;
    wire [2-1:0           ]        BRESP    ;
    wire                           BVALID   ;
    wire                           BREADY   ;
    wire [0:0             ]         BUSER   ;

    wire [IDW     -1:0    ]        ARID     ;
    wire [ADDRW-1:0  ]             ARADDR   ;
    wire [4-1:0           ]        ARREGION ;
    wire [LENGTHW-1:0   ]          ARLEN    ;
    wire [SIZEW-1:0  ]             ARSIZE   ;
    wire [2-1:0           ]        ARBURST  ;
    wire                           ARLOCK   ;
    wire [4-1:0           ]        ARCACHE  ;
    wire [3-1:0           ]        ARPROT   ;
    wire [4-1:0           ]        ARQOS    ;
    wire                           ARVALID  ;
    wire                           ARREADY  ;
    wire [0:0             ]        ARUSER   ;

    wire [IDW     -1:0    ]        RID      ;
    wire [DATAW       -1:0]        RDATA    ;
    wire [2-1:0           ]        RRESP    ;
    wire                           RLAST    ;
    wire                           RVALID   ;
    wire                           RREADY   ;
    wire [0:0             ]         RUSER   ;

    clk_rst_interface clkrst_if(mode, name, path, clkrst_trace);
    vld_rdy_data_trace_interface#(ADDRW+LENGTHW+SIZEW+IDW+2) AwTraceIf();
    assign AwTraceIf.clk = clkrst_if.clk;
    assign AwTraceIf.rst = clkrst_if.rst;
    vld_rdy_data_trace_interface#(DATAW+DATAW/8+IDW+1) WTraceIf();
    assign WTraceIf.clk = clkrst_if.clk;
    assign WTraceIf.rst = clkrst_if.rst;
    vld_rdy_data_trace_interface#(2+IDW) BTraceIf();
    assign BTraceIf.clk = clkrst_if.clk;
    assign BTraceIf.rst = clkrst_if.rst;
    vld_rdy_data_trace_interface#(ADDRW+LENGTHW+SIZEW+IDW+2) ArTraceIf();
    assign ArTraceIf.clk = clkrst_if.clk;
    assign ArTraceIf.rst = clkrst_if.rst;
    vld_rdy_data_trace_interface#(DATAW+IDW+3) RTraceIf();
    assign RTraceIf.clk = clkrst_if.clk;
    assign RTraceIf.rst = clkrst_if.rst;

    assign AwTraceIf.ready = AWREADY;
    assign AwTraceIf.valid = AWVALID;
    assign AwTraceIf.data  = {AWID, AWSIZE, AWLEN, AWBURST, AWADDR};

    assign  WTraceIf.ready = WREADY;
    assign  WTraceIf.valid = WVALID;
    assign  WTraceIf.data  = {WLAST, WID, WSTRB, WDATA};

    assign  BTraceIf.ready = BREADY;
    assign  BTraceIf.valid = BVALID;
    assign  BTraceIf.data  = {BID, BRESP};

    assign ArTraceIf.ready = ARREADY;
    assign ArTraceIf.valid = ARVALID;
    assign ArTraceIf.data  = {ARID, ARSIZE, ARLEN, ARBURST, ARADDR};

    assign  RTraceIf.ready = RREADY;
    assign  RTraceIf.valid = RVALID;
    assign  RTraceIf.data  = {RLAST, RID, RRESP, RDATA};
endinterface

//work as a master entity
interface axi_trace_master_interface #(int ADDRW=32,int DATAW=32, int LENGTHW=4, int SIZEW=3, int IDW=1) ( input string mode, input string name, input string path, input bit clkrst_trace);

    //timeunit 1ps;
    //timeprecision  1ps;

    wire [IDW-1:0    ]             AWID     ;
    wire [ADDRW-1:0  ]             AWADDR   ;
    wire [4-1:0           ]        AWREGION ;
    wire [LENGTHW-1:0   ]          AWLEN    ;
    wire [SIZEW-1:0  ]             AWSIZE   ;
    wire [2-1:0           ]        AWBURST  ;
    wire                           AWLOCK   ;
    wire [4-1:0           ]        AWCACHE  ;
    wire [3-1:0           ]        AWPROT   ;
    wire [4-1:0           ]        AWQOS    ;
    wire                           AWVALID  ;
    wire                           AWREADY  ;
    wire [0:0             ]        AWUSER   ;

    wire [IDW     -1:0    ]        WID      ;
    wire [DATAW       -1:0]        WDATA    ;
    wire [DATAW/8   -1:0  ]        WSTRB    ;
    wire                           WLAST    ;
    wire                           WVALID   ;
    wire                           WREADY   ;
    wire [0:0             ]         WUSER   ;

    wire [IDW     -1:0    ]        BID      ;
    wire [2-1:0           ]        BRESP    ;
    wire                           BVALID   ;
    wire                           BREADY   ;
    wire [0:0             ]         BUSER   ;

    wire [IDW     -1:0    ]        ARID     ;
    wire [ADDRW-1:0  ]             ARADDR   ;
    wire [4-1:0           ]        ARREGION ;
    wire [LENGTHW-1:0   ]          ARLEN    ;
    wire [SIZEW-1:0  ]             ARSIZE   ;
    wire [2-1:0           ]        ARBURST  ;
    wire                           ARLOCK   ;
    wire [4-1:0           ]        ARCACHE  ;
    wire [3-1:0           ]        ARPROT   ;
    wire [4-1:0           ]        ARQOS    ;
    wire                           ARVALID  ;
    wire                           ARREADY  ;
    wire [0:0             ]        ARUSER   ;

    wire [IDW     -1:0    ]        RID      ;
    wire [DATAW       -1:0]        RDATA    ;
    wire [2-1:0           ]        RRESP    ;
    wire                           RLAST    ;
    wire                           RVALID   ;
    wire                           RREADY   ;
    wire [0:0             ]         RUSER   ;

    clk_rst_interface clkrst_if(mode, name, path, clkrst_trace);
    vld_rdy_data_trace_interface#(ADDRW+LENGTHW+SIZEW+IDW+2) AwTraceIf();
    assign AwTraceIf.clk = clkrst_if.clk;
    assign AwTraceIf.rst = clkrst_if.rst;
    vld_rdy_data_trace_interface#(DATAW+DATAW/8+IDW+1) WTraceIf();
    assign WTraceIf.clk = clkrst_if.clk;
    assign WTraceIf.rst = clkrst_if.rst;
    vld_rdy_data_trace_interface#(2+IDW) BTraceIf();
    assign BTraceIf.clk = clkrst_if.clk;
    assign BTraceIf.rst = clkrst_if.rst;
    vld_rdy_data_trace_interface#(ADDRW+LENGTHW+SIZEW+IDW+2) ArTraceIf();
    assign ArTraceIf.clk = clkrst_if.clk;
    assign ArTraceIf.rst = clkrst_if.rst;
    vld_rdy_data_trace_interface#(DATAW+IDW+3) RTraceIf();
    assign RTraceIf.clk = clkrst_if.clk;
    assign RTraceIf.rst = clkrst_if.rst;

    assign AwTraceIf.ready = AWREADY;
    assign AWVALID = AwTraceIf.valid;
    assign {AWID, AWSIZE, AWLEN, AWBURST, AWADDR}  = AwTraceIf.data;
    assign  WTraceIf.ready = WREADY;
    assign  WVALID = WTraceIf.valid;
    assign {WLAST, WID, WSTRB, WDATA} = WTraceIf.data;
    assign  BREADY = BTraceIf.ready;
    assign  BTraceIf.valid = BVALID;
    assign  BTraceIf.data  = {BID, BRESP};

    assign ArTraceIf.ready = ARREADY;
    assign ARVALID = ArTraceIf.valid;
    assign {ARID, ARSIZE, ARLEN, ARBURST, ARADDR}  = ArTraceIf.data;
    assign  RREADY = RTraceIf.ready;
    assign  RTraceIf.valid = RVALID;
    assign  RTraceIf.data  = {RLAST, RID, RRESP, RDATA};
endinterface

//work as a slave entity
interface axi_trace_slave_interface #(int ADDRW=32,int DATAW=32, int LENGTHW=4, int SIZEW=3, int IDW=1) ( input string mode, input string name, input string path, input bit clkrst_trace);

    //timeunit 1ps;
    //timeprecision  1ps;

    wire [IDW-1:0    ]             AWID     ;
    wire [ADDRW-1:0  ]             AWADDR   ;
    wire [4-1:0           ]        AWREGION ;
    wire [LENGTHW-1:0   ]          AWLEN    ;
    wire [SIZEW-1:0  ]             AWSIZE   ;
    wire [2-1:0           ]        AWBURST  ;
    wire                           AWLOCK   ;
    wire [4-1:0           ]        AWCACHE  ;
    wire [3-1:0           ]        AWPROT   ;
    wire [4-1:0           ]        AWQOS    ;
    wire                           AWVALID  ;
    wire                           AWREADY  ;
    wire [0:0             ]        AWUSER   ;

    wire [IDW     -1:0    ]        WID      ;
    wire [DATAW       -1:0]        WDATA    ;
    wire [DATAW/8   -1:0  ]        WSTRB    ;
    wire                           WLAST    ;
    wire                           WVALID   ;
    wire                           WREADY   ;
    wire [0:0             ]         WUSER   ;

    wire [IDW     -1:0    ]        BID      ;
    wire [2-1:0           ]        BRESP    ;
    wire                           BVALID   ;
    wire                           BREADY   ;
    wire [0:0             ]         BUSER   ;

    wire [IDW     -1:0    ]        ARID     ;
    wire [ADDRW-1:0  ]             ARADDR   ;
    wire [4-1:0           ]        ARREGION ;
    wire [LENGTHW-1:0   ]          ARLEN    ;
    wire [SIZEW-1:0  ]             ARSIZE   ;
    wire [2-1:0           ]        ARBURST  ;
    wire                           ARLOCK   ;
    wire [4-1:0           ]        ARCACHE  ;
    wire [3-1:0           ]        ARPROT   ;
    wire [4-1:0           ]        ARQOS    ;
    wire                           ARVALID  ;
    wire                           ARREADY  ;
    wire [0:0             ]        ARUSER   ;

    wire [IDW     -1:0    ]        RID      ;
    wire [DATAW       -1:0]        RDATA    ;
    wire [2-1:0           ]        RRESP    ;
    wire                           RLAST    ;
    wire                           RVALID   ;
    wire                           RREADY   ;
    wire [0:0             ]         RUSER   ;

    clk_rst_interface clkrst_if(mode, name, path, clkrst_trace);
    vld_rdy_data_trace_interface#(ADDRW+LENGTHW+SIZEW+IDW+2) AwTraceIf();
    assign AwTraceIf.clk = clkrst_if.clk;
    assign AwTraceIf.rst = clkrst_if.rst;
    vld_rdy_data_trace_interface#(DATAW+DATAW/8+IDW+1) WTraceIf();
    assign WTraceIf.clk = clkrst_if.clk;
    assign WTraceIf.rst = clkrst_if.rst;
    vld_rdy_data_trace_interface#(2+IDW) BTraceIf();
    assign BTraceIf.clk = clkrst_if.clk;
    assign BTraceIf.rst = clkrst_if.rst;
    vld_rdy_data_trace_interface#(ADDRW+LENGTHW+SIZEW+IDW+2) ArTraceIf();
    assign ArTraceIf.clk = clkrst_if.clk;
    assign ArTraceIf.rst = clkrst_if.rst;
    vld_rdy_data_trace_interface#(DATAW+IDW+3) RTraceIf();
    assign RTraceIf.clk = clkrst_if.clk;
    assign RTraceIf.rst = clkrst_if.rst;

    assign AWREADY= AwTraceIf.ready ;
    assign AwTraceIf.valid= AWVALID ;
    assign AwTraceIf.data= {AWID, AWSIZE, AWLEN, AWBURST, AWADDR}  ;
    assign WREADY=  WTraceIf.ready ;
    assign WTraceIf.valid=  WVALID ;
    assign WTraceIf.data= {WLAST, WID, WSTRB, WDATA} ;
    assign BTraceIf.ready=  BREADY ;
    assign BVALID=  BTraceIf.valid ;
    assign {BID, BRESP}=  BTraceIf.data  ;

    assign ARREADY= ArTraceIf.ready ;
    assign ArTraceIf.valid= ARVALID ;
    assign ArTraceIf.data= {ARID, ARSIZE, ARLEN, ARBURST, ARADDR}  ;
    assign RTraceIf.ready=  RREADY ;
    assign RVALID=  RTraceIf.valid ;
    assign {RLAST, RID, RRESP, RDATA}=  RTraceIf.data  ;
endinterface



`endif
