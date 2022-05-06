`ifndef AXI_BASE_SEQUENCE__SV
`define AXI_BASE_SEQUENCE__SV

class axi_base_sequence extends uvm_sequence;

    event ap_done;
    event ap_ready;
    event finish;
    int ii;
    int latency;
    int axi_latency;
    virtual interface misc_interface misc_if;
    int aw_usr_delay[$];
    int  w_usr_delay[$];
    int  b_usr_delay[$];
    int ar_usr_delay[$];
    int  r_usr_delay[$];
    axi_delay_enum isusr_delay = NO_DELAY;
    axi_cfg cfg;
    axi_state state;
    `uvm_declare_p_sequencer(axi_virtual_sequencer)

    `uvm_object_utils_begin(axi_base_sequence)
    `uvm_object_utils_end

    function new (string name = "axi_base_sequence");
        super.new(name);
        axi_latency = 0;
    endfunction

    virtual task body();
        `uvm_info(this.get_full_name(), "body is called", UVM_MEDIUM)
        if(!uvm_config_db#(axi_cfg)::get(p_sequencer, "", "cfg", cfg))
            `uvm_fatal(this.get_full_name(), "axi config must be set for cfg!!!")
        state = p_sequencer.state;
    endtask
endclass
`endif

