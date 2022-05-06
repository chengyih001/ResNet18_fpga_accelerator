`ifndef AXI_MONITOR_SV
`define AXI_MONITOR_SV

class axi_monitor#(int ADDR_WIDTH=32, int STRB_WIDTH=8, int LEN_WIDTH=4, int SIZE_WIDTH=3, int ID_WIDTH=1) extends uvm_monitor;

    virtual interface axi_if#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH) vif;
    axi_cfg  cfg;
    longint       m_cycle;
    int           m_num_col;

    bit checks_enable   = 1;
    bit coverage_enable = 1;

    int aw2b_delay[][$];
    int aw2b_num;
    int ar2r_delay[][$];
    int ar2r_num;

    uvm_analysis_port #(axi_transfer) item_aw_port;
    uvm_analysis_port #(axi_transfer) item_w_port ;
    uvm_analysis_port #(axi_transfer) item_b_port ;
    uvm_analysis_port #(axi_transfer) item_ar_port;
    uvm_analysis_port #(axi_transfer) item_r_port ;
    uvm_analysis_port #(axi_transfer) item_aw2b_port;
    uvm_analysis_port #(axi_transfer) item_ar2r_port;

    `uvm_component_param_utils_begin(axi_monitor#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH))
       `uvm_field_int  (checks_enable,   UVM_ALL_ON)
       `uvm_field_int  (coverage_enable, UVM_ALL_ON)
    `uvm_component_utils_end


    function new (string name, uvm_component parent);
        super.new(name, parent);
        item_aw_port = new("item_aw_port", this);
        item_w_port = new("item_w_port", this);
        item_b_port = new("item_b_port", this);
        item_ar_port = new("item_ar_port", this);
        item_r_port = new("item_r_port", this);
        item_aw2b_port = new("item_aw2b_port", this);
        item_ar2r_port = new("item_ar2r_port", this);
    endfunction : new

    virtual function void build_phase(uvm_phase phase);
        if(!uvm_config_db#(axi_cfg)::get(this, "", "cfg", cfg))
            `uvm_fatal(this.get_full_name(), "axi cfg must be set for cfg!!!")

        if(!uvm_config_db#(virtual interface axi_if#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH))::get(this, "", "vif", vif))
            `uvm_fatal(this.get_full_name(), "axi vif must be set for vif!!!")
        aw2b_delay = new[cfg.id_num];
        aw2b_num = 0;
        ar2r_delay = new[cfg.id_num];
        ar2r_num = 0;
    endfunction

    virtual task wait_for_resetdone();
        if (cfg.reset_level == RESET_LEVEL_LOW) begin
            wait(vif.rst===1'b1);
        end else if (cfg.reset_level == RESET_LEVEL_HIGH) begin
            wait(vif.rst===1'b0);
        end else begin
            `uvm_fatal(this.get_full_name(), "invalid axi_config received, unsupported reset level!!!")
        end
    endtask

    virtual task wait_for_resetstart();
        if (cfg.reset_level == RESET_LEVEL_LOW) begin
            wait(vif.rst===1'b0);
        end else if (cfg.reset_level == RESET_LEVEL_HIGH) begin
            wait(vif.rst===1'b1);
        end else begin
            `uvm_fatal(this.get_full_name(), "invalid axi_config received, unsupported reset level!!!")
        end
    endtask

    virtual task run_phase(uvm_phase phase);
        `uvm_info (this.get_full_name(), "run_phase is called", UVM_LOW)
        forever begin
            wait_for_resetdone();
            @(posedge vif.clk);
            fork
                collect_aw_transfer();
                collect_w_transfer();
                collect_b_transfer();
                collect_ar_transfer();
                collect_r_transfer();
                collect_aw2b_transfer();
                collect_ar2r_transfer();
                begin
                    wait_for_resetstart();
                end
            join_any
            disable fork;
        end
    endtask

    virtual protected task collect_aw_transfer();
        int delay;
        int valid2ready_mode;
        axi_transfer aw_tr;
        forever begin
            forever begin
                @(posedge vif.clk);
                if(vif.AWREADY===1 || vif.AWVALID===1) begin
                    valid2ready_mode=vif.AWVALID*2+vif.AWREADY;
                    delay = 0;
                    break;
                end
            end
            if( valid2ready_mode!=3 ) begin
                forever begin
                    @(posedge vif.clk);
                    if(valid2ready_mode==1) delay--;
                    if(valid2ready_mode==2) delay++;
                    if(vif.AWREADY===1 && vif.AWVALID===1) break;
                end
            end
            aw_tr = axi_transfer::type_id::create("aw_tr");
            aw_tr . bus_bytenum= STRB_WIDTH  ;
            aw_tr.tr_type      = AW_TR          ;
            aw_tr.rw           = WR             ;
            aw_tr.id           = vif.AWID       ;
            aw_tr.addr         = vif.AWADDR     ;
            aw_tr.len          = vif.AWLEN      ;
            aw_tr.burst        = burst_enum'(vif.AWBURST) ;
            aw_tr.size         = vif.AWSIZE     ;
            aw_tr.lock         = lock_enum'(vif.AWLOCK);
            aw_tr.cache        = cache_enc_enum'(vif.AWCACHE);
            aw_tr.prot         = vif.AWPROT     ;
            aw_tr.qos          = vif.AWQOS      ;
            aw_tr.region       = vif.AWREGION   ;
            aw_tr.addr_delay   = delay          ;
            aw_tr.ainfo_rx     = 1              ;
            aw2b_num++;
            aw2b_delay[vif.AWID].push_back(0);
            aw_tr.write_delay  = (delay<0)? 0 : delay;
            item_aw_port.write(aw_tr)           ;
        end
    endtask

    virtual protected task collect_w_transfer();
        int delay;
        int valid2ready_mode;
        axi_transfer w_tr;
        forever begin
            forever begin
                @(posedge vif.clk);
                if(vif.WREADY===1 || vif.WVALID===1) begin
                    valid2ready_mode=vif.WVALID*2+vif.WREADY;
                    delay = 0;
                    break;
                end
            end
            if( valid2ready_mode!=3 ) begin
                forever begin
                    @(posedge vif.clk);
                    if(valid2ready_mode==1) delay--;
                    if(valid2ready_mode==2) delay++;
                    if(vif.WREADY===1 && vif.WVALID===1) break;
                end
            end
            w_tr = axi_transfer::type_id::create("w_tr");
            w_tr.tr_type      = W_TR           ;
            w_tr.bus_bytenum  = STRB_WIDTH     ;
            w_tr.rw           = WR             ;
            w_tr.id           = vif.WID        ;
            //w_tr.data.push_back(vif.WDATA)     ;
            for(int i=0; i<STRB_WIDTH; i++) begin
                if(vif.WSTRB[i]===1) w_tr.data.push_back(vif.WDATA[i*8+7-:8]);
            end
            w_tr.strb.push_back(vif.WSTRB)     ;
            w_tr.last         = vif.WLAST      ;
            w_tr.data_delay.push_back(delay)   ;
            item_w_port.write(w_tr);
        end
    endtask

    virtual protected task collect_b_transfer();
        int delay;
        int valid2ready_mode;
        axi_transfer b_tr;
        forever begin
            forever begin
                @(posedge vif.clk);
                if(vif.BREADY===1 || vif.BVALID===1) begin
                    valid2ready_mode=vif.BVALID*2+vif.BREADY;
                    delay = 0;
                    break;
                end
            end
            if( valid2ready_mode!=3 ) begin
                forever begin
                    @(posedge vif.clk);
                    if(valid2ready_mode==1) delay--;
                    if(valid2ready_mode==2) delay++;
                    if(vif.BREADY===1 && vif.BVALID===1) break;
                end
            end
            b_tr = axi_transfer::type_id::create("b_tr");
            b_tr.tr_type = B_TR           ;
            b_tr.bus_bytenum= STRB_WIDTH  ;
            b_tr.rw      = WR             ;
            b_tr.id      = vif.BID        ;
            b_tr.bresp   = resp_enum'(vif.BRESP);
            b_tr.brsp_delay = delay       ;
            item_b_port.write(b_tr);
        end
    endtask

    virtual protected task collect_aw2b_transfer();
        axi_transfer aw2b_tr;
        forever begin
            wait(aw2b_num > 0);
            forever begin
                @(posedge vif.clk);
                for(int j=0; j<cfg.id_num; j++) begin
                    for(int k=0; k<aw2b_delay[j].size; k++) aw2b_delay[j][k]++;
                end
                if(vif.BREADY===1 && vif.BVALID===1) break;
            end
            aw2b_tr = axi_transfer::type_id::create("aw2b_tr");
            aw2b_tr.tr_type = AW2B_TR           ;
            aw2b_tr.bus_bytenum= STRB_WIDTH     ;
            aw2b_tr.rw      = WR                ;
            aw2b_tr.id      = vif.BID           ;
            aw2b_tr.a2r_delay = aw2b_delay[vif.BID].pop_front();
            aw2b_num--;
            fork
                begin
                    #0.01;
                    item_aw2b_port.write(aw2b_tr)          ;
                end
            join_none
        end
    endtask

    virtual protected task collect_ar_transfer();
        int delay;
        int valid2ready_mode;
        axi_transfer ar_tr;
        forever begin
            forever begin
                @(posedge vif.clk);
                if(vif.ARREADY===1 || vif.ARVALID===1) begin
                    valid2ready_mode=vif.ARVALID*2+vif.ARREADY;
                    delay = 0;
                    break;
                end
            end
            if( valid2ready_mode!=3 ) begin
                forever begin
                    @(posedge vif.clk);
                    if(valid2ready_mode==1) delay--;
                    if(valid2ready_mode==2) delay++;
                    if(vif.ARREADY===1 && vif.ARVALID===1) break;
                end
            end
            ar_tr = axi_transfer::type_id::create("ar_tr");
            ar_tr.bus_bytenum= STRB_WIDTH  ;
            ar_tr.tr_type      = AR_TR          ;
            ar_tr.rw           = RD             ;
            ar_tr.id           = vif.ARID       ;
            ar_tr.addr         = vif.ARADDR     ;
            ar_tr.len          = vif.ARLEN      ;
            ar_tr.burst        = burst_enum'(vif.ARBURST);
            ar_tr.size         = vif.ARSIZE     ;
            ar_tr.lock         = lock_enum'(vif.ARLOCK);
            ar_tr.cache        = cache_enc_enum'(vif.ARCACHE);
            ar_tr.prot         = vif.ARPROT     ;
            ar_tr.qos          = vif.ARQOS      ;
            ar_tr.region       = vif.ARREGION   ;
            ar_tr.addr_delay   = delay          ;
            ar_tr.ainfo_rx     = 1              ;
            ar2r_delay[vif.ARID].push_back(0);
            ar2r_num++;
            ar_tr.read_delay  = (delay<0)? 0 : delay;
            item_ar_port.write(ar_tr);
        end
    endtask

    virtual protected task collect_r_transfer();
        int delay;
        int valid2ready_mode;
        axi_transfer r_tr;
        forever begin
            forever begin
                @(posedge vif.clk);
                if(vif.RREADY===1 || vif.RVALID===1) begin
                    valid2ready_mode=vif.RVALID*2+vif.RREADY;
                    delay = 0;
                    break;
                end
            end
            if( valid2ready_mode!=3 ) begin
                forever begin
                    @(posedge vif.clk);
                    if(valid2ready_mode==1) delay--;
                    if(valid2ready_mode==2) delay++;
                    if(vif.RREADY===1 && vif.RVALID===1) break;
                end
            end
            r_tr = axi_transfer::type_id::create("r_tr");
            r_tr.tr_type      = R_TR           ;
            r_tr.bus_bytenum= STRB_WIDTH  ;
            r_tr.rw           = RD             ;
            r_tr.id           = vif.RID        ;
            for(int i=0; i<STRB_WIDTH; i++) begin
                //r_tr.data .push_back(vif.RDATA)    ;
                r_tr.data.push_back(vif.RDATA[i*8+7-:8]);
            end
            r_tr.rresp.push_back( resp_enum'(vif.RRESP) )    ;
            r_tr.data_delay.push_back(delay)   ;
            r_tr.last         = vif.RLAST      ;
            item_r_port.write(r_tr);
        end
    endtask

    virtual protected task collect_ar2r_transfer();
        axi_transfer ar2r_tr;
        forever begin
            wait(ar2r_num > 0);
            forever begin
                @(posedge vif.clk);
                for(int j=0; j<cfg.id_num; j++) begin
                    for(int k=0; k<ar2r_delay[j].size; k++) ar2r_delay[j][k]++;
                end
                if(vif.RREADY===1 && vif.RVALID===1 && vif.RLAST === 1) break;
            end
            ar2r_tr = axi_transfer::type_id::create("ar2r_tr");
            ar2r_tr.tr_type = AR2R_TR           ;
            ar2r_tr.bus_bytenum= STRB_WIDTH     ;
            ar2r_tr.rw      = RD                ;
            ar2r_tr.id      = vif.RID           ;
            ar2r_tr.a2r_delay = ar2r_delay[vif.RID].pop_front();
            ar2r_num--;
            fork
                begin
                    #0.01;
                    item_ar2r_port.write(ar2r_tr);
                end
            join_none
        end
    endtask
    // virtual task collect_cycle_count();
    //   forever begin
    //     @(posedge m_vif.AXI_ACLK);
    //     m_cycle += 1;
    //   end
    // endtask

    // function void AXI_slave_monitor::report();
    //   `uvm_info(get_type_name(), $sformatf("\nReport: AXI slave monitor collected %0d transfers", m_num_col),
    //     UVM_LOW)
    // endfunction
endclass

`endif

