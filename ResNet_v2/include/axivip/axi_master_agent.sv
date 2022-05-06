
`ifndef AXI_MASTER_AGENT_SV
`define AXI_MASTER_AGENT_SV

class axi_master_agent  #(int ADDR_WIDTH=32,int STRB_WIDTH=8,int LEN_WIDTH=4,int SIZE_WIDTH=3,int ID_WIDTH=1) extends uvm_agent;

    axi_cfg             cfg  ;
    axi_master_awdrv#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH)     awdrv;
    axi_sequencer       awsqr;

    axi_master_wdrv#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH)      wdrv;
    axi_sequencer       wsqr;

    axi_master_bdrv#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH)      bdrv;
    axi_sequencer       bsqr;

    axi_master_ardrv#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH)     ardrv;
    axi_sequencer       arsqr;

    axi_master_rdrv#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH)      rdrv;
    axi_sequencer       rsqr;


    `uvm_component_param_utils_begin(axi_master_agent#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH))
    `uvm_component_utils_end

    function new(string name, uvm_component parent);
        super.new(name, parent);
    endfunction : new

    virtual function void build_phase(uvm_phase phase);
        super.build();
        if(!uvm_config_db #(axi_cfg)::get(this, "", "cfg", cfg))
            `uvm_fatal(this.get_full_name(), "axi config must be set for cfg!!!")

        awdrv = axi_master_awdrv#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH)::type_id::create("awdrv", this);
        awsqr = axi_sequencer  ::type_id::create("awsqr", this);

        wdrv = axi_master_wdrv  #(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH)::type_id::create("wdrv", this);
        wsqr = axi_sequencer   ::type_id::create("wsqr", this);

        bdrv = axi_master_bdrv  #(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH)::type_id::create("bdrv", this);
        bsqr = axi_sequencer   ::type_id::create("bsqr", this);

        ardrv = axi_master_ardrv#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH)::type_id::create("ardrv", this);
        arsqr = axi_sequencer  ::type_id::create("arsqr", this);

        rdrv = axi_master_rdrv  #(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH)::type_id::create("rdrv", this);
        rsqr = axi_sequencer   ::type_id::create("rsqr", this);
    endfunction : build_phase

    virtual function void connect_phase(uvm_phase phase);
        super.connect_phase(phase);
        awdrv.seq_item_port.connect(awsqr.seq_item_export);
        wdrv .seq_item_port.connect(wsqr.seq_item_export);
        bdrv .seq_item_port.connect(bsqr.seq_item_export);
        ardrv.seq_item_port.connect(arsqr.seq_item_export);
        rdrv .seq_item_port.connect(rsqr.seq_item_export);
    endfunction : connect_phase
endclass
`endif

