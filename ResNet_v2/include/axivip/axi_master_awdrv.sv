`ifndef AXI_MASTER_AWDRV_SV
`define AXI_MASTER_AWDRV_SV

class axi_master_awdrv #(int ADDR_WIDTH=32,int STRB_WIDTH=8,int LEN_WIDTH=4,int SIZE_WIDTH=3,int ID_WIDTH=1)
        extends axi_driver_base #(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH);

	`uvm_component_param_utils_begin(axi_master_awdrv#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH))
	`uvm_component_utils_end

    function new (string name, uvm_component parent);
        super.new(name, parent);
    endfunction

    virtual task reset_signals();
        vif.AWVALID  <= 0;
        vif.AWID     <= 0;
        vif.AWLEN    <= 0;
        vif.AWSIZE   <= 0;
        vif.AWBURST  <= 0;
        vif.AWLOCK   <= 0;
    endtask

    virtual task get_and_drive();
        forever begin
            seq_item_port.get_next_item(req);
            if (req.addr_delay >= 0) begin
                wait(vif.AWREADY==='h1);
                #0;
                repeat(req.addr_delay) @(posedge vif.clk);
                if (req.addr_delay > 0) #0;
            end
            vif.AWVALID    = 1'b1;
            vif.AWID       = req.id;
            vif.AWADDR     = req.addr;
            vif.AWREGION   = req.region;
            vif.AWLEN      = req.len;
            vif.AWSIZE     = req.size;
            vif.AWBURST    = req.burst;
            vif.AWLOCK     = req.lock ;
            vif.AWCACHE    = req.cache;
            vif.AWPROT     = req.prot ;
            vif.AWQOS      = req.qos  ;
            vif.AWUSER     = req.user ;
            `uvm_info (this.get_full_name(), "set vif.awvalid = 1", UVM_HIGH)
            forever begin
                @(posedge vif.clk);
                if ((vif.AWREADY === 1) && (vif.AWVALID === 1)) begin
                    `uvm_info (this.get_full_name(), "set vif.awready = 0", UVM_HIGH)
                    break;
                end
            end
            #0;
            vif.AWVALID = 1'b0;
            seq_item_port.item_done();
        end
    endtask
endclass

`endif


