`ifndef AXI_MASTER_BDRV_SV
`define AXI_MASTER_BDRV_SV

class axi_master_bdrv #(int ADDR_WIDTH=32,int STRB_WIDTH=8,int LEN_WIDTH=4,int SIZE_WIDTH=3,int ID_WIDTH=1)
        extends axi_driver_base #(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH);

	`uvm_component_param_utils_begin(axi_master_bdrv#(ADDR_WIDTH,STRB_WIDTH,LEN_WIDTH,SIZE_WIDTH,ID_WIDTH))
	`uvm_component_utils_end

    function new (string name, uvm_component parent);
        super.new(name, parent);
    endfunction : new

    virtual task reset_signals();
        vif.BREADY <= 0;
    endtask

    virtual task get_and_drive();
        forever begin
            int delay;
            seq_item_port.get_next_item(req);
            delay = req.brsp_delay;
            if (delay >= 0) begin
                wait(vif.BVALID===1'b1);
                #0;
                repeat(delay) @(posedge vif.clk);
                if (delay > 0) #0;
            end
            vif.BREADY = 1'b1;
            `uvm_info (this.get_full_name(), "set vif.bready = 1", UVM_HIGH)
            forever begin
                @(posedge vif.clk);
                if ((vif.BREADY === 1) && (vif.BVALID === 1)) begin
                    `uvm_info (this.get_full_name(), "set vif.bready = 0", UVM_HIGH)
                    break;
                end
            end
            #0;
            vif.BREADY = 1'b0;
            seq_item_port.item_done();
        end
    endtask
endclass

`endif


