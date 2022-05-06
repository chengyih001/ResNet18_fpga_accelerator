`ifndef AXI_ENV_SV
`define AXI_EMV_SV

class axi_env#(int ADDR_WIDTH=32, int STRB_WIDTH=8, int LEN_WIDTH=4, int SIZE_WIDTH=3, int ID_WIDTH=1) extends uvm_env;

    axi_cfg  cfg;

    axi_master_agent#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH)  master;
    axi_slave_agent#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH)   slave;
    axi_monitor#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH)       monitor;
    axi_virtual_sequencer vsqr;
    axi_state         state;
    uvm_analysis_port#(axi_transfer) item_wtr_port;
    uvm_analysis_port#(axi_transfer) item_rtr_port;

    `uvm_component_param_utils_begin(axi_env#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH))
        `uvm_field_object (cfg, UVM_DEFAULT)
    `uvm_component_utils_end


    function new(string name, uvm_component parent);
        super.new(name, parent);
        item_wtr_port = new("item_wtr_port", this);
        item_rtr_port = new("item_rtr_port", this);
    endfunction : new

    virtual function void build_phase(uvm_phase phase);
        super.build_phase(phase);
        if(!uvm_config_db#(axi_cfg)::get(this, "", "cfg", cfg)) begin
            `uvm_info(this.get_full_name(), "no cfg get from high level, generate a default cfg", UVM_LOW)
            cfg = axi_cfg::type_id::create("cfg");
            cfg.set_default();
        end
        uvm_config_db#(axi_cfg)::set(this, "*", "cfg", cfg);

        monitor = axi_monitor#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH)::type_id::create("monitor", this);
        state = axi_state::type_id::create("state", this);

        if(cfg.drv_type==MASTER) master = axi_master_agent#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH)::type_id::create("master",this);
        if(cfg.drv_type==SLAVE) slave = axi_slave_agent#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH)::type_id::create("slave",this);
        if(cfg.drv_type==SLAVE||cfg.drv_type==MASTER)
            vsqr = axi_virtual_sequencer::type_id::create("vsqr", this);
    endfunction : build_phase

    virtual function void connect_phase(uvm_phase phase);
        super.connect_phase(phase);

        if(cfg.drv_type==SLAVE) begin
            vsqr.state = state;
            vsqr.slave_awsqr = slave.awsqr;
            vsqr.slave_wsqr  = slave.wsqr;
            vsqr.slave_bsqr  = slave.bsqr;
            vsqr.slave_arsqr = slave.arsqr;
            vsqr.slave_rsqr  = slave.rsqr;
        end
        if(cfg.drv_type==MASTER) begin
            vsqr.state = state;
            vsqr.master_awsqr = master.awsqr;
            vsqr.master_wsqr  = master.wsqr;
            vsqr.master_bsqr  = master.bsqr;
            vsqr.master_arsqr = master.arsqr;
            vsqr.master_rsqr  = master.rsqr;
        end

        monitor.item_aw_port  .connect(state.aw_imp   );
        monitor.item_w_port   .connect(state.w_imp    );
        monitor.item_b_port   .connect(state.b_imp    );
        monitor.item_aw2b_port.connect(state.aw2b_imp );
        monitor.item_ar_port  .connect(state.ar_imp   );
        monitor.item_r_port   .connect(state.r_imp    );
        monitor.item_ar2r_port.connect(state.ar2r_imp );

        state.item_wtr_port   .connect(item_wtr_port);
        state.item_rtr_port   .connect(item_rtr_port);
    endfunction

    virtual task run_phase(uvm_phase phase);
        `uvm_info(this.get_full_name(), "example_env is running", UVM_LOW)
    endtask
endclass
`endif
