`ifndef AXI_DRIVER_BASE_SV
`define AXI_DRIVER_BASE_SV

class axi_driver_base#(int ADDR_WIDTH=32, int STRB_WIDTH=8, int LEN_WIDTH=4, int SIZE_WIDTH=3, int ID_WIDTH=1) extends uvm_driver#(axi_transfer);

    virtual interface axi_if#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH, SIZE_WIDTH,ID_WIDTH) vif;
    axi_cfg           cfg;
    int unsigned      m_num_sent;

    axi_transfer tr;
    uvm_analysis_port #(axi_transfer) item_collect_port;

    `uvm_component_param_utils_begin(axi_driver_base#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH))
        `uvm_field_int(m_num_sent, UVM_ALL_ON)
    `uvm_component_utils_end

    function new (string name, uvm_component parent);
        super.new(name, parent);
        item_collect_port = new("item_read_imp", this);
    endfunction : new

    virtual function void build_phase(uvm_phase phase);
        super.build_phase(phase);
        if(!uvm_config_db#(axi_cfg)::get(this, "", "cfg", cfg))
            `uvm_fatal(this.get_full_name(), "axi cfg must be set for cfg!!!")

        if(!uvm_config_db#(virtual interface axi_if#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH) )::get(this, "", "vif", vif))
            `uvm_fatal(this.get_full_name(), "axi vif must be set for vif!!!")
    endfunction

    virtual task reset_signals();
    endtask

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

    virtual task get_and_drive();
    endtask

    virtual task run_phase(uvm_phase phase);
        `uvm_info (this.get_full_name(), "run_phase is called", UVM_LOW)
        forever begin
            reset_signals();
            wait_for_resetdone();
            @(posedge vif.clk);
            fork
                get_and_drive();
                begin
                    wait_for_resetstart();
                end
            join_any
            disable fork;
        end
    endtask
endclass

`endif


