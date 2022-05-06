`ifndef AXI_MASTER_ARDRV_SV
`define AXI_MASTER_ARDRV_SV

class axi_master_ardrv #(int ADDR_WIDTH=32,int STRB_WIDTH=8,int LEN_WIDTH=4,int SIZE_WIDTH=3,int ID_WIDTH=1)
        extends axi_driver_base #(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH);

	`uvm_component_param_utils_begin(axi_master_ardrv#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH))
	`uvm_component_utils_end

    function new (string name, uvm_component parent);
        super.new(name, parent);
    endfunction : new

    virtual task reset_signals();
        vif.ARVALID    <= 0;
        vif.ARID       <= 0;
        vif.ARLEN      <= 0;
        vif.ARSIZE     <= 0;
        vif.ARBURST    <= 0;
    endtask

    virtual task get_and_drive();
        forever begin
            seq_item_port.get_next_item(req);
            if (req.addr_delay >= 0) begin
                wait(vif.ARREADY);
                #0;
                repeat(req.addr_delay) @(posedge vif.clk);
                if (req.addr_delay > 0) #0;
            end
            vif.ARVALID    = 1'b1;
            vif.ARID       = req.id;
            vif.ARADDR     = req.addr;
            vif.ARLEN      = req.len;
            vif.ARSIZE     = req.size;
            vif.ARBURST    = req.burst;
            vif.ARLOCK     = req.lock ;
            vif.ARREGION   = req.region;
            vif.ARCACHE    = req.cache;
            vif.ARPROT     = req.prot ;
            vif.ARQOS      = req.qos  ;
            vif.ARUSER     = req.user ;

            `uvm_info (this.get_full_name(), "set vif.arvalid = 1", UVM_HIGH)
            forever begin
                @(posedge vif.clk);
                if ((vif.ARREADY == 1) && (vif.ARVALID == 1)) begin
                    `uvm_info (this.get_full_name(), "set vif.arvalid = 0", UVM_HIGH)
                    break;
                end
            end
            #0;
            vif.ARVALID = 1'b0;
            seq_item_port.item_done();
        end
    endtask
endclass

`endif


